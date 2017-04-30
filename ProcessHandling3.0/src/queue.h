#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "type.h"
#include <stdbool.h>

/************ 公共接口 ****************/

/**
* brief: 创建并初始化队列
* para:	无
* return: Queue指针
*/
extern Queue *new_queue();


/**
* brief: 清空队列的元素
* para:	Queue 需要清空的队列
* return: void
*/
extern void clear_queue(Queue *q);

/**
* brief: 获取队列长度
* para:	q队列指针
* return: int类型的队列长度
*/
extern int queue_length(Queue *q);

/**
* brief: 将PCB插入队列
* para:	q队列指针 p进程控制块指针
* return: void
*/
extern void insert(Queue *q, PCB * p);

/**
* brief: 删除队列元素
* para:	q队列指针 p被删除元素
* return: void
*/
extern void delete(Queue *q, PCB *p);

/**
* brief: 判断队列是否为空
* para:	q队列指针
* return: bool值，空为true，非空为false
*/
extern bool is_empty(Queue *q);

#endif // !_QUEUE_H_
