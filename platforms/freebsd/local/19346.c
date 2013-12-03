source: http://www.securityfocus.com/bid/456/info


Solaris 2.6 and many other unices/clones have a serious problem with their unix domain socket implementation that has it's origins in old BSD code. Any unix socket created by any application is set mode 4777. In Solaris versions 2.5 and earlier, the permissions were ignored completely. The applications are vulnerable to being connected to and written to by anyone. This could lead to a whole number of application-specific security compromises. 

Here is some sample code (by Nirva):

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>

main(int argc, char *argv[])
{
struct sockaddr_un addr;
int s;

s = socket(AF_UNIX, SOCK_STREAM, 0);

bzero(&addr, sizeof(addr));
addr.sun_family = AF_UNIX;
strcpy(addr.sun_path, "/tmp/yoursocket");
if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
perror("bind");
exit(1);
}
sleep(100);

exit(0);
} 