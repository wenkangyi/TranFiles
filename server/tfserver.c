#include "../sharesource/sstime.h"


int main(int argn,void **argv){
	time_t start_time = GetTime();
	
	
	time_t end_time = GetTime();
	CalcTime(start_time,end_time);
	return 0;
}