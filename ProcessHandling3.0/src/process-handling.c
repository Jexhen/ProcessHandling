#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "process_handling.h"
#include "queue.h"

PCB *g_pb_arr = NULL;//ȫ�ֶ�̬����,���ڴ洢����������Ľ���
int g_length;//ȫ������ĳ���

//˽�к���: �����н��̰��յ���ʱ���Ŷ�
static void sort()
{
	PCB tmp;

	for (int i = 0; i < g_length; i++) {
		int min = i;
		for (int j = i + 1; j < g_length; j++) {
			if (g_pb_arr[j].arrive_time < g_pb_arr[min].arrive_time) {
				min = j;
			}
		}
		if (min != i) {
			tmp = g_pb_arr[min];
			g_pb_arr[min] = g_pb_arr[i];
			g_pb_arr[i] = tmp;
		}

	}
}

//˽�к���: ��ʼ��һ�����̣���generate_processes()���ã������⹫��
static void init_process(PCB *p)
{
	static char name = 'a';//�Զ����������
	static int cnt = 0;//������
	static int flag = 1;//�ж��Ƿ��ǵ�һ��ȫ�ֳ�ʼ��

	//Ϊ�˷�������㷨�Ƚϣ�����һ������ȫ��Ԫ�س�ʼ����ɺ������������ʱ�����Ҫ����ʱ��Ϊ�㶨ֵ���Ժ��ʼ������ı�
	if (flag == 1) {
		p->pcocess_name = name++;
		if (cnt == 1)
			p->arrive_time = 0;
		else
			p->arrive_time = rand() % 13 + 1;
		p->need_time = rand() % 11;
	}
	p->used_time = 0;
	p->process_status = 0;//0����״̬δ����,1WAIT�ȴ�,2RUN������,3FINISH����
	p->wait_time = 0;
	p->priority_num = 0;
	p->finish_time = -1;//-1��ʾδ���
	p->to_time = 0;
	p->wto_time = 0;
	p->next = NULL;
	cnt++;
	//�������������ȫ�����鳤�ȱ�������ȫ��Ԫ���Ѿ���ʼ�����
	//����־��Ϊ0�����Ժ����³�ʼ��ʱ����ʼ��������������ʱ�����Ҫ����ʱ��Ĺ���ʵ��
	if (cnt == g_length) {
		flag = 0;
		sort();//ȫ������Ԫ�س�ʼ�����Ժ���ݵ���ʱ���������Ҳֻ������һ��
	}
}

//˽�к��������������Ŀ��������gennerate_process()��������
static int input()
{
	int num;
	int c;

	printf("����������Ҫ�����Ľ�����Ŀ:\n");
	c = scanf_s("%d", &num);
	if (c <= 0) {
		printf("ERROR: ������Ĳ�������������\n");
		exit(-1);
	}

	return num;
}


/**
* brief: �����û�Ҫ����Ŀ�����������̣�������ȫ�ֶ�̬����
* param: ��
* return: void
*/
static void generate_processes()
{	
	//Ϊ����Ƚϸ��ֵ����㷨��ȫ������һ��ȷ�����ı�
	if (g_pb_arr == NULL) {
		int num = input();
		g_length = num;
		g_pb_arr = (PCB*)malloc(g_length * sizeof(PCB));
	}
	for (int i = 0; i < g_length; i++) {
		init_process(&(g_pb_arr[i]));
	}
}


/**
* brief: ÿ�ε���ʵʱ��ӡ���
* param: wq��������ָ�룬rp��ǰ���н��̣�fq����ɶ���ָ��
* return: int���͵Ķ��г���
*/
static void print(Queue *wq, PCB *rp, Queue *fq)
{
	printf("***************** �������еĽ��� *******************\n");
	printf("������ ������ ����ʱ�� ��Ҫ����ʱ�� ������ʱ�� ����״̬\n");
	if (rp != NULL)
		printf("%3c %8.2f %6d %7d %13d %8d\n", rp->pcocess_name, rp->priority_num,rp->arrive_time,rp->need_time, rp->used_time, rp->process_status);
	printf("\n");

	printf("***************** �������еĽ��� *******************\n");
	printf("������ ������ ����ʱ�� ��Ҫ����ʱ�� ������ʱ�� ����״̬\n");
	PCB *wp = wq->queue_head;
	while (wp != NULL) {
		printf("%3c %8.2f %6d %7d %13d %8d\n", wp->pcocess_name, wp->priority_num, wp->arrive_time, wp->need_time, wp->used_time, wp->process_status);
		wp = wp->next;
	}
	printf("\n");

	printf("***************** �Ѿ���ɵĽ��� *******************\n");
	printf("������ ������ ����ʱ�� ����ʱ�� ��Ҫ����ʱ�� ������ʱ�� ��תʱ�� ��Ȩ��תʱ�� ����״̬\n");
	PCB *fp = fq->queue_head;
	while (fp != NULL) {
		printf("%3c %8.2f %6d %6d %10d %13d %9.2f %9.2f %8d\n", fp->pcocess_name,fp->priority_num, fp->arrive_time, fp->finish_time, fp->need_time, fp->used_time, fp->to_time, fp->wto_time, fp->process_status);
		fp = fp->next;
	}
	printf("\n");

	printf("***************** ���н���PCB��Ϣ *******************\n");
	printf("������ ������ ����ʱ�� ����ʱ�� ��Ҫ����ʱ�� ������ʱ�� ��תʱ�� ��Ȩ��תʱ�� ����״̬\n");
	for (int i = 0; i < g_length; i++) {
		printf("%3c %8.2f %6d %6d %10d %13d %9.2f %9.2f %8d\n", g_pb_arr[i].pcocess_name, g_pb_arr[i].priority_num, g_pb_arr[i].arrive_time, g_pb_arr[i].finish_time, g_pb_arr[i].need_time, g_pb_arr[i].used_time, g_pb_arr[i].to_time, g_pb_arr[i].wto_time, g_pb_arr[i].process_status);
	}

	printf("\n\n");
}

//�ڲ�����: ���ڸ��µȴ�����,����Ҫ���н��̴ﵽ��ǰʱ��ʱ��������������в��޸���ز���
static void update_wq(Queue* wq, int nowtime)
{
	for (int i = 0; i < g_length; i++) {
		//������Ԫ�ص�״̬��������Ԫ��֮һ���ҵ���ʱ��ȵ�ǰʱ����ʱ��������ӽ��ȴ�����
		if (g_pb_arr[i].process_status == 0 && g_pb_arr[i].arrive_time <= nowtime) {
			g_pb_arr[i].process_status = WAIT;
			g_pb_arr[i].wait_time = nowtime - g_pb_arr[i].arrive_time;
			g_pb_arr[i].priority_num = (g_pb_arr[i].wait_time + g_pb_arr[i].need_time) * 1.0 / g_pb_arr[i].need_time;
			insert(wq, &(g_pb_arr[i]));
		}
	}
}

//��û�н������в��ҵȴ�����û�н���ʱ���ڲ���ȫ�������Ƿ���δ���н���
//�ҵ�����trueû�ҵ�����false
static bool search(Queue *wq) 
{
	bool flag = false;

	for (int i = 0; i < g_length; i++) {
		if (g_pb_arr[i].process_status == 0) {
			g_pb_arr[i].process_status = WAIT;
			insert(wq, &(g_pb_arr[i]));
			flag = true;
			break;//�ҵ�һ������ֹͣ
		}
	}

	return flag;
}

//����ƽ����תʱ��
static double avr_to_time()
{
	double sum = 0;

	for (int i = 0; i < g_length; i++) {
		sum += g_pb_arr[i].to_time;
	}

	return sum / g_length;
}

//�����Ȩƽ����תʱ��
static double avr_wto_time()
{
	double sum = 0;

	for (int i = 0; i < g_length; i++) {
		sum += g_pb_arr[i].wto_time;
	}

	return sum / g_length;
}

/**
* brief: �̽������ȵ���
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
static void SJF(Queue *wq, Queue *fq)
{
	PCB *run_pro = NULL;
	int nowtime = 0;//��ǰʱ���ʼֵΪ0

	while (1) {
		//����ȴ����в�Ϊ���ҵ������������ʱ��Ľ���
		if (!is_empty(wq)) {
			PCB *tmp = wq->queue_head;
			PCB *min = wq->queue_head;
			while (tmp != NULL) {
				if (tmp->need_time < min->need_time)
					min = tmp;
				tmp = tmp->next;
			}
			run_pro = min;//��ʼ����
			delete(wq, min);//�ӵȴ�������ɾ��
		}
		else {
			//�ȴ�����Ϊ�գ�һ�������ȫ�����̶��������ˣ���һ��������ǽ��̻�δ����
			//��������ȴ�����Ϊ��,�����ύ�����������ȫ�������в����Ƿ���δ���еĽ��̣���˵�����н���δ����
			if (run_pro == NULL) {
				if (search(wq)) {
					//Ϊ��ģ�⣬��δ����Ľ���ֱ�ӽ���ǰʱ�����������ҵ��Ľ�����ӵ��ȴ����в�Ͷ������
					nowtime = wq->queue_head->arrive_time;
					run_pro = wq->queue_head;//Ͷ������
					delete(wq, run_pro);
				}
				else
					break;//���ȫ������Ҳû����Ҫ���еĽ���˵��ȫ�����̵�����Ͻ���
			}
		}
		printf("��ǰʱ��: %d\n", nowtime);
		run_pro->process_status = RUN;
		print(wq, run_pro, fq);
		nowtime += run_pro->need_time;
		//���ݵ�ǰʱ����µȴ�����
		update_wq(wq, nowtime);
		//���µ�ǰ�����������
		run_pro->used_time = run_pro->need_time;//����ʱ�������Ҫʱ��
		run_pro->finish_time = nowtime;//����ʱ����ڵ�ǰʱ��
		run_pro->process_status = FINISH;//�޸�״̬Ϊ����
		run_pro->to_time = run_pro->finish_time - run_pro->arrive_time;//������תʱ��
		run_pro->wto_time = run_pro->to_time / run_pro->need_time;//�����Ȩ��תʱ��
		//����������н�����ӽ��������У�
		insert(fq, run_pro);
		run_pro = NULL;//����run_proΪ�ձ����ͷ�CPU��Դ
	}
	print(wq, run_pro, fq);
	clear_queue(fq);
	clear_queue(wq);
	printf("�̽������ȵ���:\n");
	printf("ƽ����תʱ��: %f\tƽ����Ȩ��תʱ��: %f\n\n", avr_to_time(), avr_wto_time());
}


/**
* brief: ʱ��Ƭ��ת����
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
static void RR(Queue *wq, Queue *fq)
{
	PCB *run_pro = NULL;//CPU��ԴNULLΪδռ��
	int timeslice = rand() % 6 + 1;//ʱ��Ƭ
	int nowtime = 0;//��ǰʱ��
	int j;//����ʱ��Ƭ����ʱ

	while (1) {
		j = timeslice;
		
		//����ȴ����в�Ϊ�յ����ͷԪ������
		if (!is_empty(wq)) {
			run_pro = wq->queue_head;
			delete(wq, run_pro);
		}
		else {
			//�ȴ�����Ϊ�գ�һ�������ȫ�����̶��������ˣ���һ��������ǽ��̻�δ����
			//��������ȴ�����Ϊ��,�����ύ�����������ȫ�������в����Ƿ���δ���еĽ��̣���˵�����н���δ����
			if (run_pro == NULL) {
				if (search(wq)) {
					//Ϊ��ģ�⣬��δ����Ľ���ֱ�ӽ���ǰʱ�����������ҵ��Ľ�����ӵ��ȴ����в�Ͷ������
					nowtime = wq->queue_head->arrive_time;
					run_pro = wq->queue_head;
					delete(wq, run_pro);
				}
				else
					break;//���ȫ������Ҳû����Ҫ���еĽ���˵��ȫ�����̵�����Ͻ���
			}
		}
		printf("��ǰʱ��: %d\n", nowtime);
		run_pro->process_status = RUN;
		print(wq, run_pro, fq);
		//ѭ������ʱ��Ƭ�ڽ��̵�����jΪʱ��Ƭ����ʱ
		while (j != 0) {
			nowtime++;//ÿ����ʱһ�ε�ǰʱ���1
			//���ݵ�ǰʱ����µȴ�����
			update_wq(wq, nowtime);
			(run_pro->used_time)++;//ͬʱ��ǰ���̵�����ʱ���1
			//�ж���ʱ��Ƭ������ʱ���Ƿ������Ҫ����ʱ�䣬��˵������������ϣ��Ըý���������Խ����޸ģ����ͷ�CPU��Դ
			if (run_pro->used_time == run_pro->need_time) {
				run_pro->finish_time = nowtime;//���н���ʱ����ڵ�ǰʱ��
				run_pro->process_status = FINISH;//�޸�״̬Ϊ����
				run_pro->to_time = run_pro->finish_time - run_pro->arrive_time;//������תʱ��
				run_pro->wto_time = run_pro->to_time / run_pro->need_time;//�����Ȩ��תʱ��
				insert(fq, run_pro);//��ӵ���ɶ���
				break;
			}
			j--;//ʱ��Ƭ����ʱ
		}
		//��������ʱ��Ƭ������ѭ�������������һ���ǽ����Ѿ���ɣ�һ���ǽ���δ���
		//���δ��ɣ����ý���������ӵ��������ж�β
		if (run_pro->used_time != run_pro->need_time) {
			run_pro->process_status = WAIT;
			insert(wq, run_pro);
		}
		run_pro = NULL;//�ͷ�CPU��Դ
	}
	print(wq, run_pro, fq);
	clear_queue(fq);
	clear_queue(wq);
	printf("ʱ��Ƭ��ת����(ʱ��ƬΪ%d): \n", timeslice);
	printf("ƽ����תʱ��: %f\tƽ����Ȩ��תʱ��: %f\n\n", avr_to_time(), avr_wto_time());
}

/**
* brief: ����Ӧ�����ȵ���
* para:	wq��������ָ�룬fq����ɶ���ָ��
* return: void
*/
static void HRRN(Queue *wq, Queue *fq)
{
	PCB *run_pro = NULL;//CPU��ԴNULLΪ��ռ��
	int nowtime = 0;//��ǰʱ��

	while (1) {
		//����ȴ����в�Ϊ���ҵ��������ȼ���ߵĽ���
		if (!is_empty(wq)) {
			//�Ӿ���������ѡ�����������Ľ���
			PCB *tmp = wq->queue_head;
			PCB *max_prio = wq->queue_head;
			while (tmp != NULL) {
				if (tmp->priority_num > max_prio->priority_num) {
					max_prio = tmp;
				}
				tmp = tmp->next;
			}
			run_pro = max_prio;
			delete(wq, max_prio);
		}
		else {
			//�ȴ�����Ϊ�գ�һ�������ȫ�����̶��������ˣ���һ��������ǽ��̻�δ����
			//��������ȴ�����Ϊ��,�����ύ�����������ȫ�������в����Ƿ���δ���еĽ��̣���˵�����н���δ����
			if (run_pro == NULL) {
				if (search(wq)) {
					//Ϊ��ģ�⣬��δ����Ľ���ֱ�ӽ���ǰʱ�����������ҵ��Ľ�����ӵ��ȴ����в�Ͷ������
					nowtime = wq->queue_head->arrive_time;
					run_pro = wq->queue_head;
					delete(wq, run_pro);//���ȫ������Ҳû����Ҫ���еĽ���˵��ȫ�����̵�����Ͻ���
				}
				else
					break;
			}
		}
		printf("��ǰʱ��: %d\n", nowtime);
		run_pro->process_status = RUN;
		print(wq, run_pro, fq);//ÿ����һ��ʵʱ��ӡ
		nowtime += run_pro->need_time;//�޸ĵ�ǰʱ��
		//���ݵ�ǰʱ����µȴ�����
		update_wq(wq, nowtime);
		run_pro->used_time = run_pro->need_time;//���̽���������ʱ�����Ҫʱ�����
		run_pro->finish_time = nowtime;//���ʱ����ڵ�ǰʱ��
		run_pro->process_status = FINISH;//�޸�״̬Ϊ���
		run_pro->to_time = run_pro->finish_time - run_pro->arrive_time;//������תʱ��
		run_pro->wto_time = run_pro->to_time / run_pro->need_time;//�����Ȩ��תʱ��
		insert(fq, run_pro);//�����������
		run_pro = NULL;//�ͷ�CPU��Դ
	}
	print(wq, run_pro, fq);
	clear_queue(fq);
	clear_queue(wq);
	printf("����Ӧ�����ȵ���:\n");
	printf("ƽ����תʱ��: %f\tƽ����Ȩ��תʱ��: %f\n\n", avr_to_time(), avr_wto_time());
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
* brief: ���û������������棬���̵��ȵ����
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
				generate_processes();
				SJF(wq, fq);//���öν�������
				break;
			case 'b':
				generate_processes();
				RR(wq, fq);//����ʱ��Ƭ��ת
				break;
			case 'c':
				generate_processes();
				HRRN(wq, fq);//���ø���Ӧ������
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

/**
* brief: ��������
* para:	Queue ��Ҫ���ٵĶ���
* return: void
*/
extern void destroy(Queue *wq, Queue *fq)
{
	free(fq);
	free(wq);
	free(g_pb_arr);
}