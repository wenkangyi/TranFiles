#include "../sharesource/sstime.h" //在这里已经指定了路径
#include <inttypes.h>

//return sockfd
int Connection(char *ip,unsigned short int port)
{

}



//----------  Socket Control End ---------------

int main(int argn,void **argv){
	time_t start_time = GetTime();
	
	

	time_t end_time = GetTime();
	CalcTime(start_time,end_time);
	
	return 0;
}
