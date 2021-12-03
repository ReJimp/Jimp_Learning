#include "interrupt.h"
#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"

#define PIC_M_CTRL  0x20     //主片控制端口
#define PIC_M_DATA  0x21     //主片数据端口
#define PIC_S_CTRL  0xa0     //从片控制端口
#define PIC_S_DATA  0xa1     //从片数据端口

#define IDT_DESC_CNT 0x30    //中断数量

#define EFLAGS_IF   0x00000200 //if位置1
#define GET_EFLAGS(EFLAG_VAR)   __asm volatile ("pushfl; popl %0" : "=g"(EFLAG_VAR))


// 中断门描述符结构体
struct gate_desc
{
    uint16_t func_offset_low_word;
    uint16_t selector;
    uint8_t dcount;
    uint8_t attribute;
    uint16_t func_offset_high_word;
};

// 静态声明
static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function);
static struct gate_desc idt[IDT_DESC_CNT];

// 保存异常的名字
char* intr_name[IDT_DESC_CNT];
// 真正的中断处理函数地址表
intr_handler idt_table[IDT_DESC_CNT];
// 中断处理函数入口地址表
extern intr_handler intr_entry_table[IDT_DESC_CNT];

// 初始化可编程中断控制器8259A
static void pic_init() {
    put_str("  pic_init start\n");
    // 初始化主片
    outb(PIC_M_CTRL, 0x11); // ICW1: 边沿触发,级联8259, 需要ICW4.
    outb(PIC_M_DATA, 0x20); // ICW2: 起始中断向量号为0x20,也就是IR[0-7] 为 0x20 ~ 0x27
    outb(PIC_M_DATA, 0x04); // ICW3: IR2接从片
    outb(PIC_M_DATA, 0x01); // ICW4: 8086模式, 正常EOI

    // 初始化从片
    outb(PIC_S_CTRL, 0x11); // ICW1: 边沿触发,级联8259, 需要ICW4
    outb(PIC_S_DATA, 0x28); // ICW2: 起始中断向量号为0x28,也就是IR[8-15] 为 0x28 ~ 0x2F
    outb(PIC_S_DATA, 0x02); // ICW3: 设置从片连接到主片的IR2引脚
    outb(PIC_S_DATA, 0x01); // ICW4: 8086模式, 正常EOI

    // 打开主片IR0，接受时钟中断
    // outb(PIC_M_DATA, 0xfe);
    // outb(PIC_S_DATA, 0xff);

    // 打开主片IR1，接受键盘中断
    // outb(PIC_M_DATA, 0xfd);
    // outb(PIC_S_DATA, 0xff);

    // 同时开启时钟和键盘中断
    outb(PIC_M_DATA, 0xfc);
    outb(PIC_S_DATA, 0xff);

    put_str("  pic_init done\n");
}

// 创建中断门描述符
static void make_idt_desc(struct gate_desc* p_gdesc, uint8_t attr, intr_handler function) {
    p_gdesc->func_offset_low_word = (uint32_t)function & 0x0000FFFF;
    p_gdesc->selector = SELECTOR_K_CODE;
    p_gdesc->dcount = 0;
    p_gdesc->attribute = attr;
    p_gdesc->func_offset_high_word = ((uint32_t)function & 0xFFFF0000) >> 16;
}

// 初始化中断描述符表
static void idt_desc_init() {
    put_str("  idt_desc_init start\n");
    for(int i = 0; i < IDT_DESC_CNT; ++i) {
        make_idt_desc(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]);
    }
    put_str("  idt_desc_init done\n");
}

// 通用的中断处理函数
static void general_intr_handler(uint8_t vec_nr) {
    if(vec_nr == 0x27 || vec_nr == 0x2f)
    {
        // IRQ7和IRQ15会产生伪中断
        return;
    }
    // 将光标置为0,从屏幕左上角清出一片打印异常信息的区域,方便阅读
    set_cursor(0);
    int cursor_pos = 0;
    while (cursor_pos < 320)
    {
        put_char(' ');
        ++cursor_pos;
    }
    
    set_cursor(0);
    put_str("[+]***** exception message start *****[+]\n");
    set_cursor(88);
    put_str(intr_name[vec_nr]);
    // 若为 Pagefault，将缺失的地址打印出来并悬停
    if(vec_nr == 14) {
        int page_fault_vaddr = 0;
        // cr2 是存放造成 page_fault 的地址
        __asm("movl %%cr2, %0" : "=r"(page_fault_vaddr));
        put_str("\npage_fault_addr: ");
        put_int(page_fault_vaddr);
    }
    put_str("[+]***** exception message end *****[+]\n");
    while(1);
}

// 中断处理函数注册以及异常名称注册
static void exception_init() {
    for(int i = 0; i < IDT_DESC_CNT; ++i) {
        idt_table[i] = general_intr_handler;
        intr_name[i] = "unknown";
    }
    // 单独设置某些异常名字
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Available Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "Coprocessor Segment Overrun";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP General Protection Exception";
    intr_name[14] = "#PF Page-Fault Exception";
    // intr_name[15] 第15项是intel保留项，未使用
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
}

// 开中断并返回开中断前的状态
enum intr_status intr_enable() {
    enum intr_status old_status;
    if(INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        return old_status;
    }
    else {
        old_status = INTR_OFF;
        __asm volatile ("sti");                //开中断
        return old_status;
    }
}

// 关中断并返回关中断前的状态
enum intr_status intr_disable() {
    enum intr_status old_status;
    if(INTR_ON == intr_get_status()) {
        old_status = INTR_ON;
        __asm volatile ("cli" : : : "memory"); //关中断 
        return old_status;
    }
    else {
        old_status = INTR_OFF;
        return old_status;
    }
}

// 设置中断状态为status
enum intr_status intr_set_status(enum intr_status status) {
    return status & INTR_ON ? intr_enable() : intr_disable();
}

// 获取当前中断的状态
enum intr_status intr_get_status() {
    uint32_t eflags = 0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF & eflags) ? INTR_ON : INTR_OFF;
}

// 在中断处理程序数组第vector_no个元素中注册安装中断处理程序function
void register_handler(uint8_t vector_no, intr_handler function)
{
    idt_table[vector_no] = function;
}

// 完成中断的所有初始化工作
void idt_init() {
    put_str(" idt_init start\n");
    idt_desc_init();
    exception_init();
    pic_init();

    // 加载idt
    uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)(uint32_t)idt << 16));
    __asm volatile("lidt %0" : : "m"(idt_operand));
    put_str(" idt_init done\n");
}