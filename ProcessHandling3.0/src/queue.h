#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "type.h"
#include <stdbool.h>

/************ �����ӿ� ****************/

/**
* brief: ��������ʼ������
* para:	��
* return: Queueָ��
*/
extern Queue *new_queue();


/**
* brief: ��ն��е�Ԫ��
* para:	Queue ��Ҫ��յĶ���
* return: void
*/
extern void clear_queue(Queue *q);

/**
* brief: ��ȡ���г���
* para:	q����ָ��
* return: int���͵Ķ��г���
*/
extern int queue_length(Queue *q);

/**
* brief: ��PCB�������
* para:	q����ָ�� p���̿��ƿ�ָ��
* return: void
*/
extern void insert(Queue *q, PCB * p);

/**
* brief: ɾ������Ԫ��
* para:	q����ָ�� p��ɾ��Ԫ��
* return: void
*/
extern void delete(Queue *q, PCB *p);

/**
* brief: �ж϶����Ƿ�Ϊ��
* para:	q����ָ��
* return: boolֵ����Ϊtrue���ǿ�Ϊfalse
*/
extern bool is_empty(Queue *q);

#endif // !_QUEUE_H_
