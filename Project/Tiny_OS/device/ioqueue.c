#include "ioqueue.h"
#include "interrupt.h"
#include "global.h"
#include "debug.h"

// 初始化ioqueue
void ioqueue_init(struct ioqueue* ioq)
{
    lock_init(&ioq->qlock);                 // 初始化锁
    ioq->producer = ioq->consumer = NULL;   // 生产者与消费者初始化
    ioq->head = ioq->tail = 0;              // 首尾指针置0
}

// 返回pos在缓冲区下一个位置
static int32_t next_pos(int32_t pos)
{
    return (pos + 1) % bufsize;
}

// 判断队列是否已满
bool ioq_full(struct ioqueue* ioq)
{
    ASSERT(intr_get_status() == INTR_OFF);
    return next_pos(ioq->head) == ioq->tail;
}

// 判断队列是否为空
static bool ioq_empty(struct ioqueue* ioq)
{
    ASSERT(intr_get_status() == INTR_OFF);
    return ioq->head == ioq->tail;
}

// 使当前生产者或消费者在此缓冲区上等待
static void ioq_wait(struct task_struct** waiter)
{
    ASSERT(*waiter == NULL && waiter != NULL);
    *waiter = running_thread();
    thread_block(TASK_BLOCKED);
}

// 唤醒waiter
static void wakeup(struct task_struct** waiter)
{
    ASSERT(*waiter != NULL);
    thread_unblock(*waiter);
    *waiter = NULL;
}

// 消费者获取字符
char ioq_getchar(struct ioqueue* ioq)
{
    ASSERT(intr_get_status() == INTR_OFF);
    while (ioq_empty(ioq))
    {
        lock_acquire(&ioq->qlock);
        ioq_wait(&ioq->consumer);
        lock_release(&ioq->qlock);
    }
    
    char character = ioq->buf[ioq->tail];
    ioq->tail = next_pos(ioq->tail);

    if(ioq->producer != NULL)
    {
        wakeup(&ioq->producer);
    }
    return character;
}

// 生产者写入字符
void ioq_putchar(struct ioqueue* ioq, char character)
{
    ASSERT(intr_get_status() == INTR_OFF);
    while (ioq_full(ioq))
    {
        lock_acquire(&ioq->qlock);
        ioq_wait(&ioq->producer);
        lock_release(&ioq->qlock);
    }
    ioq->buf[ioq->head] = character;
    ioq->head = next_pos(ioq->head);

    if(ioq->consumer != NULL)
    {
        wakeup(&ioq->consumer);
    }
}