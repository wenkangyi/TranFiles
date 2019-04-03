#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__


typedef struct{
    unsigned char head;
    unsigned char ver;
    //0-->hex
    //1-->string
    //2-->josn
    //3-->html
    //....
    unsigned char format;

    //0-->start frame
    //1-->end frame
    //2-->data frame
    unsigned char cmd;

    //cmd !=0
    unsigned int fileLocation;
    unsigned int bufLen;
    //buf --> memory address
    void *buf;

    unsigned char tail;
} TranPro;




#endif
