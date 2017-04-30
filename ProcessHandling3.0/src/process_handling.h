#ifndef _PROCESS_SCHEDULING_H_
#define _PROCESS_SCHEDULING_H_
#include "Type.h"
#define WAIT 1//进程状态：就绪wait
#define RUN 2//进程状态：运行run
#define FINISH 3//进程状态：结束finish

/************ 公共接口 ****************/

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

