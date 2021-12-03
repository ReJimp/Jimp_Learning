#ifndef __THREAD_THREAD_H
#define __THREAD_THREAD_H
#include "stdint.h"
#include "list.h"
#include "bitmap.h"
#include "memory.h"

// 通用函数类型，在许多线程函数中作为形参类型
typedef void thread_func(void*);

// 进程或线程的状态
enum task_status {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITTING,
    TASK_HANGING,
    TASK_DIED
};

/***********   中断栈intr_stack   ***********
* 用于中断发生时保护程序(线程或进程)的上下文环境
* 发生于某个进程或者线程被外部中断或软中断打断时
* 此栈在线程自己的内核栈中位置固定,所在页的最顶端
********************************************/
struct intr_stack {
    uint32_t vec_no;    //kernel.S 宏VECTOR中压入的中断号
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    // 低特权级进入高特权级时压入
    uint32_t err_code;  // err_code会被压入在eip之后
    void (*eip)();      // EIP
    uint32_t cs;        // CS
    uint32_t eflags;    // eflags
    void* esp;          // ESP
    uint32_t ss;        // SS
};

/***********  线程栈thread_stack  ***********
* 用于存储线程中待执行的函数
* 在线程的内核栈中位置不固定
* 在switch_to时保存线程环境
******************************************/
struct thread_stack {
    // 遵守ABI规范
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    // 线程第一次执行时,eip指向待调用的函数kernel_thread,其它时候eip是指向switch_to的返回地址,通过ret调用
    void (*eip)(thread_func* func, void* func_arg);

    // 仅供第一次被调度上CPU时使用

    // kernel_thread(thread_func* func, void* func_arg) { 
    //     func(func_arg); 
    // }
    // |return addr|
    // |    arg4   |
    // |    arg8   |
    void (*unused_retaddr); // 第一次调用thread_func，参数unused_ret用于占位充数为返回地址，但是不会返回
    thread_func* function;  // 由Kernel_thread所调用的函数名
    void *func_arg;         // 由Kernel_thread所调用的函数所需的参数
};

// 进程或线程的PCB
struct task_struct {
    uint32_t* self_kstack;              // 各内核线程都用自己的内核栈
    enum task_status status;
    char name[20];
    uint8_t priority;                   // 线程优先级
    uint8_t ticks;                      // 处理器分配的时间片
    uint32_t elapsed_ticks;             // 任务至今执行总时间
    struct list_elem general_tag;       // general_tag的作用是用于线程在一般的队列中的结点  
    struct list_elem all_list_tag;      // all_list_tag的作用是用于线程队列thread_all_list中的结点
    uint32_t* pgdir;                    // 进程自己页表的虚拟地址
    struct virtual_addr userprog_vaddr; // 
    uint32_t stack_magic;               // 用这串数字做栈的边界标记,用于检测栈的溢出
};

void thread_create(struct task_struct* pthread, thread_func function, void* func_arg);
void init_thread(struct task_struct* pthread, char* name, int prio);
struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg);
struct task_struct* running_thread();
void schedule();
void thread_init();
void thread_block(enum task_status stat);
void thread_unblock(struct task_struct* pthread);

#endif