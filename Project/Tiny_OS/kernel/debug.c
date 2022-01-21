#include "debug.h"
#include "interrupt.h"
#include "print.h"

// 打印文件名、行号、函数名、条件，并悬停程序
void panic_spin(char* filename, int line, const char* func, const char* condition) {
    intr_disable();
    put_str("[-] error occur :( \n");

    put_str("filename: ");
    put_str(filename);
    put_char('\n');

    put_str("line: 0x");
    put_int(line);
    put_char('\n');

    put_str("function: ");
    put_str((char*)func);
    put_char('\n');

    put_str("condition: ");
    put_str((char*)condition);
    put_char('\n');

    while(1);

}
