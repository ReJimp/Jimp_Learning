#include "memory.h"
#include "stdint.h"
#include "print.h"

// 页大小
#define PG_SIZE 4096
/***************  位图地址 ********************
* 因为0xc009f000是内核主线程栈顶，0xc009e000是内核主线程的pcb.
* 一个页框大小的位图可表示128M内存, 位图位置安排在地址0xc009a000,
* 这样本系统最大支持4个页框的位图,即512M        */
#define MEM_BITMAP_BASE 0xc009a000
// 0xc0000000是内核从虚拟地址3G起. 0x100000意指跨过低端1M内存,使虚拟地址在逻辑上连续 
#define K_HEAP_START 0xc0100000

// 内存池结构，生产两个实例用于管理内核内存池和用户内存池
struct pool {
    struct bitmap pool_bitmap;  // 位图结构，用于管理物理内存
    uint32_t phy_addr_start;    // 物理内存起始地址
    uint32_t pool_size;         // 池容量
};

struct pool kernel_pool, user_pool; // 实例化物理内存池
struct virtual_addr kernel_vaddr;   // 为内核分配虚拟地址

// 初始化内核池
static void mem_pool_init(uint32_t all_mem) {
    put_str(" mem_pool_init start\n");
    // 1页目录表+255页表   
    uint32_t page_table_size = PG_SIZE * 256;
    // 跨越低1MB内存以及页目录、页表
    uint32_t used_mem = page_table_size + 0x100000;
    // 空闲内存
    uint32_t free_mem = all_mem - used_mem;
    // 空闲物理页数
    uint16_t all_free_pages = free_mem / PG_SIZE;
    // 空闲内核页数
    uint16_t kernel_free_pages = all_free_pages / 2;
    // 空闲用户页数
    uint16_t user_free_pages = all_free_pages - kernel_free_pages;

    // 不处理余数，会丢失部分物理内存
    // 内核位图bits数组字节数
    uint32_t kbm_length = kernel_free_pages / 8;
    // 用户位图bits数组字节数
    uint32_t ubm_length = user_free_pages / 8;

    // 内核空闲空间起始地址
    uint32_t kp_start = used_mem;
    // 用户空闲空间起始地址
    uint32_t up_start = kp_start + kernel_free_pages * PG_SIZE;

    kernel_pool.phy_addr_start = kp_start;
    user_pool.phy_addr_start = up_start;

    kernel_pool.pool_size = kernel_free_pages * PG_SIZE;
    user_pool.pool_size = user_free_pages * PG_SIZE;

    kernel_pool.pool_bitmap.btmp_bytes_len = kbm_length;
    user_pool.pool_bitmap.btmp_bytes_len = ubm_length;

    kernel_pool.pool_bitmap.bits = (void*)MEM_BITMAP_BASE;
    user_pool.pool_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length);

    // 内存池信息检查
    put_str("  kernel_pool_bitmap_start: ");
    put_int((int)kernel_pool.pool_bitmap.bits);
    put_char('\n');
    put_str("  kernel_pool_bitmap_end: ");
    put_int((int)kernel_pool.pool_bitmap.bits + kernel_pool.pool_bitmap.btmp_bytes_len);
    put_char('\n');

    put_str("  kernel_pool_phy_addr_start: ");
    put_int((int)kernel_pool.phy_addr_start);
    put_char('\n');
    put_str("  kernel_pool_phy_addr_end: ");
    put_int((int)kernel_pool.phy_addr_start + kernel_pool.pool_size);
    put_char('\n');

    put_str("  user_pool_bitmap_start: ");
    put_int((int)user_pool.pool_bitmap.bits);
    put_char('\n');
    put_str("  user_pool_bitmap_end: ");
    put_int((int)user_pool.pool_bitmap.bits + user_pool.pool_bitmap.btmp_bytes_len);
    put_char('\n');

    put_str("  user_pool_phy_addr_start: ");
    put_int((int)kernel_pool.phy_addr_start);
    put_char('\n');
    put_str("  user_pool_phy_addr_end: ");
    put_int((int)user_pool.phy_addr_start + user_pool.pool_size);
    put_char('\n');

    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    kernel_vaddr.vaddr_bitmap.btmp_bytes_len = kbm_length;

    kernel_vaddr.vaddr_bitmap.bits = (void*)(MEM_BITMAP_BASE + kbm_length + ubm_length);
    kernel_vaddr.vaddr_start = K_HEAP_START;

    put_str("  kernel_vaddr_vaddr_bitmap_start: ");
    put_int((int)kernel_vaddr.vaddr_bitmap.bits);
    put_char('\n');
    put_str("  kernel_vaddr_vaddr_bitmap_end: ");
    put_int((int)kernel_vaddr.vaddr_bitmap.bits + kernel_vaddr.vaddr_bitmap.btmp_bytes_len);
    put_char('\n');   

    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str(" mem_pool_init done\n");
}

// 内存管理部分初始化入口
void mem_init() {
    put_str("mem_init start\n");
    // loader.S中获取到的物理内存总量存储到0xb00地址处
    uint32_t mem_bytes_total = *(uint32_t*)(0xb00);
    mem_pool_init(mem_bytes_total);
    put_str("mem_init done\n");
}