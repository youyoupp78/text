#pragma once
#include <vector>
#include <thread>//c++11标准库中的线程
#include <stdlib.h>
#include<limits.h>
#include <string.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <syslog.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#define __USE_POSIX199309
#define PERROR(s)  printf("%s\n",s)