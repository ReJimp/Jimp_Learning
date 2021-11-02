#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "timer.h"
#include "memory.h"
#include "thread.h"
#include "console.h"

// 初始化所有模块
void init_all() {
    put_str("init_all start\n");
    idt_init();
    mem_init();
    thread_init();
    timer_init();
    console_int();
    put_str("init_all done\n");
}