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
	int                   etachstate;      //线程的分离状态
	int                   schedpolicy;     //线程调度策略
	structsched_param     schedparam;      //线程的调度参数
	int                   inheritsched;    //线程的继承性
	int                   scope;           //线程的作用域
	size_t                guardsize;       //线程栈末尾的警戒缓冲区大小
	int                   stackaddr_set;   //线程的栈设置
	void*                 stackaddr;       //线程栈的位置
	size_t                stacksize;       //线程栈的大小
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
	int f_count;//计数器
	pthread_mutex_t  f_lock;//互斥量/锁
	
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
	/*消息队列*/
};

class JobQueue
{
public:
	JobQueue() ;
	~JobQueue() ;

public:
	int queue_init( );
	/*头插法*/
	void insert_job(struct job *jp);
	/*尾插法*/
	void append_job( struct job * jp);

	void remove_job( struct job * jp);

	struct job * find_job( pthread_t tid);

public:
	struct jobqueue *qp;
};
