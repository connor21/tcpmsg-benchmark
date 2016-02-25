// tcpechoclient.cpp : 
// benchmark for small message exchange on TCP/IP
//

#define OUTER_LOOP 10
#define LOOP_COUNT 20
#define TEST_MESSAGE "Hallo TCP EchoHallo TCP\r\n"

#include "winsock2.h"
#include "process.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

long sckClientNoConn(const char *saddr);
long sckClientConn(const char *saddr);


int main(int argc, char* argv[])
{
	WSADATA wsa;
    int c = sizeof(struct sockaddr_in);
	int i;
	long totaltime = 0;
	long cpu_time;
	int msgtotal;
	double msgpersec;
	double secpermessage;
	char *serveraddr;

	if ( argc != 2 ) /* argc should be 2 for correct execution */
    {
        /* We print argv[0] assuming it is the program name */
        printf( "usage: tcpechoclient <server-address>" );
    }
    else 
	{
		serveraddr = argv[1];

		puts("TCP/IP socket ECHO v1.0 client\n");

		printf("Initialising Winsock...\n");
		if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
		{
			printf("Failed. Error Code : %d",WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		printf("Initialised.\n");


		// test message exchange with connection reuse
   		
		printf("test server address is %s\n", serveraddr);
		sckClientNoConn((const char*)serveraddr);  // initialize transfer

		printf("\nrun test with connection reuse\n");
		for (i=0; i<OUTER_LOOP; i++)
		{
			cpu_time = sckClientNoConn((const char*)serveraddr);      
			totaltime += cpu_time;
   			printf("%i messages with %i byte tranferred in %i microseconds \n", LOOP_COUNT, LOOP_COUNT*(strlen(TEST_MESSAGE)+1),cpu_time);
			Sleep(500);
		}

		msgtotal = LOOP_COUNT*OUTER_LOOP;
		secpermessage = double(totaltime)/1000.0/msgtotal;
		msgpersec = 1000.0/secpermessage;
		printf("\n**reuse connection: %i messages with %i byte tranferred in %i microseconds - %f messages per second\n", msgtotal, msgtotal*(strlen(TEST_MESSAGE)+1),totaltime, msgpersec);
	
		// test message exchange with one connection per message

		printf("\nrun test with connect per message\n");
		totaltime = 0;
		for (i=0; i<OUTER_LOOP; i++)
		{
			cpu_time = sckClientConn((const char*)serveraddr);      
			totaltime += cpu_time;
   			printf("%i messages with %i byte tranferred in %i microseconds \n", LOOP_COUNT, LOOP_COUNT*(strlen(TEST_MESSAGE)+1),cpu_time);
			Sleep(500);
		}

		secpermessage = double(totaltime)/1000.0/msgtotal;
		msgpersec = 1000.0/secpermessage;

		printf("\n**connect per message: %i messages with %i byte tranferred in %i microseconds - %f messages per second\n", msgtotal, msgtotal*(strlen(TEST_MESSAGE)+1),totaltime, msgpersec);
	} // end else

	return 0;
}


// message exchange with one connection per message
long sckClientConn(const char *saddr)
{
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency); 

	char buff[512];
	int  n;
	int  x;
	struct sockaddr_in server;
	SOCKET s; 

	QueryPerformanceCounter(&StartingTime);

	for (x = 0; x < LOOP_COUNT; x++)
	{
    
		//Create a socket
		if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(saddr);
		server.sin_port = htons( 8888 );

		//Connect to remote server
		if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
		{
			puts("connect error");
		}

		if( send(s , TEST_MESSAGE , strlen(TEST_MESSAGE) , 0) < 0)
		{
			puts("Send failed");
		}

		n=recv(s,buff,512,0);
		if(n==SOCKET_ERROR )			
			puts("error");
		else if(n==0)
			puts("closed");
		buff[n]=0;
		closesocket(s);

	}

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	
    return ((long) (ElapsedMicroseconds.QuadPart)); // microseconds
    
}

// message exchange with connection reuse
long sckClientNoConn(const char *saddr)
{
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency); 

	char buff[512];
	int  n;
	int  x;
	struct sockaddr_in server;
	SOCKET s;

	QueryPerformanceCounter(&StartingTime);

		//Create a socket
		if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(saddr);
		server.sin_port = htons( 8888 );

		//Connect to remote server
		if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
		{
			puts("connect error");
		}

	for (x = 0; x < LOOP_COUNT; x++)
	{
    
		if( send(s , TEST_MESSAGE , strlen(TEST_MESSAGE) , 0) < 0)
		{
			puts("Send failed");
		}

		n=recv(s,buff,512,0);
		if(n==SOCKET_ERROR )			
			puts("error");
		else if(n==0)
			puts("closed");
		buff[n]=0;

	}

	closesocket(s);

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;

	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

    return ((long) (ElapsedMicroseconds.QuadPart)); // microseconds
    
}

