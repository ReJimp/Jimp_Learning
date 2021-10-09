#include "print.h"
#include "init.h"
int main()
{
    put_str("I am kernel\n");
    init_all();
    __asm volatile("sti");
    while(1);
    return 0;
}