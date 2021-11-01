#include "print.h"
#include "init.h"
#include "thread.h"
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
    thread_start("k_thread_a", 31, k_thread_a, "A-args\n");
    thread_start("k_thread_b", 8, k_thread_b, "B-args\n");

    intr_enable();
    while (1)
    {
        put_str("Main\n");
    }
    

    while(1);
    return 0;
}

// 线程中运行的函数
void k_thread_a(void* arg) {
    char* para = (char*)arg;
    while (1)
    {
        put_str(para);
    }
}

// 线程中运行的函数
void k_thread_b(void* arg) {
    char* para = (char*)arg;
    while (1)
    {
        put_str(para);
    }
}