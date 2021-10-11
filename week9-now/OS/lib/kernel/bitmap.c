#include "bitmap.h"
#include "stdint.h"
#include "string.h"
#include "print.h"
#include "interrupt.h"
#include "debug.h"

// 初始化位图
void bitmap_init(struct bitmap* btmp) {
    memset(btmp->bits, 0, btmp->btmp_bytes_len);
}

// 判断bit_idx位是否为1,若为1则返回true，否则返回false
bool bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx) {
    uint32_t byte_idx = bit_idx / 8;    // 向下取整用于索引数组下标
    uint32_t bit_odd = bit_idx % 8;     // 取余用于索引数组内的位
    return (btmp->bits[byte_idx] & (BITMAP_MASK << bit_odd));
}