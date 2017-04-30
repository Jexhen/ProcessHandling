#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "process_handling.h"
#include "queue.h"

PCB *g_pb_arr = NULL;//全局动态数组,用于存储有运行请求的进程
int g_length;//全局数组的长度

//私有函数: 将所有进程按照到达时间排队
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

//私有函数: 初始化一个进程，供generate_processes()调用，不向外公开
static void init_process(PCB *p)
{
	static char name = 'a';//自动赋予进程名
	static int cnt = 0;//计数器
	static int flag = 1;//判断是否是第一次全局初始化

	//为了方便各种算法比较，当第一次数组全部元素初始化完成后进程名，到达时间和需要运行时间为恒定值，以后初始化不会改变
	if (flag == 1) {
		p->pcocess_name = name++;
		if (cnt == 1)
			p->arrive_time = 0;
		else
			p->arrive_time = rand() % 13 + 1;
		p->need_time = rand() % 11;
	}
	p->used_time = 0;
	p->process_status = 0;//0吸纳状态未到达,1WAIT等待,2RUN运行中,3FINISH结束
	p->wait_time = 0;
	p->priority_num = 0;
	p->finish_time = -1;//-1表示未完成
	p->to_time = 0;
	p->wto_time = 0;
	p->next = NULL;
	cnt++;
	//如果计数器等于全局数组长度表明数组全部元素已经初始化完毕
	//将标志设为0方便以后重新初始化时不初始化进程名，到达时间和需要运行时间的功能实现
	if (cnt == g_length) {
		flag = 0;
		sort();//全部数组元素初始化好以后根据到达时间进行排序，也只会排序一次
	}
}

//私有函数：输入进程数目函数，供gennerate_process()函数调用
static int input()
{
	int num;
	int c;

	printf("请输入您需要产生的进程数目:\n");
	c = scanf_s("%d", &num);
	if (c <= 0) {
		printf("ERROR: 您输入的不是整数！！！\n");
		exit(-1);
	}

	return num;
}


/**
* brief: 根据用户要求数目批量产生进程，并存入全局动态数组
* param: 无
* return: void
*/
static void generate_processes()
{	
	//为方便比较各种调度算法，全局数组一旦确定不改变
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
* brief: 每次调度实时打印监测
* param: wq就绪队列指针，rp当前运行进程，fq已完成队列指针
* return: int类型的队列长度
*/
static void print(Queue *wq, PCB *rp, Queue *fq)
{
	printf("***************** 正在运行的进程 *******************\n");
	printf("进程名 优先数 到达时间 需要运行时间 已运行时间 进程状态\n");
	if (rp != NULL)
		printf("%3c %8.2f %6d %7d %13d %8d\n", rp->pcocess_name, rp->priority_num,rp->arrive_time,rp->need_time, rp->used_time, rp->process_status);
	printf("\n");

	printf("***************** 就绪队列的进程 *******************\n");
	printf("进程名 优先数 到达时间 需要运行时间 已运行时间 进程状态\n");
	PCB *wp = wq->queue_head;
	while (wp != NULL) {
		printf("%3c %8.2f %6d %7d %13d %8d\n", wp->pcocess_name, wp->priority_num, wp->arrive_time, wp->need_time, wp->used_time, wp->process_status);
		wp = wp->next;
	}
	printf("\n");

	printf("***************** 已经完成的进程 *******************\n");
	printf("进程名 优先数 到达时间 结束时间 需要运行时间 已运行时间 周转时间 带权周转时间 进程状态\n");
	PCB *fp = fq->queue_head;
	while (fp != NULL) {
		printf("%3c %8.2f %6d %6d %10d %13d %9.2f %9.2f %8d\n", fp->pcocess_name,fp->priority_num, fp->arrive_time, fp->finish_time, fp->need_time, fp->used_time, fp->to_time, fp->wto_time, fp->process_status);
		fp = fp->next;
	}
	printf("\n");

	printf("***************** 所有进程PCB信息 *******************\n");
	printf("进程名 优先数 到达时间 结束时间 需要运行时间 已运行时间 周转时间 带权周转时间 进程状态\n");
	for (int i = 0; i < g_length; i++) {
		printf("%3c %8.2f %6d %6d %10d %13d %9.2f %9.2f %8d\n", g_pb_arr[i].pcocess_name, g_pb_arr[i].priority_num, g_pb_arr[i].arrive_time, g_pb_arr[i].finish_time, g_pb_arr[i].need_time, g_pb_arr[i].used_time, g_pb_arr[i].to_time, g_pb_arr[i].wto_time, g_pb_arr[i].process_status);
	}

	printf("\n\n");
}

//内部函数: 用于更新等待序列,当需要运行进程达到当前时间时添加其进入就绪队列并修改相关参数
static void update_wq(Queue* wq, int nowtime)
{
	for (int i = 0; i < g_length; i++) {
		//当数组元素的状态不是三种元素之一并且到达时间比当前时间早时，将其添加进等待队列
		if (g_pb_arr[i].process_status == 0 && g_pb_arr[i].arrive_time <= nowtime) {
			g_pb_arr[i].process_status = WAIT;
			g_pb_arr[i].wait_time = nowtime - g_pb_arr[i].arrive_time;
			g_pb_arr[i].priority_num = (g_pb_arr[i].wait_time + g_pb_arr[i].need_time) * 1.0 / g_pb_arr[i].need_time;
			insert(wq, &(g_pb_arr[i]));
		}
	}
}

//在没有进程运行并且等待队列没有进程时用于查找全局数组是否还有未运行进程
//找到返回true没找到返回false
static bool search(Queue *wq) 
{
	bool flag = false;

	for (int i = 0; i < g_length; i++) {
		if (g_pb_arr[i].process_status == 0) {
			g_pb_arr[i].process_status = WAIT;
			insert(wq, &(g_pb_arr[i]));
			flag = true;
			break;//找到一个立即停止
		}
	}

	return flag;
}

//计算平均周转时间
static double avr_to_time()
{
	double sum = 0;

	for (int i = 0; i < g_length; i++) {
		sum += g_pb_arr[i].to_time;
	}

	return sum / g_length;
}

//计算带权平均周转时间
static double avr_wto_time()
{
	double sum = 0;

	for (int i = 0; i < g_length; i++) {
		sum += g_pb_arr[i].wto_time;
	}

	return sum / g_length;
}

/**
* brief: 短进程优先调度
* para:	wq就绪队列指针，fq已完成队列指针
* return: void
*/
static void SJF(Queue *wq, Queue *fq)
{
	PCB *run_pro = NULL;
	int nowtime = 0;//当前时间初始值为0

	while (1) {
		//如果等待序列不为空找到其中最短运行时间的进程
		if (!is_empty(wq)) {
			PCB *tmp = wq->queue_head;
			PCB *min = wq->queue_head;
			while (tmp != NULL) {
				if (tmp->need_time < min->need_time)
					min = tmp;
				tmp = tmp->next;
			}
			run_pro = min;//开始运行
			delete(wq, min);//从等待队列中删除
		}
		else {
			//等待队列为空，一种情况是全部进程都运行完了，另一种情况就是进程还未到达
			//所以如果等待队列为空,则在提交了运行请求的全局数组中查找是否还有未运行的进程，有说明还有进程未到达
			if (run_pro == NULL) {
				if (search(wq)) {
					//为了模拟，有未到达的进程直接将当前时间快进，并把找到的进程添加到等待队列并投入运行
					nowtime = wq->queue_head->arrive_time;
					run_pro = wq->queue_head;//投入运行
					delete(wq, run_pro);
				}
				else
					break;//如果全局数组也没有需要运行的进程说明全部进程调度完毕结束
			}
		}
		printf("当前时间: %d\n", nowtime);
		run_pro->process_status = RUN;
		print(wq, run_pro, fq);
		nowtime += run_pro->need_time;
		//根据当前时间更新等待队列
		update_wq(wq, nowtime);
		//更新当前进程相关数据
		run_pro->used_time = run_pro->need_time;//已用时间等于需要时间
		run_pro->finish_time = nowtime;//结束时间等于当前时间
		run_pro->process_status = FINISH;//修改状态为结束
		run_pro->to_time = run_pro->finish_time - run_pro->arrive_time;//计算周转时间
		run_pro->wto_time = run_pro->to_time / run_pro->need_time;//计算带权周转时间
		//更新完毕运行结束添加进结束队列，
		insert(fq, run_pro);
		run_pro = NULL;//重置run_pro为空表明释放CPU资源
	}
	print(wq, run_pro, fq);
	clear_queue(fq);
	clear_queue(wq);
	printf("短进程优先调度:\n");
	printf("平均周转时间: %f\t平均带权周转时间: %f\n\n", avr_to_time(), avr_wto_time());
}


/**
* brief: 时间片轮转调度
* para:	wq就绪队列指针，fq已完成队列指针
* return: void
*/
static void RR(Queue *wq, Queue *fq)
{
	PCB *run_pro = NULL;//CPU资源NULL为未占用
	int timeslice = rand() % 6 + 1;//时间片
	int nowtime = 0;//当前时间
	int j;//用于时间片倒计时

	while (1) {
		j = timeslice;
		
		//如果等待队列不为空调入队头元素运行
		if (!is_empty(wq)) {
			run_pro = wq->queue_head;
			delete(wq, run_pro);
		}
		else {
			//等待队列为空，一种情况是全部进程都运行完了，另一种情况就是进程还未到达
			//所以如果等待队列为空,则在提交了运行请求的全局数组中查找是否还有未运行的进程，有说明还有进程未到达
			if (run_pro == NULL) {
				if (search(wq)) {
					//为了模拟，有未到达的进程直接将当前时间快进，并把找到的进程添加到等待队列并投入运行
					nowtime = wq->queue_head->arrive_time;
					run_pro = wq->queue_head;
					delete(wq, run_pro);
				}
				else
					break;//如果全局数组也没有需要运行的进程说明全部进程调度完毕结束
			}
		}
		printf("当前时间: %d\n", nowtime);
		run_pro->process_status = RUN;
		print(wq, run_pro, fq);
		//循环体是时间片内进程的运行j为时间片倒计时
		while (j != 0) {
			nowtime++;//每倒计时一次当前时间加1
			//根据当前时间更新等待队列
			update_wq(wq, nowtime);
			(run_pro->used_time)++;//同时当前进程的已用时间加1
			//判断在时间片内已用时间是否等于需要运行时间，是说明进程运行完毕，对该进程相关属性进行修改，并释放CPU资源
			if (run_pro->used_time == run_pro->need_time) {
				run_pro->finish_time = nowtime;//运行结束时间等于当前时间
				run_pro->process_status = FINISH;//修改状态为结束
				run_pro->to_time = run_pro->finish_time - run_pro->arrive_time;//计算周转时间
				run_pro->wto_time = run_pro->to_time / run_pro->need_time;//计算带权周转时间
				insert(fq, run_pro);//添加到完成队列
				break;
			}
			j--;//时间片倒计时
		}
		//跳出上者时间片倒数的循环有两种情况，一种是进程已经完成，一种是进程未完成
		//如果未完成，将该进程重新添加到就绪队列队尾
		if (run_pro->used_time != run_pro->need_time) {
			run_pro->process_status = WAIT;
			insert(wq, run_pro);
		}
		run_pro = NULL;//释放CPU资源
	}
	print(wq, run_pro, fq);
	clear_queue(fq);
	clear_queue(wq);
	printf("时间片轮转调度(时间片为%d): \n", timeslice);
	printf("平均周转时间: %f\t平均带权周转时间: %f\n\n", avr_to_time(), avr_wto_time());
}

/**
* brief: 高响应比优先调度
* para:	wq就绪队列指针，fq已完成队列指针
* return: void
*/
static void HRRN(Queue *wq, Queue *fq)
{
	PCB *run_pro = NULL;//CPU资源NULL为无占用
	int nowtime = 0;//当前时间

	while (1) {
		//如果等待序列不为空找到其中优先级最高的进程
		if (!is_empty(wq)) {
			//从就绪队列中选择优先数最大的进程
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
			//等待队列为空，一种情况是全部进程都运行完了，另一种情况就是进程还未到达
			//所以如果等待队列为空,则在提交了运行请求的全局数组中查找是否还有未运行的进程，有说明还有进程未到达
			if (run_pro == NULL) {
				if (search(wq)) {
					//为了模拟，有未到达的进程直接将当前时间快进，并把找到的进程添加到等待队列并投入运行
					nowtime = wq->queue_head->arrive_time;
					run_pro = wq->queue_head;
					delete(wq, run_pro);//如果全局数组也没有需要运行的进程说明全部进程调度完毕结束
				}
				else
					break;
			}
		}
		printf("当前时间: %d\n", nowtime);
		run_pro->process_status = RUN;
		print(wq, run_pro, fq);//每调度一次实时打印
		nowtime += run_pro->need_time;//修改当前时间
		//根据当前时间更新等待队列
		update_wq(wq, nowtime);
		run_pro->used_time = run_pro->need_time;//进程结束将已用时间和需要时间相等
		run_pro->finish_time = nowtime;//完成时间等于当前时间
		run_pro->process_status = FINISH;//修改状态为完成
		run_pro->to_time = run_pro->finish_time - run_pro->arrive_time;//计算周转时间
		run_pro->wto_time = run_pro->to_time / run_pro->need_time;//计算带权周转时间
		insert(fq, run_pro);//插入结束队列
		run_pro = NULL;//释放CPU资源
	}
	print(wq, run_pro, fq);
	clear_queue(fq);
	clear_queue(wq);
	printf("高响应比优先调度:\n");
	printf("平均周转时间: %f\t平均带权周转时间: %f\n\n", avr_to_time(), avr_wto_time());
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
* brief: 与用户交互的主界面，进程调度的入口
* para:	wq就绪队列指针，fq已完成队列指针
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
				SJF(wq, fq);//调用段进程优先
				break;
			case 'b':
				generate_processes();
				RR(wq, fq);//调用时间片轮转
				break;
			case 'c':
				generate_processes();
				HRRN(wq, fq);//调用高响应比优先
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

/**
* brief: 析构函数
* para:	Queue 需要销毁的队列
* return: void
*/
extern void destroy(Queue *wq, Queue *fq)
{
	free(fq);
	free(wq);
	free(g_pb_arr);
}