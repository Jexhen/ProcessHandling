#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

/**
* brief: 创建并初始化队列
* para:	无
* return: Queue指针
*/
extern Queue *new_queue()
{
	Queue *q = (Queue*)malloc(sizeof(Queue));
	q->queue_head = NULL;
	q->queue_tail = NULL;
	q->length = 0;

	return q;
}

/**
* brief: 清空队列的元素
* para:	Queue 需要清空的队列
* return: void
*/
extern void clear_queue(Queue *q)
{
	//队列的头和尾恢复初始状态
	q->queue_head = q->queue_tail = NULL;
	q->length = 0;
}

/**
* brief: 获取队列长度
* para:	q队列指针
* return: int类型的队列长度
*/
extern int queue_length(Queue *q)
{
	return q->length;
}

/**
* brief: 将PCB插入队列
* para:	q队列指针 p进程控制块指针
* return: void
*/
extern void insert(Queue *q, PCB * p)
{
	if (q == NULL) {
		printf("ERROR: 队列指针为空指针\n");
		exit(-1);
	}
	if (q->length == 0) {
		q->queue_head = p;
		q->queue_tail = p;
	} 
	else {
		q->queue_tail->next = p;
		q->queue_tail = q->queue_tail->next;
	}
	(q->length)++;
}

/**
* brief: 删除队列元素
* para:	q队列指针 p被删除元素
* return: void
*/
extern void delete(Queue *q, PCB *p) 
{
	PCB *tmp = NULL;
	if (is_empty(q)) {
		printf("ERROR: 队列为空!\n");
		return;
	}
	if (p == q->queue_head) {
		q->queue_head = q->queue_head->next;
		if (queue_length(q) == 1) {
			q->queue_tail = q->queue_head;
		}
		p->next = NULL;
		(q->length)--;
		return;
	}
	tmp = q->queue_head;
	while (tmp->next != NULL && tmp->next != p) {
		tmp = tmp->next;
	}
	if (tmp->next == NULL) {
		printf("ERROR: 没有找到该进程！\n");
		return;
	}
	else {
		if (p == q->queue_tail) {
			q->queue_tail = tmp;
		}
		tmp->next = p->next;
		p->next = NULL;
		(q->length)--;
	}
}

/**
* brief: 判断队列是否为空
* para:	q队列指针
* return: bool值，空为true，非空为false
*/
extern bool is_empty(Queue *q)
{
	return q->length == 0;
}

