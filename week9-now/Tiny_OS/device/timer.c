#include "timer.h"
#include "io.h"
#include "print.h"
#include "interrupt.h"
#include "thread.h"
#include "debug.h"

#define IRQ0_FREQUENCY      100
#define INPUT_FREQUENCY     1193180
#define COUNTER0_VALUE      INPUT_FREQUENCY / IRQ0_FREQUENCY
#define COUNTER0_PORT       0X40
#define COUNTER0_NO         0
#define COUNTER_MODE        2
#define READ_WRITE_LATCH    3
#define PIT_CONTROL_PORT    0x43

uint32_t ticks;

// 向控制寄存器写入计数器号、读写属性、模式，再写入初始值
static void frequency_set(uint8_t counter_port, uint8_t counter_no, uint8_t rwl, uint8_t counter_mode, uint16_t counter_value) {
    
    outb(PIT_CONTROL_PORT, (uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));
    outb(counter_port, (uint8_t)counter_value);
    outb(counter_port, (uint8_t)(counter_value >> 8));
}

// 时钟中断的中断处理函数
static void intr_timer_handler()
{
    struct task_struct* cur_thread = running_thread();
    // 判断PCB是否被破坏
    ASSERT(cur_thread->stack_magic == 0x19870916);
    ++(cur_thread->elapsed_ticks);
    // 从内核第一次处理时间中断后开始至今的滴哒数,内核态和用户态总共的嘀哒数
    ++ticks;
    if (cur_thread->ticks == 0)
    {
        // 时间片耗尽进行调度
        schedule();
    }
    else
    {
        // 时间片减1
        --(cur_thread->ticks);
    }
}

// 初始化PIT8253
void timer_init() {
    put_str(" timer_init start\n");
    // 设置定时周期，即发中断的周期
    frequency_set(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE, COUNTER0_VALUE);
    register_handler(0x20, intr_timer_handler);
    put_str(" timer_init done\n");
}