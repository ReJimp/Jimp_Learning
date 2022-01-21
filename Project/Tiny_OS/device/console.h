#ifndef __DEVICE_CONSOLE_H
#define __DEVICE_CONSOLE_H
#include "stdint.h"

void console_int();
void console_acquire();
void console_release();
void console_put_str(char* str);
void console_put_char(uint8_t char_asci);
void cosole_put_int(uint32_t num);

#endif