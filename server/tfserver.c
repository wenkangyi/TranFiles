#include "../sharesource/sstime.h"
#include "../sharesource/protocol.h"

//return server sockfd
int SocketInit(void)
{
	int serverSockfd;
	struct sockaddr_in serv_addr;

	serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == serverSockfd) perror("socket return error!");

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT); 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(serverSockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(serverSockfd, SOCKET_MAX_NUM);

	return serverSockfd;
}

//return client sockfd
int SocketAccept(int serverSockfd)
{
	struct sockaddr_in  clie_addr;
	socklen_t clie_addr_len;
	clie_addr_len = sizeof(clie_addr);

	int clientSockfd = accept(serverSockfd,(struct sockaddr *)&clie_addr, &clie_addr_len);
	if(-1 == clientSockfd) perror("accept error!");
	return clientSockfd;
}


// char* FilesInit(size_t fileLen)
// {
// 	//1 初始化文件
// 	char *pFile = (char *)malloc(fileLen);
// 	return pFile;
// }

void CalcStructLen()
{
	printf("StartFrame size-->:%ld\n",sizeof(StartFrame));
	printf("FrameAck size-->:%ld\n",sizeof(FrameAck));
	printf("TranPro size-->:%ld\n",sizeof(TranPro));
	printf("EndFrame size-->:%ld\n",sizeof(EndFrame));
}

int main(int argn,void **argv){
	int serverSockfd = 0;
	int clientSockfd = 0;
	char recBuf[10240];
	TranFileStruct tfs={0,NULL,"",0,0,0,0,0,FILE_NAME_MAX_LEN};
	time_t start_time = GetTime();
	//CalcStructLen();
	serverSockfd = SocketInit();
	clientSockfd = SocketAccept(serverSockfd);
	memset(recBuf,0,10240);
	while(1){
		int n = read(clientSockfd,recBuf,10240);
		if(0 >= n){
			close(clientSockfd);
			printf("Client close!\n");
			break;
		}
		
		switch (recBuf[6])
		{
			case START_FRAME_CMD:{
				SendStartFrameAck(clientSockfd,recBuf,&tfs);
			}break;
			case DATA_FRAME_CMD:{
				SendDataFrameAck(clientSockfd,recBuf,&tfs);
			}break;
			case END_FRAME_CMD:{
				SendEndFrameAck(clientSockfd,recBuf,&tfs);
			}break;

			default:
				break;
		}
		memset(recBuf,0,10240);
	}
	close(serverSockfd);

	time_t end_time = GetTime();
	CalcTime(start_time,end_time);

	printf("Server End!");

	return 0;
}