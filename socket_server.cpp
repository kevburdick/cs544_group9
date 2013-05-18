#include<winsock2.h>   
#include<iostream> 
#include<fstream>
#include <stdio.h>

#pragma   comment(lib,   "ws2_32.lib")  

#define   PORT             50000   
#define   MAXLISTEN   5    
using namespace std;
void   main(void)   
{   
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
	char nAmount;
	SOCKET mysocket;
	mysocket=socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in you;
	{
		you.sin_family=AF_INET;
		you.sin_port=htons(PORT);
		you.sin_addr.S_un.S_addr=  INADDR_ANY;
	}
	int nlisten;
	int nbind;
	nbind=bind(mysocket,(struct sockaddr *)&you,sizeof(you));
	if (nbind == SOCKET_ERROR)
    {
        printf("bind() faild! code:%d\n", WSAGetLastError());
        closesocket(mysocket); 
        WSACleanup();
        return;
	}
	nlisten=listen(mysocket,MAXLISTEN);
	if (nlisten == SOCKET_ERROR)
    {
        printf("listen() faild! code:%d\n", WSAGetLastError());
        closesocket(mysocket); 
		WSACleanup();
        return;
    }
	while(true)
	{
		SOCKET newsocket;
		struct sockaddr_in clientaddr;
		int length;
		char buffer[100];
		length=sizeof(struct sockaddr_in);
		newsocket=accept(mysocket,(struct sockaddr FAR*)&clientaddr,&length);
		if(newsocket   ==   INVALID_SOCKET)   
		{   
			printf("accept   fail   !");
		}
		recv(newsocket,buffer,sizeof(buffer),0);
		//nAmount   =   recv(newsocket, buffer,   sizeof(*buffer),   0); 
		printf("%s\0",buffer);
		closesocket(newsocket);
	}		
}