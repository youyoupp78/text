#include "common.h"
using namespace std;
#include <thread>//c++11��׼���е��߳�
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
/*����������ʼ��*/
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
/*��������ʼ��*/
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
   /*������Դ��Ϣ*/
   mq->m_next = workq;
   workq = mq;
   /*����*/
   pthread_mutex_unlock(&qlock);
   /*��Ϣ֪ͨ*/
   pthread_cond_signal(&qready);
}


////////////////////////////////////////////////////////////////////////////////
/*
static pthread_key_t   key; //keyֵ
static pthread_once_t  init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;


extern char **environ;
static void thread_init(void)
{
	//*��ʼkeyֵ����������
	pthread_key_create(&key,free);
}

char * getenv(const char *name)
{
	int i, len;
	char * envbuf;
	//ֻ�����һ�γ�ʼ��
	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&env_mutex);
	//buf��key
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
	/*�ܵ�*/
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
		/*�ӽ��̹ر�д*/
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
	/*����ʧ��*/
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
	/*����ʽ��һ������*/
	if( (fp = popen("./maintest","r")) == NULL)
	{
		cout<<"open file error"<<endl;
		return ;		
	}
	/*�Թܵ��ķ�ʽ�������*/
	while(1)
	{
		fputs("promt >>",stdout);
		fflush(stdout);
		/*��ȡ*/
		if ( (fgets(line, 256, fp)) == NULL)
		{
			break;
		}
		/*���*/
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
	/*����һ����׼��key*/
    key = ftok("/home/tristayang/projects/", IPC_CREAT);
	if(key == -1)
	{
		cout<<"create key fail"<<endl;
		return -1;
	}
	/*��������һ����Ϣ����*/
	int msgid = msgget(IPC_PRIVATE,IPC_CREAT | IPC_EXCL);
	if( msgid == -1)
	{
		cout<<"msg get fail"<<endl;
		return -1;
	}
	/*�ӱ�׼�����ȡ��Ϣ*/
	struct mymsgbuf buff;
	if( (fgets(buff.mtext,512,stdin))== nullptr)
	{
		cout<<"no get msg"<<endl;
		return -1;
	}
	int len = strlen(buff.mtext);
	/*������Ϣ*/
	int msg ;
	if( (msg = msgsnd(msgid,&buff,len,0)) == -1)
	{
		cout<<"send error"<<endl;
		return -2;
	}
	/*����ȡ��һ����Ϣ*/
	struct mymsgbuf recvbuf;
	if ( (msg =msgrcv(msgid,&recvbuf,512,0,0)) == -1)
	{
		cout<<"recv error"<<endl;
		return -2;
	}
	cout << recvbuf.mtext << endl;
	/*��Ϣ������*/
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
	/*����һ���ź�����*/
	int semid = semget(IPC_PRIVATE,1,0666);
	if(semid  ==-1)
	{
		cout<<"get semid error"<<endl;
		return ;
	}
	/*�����ź���������*/
	struct sembuf sbuf;
	sbuf.sem_num = 0;
	sbuf.sem_flg = IPC_NOWAIT;
	sbuf.sem_op = 1;//�ͷ���Դ

	int ret;
	if( (ret =semop(semid,&sbuf,1)) == -1)
	{
		cout<<"semop error"<<endl;
		return ;
	}
	/*ɾ���źż���*/
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
	/*���ӵ���ַ�ռ�,addr 0 ���ں�ָ��*/
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

   /*1 ����Ȩ��*/
   umask(0);
   /*��ȡ�ļ�����������*/
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
	   /*�����ź�*/
	   if(sigaction(SIGHUP,&sa,nullptr) < 0)
	   {
		   PERROR("sigaction error");
		   return ;
	   }
	   /*�ٴ�fork�����ӽ��̣�����*/
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
		   /*�򿪵�����ļ�*/
		   if(rl.rlim_max == RLIM_INFINITY)
		   {
			   rl.rlim_max = 1024;
		   }
		   /*�ر�*/
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
	/*�����ļ�*/
	if(lockfile(fd)< 0)
	{
		/*�����Ѿ�����*/
		if(errno  == EACCES || errno == EAGAIN)
		{
			close(fd);
			return 1;
		}
		syslog(LOG_ERR, "can't lock file");
	}
	/*ˢ��id*/
	char buf[256] = {0};
	ftruncate(fd,0);
	sprintf(buf, "%ld",(long)getpid());
	write(fd, buf, strlen(buf)+1);
	return 0;
}

/*��֤�ļ�����*/
void unixdomainsocket()
{
	int fd ;
	struct sockaddr_un un;
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path,"fun.socket");
	/*Ҳ����ʹ�õ�socketpair*/
	if( (fd =socket(AF_UNIX,SOCK_STREAM,0)) < 0 )
	{
		PERROR("SOCKET ERROR");
		return ;
	}
	/*#define offsetof(TYPE,MEMBER)  ((int)&(TYPE *)0)->MEMBER*/
	/*����ʵ�ʳ��� ƫ����+ �ļ�*/
	int size = offsetof(struct sockaddr_un,sun_path)+ strlen(un.sun_path);
	if( (bind(fd,(struct sockaddr *) &un, size)) < 0)
	{
		PERROR("BIND SOCKET ERROR");
	}
	std::cout<<"unix domain socket"<<std::endl;

}
//////////////////////////////////////////////////////////////////////////////////////
/*�����ն��ļ�*/
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


	/*�����߳�����*/
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

