#ifndef __DEVICE_IOQUEUE_H
#define __DEVICE_IOQUEUE_H
#include "stdint.h"
#include "thread.h"
#include "sync.h"

#define bufsize 64

// 环形队列
struct ioqueue
{
    struct lock qlock;
    struct task_struct* producer;   // 数据写入者
    struct task_struct* consumer;   // 数据读出者
    char buf[bufsize];       
    int32_t head;                   // 队头，数据写入处
    int32_t tail;                   // 队尾，数据读出处
};

void ioqueue_init(struct ioqueue* ioq);
bool ioq_full(struct ioqueue* ioq);
bool ioq_empty(struct ioqueue* ioq);
char ioq_getchar(struct ioqueue* ioq);
void ioq_putchar(struct ioqueue* ioq, char character);

#endif