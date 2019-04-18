#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdio.h>
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//read
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>//mmap

#include <sys/types.h>
#include <sys/socket.h>

#define SERV_PORT   8080
#define SOCKET_MAX_NUM  1000


#define PAGE_SIZE (4096)
#define PRO_VER (0x00)

#define FORMAT_HEX      (0x00)
#define FORMAT_STRING   (0x01)
#define FORMAT_JOSN     (0x02)
#define FORMAT_HTML     (0x03)

#define START_FRAME_CMD (0x00)
#define END_FRAME_CMD   (0x01)
#define DATA_FRAME_CMD  (0x02)

#define MAX_TRAN_DATA_SIZE  1024

#define FILE_NAME_MAX_LEN   100

#pragma pack (1)
//start frame
typedef struct{
    //0xFE 0xFE 0xFE 0xFE
    unsigned char head[4];
    //0-->0.0ver , 0x11-->1.1ver
    unsigned char ver;
    //0-->hex
    //1-->string
    //2-->josn
    //3-->html
    //....
    unsigned char format;
    //==========================
    //0-->start frame
    //1-->end frame
    //2-->data frame
    unsigned char cmd;

    unsigned int fileTotalLen;
    //32bit CRC
    unsigned int fileCRCValue;
    //fileName
    char fileName[FILE_NAME_MAX_LEN];
    //==========================
    //0xEF 0xEF 0xEF 0xEF
    unsigned char tail[4];
}StartFrame;

//frame Ack
typedef struct{
    //0xFE 0xFE 0xFE 0xFE
    unsigned char head[4];
    //0-->0.0ver , 0x11-->1.1ver
    unsigned char ver;
    //0-->hex
    //1-->string
    //2-->josn
    //3-->html
    //....
    unsigned char format;
    //==========================
    //0-->start frame
    //1-->end frame
    //2-->data frame
    unsigned char cmd;
    //cmd !=0
    unsigned int exeStatus;//1 success,0 false
    //==========================
    //0xEF 0xEF 0xEF 0xEF
    unsigned char tail[4];
}FrameAck;


//tran frame
typedef struct{
    //0xFE 0xFE 0xFE 0xFE
    unsigned char head[4];
    //0-->0.0ver , 0x11-->1.1ver
    unsigned char ver;
    //0-->hex
    //1-->string
    //2-->josn
    //3-->html
    //....
    unsigned char format;
    //==========================
    //0-->start frame
    //1-->end frame
    //2-->data frame
    unsigned char cmd;

    //cmd !=0
    unsigned int fileTotalLen;
    unsigned int currPoint;
    unsigned int bufLen;
    //buf --> memory address
    char buf[MAX_TRAN_DATA_SIZE];
    //==========================
    //0xEF 0xEF 0xEF 0xEF
    unsigned char tail[4];
} TranPro;



//end frame 
typedef struct{
    //0xFE 0xFE 0xFE 0xFE
    unsigned char head[4];
    //0-->0.0ver , 0x11-->1.1ver
    unsigned char ver;
    //0-->hex
    //1-->string
    //2-->josn
    //3-->html
    //....
    unsigned char format;
    //==========================
    //0-->start frame
    //1-->end frame
    //2-->data frame
    unsigned char cmd;
    //==========================
    //0xEF 0xEF 0xEF 0xEF
    unsigned char tail[4];
}EndFrame;

//============================================
typedef struct 
{
    int fd;
    char *fileMap;
    char fileName[FILE_NAME_MAX_LEN];
    int fileTotalLen;
    unsigned int fileCRCValue;
    int startPoint;
    int endPoint;
    int currPoint;
    int const maxTranSize;
}TranFileStruct;


//================= function declaration ===============
//file
extern unsigned int CalcCRCValue(char *pbuf,int len);
extern int SetFilePath(char *fPath,int fPathLen);
extern int ClearFilePath();
extern int OpenFile(const char *fPath,int flag);
extern int GetFileSize(int fd);
extern char* MmapFile2Memory(int fd,int fileLen,int flag);
extern int Munmap2Memory(char *fpStartAddr,int fileLen);
extern int CloseFile(int fd);
extern int ReadFile(int fd,int offset,char *rBuf,int rLen);
extern int WriteFile(int fd,int offset,char *wBuf,int wLen);

//protocol
extern int SendStartFrame(int sockfd,TranFileStruct *tfs);
extern int SendStartFrameAck(int sockfd,char *recBuf,TranFileStruct *tfs);
extern int SendEndFrame(int sockfd);
extern int SendEndFrameAck(int sockfd,char *recBuf,TranFileStruct *tfs);
extern int SendData(int sockfd,TranFileStruct *tfs);
extern int SendDataFrameAck(int sockfd,char *recBuf,TranFileStruct *tfs);
extern int FrameAckParsing(int sockfd);

#endif
