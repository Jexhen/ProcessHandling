#ifndef _TYPE_H_
#define _TYPE_H_

/************ 进程控制块 ****************/
struct PCB_ {
	char pcocess_name;//进程名字	
	double priority_num;//优先级数
	int arrive_time;//到达时间
	int need_time;//需要运行的时间
	int finish_time;//已运行的时间
	int wait_time;//等待时间
	int process_status;//进程状态
	struct PCB_ *next;//指向下一个元素的指针
};
typedef struct PCB_ PCB;

/************ 队列 ****************/
struct Queue_ {
	PCB *queue_head;//队头指针
	PCB *queue_tail;//队尾指针
	int length;//队列长度
};
typedef struct Queue_ Queue;

#endif // !_TYPE_H_
