#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "process_handling.h"
#include "queue.h"

//�ڲ�����: ���ڲ���һ�����̣���generate_processes()���ã������⹫��
static PCB *generate_process()
{
	static char name = 'a';
	static cnt = 0;

	PCB *p = (PCB*)malloc(sizeof(PCB));
	p->pcocess_name = name++;
	cnt++;
	if (cnt == 1) {
		p->arrive_time = 0;
	}
	else {
		p->arrive_time = rand() % 13;
	}
	p->need_time = rand() % 11;
	p->finish_time = 0;
	p->process_status = WAIT;
	p->wait_time = 0;
	p->priority_num = (p->wait_time + p->need_time) * 1.0 / p->need_time;
	p->next = NULL;

	return p;
}

/**
* brief: �ڲ��������û����������Ŀ
* para:	��
* return: �û�Ҫ������Ľ�����Ŀ
*/
static int input()
{
	int num;
	int c;

	printf("����������Ҫ�����Ľ�����Ŀ:\n");
	c = scanf_s("%d", &num, 1);
	if (c <= 0) {
		printf("ERROR: ������Ĳ�������������\n");
		exit(-1);
	}

	return num;
}

/**
* brief: �����������̣��������������
* para:	wq��������ָ��
* return: void
*/
extern void generate_processes(Queue *wq)
{
	int num = input();

	for (int i = num; i > 0; i--) {
		PCB *p = generate_process();
		insert(wq, p);
	}
}

/**
* brief: ÿ�ε���ʵʱ����ӡ
* para:	wq��������ָ�룬rp��ǰ���н��̣�fq����ɶ���ָ��
* return: int���͵Ķ��г���
*/
extern void print(Queue *wq, PCB *rp, Queue *fq)
{
	printf("***************** �������еĽ��� *******************\n");
	printf("������ ������ ����ʱ�� ��Ҫ����ʱ�� ������ʱ�� ����״̬\n");
	if (rp != NULL)
		printf("%3c %8.2f %6d %7d %13d %8d\n", rp->pcocess_name, rp->priority_num,rp->arrive_time,rp->need_time, rp->finish_time, rp->process_status);
	printf("\n");

	printf("***************** �������еĽ��� *******************\n");
	printf("������ ������ ����ʱ�� ��Ҫ����ʱ�� ������ʱ�� ����״̬\n");
	PCB *wp = wq->queue_head;
	while (wp != NULL) {
		printf("%3c %8.2f %6d %7d %13d %8d\n", wp->pcocess_name, wp->priority_num, wp->arrive_time, wp->need_time, wp->finish_time, wp->process_status);
		wp = wp->next;
	}
	printf("\n");

	printf("***************** �Ѿ���ɵĽ��� *******************\n");
	printf("������ ������ ����ʱ�� ��Ҫ����ʱ�� ������ʱ�� ����״̬\n");
	PCB *fp = fq->queue_head;
	while (fp != NULL) {
		printf("%3c %8.2f %6d %7d %13d %8d\n", fp->pcocess_name,fp->priority_num, fp->arrive_time, fp->need_time, fp->finish_time, fp->process_status);
		fp = fp->next;
	}
	printf("\n\n");
}

//�ڲ�����: ���ڸ��¾������еĵȴ�ʱ������ȼ��������⹫��
static void modify_wq(Queue *q)
{
	PCB *tmp = q->queue_head;
	while (tmp != NULL) {
		(tmp->wait_time)++;
		tmp->priority_num = (tmp->wait_time + tmp->need_time) * 1.0 / tmp->need_time;
		tmp = tmp->next;
	}
}

/**
* brief: �̽������ȵ���
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
extern void SJF(Queue *wq, Queue *fq)
{
	PCB *rp = NULL;//��ʾ��ǰ���н��̣���ʼֵΪ�գ��޵����κν���
	
	//���÷���ռʽ�Ķ̽������ȷ�ʽ
	while (1) {
		//rpΪ�ձ���û���������еĽ��̣�rp����Ҫ����ʱ��Ϊ0�����ý����Ѿ��������
		//�������������Ҫ�����ھ������н��е���
		if (rp == NULL || rp->need_time == 0) {
			// �������е�ÿ��Ԫ�أ��ҵ���Ҫ����ʱ����̵Ľ��̽��е���
			PCB *min = wq->queue_head;
			PCB *tmp = wq->queue_head->next;
			while (tmp != NULL) {
				if (tmp->need_time < min->need_time) {
					min = tmp;
				}
				tmp = tmp->next;
			}
			delete(wq, min);//�ھ������жԱ����ȵĽ��̽���ɾ������
			rp = min;//���������Ѿ�����������Ƚ�������״̬
			rp->process_status = RUN;//�޸Ľ���״̬Ϊ��������
			rp->next = NULL;
			print(wq, rp, fq);
		}
		//�������еĽ��̵���Ҫ����ʱ�����
		(rp->need_time)--;
		//�������еĽ��̵��Ѿ�����ʱ������
		(rp->finish_time)++;
		//�����������еĽ����Ƿ��Ѿ���ɣ��Ѿ���ɽ�����ӵ�����ɽ��̶���
		if (rp->need_time == 0) {
			rp->process_status = FINISH;
			insert(fq, rp);
		}
		//���¾������н��̵ĵȴ�ʱ���������
		modify_wq(wq);
		if (is_empty(wq) && rp->need_time == 0) {
			rp = NULL;//���������������Ϊ����״̬
			print(wq, rp, fq);
			clear(fq);
			clear(wq);
			break;
		}
	}
}

//�ڲ�����: ���ڸ���RR�������������еĽ��̵�����ʱ����ʱ��ƬΪ��λ
static void make_timeslice(Queue *wq, int timeslice)
{
	PCB *tmp = wq->queue_head;

	while (tmp != NULL) {
		//�������ʱ���������ʱ��Ƭ����ȡ����ʱ����ʱ��Ƭ����
		//���������ǵ�������ȡ��
		if (tmp->need_time % timeslice == 0) {
			tmp->need_time == tmp->need_time / timeslice;
		} 
		else {
			tmp->need_time = tmp->need_time / timeslice + 1;
		}
		tmp = tmp->next;
	}
}

/**
* brief: ʱ��Ƭ��ת����
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
extern void RR(Queue *wq, Queue *fq)
{
	PCB *rp = NULL;
	int  timeslice = 4;//ʱ��Ƭ

	make_timeslice(wq, timeslice);//���������еĽ��̵�����ʱ���Ϊ��ʱ��ƬΪ��λ
	while (1) {
		//�������Ϊ����״̬ʱ
		if (rp == NULL) {
			timeslice = 4;//���µ��Ⱥ�ʱ��Ƭ��������Ϊ��ֵ4
			rp = wq->queue_head;//ÿ�ε��Ⱦ������ж��׽���
			delete(wq, rp);//�������Ƚ��̴Ӿ���������ɾ��
			rp->process_status = RUN;//����״̬����Ϊ����
			rp->next = NULL;
			print(wq, rp, fq);//����һ�δ�ӡ
		}
		timeslice--;//ʱ��Ƭ�Լ����������ʱ��Ƭ����
		if (timeslice == 0) {
			//��timeslice����0ʱ���н��̵���Ҫ����ʱ���һ����λ
			(rp->need_time)--;
			//�����ʱ���һ����λ
			(rp->finish_time)++;
			//��Ҫ����ʱ��Ϊ0�޸Ľ���״̬Ϊ��������ӵ����ѽ������У�������һ�ε���
			if (rp->need_time == 0) {
				rp->process_status = FINISH;
				insert(fq, rp);
			} 
			else {
				//�����Ϊ0������״̬����Ϊ�ȴ������²��뵽�������ж�β
				rp->process_status = WAIT;
				insert(wq, rp);
			}
			//�������е����н��̵ȴ�ʱ���һ��ʱ�䵥λ�͸���������
			modify_wq(wq);
			//�����������Ϊ����״̬���������������
			rp = NULL;
		}
		
		//����������Ϊ�ղ����޽���ռ�Ŵ����ʱֹͣ����
		//��ֻ��һ�����̱�����ʱ��Ȼ���������ǿյĵ��ǻ����н����������У�����ֹͣ����
		//ֻ�е����������ǿյģ�Ҳû�н���������ʱ����ֹͣ����
		if (is_empty(wq) && rp == NULL) {
			print(wq, rp, fq);
			clear(fq);
			clear(wq);
			break;
		}
	}
}

/**
* brief: ����Ӧ�����ȵ���
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
extern void HRRN(Queue *wq, Queue *fq)
{
	PCB *rp = NULL;
	PCB *max_prio = wq->queue_head;

	while (1) {
		//�Ӿ���������ѡ�����������Ľ���
		PCB *tmp = wq->queue_head;
		while (tmp != NULL) {
			if (tmp->priority_num > max_prio->priority_num) {
				max_prio = tmp;
			}
			tmp = tmp->next;
		}
		//���rp��ΪNULL˵���н����������У�
		//��Ҫ���������еĽ��̵�״̬�޸�Ϊ�ȴ������������²��뵽�������ж�β�����Ž����ȼ��ߵĽ�������
		if (rp != NULL) {
			rp->process_status = WAIT;
			insert(wq, rp);
		}
		//�������ȼ��Ľ��̴Ӿ���������ɾ��
		delete(wq, max_prio);
		//���ý�����ӵ����������
		rp = max_prio;
		//���ý���״̬�޸�Ϊ����
		rp->process_status = RUN;
		rp->next = NULL;
		print(wq, rp, fq);
		//���ٸý�����Ҫ���е�ʱ��
		(rp->need_time)--;
		//���������е�ʱ��
		(rp->finish_time)++;
		//�����Ҫ����ʱ���Ѿ�Ϊ0�����������Ѿ����������޸�����״̬Ϊ����ɣ���������ӵ�����ɶ���
		if (rp->need_time == 0) {
			rp->process_status = FINISH;
			rp->priority_num = 0;//������ɺ����ȼ�����Ϊ0
			insert(fq, rp);
			rp = NULL;
		}
		//�޸ľ������н��̵ĵȴ�ʱ������ȼ���
		modify_wq(wq);
		//�ٴ���Ѱ�Ƿ��б��������еĽ������ȼ��ߵĽ���
		if (rp != NULL) {
			max_prio = rp;
		}
		else {
			max_prio = wq->queue_head;
		}
		if (is_empty(wq) && rp == NULL) {
			print(wq, rp, fq);
			clear(fq);
			clear(wq);
			break;
		}
	}
}

//�ֲ������� ������ʾ�˵���ʵ�ֽ���
static void show_menu()
{
	printf("****************************************************\n");
	printf("***************** ���̵���ģ�� *********************\n");
	printf("*****************    MENU      *********************\n");
	printf("****************************************************\n");
	printf("**************   a. �̽�������   *******************\n");
	printf("**************   b. ʱ��Ƭ��ת   *******************\n");
	printf("**************   c. ����Ӧ������ *******************\n");
	printf("**************   q. �˳�         *******************\n");
	printf("****************************************************\n");
	printf("����������ѡ��: \n");
}

/**
* brief: ��ʾ�˵��������û�����ִ��
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
extern void process_scheduling(Queue *wq, Queue *fq)
{
	char c;

	show_menu();
	while (1) {
		c = getchar();
		if (isalpha(c)) {
			switch (c)
			{
			case 'a': 
				generate_processes(wq);
				SJF(wq, fq);
				break;
			case 'b':
				generate_processes(wq);
				RR(wq, fq);
				break;
			case 'c':
				generate_processes(wq);
				HRRN(wq, fq);
				break;
			case 'q':
				printf("bye!\n");
				exit(0);
			default: 
				printf("û�����ѡ��\n");
				break;
			}
			show_menu();
		}
	}
}