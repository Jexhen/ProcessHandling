#ifndef _PROCESS_SCHEDULING_H_
#define _PROCESS_SCHEDULING_H_
#include "Type.h"
#define WAIT 1//进程状态：就绪wait
#define RUN 2//进程状态：运行run
#define FINISH 3//进程状态：结束finish

/************ 公共接口 ****************/

/**
* brief: 批量产生进程，并插入就绪队列
* para:	wq就绪队列指针
* return: void
*/
extern void generate_processes();

/**
* brief: 每次调度实时监测打印
* para:	wq就绪队列指针，rp当前运行进程，fq已完成队列指针
* return: int类型的队列长度
*/
extern void print(Queue *wq, PCB *rp, Queue * fq);

/**
* brief: 与用户交互的主界面，进程调度的入口
* para:	wq就绪队列指针，fq已完成队列指针
* return: void
*/
extern void process_scheduling(Queue *wq, Queue *fq);

/**
* brief: 析构函数
* para:	Queue 需要销毁的队列
* return: void
*/
extern void destroy(Queue *wq, Queue *fq);

#endif // !PROCESS_SCHEDULING_H

