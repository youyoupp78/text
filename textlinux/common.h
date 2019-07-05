#pragma once


#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <setjmp.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>



/*
typedef struct
{
	int                   etachstate;      //�̵߳ķ���״̬
	int                   schedpolicy;     //�̵߳��Ȳ���
	structsched_param     schedparam;      //�̵߳ĵ��Ȳ���
	int                   inheritsched;    //�̵߳ļ̳���
	int                   scope;           //�̵߳�������
	size_t                guardsize;       //�߳�ջĩβ�ľ��仺������С
	int                   stackaddr_set;   //�̵߳�ջ����
	void*                 stackaddr;       //�߳�ջ��λ��
	size_t                stacksize;       //�߳�ջ�Ĵ�С
}pthread_attr_t;
*/

void sig_handler(int signo);
unsigned int  sig_sleep(unsigned int second);
void forktest();
unsigned int sleep2(unsigned int secondn);

void sig_handler2(unsigned int secondn);

void sleep3();
void printfinfo();
void *thr_print(void *arg);

void *thr_f1(void *arg);

void * thr_f2(void *arg);

struct foo
{
	int f_count;//������
	pthread_mutex_t  f_lock;//������/��
	
};

struct foo * foo_alloc();
void foo_hold(struct foo * fp);
void foo_rela(struct foo * fp);
void * thread_fun1(void * arg);
struct job
{
	job * j_prev;
	job * j_next;
	pthread_t j_id;
};
struct jobqueue
{
	struct job *q_head;
	struct job *q_tail;
	pthread_rwlock_t  q_lock;
};


struct msg
{
	struct msg * m_next;
	/*��Ϣ����*/
};

class JobQueue
{
public:
	JobQueue() ;
	~JobQueue() ;

public:
	int queue_init( );
	/*ͷ�巨*/
	void insert_job(struct job *jp);
	/*β�巨*/
	void append_job( struct job * jp);

	void remove_job( struct job * jp);

	struct job * find_job( pthread_t tid);

public:
	struct jobqueue *qp;
};
