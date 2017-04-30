#ifndef _PROCESS_SCHEDULING_H_
#define _PROCESS_SCHEDULING_H_
#include "Type.h"
#define WAIT 1//����״̬������wait
#define RUN 2//����״̬������run
#define FINISH 3//����״̬������finish

/************ �����ӿ� ****************/

/**
* brief: �����������̣��������������
* para:	wq��������ָ��
* return: void
*/
extern void generate_processes();

/**
* brief: ÿ�ε���ʵʱ����ӡ
* para:	wq��������ָ�룬rp��ǰ���н��̣�fq����ɶ���ָ��
* return: int���͵Ķ��г���
*/
extern void print(Queue *wq, PCB *rp, Queue * fq);

/**
* brief: ���û������������棬���̵��ȵ����
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
extern void process_scheduling(Queue *wq, Queue *fq);

/**
* brief: ��������
* para:	Queue ��Ҫ���ٵĶ���
* return: void
*/
extern void destroy(Queue *wq, Queue *fq);

#endif // !PROCESS_SCHEDULING_H

