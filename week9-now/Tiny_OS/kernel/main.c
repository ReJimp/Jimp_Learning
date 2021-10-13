#include "print.h"
#include "init.h"
#include "memory.h"
// #include "debug.h"
int main()
{
    put_str("I am kernel\n");
    init_all();
    // __asm volatile("sti");
    // ASSERT(1==2);

    void* addr = get_kernel_pages(3);
    put_char('\n');
    put_str("get_kernel_page_start_vaddr: ");
    put_int((uint32_t)addr);
    put_char('\n');

    while(1);
    return 0;
}