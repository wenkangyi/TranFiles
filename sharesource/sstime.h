#ifndef __SSTIME_H__
#define __SSTIME_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>

#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>

#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>

#include <pthread.h>
#include "string.h"
#include "unistd.h"


extern void CalcTime(time_t start_time,time_t end_time);
extern time_t GetTime();


#endif