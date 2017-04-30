#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

/**
* brief: ��������ʼ������
* para:	��
* return: Queueָ��
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
* brief: ��ն��е�Ԫ��
* para:	Queue ��Ҫ��յĶ���
* return: void
*/
extern void clear_queue(Queue *q)
{
	//���е�ͷ��β�ָ���ʼ״̬
	q->queue_head = q->queue_tail = NULL;
	q->length = 0;
}

/**
* brief: ��ȡ���г���
* para:	q����ָ��
* return: int���͵Ķ��г���
*/
extern int queue_length(Queue *q)
{
	return q->length;
}

/**
* brief: ��PCB�������
* para:	q����ָ�� p���̿��ƿ�ָ��
* return: void
*/
extern void insert(Queue *q, PCB * p)
{
	if (q == NULL) {
		printf("ERROR: ����ָ��Ϊ��ָ��\n");
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
* brief: ɾ������Ԫ��
* para:	q����ָ�� p��ɾ��Ԫ��
* return: void
*/
extern void delete(Queue *q, PCB *p) 
{
	PCB *tmp = NULL;
	if (is_empty(q)) {
		printf("ERROR: ����Ϊ��!\n");
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
		printf("ERROR: û���ҵ��ý��̣�\n");
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
* brief: �ж϶����Ƿ�Ϊ��
* para:	q����ָ��
* return: boolֵ����Ϊtrue���ǿ�Ϊfalse
*/
extern bool is_empty(Queue *q)
{
	return q->length == 0;
}

