#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "process_handling.h"
#include "queue.h"

PCB **pcb_arr;
int length;
//内部函数: 用于产生一个进程，供generate_processes()调用，不向外公开
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
* brief: 内部函数：用户输入进程数目
* para:	无
* return: 用户要求产生的进程数目
*/
static int input()
{
	int num;
	int c;

	printf("请输入您需要产生的进程数目:\n");
	c = scanf_s("%d", &num, 1);
	if (c <= 0) {
		printf("ERROR: 您输入的不是整数！！！\n");
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
* brief: 批量产生进程，并插入就绪队列
* para:	wq就绪队列指针
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
* brief: 每次调度实时监测打印
* para:	wq就绪队列指针，rp当前运行进程，fq已完成队列指针
* return: int类型的队列长度
*/
extern void print(Queue *wq, PCB *rp, Queue *fq)
{
	printf("***************** 正在运行的进程 *******************\n");
	printf("进程名 优先数 到达时间 需要运行时间 已运行时间 进程状态\n");
	if (rp != NULL)
		printf("%3c %8.2f %6d %7d %13d %8d\n", rp->pcocess_name, rp->priority_num,rp->arrive_time,rp->need_time, rp->finish_time, rp->process_status);
	printf("\n");

	printf("***************** 就绪队列的进程 *******************\n");
	printf("进程名 优先数 到达时间 需要运行时间 已运行时间 进程状态\n");
	PCB *wp = wq->queue_head;
	while (wp != NULL) {
		printf("%3c %8.2f %6d %7d %13d %8d\n", wp->pcocess_name, wp->priority_num, wp->arrive_time, wp->need_time, wp->finish_time, wp->process_status);
		wp = wp->next;
	}
	printf("\n");

	printf("***************** 已经完成的进程 *******************\n");
	printf("进程名 优先数 到达时间 需要运行时间 已运行时间 进程状态\n");
	PCB *fp = fq->queue_head;
	while (fp != NULL) {
		printf("%3c %8.2f %6d %7d %13d %8d\n", fp->pcocess_name,fp->priority_num, fp->arrive_time, fp->need_time, fp->finish_time, fp->process_status);
		fp = fp->next;
	}
	printf("\n\n");
}

//内部函数: 用于更新就绪队列的等待时间和优先级，不向外公开
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
* brief: 短进程优先调度
* para:	wq就绪队列指针，fq已完成队列指针
* return: void
*/
extern void SJF(Queue *wq, Queue *fq)
{
	PCB *rp = NULL;//表示当前运行进程，初始值为空，无调度任何进程
	int cnt = 0;//用于统计进程数
	double nowtime = 0;//表示当前时间
	double sum_TR = 0;
	double sum_WTR = 0;
	
	//采用非抢占式的短进程优先方式
	while (1) {
		nowtime++;
		//rp为空表明没有正在运行的进程，rp的需要运行时间为0表明该进程已经运行完毕
		//以上两种情况都要重新在就绪队列进行调度
		if (rp == NULL || rp->need_time == 0) {
			// 遍历队列的每个元素，找到需要运行时间最短的进程进行调度
			PCB *min = wq->queue_head;
			PCB *tmp = wq->queue_head->next;
			while (tmp != NULL) {
				if (tmp->need_time < min->need_time) {
					min = tmp;
				}
				tmp = tmp->next;
			}
			delete(wq, min);//在就绪队列对被调度的进程进行删除操作
			rp = min;//表明进程已经被处理机调度进入运行状态
			rp->process_status = RUN;//修改进程状态为正在运行
			rp->next = NULL;
			print(wq, rp, fq);
		}
		//正在运行的进程的需要运行时间减少
		(rp->need_time)--;
		//正在运行的进程的已经运行时间增加
		(rp->finish_time)++;
		//测试正在运行的进程是否已经完成，已经完成将其添加到已完成进程队列
		if (rp->need_time == 0) {
			rp->process_status = FINISH;
			insert(fq, rp);
			sum_TR = sum_TR + (nowtime - rp->arrive_time);
			sum_WTR = sum_WTR + ((nowtime - rp->arrive_time) / rp->need_time);
			cnt++;
			rp = NULL;//将处理机重新设置为空闲状态
		}
		//更新就绪队列进程的等待时间和优先数
		modify_wq(wq);
		if (is_empty(wq) && rp == NULL) {
			print(wq, rp, fq);
			/*fq->length = 0;*/
			clear_queue(fq);
			clear_queue(wq);
			break;
		}
	}
	printf("***************** 短进程优先调度 *******************\n");
	printf("平均周转时间: %f\n", sum_TR / cnt);
	printf("平均带权周转时间： %lf\n", sum_WTR / cnt);
	printf("\n\n");
}

//内部函数: 用于辅助RR函数将就绪队列的进程的运行时间以时间片为单位
static void make_timeslice(Queue *wq, int timeslice)
{
	PCB *tmp = wq->queue_head;

	while (tmp != NULL) {
		//如果运行时间可以整除时间片，则取运行时间与时间片的商
		//否则，则将她们的商向上取整
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
* brief: 时间片轮转调度
* para:	wq就绪队列指针，fq已完成队列指针
* return: void
*/
extern void RR(Queue *wq, Queue *fq)
{
	PCB *rp = NULL;
	int  timeslice = 4;//时间片

	make_timeslice(wq, timeslice);//将就绪队列的进程的运行时间变为以时间片为单位
	while (1) {
		//因为每个时间片过后，如果就绪队列仍有进程会把rp置为NULL
		//所以只有当rp为NULL并且就绪队列有进程等待时才进行调度
		//否则只运行当前进程
		if (rp == NULL && !(is_empty(wq))) {
			rp = wq->queue_head;//每次调度就绪队列队首进程
			delete(wq, rp);//将被调度进程从就绪队列中删除
			rp->process_status = RUN;//将其状态更改为运行
			rp->next = NULL;
			print(wq, rp, fq);//调度一次打印
		}
		timeslice--;//时间片自减，减完表明时间片用完
		if (timeslice == 0) {
			//当timeslice等于0时运行进程的需要运行时间减一个单位
			(rp->need_time)--;
			//已完成时间加一个单位
			(rp->finish_time)++;
			//就绪队列的所有进程等待时间加一个时间单位和更新优先数
			modify_wq(wq);
			timeslice = 4;//时间片用完后恢复初始状态
		}
		//需要运行时间为0修改进程状态为结束，添加到以已结束队列，进行下一次调度
		if (rp->need_time == 0) {
			rp->process_status = FINISH;
			insert(fq, rp);
			rp = NULL;
		}
		else if (!(is_empty(wq))) {
			//如果没有运行完并且就绪队列还有元素才将其进程状态设置为等待，重新插入到就绪队列队尾
			rp->process_status = WAIT;
			insert(wq, rp);
			//将处理机设置为空闲状态方便调度其他进程
			rp = NULL;
		}
			
		//当就绪队列为空并且无进程占着处理机时停止调度
		//当只有一个进程被调度时虽然就绪队列是空的但是还是有进程正在运行，不能停止程序
		//只有当就绪队列是空的，也没有进程在运行时才能停止程序
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
* brief: 高响应比优先调度
* para:	wq就绪队列指针，fq已完成队列指针
* return: void
*/
extern void HRRN(Queue *wq, Queue *fq)
{
	PCB *rp = NULL;
	PCB *max_prio = wq->queue_head;

	while (1) {
		//从就绪队列中选择优先数最大的进程
		PCB *tmp = wq->queue_head;
		while (tmp != NULL) {
			if (tmp->priority_num > max_prio->priority_num) {
				max_prio = tmp;
			}
			tmp = tmp->next;
		}
		//如果无进程运行或者就绪队列优先级比当前运行进程高时才进行调度
		if (rp != max_prio) {
			//如有进行的进程将其状态修改为等待，并将其重新插入到就绪队列队尾
			if (rp != NULL) {
				rp->process_status = WAIT;
				insert(wq, rp);
			}
			//将高优先级的进程从就绪队列中删除
			delete(wq, max_prio);
			//将该进程添加到处理机运行
			rp = max_prio;
			//将该进程状态修改为运行
			rp->process_status = RUN;
			rp->next = NULL;
			print(wq, rp, fq);
		}
		//减少该进程需要运行的时间
		(rp->need_time)--;
		//增加已运行的时间
		(rp->finish_time)++;
		//如果需要运行时间已经为0，表明进程已经结束，则将修改运行状态为已完成，并将其添加到已完成队列
		if (rp->need_time == 0) {
			rp->process_status = FINISH;
			rp->priority_num = 0;//运行完成后优先级数置为0
			insert(fq, rp);
			rp = NULL;
		}
		//修改就绪队列进程的等待时间和优先级数
		modify_wq(wq);
		//再次搜寻是否有比正在运行的进程优先级高的进程
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

//局部函数： 用于显示菜单，实现交互
static void show_menu()
{
	printf("****************************************************\n");
	printf("***************** 进程调度模拟 *********************\n");
	printf("*****************    MENU      *********************\n");
	printf("****************************************************\n");
	printf("**************   a. 短进程优先   *******************\n");
	printf("**************   b. 时间片轮转   *******************\n");
	printf("**************   c. 高响应比优先 *******************\n");
	printf("**************   q. 退出         *******************\n");
	printf("****************************************************\n");
	printf("请输入您的选项: \n");
}

/**
* brief: 显示菜单并根据用户输入执行
* para:	wq就绪队列指针，fq已完成队列指针
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
				printf("没有这个选项\n");
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
* brief: 析构函数
* para:	Queue 需要销毁的队列
* return: void
*/
extern void destroy(Queue *wq, Queue *fq)
{
	free(fq);
	free(wq);
	clear_pcb_arr(pcb_arr);
}