#ifndef __LIB_IO_H
#define __LIB_IO_H
#include "stdint.h"

/**************	 机器模式   ***************
	 b -- 输出寄存器QImode名称,即寄存器中的最低8位:[a-d]l。
	 w -- 输出寄存器HImode名称,即寄存器中2个字节的部分,如[a-d]x。
	 HImode
	     “Half-Integer”模式，表示一个两字节的整数。 
	 QImode
	     “Quarter-Integer”模式，表示一个一字节的整数。 
*******************************************/ 

// 向端口写入一个字节
static inline void outb(uint16_t port, uint8_t data) {
    // a表示用寄存器al或ax或eax,对端口指定N表示0~255, d表示用dx存储端口号, %b0表示对应al,%w1表示对应dx
    __asm volatile ("outb %b0, %w1" : : "a"(data), "Nd"(port));    
}

// 将addr处起始的word_cnt个字写入端口
static inline void outsw(uint16_t port, const void* addr, uint32_t word_cnt) {
    //  +表示此限制即做输入又做输出,outsw是把ds:esi处的16位的内容写入port端口
    __asm volatile ("cld; rep outsw" : "+S"(addr), "+c"(word_cnt) : "d"(port));
}

// 从端口读取一字节返回
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm volatile ("inb %w1, %b0" : "=a"(data) : "Nd"(port));
    return data;
}

// 从端口读取word_cnt个字写入addr
static inline void insw(uint16_t port, void* addr, uint32_t word_cnt) {
    //  insw是将从端口port处读入的16位内容写入es:edi指向的内存
    __asm volatile ("cld; rep insw" : "+D"(addr), "+c"(word_cnt) : "d"(port) : "memory");
}

#endif