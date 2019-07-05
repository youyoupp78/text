#include "common.h"
#include <iostream>

using namespace std;

void exit_status1()
{
	cout << "exit1" << endl;
}

void exist_status2()
{
	cout << "exit2" << endl;
}
void registerExit()
{
	if (atexit(exit_status1) != 0)
	{
		cout << "registe exit1 fail" << endl;
	}
	if (atexit(exist_status2))
	{
		cout << "registe exit2 fail" << endl;
	}
	cout << "a registe test" << endl;

	/*三者都为对齐分配*/
	/*malloc int 随机*/
	int * p = (int *)malloc(sizeof(int));
	/*calloc 会初始化为0*/
	int * p1 = (int *)calloc(sizeof(int), 4);
	/*realloc 增量*/
	int *p2 = (int *)realloc(p, sizeof(int));

	free(p);
	free(p1);
	free(p2);

	/*获取环境变量信息与设置*/
	getenv("HOME");//
	//putenv("ADD");
	setenv("USER", "test", 0);//rewrite =0 or 1
	unsetenv("");//delete
	return;

}

jmp_buf jmpbuffer;//保存信息的

void  cmd_add(int val)
{
	/*设置跳转点*/
	if (val == 1)
	{
		longjmp(jmpbuffer, 1);
	}
	else
	{
		longjmp(jmpbuffer, 2);
	}
}
void jmptest()
{
	cmd_add(1);
	//	cmd_add(2);
}


/*
struct rlimit
{
	rlim_t rlim_cur;
	rlim_t rlim_max;
}
*/
#define doit(name) viewlimits(#name,name)
void viewlimits(char * name, int resource)
{
	struct rlimit  limit;
	if ((getrlimit(resource, &limit)) < 0)
	{
		cout << "error" << endl;
	}
	cout << limit.rlim_cur << '\t' << limit.rlim_max << endl;
}


void forktest()
{
	pid_t pid ;
	if( (pid = fork()) < 0 )
	{
		cout<<"fork error"<<endl;
		return ;
	}
	/*the fisr child   child1*/
	else if( pid == 0 )
	{
		if( (pid =fork()) < 0)
		{
			cout<<"fork"<<endl;
			return ;
		}
		/* the second child  child2*/
		else if(pid  == 0)
		{
			sleep(2); //sleep等待parent 结束
			cout<<"second child pid : "<<getpid()<<"\t";
			cout<<"second chile parent pid :"<<getppid()<<endl;

		}
		/*the first child child1 */
		else
		{
			cout<<"the first child"<<endl;
			exit(0); //正常退出
		}
	}
	/*this first parent*/
	else
	{
		cout<<"the first parent"<<endl;
		/*wait the first child*/
	    if( (waitpid(pid,NULL,0)) != pid )
		{
			cout<<"wait error"<<endl;
		}
	}
}


/*利用alarm和pause模拟一个sleep*/



void sig_handler(int signo)
{
	cout<<"have handle signal"<<endl;
}

unsigned int  sig_sleep(unsigned int second)
{
	if (signal(SIGALRM,sig_handler) < 0 )
	{
		return second;
	}
	/*设置倒计时*/
	alarm(second);
	/*程序挂起等待倒计时结束*/
	pause();
	return 0;
}

static jmp_buf env_alrm;

void sig_handler2(  int secondn)
{
	longjmp(env_alrm,1);//返回状态1
}

/*sleep2的实现*/
unsigned int sleep2(unsigned int secondn)
{
	if( signal(SIGALRM,sig_handler2) < 0)
	{
		return secondn;
	}
	if( setjmp(env_alrm) == 0)
	{
		alarm(10);
		pause();
	}
	return 0;
	 
	 
}

/*
struct sigaction
{
    void (*sa_handler)(int);
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_sigaction)(int , siginfo_t *,void *);
}
*/
void sleep3()
{
	sigset_t  news;
	sigset_t  old;
	struct  sigaction act ;
	act.sa_handler = sig_handler;//信号处理
	/*创建空信号屏蔽，不屏蔽任何信号*/
	sigemptyset(&act.sa_mask);

	/*设置为默认行为*/
	act.sa_flags = 0;
	/*代替signal。准备捕获信号*/
	sigaction(SIGINT,&act,0);

	sigemptyset(&news);//清空信号集
	sigaddset(&news,SIGINT);//添加需要设置的信号集
	/*屏蔽该信号，不响应该信号，同时保存就的信号集*/
	sigprocmask(SIG_BLOCK,&news,&old);
	cout<<"block"<<endl;
	/*解除信号屏蔽*/
	sigprocmask(SIG_SETMASK,&old,NULL);
	/*程序挂起*/
	pause();
}
/*程序挂起时能狗捕获到SIGINT信息处理，知道有一个SIGINT信号
sigsuspend 实际将sigprocmask和pause结合起来*/


void printfinfo()
{
	pid_t pid;
	pthread_t tid;
	pid = getpid();
	tid = pthread_self();
	cout << "pid:" << pid << "\t tid:" << tid << endl;
}
void *thr_print(void *arg)
{
	printfinfo();
	return nullptr;
}

void *thr_f1(void *arg)
{
	cout << "thread1 exit" << endl;
	//sleep(3);
	return ((void *)0);
}

void * thr_f2(void *arg)
{
	cout << "thread2 exit" << endl;
	pthread_exit((void *)1);
}

void * thread_fun1( void * arg)
{
	int i =(int)(*(int *)arg);
	pthread_t tid =pthread_self();
	cout<<"thread"<<i<<" has run"<<tid<<endl;
	return NULL;
}

/*动态分配*/
struct foo * foo_alloc()
{
	struct foo * fp;
	/*动态分配一个foo 结构，存放计数和互斥量*/
	if( (fp = (struct foo *) malloc(sizeof(struct foo))) != NULL)
	{
		/*初始化*/
		fp->f_count = 0;
		if(pthread_mutex_init(&fp->f_lock,NULL)!=0)
		{
			free(fp);
			cout<<"init pthread_mutex_t fail"<<endl;
			return NULL;
		}
	}
	return fp;
}


void foo_hold( struct foo * fp)
{
	/*使用互斥量访问*/
	pthread_mutex_lock(&fp->f_lock);
	fp->f_count ++;
	pthread_mutex_unlock(&fp->f_lock);
}

void foo_rela(struct foo * fp)
{
	pthread_mutex_lock(&fp->f_lock);
	if( -- fp->f_count  == 0)
	{
		pthread_mutex_unlock(&fp->f_lock);
		pthread_mutex_destroy(&fp->f_lock);
		free(fp);
	}
	else
	{
		pthread_mutex_unlock(&fp->f_lock);
	}
}

JobQueue::JobQueue()
{
	qp = new struct jobqueue();
}
JobQueue::~JobQueue()
{
	delete qp;
}
int JobQueue::queue_init()
{
	qp->q_head = NULL;
	qp->q_tail = NULL;
	int err = 0;
	if( (err = pthread_rwlock_init(&qp->q_lock,NULL)) !=0 )
	{
		return err;
	}
	return 0;
}
/*头插法*/
void JobQueue::insert_job(struct job *jp)
{
	/*操作加锁*/
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = qp->q_head;
	jp->j_prev = NULL;
	if( qp->q_head != NULL)
	{
		qp->q_head->j_prev = jp;
	}
	else
	{
		qp->q_tail = jp;
	}
	qp->q_head = jp;
	pthread_rwlock_unlock(&qp->q_lock);
}
/*尾插法*/
void JobQueue::append_job( struct job * jp)
{
	pthread_rwlock_wrlock(&qp->q_lock );
	jp->j_next =NULL;
	jp->j_prev = qp->q_tail;
	if( qp->q_tail !=NULL)
	{
		 qp->q_tail->j_next = jp;
	}
	else
	{
		qp->q_head = jp;
	}
	qp->q_tail =jp;
	pthread_rwlock_unlock(&qp->q_lock);
}


void JobQueue::remove_job(struct job * jp)
{
	pthread_rwlock_wrlock(&qp->q_lock );
	/*队列头*/
	if( qp->q_head == jp)
	{
		 qp->q_head = jp->j_next;
		 if( qp->q_tail == jp)
		 {
			 qp->q_tail = NULL;
		 }
	}
	else if( qp->q_tail == jp)
	{
		qp->q_tail = jp->j_prev;
		if( qp->q_head == jp)
		{
			qp->q_head =NULL;
		}
	}
	else
	{
		jp->j_next->j_prev = jp->j_prev;
		jp->j_prev->j_next = jp->j_next;
	}
	pthread_rwlock_unlock(&qp->q_lock);
}

struct job *JobQueue::find_job( pthread_t tid)
{
	struct job * jp;
	if( pthread_rwlock_rdlock(&qp->q_lock) != 0)
	{
		return NULL;
	}
	for( jp =qp->q_head; jp !=qp->q_tail; jp =jp->j_next)
	{
		if(pthread_equal(jp->j_id, tid))
		{
			break;
		}
	}
	pthread_rwlock_unlock(&qp->q_lock);
	return jp;
}