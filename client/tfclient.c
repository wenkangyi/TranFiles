#include "../sharesource/sstime.h" //在这里已经指定了路径
#include "../sharesource/protocol.h" //在这里已经指定了路径
#include <inttypes.h>

//return:On success,return sockfd;false return -1
int Connection(char *ip)
{
 
	//客户端只需要一个套接字文件描述符，用于和服务器通信
	int clientSocket;
	//描述服务器的socket
	struct sockaddr_in serverAddr;
	char sendbuf[200];
	char recvbuf[200];
	int iDataNum;
	
	if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERV_PORT);
	//指定服务器端的ip，本地测试：127.0.0.1
	//inet_addr()函数，将点分十进制IP转换成网络字节序IP
	serverAddr.sin_addr.s_addr = inet_addr(ip);
	
	if(connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect");
		return -1;
	}

	return clientSocket;
}

void FileInit(TranFileStruct *tfs,char *fileName){
	memcpy(tfs->fileName,fileName,strlen(fileName));// tfs.fileName
	tfs->fd = OpenFile(tfs->fileName,O_RDONLY);
	tfs->fileTotalLen = GetFileSize(tfs->fd);
	tfs->startPoint = 0;
	tfs->currPoint = 0;
	tfs->endPoint = tfs->fileTotalLen;
	tfs->fileMap = MmapFile2Memory(tfs->fd,tfs->fileTotalLen,PROT_READ);
	tfs->fileCRCValue = CalcCRCValue(tfs->fileMap,tfs->fileTotalLen);
}

//----------  Socket Control End ---------------

int main(int argn,void **argv){
	
	char recbuf[10240];
	int recLen = 0;
	TranFileStruct tfs={0,NULL,"",0,0,0,0,0,FILE_NAME_MAX_LEN};
	int exeStatus = 0;

	if(argn < 3)
	{
		printf("input:./appname serverIP filename\n");
		return 0;
	}
	int clientSockfd = Connection(argv[1]);
	
	FileInit(&tfs,argv[2]);

	//send(clientSockfd,"hello,server!",sizeof("hello,server!"),0);
	//recLen = recv(clientSockfd,recbuf,sizeof(recbuf),0);
	printf("recv data -->:%s\n",recbuf);
	
	time_t start_time = GetTime();
	
	SendStartFrame(clientSockfd,&tfs);
	while(( (exeStatus = FrameAckParsing(clientSockfd)) == 0) && (tfs.currPoint < tfs.endPoint)){
		SendData(clientSockfd,&tfs);
	}
	if(1 == exeStatus){
		SendEndFrame(clientSockfd);
		exeStatus = FrameAckParsing(clientSockfd);
	}

	close(clientSockfd);
	if(0 == exeStatus) perror("exeStatus = 0");
	time_t end_time = GetTime();
	CalcTime(start_time,end_time);
	
	return 0;
}
