#include "../sharesource/sstime.h"

//return sockfd
int SocketInit(void)
{

}


char* FilesInit(size_t fileLen)
{
	//1 初始化文件
	char *pFile = (char *)malloc(fileLen);
	return pFile;
}



int main(int argn,void **argv){
	time_t start_time = GetTime();
	
	
	time_t end_time = GetTime();
	CalcTime(start_time,end_time);
	return 0;
}