#include "../sharesource/sstime.h" //在这里已经指定了路径
#include <inttypes.h>


#define FILE_NAME_MAX_LEN		50
#define SET_ZERO				0
#define FRAME_MAX_SIZE			512
#define READ_FILE_MAX_SIZE		4096//4k
#define BUFFER_POOL_MAX_GROUP	32


#pragma pack (1)
typedef struct FileControlStruct{
	unsigned int file_len;//文件总长度
	unsigned int total_frame;//总帧数
	unsigned int remain_frame;//剩余帧数
	unsigned int last_frame_len;//最后一帧字节数
}FCS; //1字节对齐

//缓冲结构
typedef struct BufferStruct{
	char full_flag;//full=1,null=0
	char pos;//max val = (READ_FILE_MAX_SIZE / FREAM_MAX_SIZE) - 1
	char buf[READ_FILE_MAX_SIZE];
}BS;

//传输缓冲区
typedef struct TranBuffer{
	unsigned int write_pos;
	unsigned int read_pos;
	BS bs_arr[BUFFER_POOL_MAX_GROUP];
}TB;

//---------------------------------------------

//------------  File Control ------------------

void ReadFile(int fd,FCS *fcs,TB *tb){
	static unsigned int seek_num = 0;
	for(int i=0;i<BUFFER_POOL_MAX_GROUP;i++){
		//多线程需要这一行
		while(tb->bs_arr[i].full_flag == 1) sched_yield();
		if(fcs->total_frame == (seek_num+1)){
			read(fd,tb->bs_arr[i].buf,fcs->last_frame_len);
		}else{
			read(fd,tb->bs_arr[i].buf,READ_FILE_MAX_SIZE);
			seek_num ++;
			lseek(fd,seek_num*READ_FILE_MAX_SIZE,SEEK_SET);//定位到文件开头
		}
		tb->bs_arr[i].pos = 0;
		tb->bs_arr[i].full_flag = 1;
	}
}
//计算读取文件帧数
void CalcFileFrame(FCS *fcs,int max_size){
	fcs->total_frame = fcs->file_len / max_size;
	if((fcs->file_len % max_size) > 0){
		fcs->total_frame ++;
		fcs->last_frame_len = fcs->file_len % max_size;
	}
	else{
		fcs->last_frame_len = max_size;
	}
}

int OpenFile(int fd,char *file_name){
	fd = open(file_name,"rb");
	if(fd == -1){
		perror("open file error!");
		return 0;
	}
	return 1;
}

int GetFileLen(int fd){
	lseek(fd,0L,SEEK_END);//移到文件结尾
	int file_len = ftell(fd);//得到文件大小
	lseek(fd,0L,SEEK_SET);//定位到文件开头
	return file_len;
}

//--------  File Control End  --------------

//------------  Socket Control ------------------
int ConnectionServer(){
	struct addrinfo hint, *result;
    int res, sfd;

    memset(&hint, SET_ZERO, sizeof(hint));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
	
	res = getaddrinfo("127.0.0.1", "8989", &hint, &result);
    if (res == -1) 
    {
        perror("error : cannot get socket address!\n");
        exit(1);
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) 
    {
        perror("error : cannot get socket file descriptor!\n");
        exit(1);
    }

    res = connect(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) 
    {
        perror("error : cannot connect the server!\n");
        exit(1);
    }
	
	return sfd;
}

int SendData(int sfd,TB *tb){
	
	write(sfd, buf, strlen(buf));
}

//----------  Socket Control End ---------------

int main(int argn,void **argv){
	time_t start_time = GetTime();
	FCS rfcs;//读文件
	FCS sfcs;//发送文件
	int file_fd;
	char file_name[FILE_NAME_MAX_LEN];
	TB tb;
	
	memset((char*)rfcs,SET_ZERO,sizeof(rfcs));
	memset((char*)sfcs,SET_ZERO,sizeof(sfcs));
	memset(file_name,SET_ZERO,FILE_NAME_MAX_LEN);
	memset(tb,SET_ZERO,sizeof(tb));
	
	//如果打开文件失败，直接退出
	if(OpenFile(file_fd) == 0) return exit(1);
	//获取文件长度
	rfcs.file_len = GetFileLen(file_fd);
	sfcs.file_len = rfcs.file_len;
	//计算帧参数
	CalcFileFrame(rfcs,READ_FILE_MAX_SIZE);
	CalcFileFrame(sfcs,FRAME_MAX_SIZE);
	
	
	close(fd);
	time_t end_time = GetTime();
	CalcTime(start_time,end_time);
	
	return 0;
}
