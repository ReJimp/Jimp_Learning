#include "print.h"
#include "init.h"
#include "thread.h"
#include "interrupt.h"
#include "console.h"
// #include "memory.h"
// #include "debug.h"

int main()
{
    put_str("I am kernel\n");
    init_all();
    // 测试时钟中断
    // __asm volatile("sti");

    // 测试断言
    // ASSERT(1==2);

    // 测试内存管理
    // void* addr = get_kernel_pages(3);
    // put_char('\n');
    // put_str("get_kernel_page_start_vaddr: ");
    // put_int((uint32_t)addr);
    // put_char('\n');

    // 测试单线程
    // thread_start("k_thread_a", 31, k_thread_a, "k_thread_a-args\n");

    // 测试多线程
    // thread_start("k_thread_a", 64, k_thread_a, "A-args ");
    // thread_start("k_thread_b", 32, k_thread_b, "B-args ");
    // thread_start("k_thread_c", 16, k_thread_c, "C-args ");
    // thread_start("k_thread_d", 8, k_thread_d, "D-args ");

    // intr_enable();
    // while (1)
    // {
    //     console_put_str("Main ");
    // }

    // 测试键盘中断
    intr_enable();
    return 0;
}

// 线程中运行的函数
void k_thread_a(void* arg) {
    char* para = (char*)arg;
    while (1)
    {
        console_put_str(para);
    }
}

// 线程中运行的函数
void k_thread_b(void* arg) {
    char* para = (char*)arg;
    while (1)
    {
        console_put_str(para);
    }
}

// 线程中运行的函数
void k_thread_c(void* arg) {
    char* para = (char*)arg;
    while (1)
    {
        console_put_str(para);
    }
}

// 线程中运行的函数
void k_thread_d(void* arg) {
    char* para = (char*)arg;
    while (1)
    {
        console_put_str(para);
    }
}