#include "protocol.h"


char *filePath = NULL;
//file memory address
char *fileMap = NULL;
int fileLen = 0;

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

int GetFileSize(int fd)
{
    struct stat statbuf;
    if(-1 == fstat(fd,&statbuf))
    {
        perror("GetFileSize function fstat false!");
    }
    return statbuf.st_size;
}

//loading file
//fd --> file descriptor
//fileLen --> file length
//On success,return memory address;false return -1
char* MmapFile2Memory(int fd,int fileLen)
{
    return (char *)mmap(0, fileLen, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);
}

//uninstall file
//fpStartAddr --> file to memory start address 
//fileLen --> file lenght
//return:On success,return 0;false return -1
int Munmap2Memory(char *fpStartAddr,int fileLen)
{
    return munmap(fpStartAddr,fileLen);
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

//function testing
void main(int argn,void **argv)
{
    if(argn < 2)
    {
        perror("Params Num < 2");
        exit(0);
    }
    int fd = OpenFile(argv[1],O_CREAT | O_RDWR);
    if(-1 == fd) perror("open file error!");
    
    //sizeof("1234567890abc") must sub -1
    //WriteFile(fd,0,"1234567890abc",sizeof("1234567890abc")-1);
    //WriteFile(fd,2,"defghijk",sizeof("defghijk")-1);
    
    // char wbuf[10] ;
    // memset(wbuf,0,10);
    // for(int i=0;i<sizeof(wbuf);i++) wbuf[i] = '0' + i;
    // WriteFile(fd,0,wbuf,sizeof(wbuf));
    
    // for(int i=0;i<sizeof(wbuf);i++) wbuf[i] = 'a' + i;
    // WriteFile(fd,2,wbuf,sizeof(wbuf));
    
    // char rbuf[10];
    // memset(rbuf,0,10);
    // int ret = ReadFile(fd,9,rbuf,2);

    fileLen = GetFileSize(fd);
    printf("fileLen=%d\r\n",fileLen);
    fileMap = MmapFile2Memory(fd,fileLen);
    if((char *)-1 == fileMap) perror("mmap error!");

    //for(int index=0;index<fileLen;index++) 
    printf("file = %s\r\n",fileMap);

    Munmap2Memory(fileMap,fileLen);
    CloseFile(fd);
    // printf("ret =%d \r\n",ret);
    // for(int index=0;index<ret;index++)
    //     printf("rbuf[%d] =%c\r\n",index,rbuf[index]);
}



