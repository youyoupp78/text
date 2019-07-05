#include "common.h"
using namespace std;
#include <thread>//c++11标准库中的线程
#include "testheader.h"
int thread_test()
{
	pthread_t t1,t2;
	void * tret;
	if(pthread_create(&t1,NULL,thr_f1,NULL) != 0)
	{
		cout<<"create thread1 error"<<endl;
		return -1;
	}
	if(pthread_create(&t2,NULL,thr_f2,NULL) !=0)
	{
		cout<<"create thread2 error"<<endl;
		return -1;
	}
	//pthread_cancel(t1);
	if(pthread_join(t1, &tret) == 0)
	{
		cout << "thread exit code :" << tret << endl;
	}
	else
	{
		return -2;
	}
	if(pthread_join(t2,&tret) == 0)
	{
		cout<<"thread2 exit code:"<<tret<<endl;
	}
	else
	{
		return -2;
	}
	sleep(10);
	
	return 0;
}


void JobQueueTest()
{
	JobQueue jobQ;
	jobQ.queue_init();
	
	pthread_t tid;
	for (int i = 0; i < 4; i++)
	{
		struct job  data = { NULL,NULL,0 };
		if(pthread_create(&tid, NULL, thread_fun1, &i) != 0)
		{
			return ;
		}
		data.j_id = tid;
		jobQ.insert_job(&data);

	}
	struct job *p;
	for( p = jobQ.qp->q_head; p != jobQ.qp->q_tail; p=p->j_next)
	{
		cout<<p->j_id<<endl;
	}
	sleep(20);
	

}


struct msg * workq;
/*条件变量初始化*/
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
/*互斥量初始化*/
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void process_msg()
{
    struct msg * mq;
	while(1)
	{
		pthread_mutex_lock(&qlock);
		while( workq ==NULL)
		{
			pthread_cond_wait(&qready,&qlock);
		}
		mq = workq;
		workq = workq->m_next;
		pthread_mutex_unlock(&qlock);
	}
}

void enqueue_msg(struct msg * mq)
{
   pthread_mutex_lock(&qlock);
   /*竞争资源信息*/
   mq->m_next = workq;
   workq = mq;
   /*解锁*/
   pthread_mutex_unlock(&qlock);
   /*消息通知*/
   pthread_cond_signal(&qready);
}


////////////////////////////////////////////////////////////////////////////////
/*
static pthread_key_t   key; //key值
static pthread_once_t  init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;


extern char **environ;
static void thread_init(void)
{
	//*初始key值与析构函数
	pthread_key_create(&key,free);
}

char * getenv(const char *name)
{
	int i, len;
	char * envbuf;
	//只会调用一次初始化
	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&env_mutex);
	//buf与key
	envbuf = pthread_getspecific(key);
	if (envbuf == NULL)
	{
		envbuf = (char *)malloc(sizeof(char) * 20);
		//
		if (envbuf == NULL)
		{
			pthread_mutex_unlock(&env_mutex);
			return NULL;
		}
		pthread_setspecific(key,envbuf);
	}
//
	len = strlen(name);
	for( int i = 0; environ[i] != NULL; ++i)
	{
		if(	(strncmp(environ[i],name,len) )== 0 &&
		   environ[i][len] == '=')
		{
			strcpy(envbuf,environ[i][len+1]);
			return envbuf;
		}
    }
	pthread_mutex_unlock(&env_mutex);
	return NULL;
}
*/

/////////////////////////////////////////////////////////////////////////
void fifotest()
{
	/*管道*/
	int fd[2];
	pid_t pid;
	char buff[256] = { 0 };
	if( pipe(fd) < 0)
	{
		cout<<"create pipe fail"<<endl;
	}
	if( (pid =fork()) < 0)
	{
		cout<<"fork fail "<<endl;
	}
	if( pid == 0)
	{
		/*子进程关闭写*/
		//close(fd[1]);
		int n = read(fd[0],buff,12);
		write(STDOUT_FILENO,buff,n);
	}
	else
	{
		//close(fd[0]);
		write(fd[1],"hello world\n",12);
	}
}


pid_t pidf;
int fd3[2],fd4[2];

void TELL_PARENT()
{
	if (write(fd4[1], "C", 1) != 1)
	{
		cout << "child write error" << endl;
	}
}
void TELL_CHILD()
{
	if (write(fd3[1], "P", 1) != 1)
	{
		cout << "parent write error" << endl;
	}
}

void WAIT_PARENT()
{
	char buf;
	if (read(fd3[0], &buf, 1) != 1)
	{
		cout << "child read error" << endl;
	}
	cout<<buf<<endl;
	if (buf != 'P')
	{
		cout << "reqd data error:" << buf << endl;
	}
}

void WAIT_CHILD()
{
	char buf;
	if (read(fd4[0], &buf, 1) != 1)
	{
		cout << "parent read error" << endl;
	}
	cout<<buf<<endl;
	if (buf != 'C')
	{
		cout << "read data error:" << buf << endl;
		return;
	}

}


void fifotest2()
{
	/*创建失败*/
	if (pipe(fd3) < 0 || pipe(fd4) < 0)
	{
		cout<<"pipe error"<<endl;
		return ;
	}
	if ((pidf = fork()) < 0)
	{
		cout << "fork error" << endl;
	}
	/*child*/
	if (pidf == 0)
	{
		cout<<"chid"<<endl;
		TELL_CHILD();
		WAIT_CHILD();


	}
	/*parent*/
	else
	{
		cout<<"parent"<<endl;
		WAIT_PARENT();
		TELL_PARENT();
		
	}

}

void popentest()
{
	FILE * fp ;
	char line[256] = {0};
	/*读方式打开一个程序*/
	if( (fp = popen("./maintest","r")) == NULL)
	{
		cout<<"open file error"<<endl;
		return ;		
	}
	/*以管道的方式处理输出*/
	while(1)
	{
		fputs("promt >>",stdout);
		fflush(stdout);
		/*读取*/
		if ( (fgets(line, 256, fp)) == NULL)
		{
			break;
		}
		/*输出*/
		if(fputs(line,stdout)==EOF)
		{
			cout<<"puts error"<<endl;
		}

	}
	if( fclose(fp) ==-1)
	{
		cout<<"close error"<<endl;
	}
	putchar('\n');
	return ;

	
}

/////////////////////////////////////////////////////////////////////////////
struct mymsgbuf
{
	long mtype;
	char mtext[512];
};

#define KEY 1234
int msgtest()
{
	key_t key;
	/*创建一个标准的key*/
    key = ftok("/home/tristayang/projects/", IPC_CREAT);
	if(key == -1)
	{
		cout<<"create key fail"<<endl;
		return -1;
	}
	/*创建并打开一个消息队列*/
	int msgid = msgget(IPC_PRIVATE,IPC_CREAT | IPC_EXCL);
	if( msgid == -1)
	{
		cout<<"msg get fail"<<endl;
		return -1;
	}
	/*从标准输入获取消息*/
	struct mymsgbuf buff;
	if( (fgets(buff.mtext,512,stdin))== nullptr)
	{
		cout<<"no get msg"<<endl;
		return -1;
	}
	int len = strlen(buff.mtext);
	/*放置消息*/
	int msg ;
	if( (msg = msgsnd(msgid,&buff,len,0)) == -1)
	{
		cout<<"send error"<<endl;
		return -2;
	}
	/*从中取第一个消息*/
	struct mymsgbuf recvbuf;
	if ( (msg =msgrcv(msgid,&recvbuf,512,0,0)) == -1)
	{
		cout<<"recv error"<<endl;
		return -2;
	}
	cout << recvbuf.mtext << endl;
	/*消息出队列*/
	if( (msg = msgctl(msgid,IPC_RMID,nullptr)) == -1)
	{
		cout<<"ctl error"<<endl;
		return -2;
	}


}


void sigsettest()
{
	key_t key;
	if( (key =ftok(".",IPC_CREAT)) == -1)
	{
		cout<<"get key error"<<endl;
		return;
	}
	/*创建一个信号量集*/
	int semid = semget(IPC_PRIVATE,1,0666);
	if(semid  ==-1)
	{
		cout<<"get semid error"<<endl;
		return ;
	}
	/*定义信号量集操作*/
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_flg = IPC_NOWAIT;
	sbuf.sem_op = 1;//释放资源

	int ret;
	if( (ret =semop(semid,&sbuf,1)) == -1)
	{
		cout<<"semop error"<<endl;
		return ;
	}
	/*删除信号集合*/
	if( (ret=semctl(semid,0, IPC_RMID)) ==-1)
	{
		cout<<"rm error"<<endl;
		return ;
	}
}

void shmtest()
{
	key_t key;
	if ((key = ftok(".", IPC_CREAT)) == -1)
	{
		cout << "get key error" << endl;
		return;
	}
	int shmid = shmget(IPC_PRIVATE,1000,0600);
	if(shmid == -1)
	{
		cout<<"shm get error"<<endl;
		return ;
	}
	/*连接到地址空间,addr 0 由内核指定*/
	char * shmptr = (char *)shmat(shmid,0,0);
	if((void *)shmptr == (void *)-1)
	{
		cout<<"shmat error"<<endl;
		return ;
	}
	if((shmctl(shmid,IPC_RMID,0)) < 0)
	{
		cout<<"error"<<endl;
		return ;
	}
}

#define LOCKFILE  "/var/run/daemon.pid"
#define LOCKMODE  (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

void daemontest( const char * cmd )
{
   pid_t pid;
   struct rlimit rl;
   struct sigaction sa;

   /*1 屏蔽权限*/
   umask(0);
   /*获取文件描述符数量*/
   if(getrlimit(RLIMIT_NOFILE,&rl)<0)
   {
	   PERROR("can't get limit");
   }
   if( (pid= fork()) <0)
   {
	   PERROR("fork error");
	   return ;
   }
   /*parent*/
   if( pid > 0)
   {
	   return ;
   }
   else
   {
	   setsid();
	   cout<<"child"<<endl;
	   sa.sa_handler = SIG_IGN;
	   sigemptyset(&sa.sa_mask);
       sa.sa_flags = 0;
	   /*屏蔽信号*/
	   if(sigaction(SIGHUP,&sa,nullptr) < 0)
	   {
		   PERROR("sigaction error");
		   return ;
	   }
	   /*再次fork出孙子进程？？？*/
	   if( (pid =fork()) <0)
	   {
		   PERROR("FORK 2 ERROR");
		   return ;
	   }
	   if(pid > 0)
	   {
		   return ;
	   }
	   else
	   {
		   cout<<"grandchild"<<endl;
		   if(chdir("/") < 0)
		   {
			   PERROR("CD ERROR");
			   return;
		   }
		   /*打开的最大文件*/
		   if(rl.rlim_max == RLIM_INFINITY)
		   {
			   rl.rlim_max = 1024;
		   }
		   /*关闭*/
		   for( int i =0; i <rl.rlim_max; i++)
		   {
			   close(i);
		   }
		   int fd0 = open("/dev/null", O_RDWR);
		   int fd1 = dup(0);
		   int fd2 = dup(0);

		   openlog(cmd,LOG_CONS,LOG_DAEMON);
		   if( fd0 != 0 || fd1!= 1|| fd2!=2)
		   {
			   PERROR("unexpected file");
			   return;
		   }
		   sleep(10);

	   }
   }
}
int lockfile( int fd)
{
	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len =0;
	return (fcntl(fd,F_SETLK,&fl));
}
int already_run()
{
	int fd = open(LOCKFILE, O_RDWR|O_CREAT,LOCKMODE);
	if(fd <0)
	{
		syslog(LOG_ERR,"open lockfile error %s, %s", LOCKFILE,strerror(errno));
		return -1;
	}
	/*加锁文件*/
	if(lockfile(fd)< 0)
	{
		/*进程已经运行*/
		if(errno  == EACCES || errno == EAGAIN)
		{
			close(fd);
			return 1;
		}
		syslog(LOG_ERR, "can't lock file");
	}
	/*刷新id*/
	char buf[256] = {0};
	ftruncate(fd,0);
	sprintf(buf, "%ld",(long)getpid());
	write(fd, buf, strlen(buf)+1);
	return 0;
}

/*验证文件问题*/
void unixdomainsocket()
{
	int fd ;
	struct sockaddr_un un;
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path,"fun.socket");
	/*也可以使用的socketpair*/
	if( (fd =socket(AF_UNIX,SOCK_STREAM,0)) < 0 )
	{
		PERROR("SOCKET ERROR");
		return ;
	}
	/*#define offsetof(TYPE,MEMBER)  ((int)&(TYPE *)0)->MEMBER*/
	/*计算实际长度 偏移量+ 文件*/
	int size = offsetof(struct sockaddr_un,sun_path)+ strlen(un.sun_path);
	if( (bind(fd,(struct sockaddr *) &un, size)) < 0)
	{
		PERROR("BIND SOCKET ERROR");
	}
	std::cout<<"unix domain socket"<<std::endl;

}
//////////////////////////////////////////////////////////////////////////////////////
/*创建空洞文件*/
void createfile()
{
	int fd ;
	if( (fd = creat("file.hole",O_CREAT | O_WRONLY)) <0)
	{
		return ;
	}
	char buff[12] ="hello world";

	if( write(fd,buff,12) < 0)
	{
		return ;
	}
	/*offset*/
	if( lseek(fd,1024,SEEK_SET) < 0)
	{
		return ;
	}
	if(write(fd,buff,12) < 0)
	{
		return;
	}

}


void duptest()
{
	int fd1,fd2;
	fd1 = open("dup1.txt", O_RDONLY);
	fd2 = open("dup1.txt", O_RDONLY);
	char buf[2];
	int n =read(fd2,buf,1);
	n = dup2(fd2,fd1);
	n =read(fd1,buf,1);
	std::cout<<buf<<std::endl;

}
void dupforktest()
{
	pid_t pid;
	int fd = open("dup1.txt",O_RDONLY);
	char buf[2];
	if( (pid =fork()) <0 )
	{
		return ;
	}
	int n ;
	if( pid == 0)
	{
		n = read(fd,buf,1);
		return ;
	}
	wait(nullptr);
	n = read(fd, buf,1);
	std::cout<<buf<<std::endl;
}

#define BSC(x) ((x)+(x))
void basetest()
{
	int a = 0;
	int b = 1;
	int c = (a++) + (++b);
	int d = (++a) + (++a) + (++a);
	a =2 ;
	b =-2 ;
	c =2/(-2);
	d = 2%(-2);
	printf("BSC(a)");
#define X 3
#define Y X*2
#undef X
#define X 2
	int z =Y;

	char m[5] = {'A','B','C','D'};
	char (*p1)[5] = &m;

	int num[4] = {1,2,3,4};
	int *p3 = (int *)(&num+1);
//	int *p4 = (int *)((int)num+1);

	union
	{
		int i;
		char ch;

	}cu;
	cu.i =1;
	cout<<(cu.ch ==1)<<endl;
	
}
int main(int argc, char * argv[])
{
    //forktest();
	/*
	char **ptr;
	char ** environ;
	for (int i = 0; i < argc; ++i)
	{
		printf("argv[%d]=%s\n", i, argv[i]);
	}
	for (ptr = environ; *ptr != nullptr; ptr++)
	{
		printf("%s\n", *ptr);
	}
	*/
	//sleep2(10);


	/*创建线程用例*/
	/*
	pthread_t ntid;
	int err = pthread_create(&ntid, nullptr, thr_print, nullptr);
	if (err != 0)
	{
		cout << "create thread error" << endl;
		return-1;
	}
	printfinfo();
	sleep(5);
	*/

	//thread_test();

	//JobQueueTest();
	//fifotest();
	//fifotest2();
	//popentest();
	//msgtest();
	//sigsettest();
	//shmtest();
	//daemontest("ls");
	//unixdomainsocket();
	//createfile();
	//duptest();
	//dupforktest();
    basetest();
	return 0;



}

