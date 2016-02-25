// tcpechosrv.cpp : simple multithreaded echo server
//

#include "winsock2.h"
#include "process.h"
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

// process data on socket
void sckProcessData(void *param);

int main(int argc, char* argv[])
{
    WSADATA wsa; 
    SOCKET new_socket, s;
	struct sockaddr_in server, client;
    int c = sizeof(struct sockaddr_in);

	puts("TCP/IP socket ECHO v1.0 server");

	printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
     
    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
 
    printf("Socket created.\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    puts("Bind done");

	//Listen to incoming connections
    listen(s , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
	while(TRUE)
	{
		new_socket = accept(s , (struct sockaddr *)&client, &c);
		if (new_socket == INVALID_SOCKET)
		{
			printf("accept failed with error code : %d" , WSAGetLastError());
		}
		else
		{
			_beginthread(sckProcessData, 0, (LPVOID)new_socket);
		}
	}

	closesocket(s);
    WSACleanup();
	return 0;
}

// process data on socket
void sckProcessData(void *param)
{
	char buff[512];
	int  n;
	SOCKET client=(SOCKET)param; 

	while(true) 
	{
		n=recv(client,buff,512,0);
        if(n==SOCKET_ERROR )			
            break;
		else if(n==0)
			break; // connection closed
        buff[n]=0;
		send(client,buff,strlen(buff),0);
	}
	closesocket(client);
	_endthread();
}


