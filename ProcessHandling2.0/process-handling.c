#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "process_handling.h"
#include "queue.h"

PCB **pcb_arr;
int length;
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

static void sort(PCB **pa, int len)
{
	PCB *tmp;

	for (int i = 0; i < len; i++) {
		PCB *min = pa[i];
		for (int j = i + 1; j < length; j++) {
			if (pa[j]->arrive_time < min->arrive_time) {
				tmp = pa[j];
				pa[j] = pa[i];
				pa[i] = tmp;
			}
		}
		
	}
}

/**
* brief: �����������̣��������������
* para:	wq��������ָ��
* return: void
*/
extern void generate_processes()
{
	int num = input();
	
	length = num;
	pcb_arr = (PCB*)malloc(length * sizeof(PCB));
	for (int i = 0; i < length; i++) {
		PCB *p = generate_process();
		pcb_arr[i] = p;
	}
	sort(pcb_arr, length);
}

static PCB *add_to_queue(Queue *wq)
{
	PCB *tmp_pcb = (PCB*)malloc(length * sizeof(PCB));
	for (int i = 0; i < length; i++) {
		tmp_pcb[i] = *pcb_arr[i];
		insert(wq, &(tmp_pcb[i]));
	}
	return tmp_pcb;
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
	int cnt = 0;//����ͳ�ƽ�����
	double nowtime = 0;//��ʾ��ǰʱ��
	double sum_TR = 0;
	double sum_WTR = 0;
	
	//���÷���ռʽ�Ķ̽������ȷ�ʽ
	while (1) {
		nowtime++;
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
			rp = min;//���������Ѿ������������Ƚ�������״̬
			rp->process_status = RUN;//�޸Ľ���״̬Ϊ��������
			rp->next = NULL;
			print(wq, rp, fq);
		}
		//�������еĽ��̵���Ҫ����ʱ�����
		(rp->need_time)--;
		//�������еĽ��̵��Ѿ�����ʱ������
		(rp->finish_time)++;
		//�����������еĽ����Ƿ��Ѿ���ɣ��Ѿ���ɽ������ӵ�����ɽ��̶���
		if (rp->need_time == 0) {
			rp->process_status = FINISH;
			insert(fq, rp);
			sum_TR = sum_TR + (nowtime - rp->arrive_time);
			sum_WTR = sum_WTR + ((nowtime - rp->arrive_time) / rp->need_time);
			cnt++;
			rp = NULL;//����������������Ϊ����״̬
		}
		//���¾������н��̵ĵȴ�ʱ���������
		modify_wq(wq);
		if (is_empty(wq) && rp == NULL) {
			print(wq, rp, fq);
			/*fq->length = 0;*/
			clear_queue(fq);
			clear_queue(wq);
			break;
		}
	}
	printf("***************** �̽������ȵ��� *******************\n");
	printf("ƽ����תʱ��: %f\n", sum_TR / cnt);
	printf("ƽ����Ȩ��תʱ�䣺 %lf\n", sum_WTR / cnt);
	printf("\n\n");
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
		//��Ϊÿ��ʱ��Ƭ������������������н��̻��rp��ΪNULL
		//����ֻ�е�rpΪNULL���Ҿ��������н��̵ȴ�ʱ�Ž��е���
		//����ֻ���е�ǰ����
		if (rp == NULL && !(is_empty(wq))) {
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
			//�������е����н��̵ȴ�ʱ���һ��ʱ�䵥λ�͸���������
			modify_wq(wq);
			timeslice = 4;//ʱ��Ƭ�����ָ���ʼ״̬
		}
		//��Ҫ����ʱ��Ϊ0�޸Ľ���״̬Ϊ���������ӵ����ѽ������У�������һ�ε���
		if (rp->need_time == 0) {
			rp->process_status = FINISH;
			insert(fq, rp);
			rp = NULL;
		}
		else if (!(is_empty(wq))) {
			//���û�������겢�Ҿ������л���Ԫ�زŽ������״̬����Ϊ�ȴ������²��뵽�������ж�β
			rp->process_status = WAIT;
			insert(wq, rp);
			//������������Ϊ����״̬���������������
			rp = NULL;
		}
			
		//����������Ϊ�ղ����޽���ռ�Ŵ�����ʱֹͣ����
		//��ֻ��һ�����̱�����ʱ��Ȼ���������ǿյĵ��ǻ����н����������У�����ֹͣ����
		//ֻ�е����������ǿյģ�Ҳû�н���������ʱ����ֹͣ����
		if (is_empty(wq) && rp == NULL) {
			print(wq, rp, fq);
			/*fq->length = 0;*/
			clear_queue(fq);
			clear_queue(wq);
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
		//����޽������л��߾����������ȼ��ȵ�ǰ���н��̸�ʱ�Ž��е���
		if (rp != max_prio) {
			//���н��еĽ��̽���״̬�޸�Ϊ�ȴ������������²��뵽�������ж�β
			if (rp != NULL) {
				rp->process_status = WAIT;
				insert(wq, rp);
			}
			//�������ȼ��Ľ��̴Ӿ���������ɾ��
			delete(wq, max_prio);
			//���ý������ӵ�����������
			rp = max_prio;
			//���ý���״̬�޸�Ϊ����
			rp->process_status = RUN;
			rp->next = NULL;
			print(wq, rp, fq);
		}
		//���ٸý�����Ҫ���е�ʱ��
		(rp->need_time)--;
		//���������е�ʱ��
		(rp->finish_time)++;
		//�����Ҫ����ʱ���Ѿ�Ϊ0�����������Ѿ����������޸�����״̬Ϊ����ɣ����������ӵ�����ɶ���
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
		else if (!(is_empty(wq))){
			max_prio = wq->queue_head;
		}
		else {
			print(wq, rp, fq);
			/*fq->length = 0;*/
			clear_queue(fq);
			clear_queue(wq);
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
	

	generate_processes();
	show_menu();
	while (1) {
		PCB *p;
		c = getchar();
		if (isalpha(c)) {
			switch (c)
			{
			case 'a': 
				p = add_to_queue(wq);
				SJF(wq, fq);
				free(p);
				break;
			case 'b':
				p = add_to_queue(wq);
				RR(wq, fq);
				free(p);
				break;
			case 'c':
				p = add_to_queue(wq);
				HRRN(wq, fq);
				free(p);
				break;
			case 'q':
				printf("bye!\n");
				return;
			default: 
				printf("û�����ѡ��\n");
				break;
			}
			show_menu();
		}
	}
}


static void clear_pcb_arr(PCB **p)
{
	for (int i = 0; i < length; i++) {
		free(p[i]);
	}
	free(p);
}

/**
* brief: ��������
* para:	Queue ��Ҫ���ٵĶ���
* return: void
*/
extern void destroy(Queue *wq, Queue *fq)
{
	free(fq);
	free(wq);
	clear_pcb_arr(pcb_arr);
}