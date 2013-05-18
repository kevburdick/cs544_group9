#include   <winsock2.h>   
#include   <winbase.h>   
#include   <iostream>  

#pragma   comment(lib,   "ws2_32.lib")  

#define   PORT   50000  
using namespace std;

void   main(void)   
{   
	int s;
	char buffer[100]="This is from the client";
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1,1);
	
	err= WSAStartup(wVersionRequested, &wsaData);
	if (err !=0)
	{
		return ;
	}

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return;
	}
    SOCKET mysocket;
	mysocket=socket(AF_INET, SOCK_STREAM, 0);
	if (mysocket<0)
	{
		perror("socket() called failed.\n");
		exit(-1);
	}
	struct sockaddr_in you;
	{
		you.sin_family=AF_INET;
		you.sin_port=ntohs(PORT);
		you.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
		if(connect(mysocket, (sockaddr FAR*)&you,sizeof(sockaddr_in))<0)
		{
			perror("connect failed.\n");
			closesocket(mysocket);
			system("pause");
		}
		while(true)
		{
			//cout<<"Please type in a sentence."<<endl;
			//cin>>buffer;
			//s=send(mysocket,*buffer,strlen(*buffer)+,0);
			send(mysocket,buffer,strlen(buffer)+1,0);
			printf("%s\n",buffer);
			Sleep(5000);
			closesocket(mysocket);
			exit (0);
		}
	}
}