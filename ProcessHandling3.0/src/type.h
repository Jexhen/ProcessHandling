#ifndef _TYPE_H_
#define _TYPE_H_

/************ ���̿��ƿ� ****************/
struct PCB_ {
	char pcocess_name;//��������	
	double priority_num;//���ȼ���
	int arrive_time;//����ʱ��
	int need_time;//��Ҫ���е�ʱ��
	int used_time;//�����е�ʱ��
	int wait_time;//�ȴ�ʱ��
	int finish_time;//���н���ʱ��
	int process_status;//����״̬
	double to_time;//��תʱ��
	double wto_time;//��Ȩ��תʱ��
	struct PCB_ *next;//ָ����һ��Ԫ�ص�ָ��
};
typedef struct PCB_ PCB;

/************ ���� ****************/
struct Queue_ {
	PCB *queue_head;//��ͷָ��
	PCB *queue_tail;//��βָ��
	int length;//���г���
};
typedef struct Queue_ Queue;

#endif // !_TYPE_H_
