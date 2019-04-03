#include "sstime.h"

//计算两个时间差
void CalcTime(time_t start_time,time_t end_time)
{
	//time_t t;
    struct tm *start_locp = NULL;
	struct tm *end_locp = NULL;
    struct tm start_loc;
	struct tm end_loc;
	char filename[50];
	int i = 0;
	
	//t = time(NULL);
	locp = localtime(&t);
	if (locp != NULL)
	{
		loc = *locp;
		printf ("%d:%d:%d %s\n",loc.tm_hour,loc.tm_min,loc.tm_sec,s);
		
	}	
}

time_t GetTime()
{
	return time(null);
}


//写日志函数，可重入
//s为要记录的字符串
void WriteLog(char *s)
{
	time_t t;
    struct tm *locp = NULL;
    struct tm loc;
	char filename[50];
	int i = 0;
	FILE *fd;

	#if SYS_DEBUG
	printf("%s\r\n",s);
	#endif
	
	t = time(NULL);
	locp = localtime(&t);
	if (locp != NULL)
	{
		loc = *locp;
		for(i=0;i<50;i++)
			filename[i] = 0;
		
		sprintf( filename, "/home/Applog/App%d-%d-%d", (loc.tm_year+1900), (loc.tm_mon+1), loc.tm_mday);
		fd = fopen(filename,"a+");
		fprintf (fd, "%d:%d:%d %s\n",loc.tm_hour,loc.tm_min,loc.tm_sec,s);
		fclose(fd);
	}	
}

