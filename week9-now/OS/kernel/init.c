#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "../device/timer.h"

// 初始化所有模块
void init_all() {
    put_str("init_all start\n");
    idt_init();
    timer_init();
    put_str("init_all done\n");
}