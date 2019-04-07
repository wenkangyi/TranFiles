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
//sleekSize --> 偏移位置
//rBuf --> 读缓存地址
//rLen --> 读长度,单位:byte
int ReadFile(int fd,int sleekSize,char *rBuf,int rLen)
{

}


int WriteFile(int fd,int sleekSize,char *wBuf,int wLen)
{

}



