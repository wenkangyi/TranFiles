#include "protocol.h"


char *filePath = NULL;

//false --> 0; true --> 1
int SetFilePath(char *fPath,int fPathLen)
{
    filePath = (char*)malloc(fPathLen);
    if(filePath == NULL) return 0;
}

//false --> 0; true --> 1
int ClearFilePath()
{
    if(filePath == NULL) return 1;
    free(filePath);
    filePath = NULL;
    return 1;
}

//false --> -1; true --> fd
int OpenFile(const char *fPath,int flag)
{
    int fd = -1;
    fd = open(fPath,flag);
    if(-1 == fd)
    {
        perror("open file false;");
    }
    return fd;
}

//false --> 0; true --> 1
int CloseFile(int fd)
{
    if(0 != close(fd))
    {
        perror("close file error!");
        return 0;
    }
    return 1;
}

//read :false --> 0; true --> 1
//offset --> 偏移位置
//rBuf --> 读缓存地址
//rLen --> 读长度,单位:byte
int ReadFile(int fd,int offset,char *rBuf,int rLen)
{
    int rByteSize = -1;
    if(fd < 0)
    {
        return 0;
    }

    printf("ReadFile offset=%d\r\n",offset);
    if(lseek(fd,offset,SEEK_SET) < 0)
    {
        perror("ReadFile lseek false!");
        return 0;
    }

    rByteSize = read(fd,rBuf,rLen);
    if(-1 == rByteSize)
    {
        perror("ReadFile read false!");
        return 0;
    }
    return rByteSize;
}


int WriteFile(int fd,int offset,char *wBuf,int wLen)
{
    int wByteSize = -1;
    if(fd < 0)
    {
        return 0;
    }

    printf("WriteFile offset=%d\r\n",offset);

    if(lseek(fd,offset,SEEK_SET) < 0)
    {
        perror("WriteFile lseek false!");
        return 0;
    }
    printf("WriteFile wLen=%d\r\n",wLen);
    wByteSize = write(fd,wBuf,wLen);
    if(-1 == wByteSize)
    {
        perror("WriteFile read false!");
        return 0;
    }
    return wByteSize;
}


void main(int argn,void **argv)
{
    if(argn < 2)
    {
        perror("Params Num < 2");
        exit(0);
    }
    int fd = OpenFile(argv[1],O_CREAT | O_RDWR);

    //sizeof("1234567890abc") 
    WriteFile(fd,0,"1234567890abc",sizeof("1234567890abc"));
    WriteFile(fd,2,"defghijk",sizeof("defghijk"));
    char rbuf[10];
    memset(rbuf,0,10);
    int ret = ReadFile(fd,9,rbuf,2);
    CloseFile(fd);
    printf("ret =%d \r\n",ret);
    for(int index=0;index<ret;index++)
        printf("rbuf[%d] =%c\r\n",index,rbuf[index]);
}



