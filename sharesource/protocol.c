#include "protocol.h"


char *filePath = NULL;
//file memory address
char *fileMap = NULL;
int fileLen = 0;

//add
unsigned int CalcCRCValue(char *pbuf,int len)
{
    unsigned int crcValue = 0;
    printf("-->CalcCRCValue\n");
    for(int i=0;i<len;i++) crcValue += pbuf[i];
    return crcValue;
}

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
    printf("-->OpenFile\n");
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
    printf("-->GetFileSize\n");
    if(-1 == fstat(fd,&statbuf))
    {
        perror("GetFileSize function fstat false!");
    }
    return statbuf.st_size;
}

//loading file
//fd --> file descriptor
//flag --> PROT_READ | PROT_WRITE
//fileLen --> file length
//On success,return memory address;false return -1
char* MmapFile2Memory(int fd,int fileLen,int flag)
{
    printf("-->MmapFile2Memory\n");
    return (char *)mmap(0, fileLen, flag, MAP_SHARED,fd, 0);
}

//uninstall file
//fpStartAddr --> file to memory start address 
//fileLen --> file lenght
//return:On success,return 0;false return -1
int Munmap2Memory(char *fpStartAddr,int fileLen)
{
    printf("-->Munmap2Memory\n");
    return munmap(fpStartAddr,fileLen);
}

//false --> 0; true --> 1
int CloseFile(int fd)
{
    printf("-->CloseFile\n");
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
    printf("-->ReadFile\n");
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
    printf("-->WriteFile\n");
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


int SendStartFrame(int sockfd,TranFileStruct *tfs)
{
    StartFrame sf ;
    printf("-->SendStartFrame\n");
    for(int i=0;i<sizeof(sf.head);i++) sf.head[i] = 0xFE;
    sf.ver = PRO_VER;
    sf.format = FORMAT_HEX;
    sf.cmd = START_FRAME_CMD;
    sf.fileTotalLen = tfs->fileTotalLen;
    sf.fileCRCValue = tfs->fileCRCValue;
    memcpy(sf.fileName,tfs->fileName,FILE_NAME_MAX_LEN);
    for(int i=0;i<sizeof(sf.tail);i++) sf.tail[i] = 0xEF;

    printf("send data lenght %ld\r\n",sizeof(sf));

    int ret = send(sockfd, (char *)&sf, sizeof(sf), 0);
    if(-1 == ret ) perror("SendData send error!");
    return ret;
}

int SendStartFrameAck(int sockfd,char *recBuf,TranFileStruct *tfs)
{
    StartFrame *sf = (StartFrame*)recBuf;
    FrameAck fa;
    printf("-->SendStartFrameAck\n");
    int fileTotalLen = 0;
    memcpy(fa.head,sf->head,4);
    fa.ver = sf->ver;
    fa.format = sf->format;
    fa.cmd = sf->cmd;
    fileTotalLen = sf->fileTotalLen;
    tfs->fileCRCValue = sf->fileCRCValue;
    memcpy(fa.tail,sf->tail,4);

    //create file
    int fd = OpenFile(sf->fileName,O_CREAT | O_RDWR);
    if(-1 == fd) perror("SendStartFrameAck openfile error!");
    char wbuf[sf->fileTotalLen];
    memset(wbuf,0,sf->fileTotalLen);
    WriteFile(fd,0,wbuf,sf->fileTotalLen);
    char *fileMap = MmapFile2Memory(fd,fileTotalLen,PROT_READ | PROT_WRITE);
    if((char*)-1 == fileMap)
    {
        fa.exeStatus = 0;
        send(sockfd,(char*)&fa,sizeof(fa),0);
        Munmap2Memory(fileMap,fileTotalLen);
        CloseFile(fd);
        perror("SendStartFrameAck mmap file error!");
        return -1;
    }
    memset(fileMap,0,fileTotalLen);
    //Munmap2Memory(fileMap,fileTotalLen);
    //CloseFile(fd);
    tfs->fileMap = fileMap;
    fa.exeStatus = 1;
    int ret = send(sockfd,(char*)&fa,sizeof(fa),0);
    if(-1 == ret) perror("SendStartFrameAck send error!");
    return ret;
}

int SendEndFrame(int sockfd)
{
    EndFrame ef;

    printf("-->SendEndFrame\n");

    for(int i=0;i<sizeof(ef.head);i++) ef.head[i] = 0xFE;
    ef.ver = PRO_VER;
    ef.format = FORMAT_HEX;
    ef.cmd = END_FRAME_CMD;

    for(int i=0;i<sizeof(ef.tail);i++) ef.tail[i] = 0xEF;

    printf("send data lenght %ld\r\n",sizeof(ef));

    int ret = send(sockfd, (char *)&ef, sizeof(ef), 0);
    if(-1 == ret ) perror("SendData send error!");
    return ret;
}

int SendEndFrameAck(int sockfd,char *recBuf,TranFileStruct *tfs)
{
    EndFrame *ef = (EndFrame*)recBuf;
    FrameAck fa;
    //int fileTotalLen = 0;

    printf("-->SendEndFrameAck\n");

    memcpy(fa.head,ef->head,4);
    fa.ver = ef->ver;
    fa.format = ef->format;
    fa.cmd = ef->cmd;
    memcpy(fa.tail,ef->tail,4);
    printf("tfs->fileTotalLen-->%d\n",tfs->fileTotalLen);
    unsigned int crcValue = CalcCRCValue(tfs->fileMap,tfs->fileTotalLen);
    if(crcValue == tfs->fileCRCValue){
        fa.exeStatus = 1;
    }else{
        fa.exeStatus = 0;
    }
    
    Munmap2Memory(tfs->fileMap,tfs->fileTotalLen);
    CloseFile(tfs->fd);
    int ret = send(sockfd,(char*)&fa,sizeof(fa),0);
    return ret;
}

//send data
//sockfd
//tfs --> TranFileStruct *
int SendData(int sockfd,TranFileStruct *tfs)
{
    //tfs->fileMap
    TranPro tp;

    printf("-->SendData\n");

    for(int i=0;i<sizeof(tp.head);i++) tp.head[i] = 0xFE;
    tp.ver = PRO_VER;
    tp.format = FORMAT_HEX;
    tp.cmd = DATA_FRAME_CMD;
    tp.fileTotalLen = tfs->fileTotalLen;
    tp.currPoint = tfs->currPoint;
    
    if((tfs->currPoint + tfs->maxTranSize) > tfs->endPoint)
    {
        tp.bufLen = tfs->endPoint - tfs->currPoint;
    }
    else{
        tp.bufLen = tfs->maxTranSize;
    }
    
    memset(tp.buf,0,MAX_TRAN_DATA_SIZE);
    memcpy(tp.buf,&tfs->fileMap[tfs->currPoint], tp.bufLen);
    tfs->currPoint += tp.bufLen;

    for(int i=0;i<sizeof(tp.tail);i++) tp.tail[i] = 0xEF;

    printf("send data lenght %ld\r\n",sizeof(tp));

    int ret = send(sockfd, (char *)&tp, sizeof(tp), 0);
    if(-1 == ret ) perror("SendData send error!");
    return ret;
}

int SendDataFrameAck(int sockfd,char *recBuf,TranFileStruct *tfs)
{
    TranPro *ef = (TranPro*)recBuf;
    FrameAck fa;
    int fileTotalLen = 0;

    printf("-->SendDataFrameAck\n");

    memcpy(fa.head,ef->head,4);
    fa.ver = ef->ver;
    fa.format = ef->format;
    fa.cmd = ef->cmd;
    memcpy(fa.tail,ef->tail,4);
    printf("recv file data-->%s\n",ef->buf);
    memcpy(&tfs->fileMap[tfs->currPoint],ef->buf,ef->bufLen);
    tfs->currPoint += ef->bufLen;
    fa.exeStatus = 1;

    int ret = send(sockfd,(char*)&fa,sizeof(fa),0);
    return ret;
}

//1 success,0 false
int FrameAckParsing(int sockfd)
{
    printf("-->FrameAckParsing\n");
    char recBuf[10240];
    int recLen = recv(sockfd,recBuf,sizeof(recBuf),0);
    if(recLen<=0) perror("FrameAckParsing\n");
    FrameAck *pfa = (FrameAck*)recBuf;
    return pfa->exeStatus;        
}

//function testing
// void main(int argn,void **argv)
// {
//     if(argn < 2)
//     {
//         perror("Params Num < 2");
//         exit(0);
//     }
//     int fd = OpenFile(argv[1],O_CREAT | O_RDWR);
//     if(-1 == fd) perror("open file error!");
    
//     //sizeof("1234567890abc") must sub -1
//     //WriteFile(fd,0,"1234567890abc",sizeof("1234567890abc")-1);
//     //WriteFile(fd,2,"defghijk",sizeof("defghijk")-1);
    
//     // char wbuf[10] ;
//     // memset(wbuf,0,10);
//     // for(int i=0;i<sizeof(wbuf);i++) wbuf[i] = '0' + i;
//     // WriteFile(fd,0,wbuf,sizeof(wbuf));
    
//     // for(int i=0;i<sizeof(wbuf);i++) wbuf[i] = 'a' + i;
//     // WriteFile(fd,2,wbuf,sizeof(wbuf));
    
//     // char rbuf[10];
//     // memset(rbuf,0,10);
//     // int ret = ReadFile(fd,9,rbuf,2);

//     fileLen = GetFileSize(fd);
//     printf("fileLen=%d\r\n",fileLen);
//     fileMap = MmapFile2Memory(fd,fileLen);
//     if((char *)-1 == fileMap) perror("mmap error!");

//     //for(int index=0;index<fileLen;index++) 
//     printf("file = %s\r\n",fileMap);

//     Munmap2Memory(fileMap,fileLen);
//     CloseFile(fd);
//     // printf("ret =%d \r\n",ret);
//     // for(int index=0;index<ret;index++)
//     //     printf("rbuf[%d] =%c\r\n",index,rbuf[index]);
// }



