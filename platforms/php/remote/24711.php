source: http://www.securityfocus.com/bid/11557/info

It is reported that cURL allows malicious users to bypass 'open_basedir' restrictions in PHP scripts. This issue is due to a failure of the cURL module to properly enforce PHPs 'open_basedir' restriction.

Users with the ability to create or modify PHP scripts on a server computer hosting the vulnerable software can reportedly exploit this vulnerability to bypass the 'open_basedir' restriction, and access arbitrary files with the privileges of the web server. This may aid them in further attacks.

This vulnerability possibly results in a false sense of security, as administrators expect that the restrictions in place prevent malicious users from gaining access to sensitive information.

<?php
$ch = curl_init("file:///etc/parla");
$file=curl_exec($ch);
echo $file
?>