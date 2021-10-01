#include "print.h"
int main()
{
    put_str("Jimp will be the master of pwn\n");
    put_int(0x12345678);
    put_char('\n');
    put_int(0xdeadbeef);
    put_char('\n');
    put_int(0x00000000);
    put_char('\n');
    put_int(0x00005678);
    while(1);
    return 0;
}